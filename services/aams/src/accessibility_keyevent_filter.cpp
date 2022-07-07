/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessibility_keyevent_filter.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    int64_t g_taskTime = 500;
} // namespace

static bool IsWantedKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t keyCode = event.GetKeyCode();
    if (keyCode == MMI::KeyEvent::KEYCODE_VOLUME_UP || keyCode == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
        return true;
    }
    return false;
}

KeyEventFilter::KeyEventFilter()
{
    HILOG_DEBUG();

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    timeoutHandler_ = std::make_shared<KeyEventFilterEventHandler>(runner_, *this);
    if (!timeoutHandler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

KeyEventFilter::~KeyEventFilter()
{
    HILOG_DEBUG();

    eventMaps_.clear();
}

bool KeyEventFilter::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    bool whetherIntercept = IsWantedKeyEvent(event);
    if (whetherIntercept) {
        DispatchKeyEvent(event);
        return true;
    }
    EventTransmission::OnKeyEvent(event);
    return false;
}

void KeyEventFilter::SetServiceOnKeyEventResult(AccessibleAbilityConnection &connection, bool isHandled,
    uint32_t sequenceNum)
{
    HILOG_DEBUG("isHandled[%{public}d], sequenceNum[%{public}u].", isHandled, sequenceNum);

    std::shared_ptr<ProcessingEvent> processingEvent = FindProcessingEvent(connection, sequenceNum);
    if (!processingEvent) {
        HILOG_DEBUG("No event being processed.");
        return;
    }

    if (!isHandled) {
        if (!processingEvent->usedCount_) {
            timeoutHandler_->RemoveEvent(processingEvent->seqNum_);
            EventTransmission::OnKeyEvent(*processingEvent->event_);
        }
    } else {
        timeoutHandler_->RemoveEvent(processingEvent->seqNum_);
        RemoveProcessingEvent(processingEvent);
    }
}

void KeyEventFilter::ClearServiceKeyEvents(AccessibleAbilityConnection &connection)
{
    HILOG_DEBUG();

    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        if (iter->first.GetRefPtr() != &connection) {
            continue;
        }

        for (auto &val : iter->second) {
            val->usedCount_--;
            if (!val->usedCount_) {
                EventTransmission::OnKeyEvent(*val->event_);
            }
        }
        eventMaps_.erase(iter);
        break;
    }
}

void KeyEventFilter::DispatchKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();

    std::shared_ptr<ProcessingEvent> processingEvent = nullptr;
    std::shared_ptr<MMI::KeyEvent> copyEvent = nullptr;
    sequenceNum_++;
    for (auto iter = connectionMaps.begin(); iter != connectionMaps.end(); iter++) {
        if (iter->second->OnKeyPressEvent(event, sequenceNum_)) {
            if (!processingEvent) {
                processingEvent = std::make_shared<ProcessingEvent>();
                copyEvent = std::make_shared<MMI::KeyEvent>(event);
                processingEvent->event_ = copyEvent;
                processingEvent->seqNum_ = sequenceNum_;
            }
            processingEvent->usedCount_++;

            if (eventMaps_.find(iter->second) == eventMaps_.end()) {
                std::vector<std::shared_ptr<ProcessingEvent>> processingEvens;
                eventMaps_.insert(std::make_pair(iter->second, processingEvens));
            }
            eventMaps_.at(iter->second).emplace_back(processingEvent);
        }
    }

    if (!processingEvent) {
        HILOG_DEBUG("No service handles the event.");
        sequenceNum_--;
        EventTransmission::OnKeyEvent(event);
        return;
    }

    timeoutHandler_->SendEvent(sequenceNum_, processingEvent, g_taskTime);
}

bool KeyEventFilter::RemoveProcessingEvent(std::shared_ptr<ProcessingEvent> event)
{
    HILOG_DEBUG();

    bool haveEvent = false;
    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        for (auto val = iter->second.begin(); val != iter->second.end(); val++) {
            if (*val != event) {
                continue;
            }
            (*val)->usedCount_--;
            iter->second.erase(val);
            haveEvent = true;
            break;
        }
    }

    return haveEvent;
}

std::shared_ptr<KeyEventFilter::ProcessingEvent> KeyEventFilter::FindProcessingEvent(
    AccessibleAbilityConnection &connection, uint32_t sequenceNum)
{
    HILOG_DEBUG();

    std::shared_ptr<ProcessingEvent> processingEvent = nullptr;

    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        if (iter->first.GetRefPtr() != &connection) {
            continue;
        }

        for (auto val = iter->second.begin(); val != iter->second.end(); val++) {
            if ((*val)->seqNum_ != sequenceNum) {
                continue;
            }
            processingEvent = *val;
            iter->second.erase(val);
            processingEvent->usedCount_--;
            break;
        }
        break;
    }

    return processingEvent;
}

void KeyEventFilter::DestroyEvents()
{
    HILOG_DEBUG();

    timeoutHandler_->RemoveAllEvents();
    eventMaps_.clear();
    EventTransmission::DestroyEvents();
}

void KeyEventFilter::SendEventToNext(MMI::KeyEvent &event)
{
    HILOG_DEBUG();
    EventTransmission::OnKeyEvent(event);
}

KeyEventFilterEventHandler::KeyEventFilterEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, KeyEventFilter &keyEventFilter)
    : AppExecFwk::EventHandler(runner), keyEventFilter_(keyEventFilter)
{
    HILOG_DEBUG();
}

void KeyEventFilterEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    if (!event) {
        HILOG_ERROR("event is null.");
        return;
    }

    auto processingEvent = event->GetSharedObject<KeyEventFilter::ProcessingEvent>();
    if (processingEvent->seqNum_ != event->GetInnerEventId()) {
        HILOG_ERROR("event is wrong.");
        return;
    }

    bool haveEvent = keyEventFilter_.RemoveProcessingEvent(processingEvent);
    if (haveEvent) {
        keyEventFilter_.SendEventToNext(*processingEvent->event_);
    }
}
} // namespace Accessibility
} // namespace OHOS