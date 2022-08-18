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

#include "accessibility_mouse_key.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
struct MouseMoveOffset {
    int32_t offsetX = 0;
    int32_t offsetY = 0;
};
namespace {
    constexpr size_t ITEM_COUNT_1 = 1;
    constexpr int32_t MOVE_LEFT_STEP = -5;
    constexpr int32_t MOVE_RIGHT_STEP = 5;
    constexpr int32_t MOVE_UP_STEP = -5;
    constexpr int32_t MOVE_DOWN_STEP = 5;
    const std::vector<int32_t> MOUSE_KEYCODE_V = {
        MMI::KeyEvent::KEYCODE_NUMPAD_1, MMI::KeyEvent::KEYCODE_NUMPAD_2, MMI::KeyEvent::KEYCODE_NUMPAD_3,
        MMI::KeyEvent::KEYCODE_NUMPAD_4, MMI::KeyEvent::KEYCODE_NUMPAD_5, MMI::KeyEvent::KEYCODE_NUMPAD_6,
        MMI::KeyEvent::KEYCODE_NUMPAD_7, MMI::KeyEvent::KEYCODE_NUMPAD_8, MMI::KeyEvent::KEYCODE_NUMPAD_9,
        MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE, MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY,
        MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT, MMI::KeyEvent::KEYCODE_NUMPAD_ADD};
    const std::map<int32_t, MouseMoveOffset> MOUSE_MOVE_OFFSET_M = {
        {MMI::KeyEvent::KEYCODE_NUMPAD_1, {MOVE_LEFT_STEP, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_2, {0, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_3, {MOVE_RIGHT_STEP, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_4, {MOVE_LEFT_STEP, 0}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_6, {MOVE_RIGHT_STEP, 0}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_7, {MOVE_LEFT_STEP, MOVE_UP_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_8, {0, MOVE_UP_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_9, {MOVE_RIGHT_STEP, MOVE_UP_STEP}}};
} // namespace

bool AccessibilityMouseKey::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t sourceType = event.GetSourceType();
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointers = event.GetPointerIds();
    size_t pointerCount = pointers.size();
    if ((sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (action == MMI::PointerEvent::POINTER_ACTION_MOVE) &&
        (pointerCount == ITEM_COUNT_1)) {
        UpdateLastMouseEvent(event);
    }
    return false;
}

bool AccessibilityMouseKey::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t keyCode = event.GetKeyCode();
    size_t pressedKeyCount = event.GetPressedKeys().size();
    if (IsMouseKey(keyCode) && (pressedKeyCount == ITEM_COUNT_1)) {
        if (event.GetKeyAction() == MMI::KeyEvent::KEY_ACTION_DOWN) {
            ExecuteMouseKey(keyCode);
        }
        return true;
    }
    return false;
}

void AccessibilityMouseKey::UpdateLastMouseEvent(const MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    lastMouseMoveEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

bool AccessibilityMouseKey::IsMouseKey(int32_t keyCode) const
{
    HILOG_DEBUG("keyCode:%{public}d", keyCode);
    if (std::find(MOUSE_KEYCODE_V.begin(), MOUSE_KEYCODE_V.end(), keyCode) != MOUSE_KEYCODE_V.end()) {
        return true;
    }
    return false;
}

void AccessibilityMouseKey::ExecuteMouseKey(int32_t keyCode)
{
    HILOG_DEBUG("keyCode:%{public}d", keyCode);

    if ((keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_1) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_2) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_3) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_4) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_6) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_7) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_8) ||
        (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_9)) {
        auto iter = MOUSE_MOVE_OFFSET_M.find(keyCode);
        if (iter != MOUSE_MOVE_OFFSET_M.end()) {
            MoveMousePointer(iter->second.offsetX, iter->second.offsetY);
        }
    } else if (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_5) {
        SendMouseClickEvent(SINGLE_CLICK);
    } else if (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE) {
        selectedKeyType_ = LEFT_KEY;
    } else if (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY) {
        selectedKeyType_ = BOOTH_KEY;
    } else if (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT) {
        selectedKeyType_ = RIGHT_KEY;
    } else if (keyCode == MMI::KeyEvent::KEYCODE_NUMPAD_ADD) {
        SendMouseClickEvent(DOUBLE_CLICK);
    }
}

void AccessibilityMouseKey::MoveMousePointer(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG("offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);

    EventTransmission::OnMoveMouse(offsetX, offsetY);
}

void AccessibilityMouseKey::SendMouseClickEvent(CLICK_TYPE clickType)
{
    HILOG_DEBUG();

    if (!lastMouseMoveEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }

    int64_t nowTime = GetSystemTime();
    // Update event information.
    lastMouseMoveEvent_->SetActionTime(nowTime);
    lastMouseMoveEvent_->SetActionStartTime(nowTime);

    // Update pointer item information.
    int32_t pointerId = lastMouseMoveEvent_->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    lastMouseMoveEvent_->GetPointerItem(pointerId, item);
    item.SetDownTime(nowTime);
    item.SetPressed(true);
    lastMouseMoveEvent_->UpdatePointerItem(pointerId, item);

    for (uint32_t clickCount = 0; clickCount < clickType; clickCount ++) {
        HILOG_DEBUG("selectedKeyType:%{public}u", selectedKeyType_);
        if (selectedKeyType_ == LEFT_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        } else if (selectedKeyType_ == RIGHT_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        } else if (selectedKeyType_ == BOOTH_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);

            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        }
    }
}

void AccessibilityMouseKey::PerformMouseAction(int32_t buttonId, int32_t actionType)
{
    HILOG_DEBUG();

    if (!lastMouseMoveEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }
    lastMouseMoveEvent_->SetButtonId(buttonId);
    lastMouseMoveEvent_->SetButtonPressed(buttonId);
    lastMouseMoveEvent_->SetPointerAction(actionType);
    EventTransmission::OnPointerEvent(*lastMouseMoveEvent_);
}

int64_t AccessibilityMouseKey::GetSystemTime() const
{
    HILOG_DEBUG();

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}
} // namespace Accessibility
} // namespace OHOS