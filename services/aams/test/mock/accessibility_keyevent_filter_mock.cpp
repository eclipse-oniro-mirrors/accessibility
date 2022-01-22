/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "power_mgr_client.h"
bool testKeyevent = false;
namespace OHOS {
namespace Accessibility {

int64_t TASK_TIME = 500;

KeyEventFilter::KeyEventFilter()
{
    HILOG_DEBUG();

    aams_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (aams_ != nullptr) {
        HILOG_DEBUG();

        runner_ = aams_->GetMainRunner();
        if (!runner_) {
            HILOG_ERROR("get runner failed");
            return ;
        }

        timeouthandler_ = std::make_shared<KeyEventFilterEventHandler>(runner_, *this);
        if (!timeouthandler_) {
            HILOG_ERROR("create event handler failed");
            return ;
        }
    }
}

KeyEventFilter::~KeyEventFilter()
{
    HILOG_DEBUG();

    eventMaps_.clear();
}

void KeyEventFilter::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    bool whetherIntercept = aams_->IsWantedKeyEvent(event);
    if (whetherIntercept) {
        DispatchKeyEvent(event);
    } else {
        EventTransmission::OnKeyEvent(event);
    }
}

void KeyEventFilter::SetServiceOnKeyEventResult(AccessibleAbilityConnection &connection, bool isHandled,
    uint32_t sequenceNum)
{
    testKeyevent = isHandled;
}

void KeyEventFilter::ClearServiceKeyEvents(AccessibleAbilityConnection &connection)
{
    HILOG_DEBUG();

    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        if (iter->first.GetRefPtr() != &connection) {
            continue;
        }

        for (auto val : iter->second) {
            val->usedCount_--;
            if (val->usedCount_ == 0) {
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
    
    sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();

    std::shared_ptr<ProcessingEvent> processingEvent = nullptr;
    std::shared_ptr<MMI::KeyEvent> copyEvent = nullptr;
    sequenceNum_++;
    for (auto iter = connectionMaps.begin(); iter != connectionMaps.end(); iter++) {
        if (iter->second->OnKeyPressEvent(event, sequenceNum_)) {
            if (processingEvent == nullptr) {
                copyEvent = std::make_shared<MMI::KeyEvent>(event);
                processingEvent = std::make_shared<ProcessingEvent>();
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

    if (processingEvent == nullptr) {
        HILOG_DEBUG("No service handles the event.");
        sequenceNum_--;
        EventTransmission::OnKeyEvent(event);
        return;
    }

    timeouthandler_->SendEvent(sequenceNum_, processingEvent, TASK_TIME);
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
    AccessibleAbilityConnection& connection, uint32_t sequenceNum)
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

    timeouthandler_->RemoveAllEvents();
    eventMaps_.clear();
}

void KeyEventFilter::SendEventToParent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();
    EventTransmission::OnKeyEvent(event);
}

KeyEventFilterEventHandler::KeyEventFilterEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, KeyEventFilter &keyEventFilter)
    : AppExecFwk::EventHandler(runner), keyEventFilter_(keyEventFilter)
{
    HILOG_DEBUG("KeyEventFilterEventHandler is created");
}

void KeyEventFilterEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    if (event == nullptr) {
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
        keyEventFilter_.SendEventToParent(*processingEvent->event_);
    }
}

}
}  // namespace accessibility