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

#include "accessibility_short_key.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
static const int32_t KEY_ITEM_COUNT_1 = 1;
static const uint32_t SHORT_KEY_TIMEOUT_MSG = 1;
static const int32_t MULTI_PRESS_TIMER = 300; // ms
static const int32_t TRIPLE_PRESS_COUNT = 3;

AccessibilityShortKey::AccessibilityShortKey()
{
    HILOG_DEBUG();

    std::shared_ptr<AppExecFwk::EventRunner> runner =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
    if (!runner) {
        HILOG_ERROR("get runner failed");
        return;
    }

    timeouthandler_ = std::make_shared<ShortKeyEventHandler>(runner, *this);
    if (!timeouthandler_) {
        HILOG_ERROR("create event handler failed");
    }
}

AccessibilityShortKey::~AccessibilityShortKey()
{
    HILOG_DEBUG();

    cachedKeyEvents_.clear();
    timeouthandler_ = nullptr;
    lastKeyAction_ = MMI::KeyEvent::KEY_ACTION_UNKNOWN;
}

void AccessibilityShortKey::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t keycode = event.GetKeyCode();
    int32_t pressedKeyCount = event.GetPressedKeys().size();
    if ((keycode != MMI::KeyEvent::KEYCODE_POWER) ||
        (pressedKeyCount > KEY_ITEM_COUNT_1)) {
        HILOG_DEBUG("key[%{public}d] is not power key, or the number[%{public}d]\
            of keys pressed is greater than 1.", keycode, pressedKeyCount);
        EventTransmission::OnKeyEvent(event);
        return;
    }

    RecognizeShortKey(event);
}

void AccessibilityShortKey::SendKeyEventToNext()
{
    HILOG_DEBUG();

    for (std::shared_ptr<MMI::KeyEvent> keyEvent : cachedKeyEvents_) {
        EventTransmission::OnKeyEvent(*keyEvent);
    }

    ClearCachedEventsAndMsg();
}

void AccessibilityShortKey::DestroyEvents()
{
    HILOG_DEBUG();

    ClearCachedEventsAndMsg();
}

void AccessibilityShortKey::RecognizeShortKey(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t action = event.GetKeyAction();
    int32_t pressedKeyCount = event.GetPressedKeys().size();
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    AddCachedKeyEvent(keyEvent);

    if (action == MMI::KeyEvent::KEY_ACTION_DOWN) {
        if (pressedKeyCount != KEY_ITEM_COUNT_1) {
            SendKeyEventToNext();
            return;
        }
        if (lastKeyAction_ != MMI::KeyEvent::KEY_ACTION_DOWN) {
            timeouthandler_->RemoveEvent(SHORT_KEY_TIMEOUT_MSG);
            timeouthandler_->SendEvent(SHORT_KEY_TIMEOUT_MSG, 0, MULTI_PRESS_TIMER);
        }
        lastKeyAction_ = action;
    } else if (action == MMI::KeyEvent::KEY_ACTION_UP) {
        if (pressedKeyCount || !IsUpValid()) {
            SendKeyEventToNext();
            return;
        }
        lastKeyAction_ = action;
        if (IsTriplePress()) {
            OnShortKey();
        }
    } else {
        SendKeyEventToNext();
    }
}

void AccessibilityShortKey::OnShortKey()
{
    HILOG_DEBUG();

    ClearCachedEventsAndMsg();
    Singleton<AccessibleAbilityManagerService>::GetInstance().EnableShortKeyTargetAbility();
}

void AccessibilityShortKey::AddCachedKeyEvent(std::shared_ptr<MMI::KeyEvent> &event)
{
    HILOG_DEBUG();

    cachedKeyEvents_.emplace_back(event);
}

bool AccessibilityShortKey::IsTriplePress()
{
    HILOG_DEBUG();

    uint32_t upEventCount = 0;
    int32_t action = MMI::KeyEvent::KEY_ACTION_UNKNOWN;
    for (std::shared_ptr<MMI::KeyEvent> keyEvent : cachedKeyEvents_) {
        action = keyEvent->GetKeyAction();
        if (action == MMI::KeyEvent::KEY_ACTION_UP) {
            upEventCount++;
        }
    }

    if (upEventCount >= TRIPLE_PRESS_COUNT) {
        return true;
    }
    return false;
}

bool AccessibilityShortKey::IsUpValid()
{
    HILOG_DEBUG();

    if (lastKeyAction_ == MMI::KeyEvent::KEY_ACTION_DOWN) {
        return true;
    }
    return false;
}

void AccessibilityShortKey::ClearCachedEventsAndMsg()
{
    HILOG_DEBUG();

    cachedKeyEvents_.clear();
    lastKeyAction_ = MMI::KeyEvent::KEY_ACTION_UNKNOWN;
    timeouthandler_->RemoveEvent(SHORT_KEY_TIMEOUT_MSG);
}

AccessibilityShortKey::ShortKeyEventHandler::ShortKeyEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityShortKey &shortKey) : AppExecFwk::EventHandler(runner), shortKey_(shortKey)
{
    HILOG_DEBUG("ShortKeyEventHandler is created");
}

void AccessibilityShortKey::ShortKeyEventHandler::ProcessEvent(
    const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    switch (event->GetInnerEventId()) {
        case SHORT_KEY_TIMEOUT_MSG:
            shortKey_.SendKeyEventToNext();
            break;
        default:
            break;
    }
}
} // namespace Accessibility
} // namespace OHOS