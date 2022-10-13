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
    // item count
    constexpr size_t ITEM_COUNT_1 = 1;
    constexpr size_t ITEM_COUNT_2 = 2;
    constexpr size_t ITEM_COUNT_3 = 3;
    // move offset
    constexpr int32_t MOVE_LEFT_STEP = -5;
    constexpr int32_t MOVE_RIGHT_STEP = 5;
    constexpr int32_t MOVE_UP_STEP = -5;
    constexpr int32_t MOVE_DOWN_STEP = 5;
    // speed multiple
    constexpr float SPEED_UP_MULTIPLE = 5.0f;
    constexpr float SLOW_DOWN_MULTIPLE = 0.5f;
    // result of parsing meta key
    constexpr int32_t INVALID_KEY = -1;
    constexpr int32_t NONE_KEY = 0;
    constexpr int32_t CTRL_KEY = 1;
    constexpr int32_t SHIFT_KEY = 2;
    constexpr int32_t CTRL_SHIFT_KEY = 3;
    // array(PRESSED_METAKEYS_TBL)'s length
    constexpr int32_t ROW_COUNT = 21;
    constexpr int32_t COLUMN_COUNT = 3;
    const std::vector<int32_t> MOUSE_KEYCODE_V = {
        MMI::KeyEvent::KEYCODE_NUMPAD_1, MMI::KeyEvent::KEYCODE_NUMPAD_2, MMI::KeyEvent::KEYCODE_NUMPAD_3,
        MMI::KeyEvent::KEYCODE_NUMPAD_4, MMI::KeyEvent::KEYCODE_NUMPAD_5, MMI::KeyEvent::KEYCODE_NUMPAD_6,
        MMI::KeyEvent::KEYCODE_NUMPAD_7, MMI::KeyEvent::KEYCODE_NUMPAD_8, MMI::KeyEvent::KEYCODE_NUMPAD_9,
        MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE, MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY,
        MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT, MMI::KeyEvent::KEYCODE_NUMPAD_ADD};
    const std::vector<int32_t> MOUSE_MOVE_KEYCODE_V = {
        MMI::KeyEvent::KEYCODE_NUMPAD_1, MMI::KeyEvent::KEYCODE_NUMPAD_2, MMI::KeyEvent::KEYCODE_NUMPAD_3,
        MMI::KeyEvent::KEYCODE_NUMPAD_4, MMI::KeyEvent::KEYCODE_NUMPAD_6, MMI::KeyEvent::KEYCODE_NUMPAD_7,
        MMI::KeyEvent::KEYCODE_NUMPAD_8, MMI::KeyEvent::KEYCODE_NUMPAD_9};
    const std::vector<int32_t> CTRL_SHIFT_KEYCODE_V = {
        MMI::KeyEvent::KEYCODE_CTRL_LEFT, MMI::KeyEvent::KEYCODE_CTRL_RIGHT,
        MMI::KeyEvent::KEYCODE_SHIFT_LEFT, MMI::KeyEvent::KEYCODE_SHIFT_RIGHT};
    const std::map<int32_t, MouseMoveOffset> MOUSE_MOVE_OFFSET_M = {
        {MMI::KeyEvent::KEYCODE_NUMPAD_1, {MOVE_LEFT_STEP, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_2, {0, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_3, {MOVE_RIGHT_STEP, MOVE_DOWN_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_4, {MOVE_LEFT_STEP, 0}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_6, {MOVE_RIGHT_STEP, 0}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_7, {MOVE_LEFT_STEP, MOVE_UP_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_8, {0, MOVE_UP_STEP}},
        {MMI::KeyEvent::KEYCODE_NUMPAD_9, {MOVE_RIGHT_STEP, MOVE_UP_STEP}}};
    const int32_t PRESSED_METAKEYS_TBL[ROW_COUNT][COLUMN_COUNT] = {
        {MMI::KeyEvent::KEYCODE_UNKNOWN, MMI::KeyEvent::KEYCODE_UNKNOWN, NONE_KEY},
        {MMI::KeyEvent::KEYCODE_UNKNOWN, MMI::KeyEvent::KEYCODE_CTRL_LEFT, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_UNKNOWN, MMI::KeyEvent::KEYCODE_CTRL_RIGHT, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_UNKNOWN, MMI::KeyEvent::KEYCODE_SHIFT_LEFT, SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_UNKNOWN, MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, SHIFT_KEY},

        {MMI::KeyEvent::KEYCODE_CTRL_LEFT, MMI::KeyEvent::KEYCODE_UNKNOWN, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_LEFT, MMI::KeyEvent::KEYCODE_CTRL_RIGHT, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_LEFT, MMI::KeyEvent::KEYCODE_SHIFT_LEFT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_LEFT, MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, CTRL_SHIFT_KEY},

        {MMI::KeyEvent::KEYCODE_CTRL_RIGHT, MMI::KeyEvent::KEYCODE_UNKNOWN, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_RIGHT, MMI::KeyEvent::KEYCODE_CTRL_LEFT, CTRL_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_RIGHT, MMI::KeyEvent::KEYCODE_SHIFT_LEFT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_CTRL_RIGHT, MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, CTRL_SHIFT_KEY},

        {MMI::KeyEvent::KEYCODE_SHIFT_LEFT, MMI::KeyEvent::KEYCODE_UNKNOWN, SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_LEFT, MMI::KeyEvent::KEYCODE_CTRL_LEFT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_LEFT, MMI::KeyEvent::KEYCODE_CTRL_RIGHT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_LEFT, MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, SHIFT_KEY},

        {MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, MMI::KeyEvent::KEYCODE_UNKNOWN, SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, MMI::KeyEvent::KEYCODE_CTRL_LEFT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, MMI::KeyEvent::KEYCODE_CTRL_RIGHT, CTRL_SHIFT_KEY},
        {MMI::KeyEvent::KEYCODE_SHIFT_RIGHT, MMI::KeyEvent::KEYCODE_SHIFT_LEFT, SHIFT_KEY},
    };
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

    int32_t actionKey = MMI::KeyEvent::KEYCODE_UNKNOWN;
    int32_t metaKey1 = MMI::KeyEvent::KEYCODE_UNKNOWN;
    int32_t metaKey2 = MMI::KeyEvent::KEYCODE_UNKNOWN;
    std::vector<int32_t> pressedKeys = event.GetPressedKeys();
    if (IsMouseKey(pressedKeys, actionKey, metaKey1, metaKey2)) {
        return ExecuteMouseKey(actionKey, metaKey1, metaKey2);
    }
    return false;
}

void AccessibilityMouseKey::UpdateLastMouseEvent(const MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    lastMouseMoveEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

bool AccessibilityMouseKey::IsMouseKey(const std::vector<int32_t> &pressedKeys, int32_t &actionKey,
    int32_t &metaKey1, int32_t &metaKey2) const
{
    HILOG_DEBUG();

    size_t pressedKeyCount = pressedKeys.size();
    if (pressedKeyCount == ITEM_COUNT_1) {
        if (std::find(MOUSE_KEYCODE_V.begin(), MOUSE_KEYCODE_V.end(), pressedKeys[0]) != MOUSE_KEYCODE_V.end()) {
            actionKey = pressedKeys[0];
            return true;
        }
    } else if (pressedKeyCount == ITEM_COUNT_2) {
        for (size_t i = 0; i < ITEM_COUNT_2; i++) {
            if (std::find(MOUSE_MOVE_KEYCODE_V.begin(), MOUSE_MOVE_KEYCODE_V.end(), pressedKeys[i]) ==
                MOUSE_MOVE_KEYCODE_V.end()) {
                continue;
            }
            actionKey = pressedKeys[i];
            size_t Index = (i + 1) % ITEM_COUNT_2;
            if (std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index]) !=
                CTRL_SHIFT_KEYCODE_V.end()) {
                metaKey1 = pressedKeys[Index];
                return true;
            }
        }
    } else if (pressedKeyCount == ITEM_COUNT_3) {
        for (size_t i = 0; i < ITEM_COUNT_3; i++) {
            if (std::find(MOUSE_MOVE_KEYCODE_V.begin(), MOUSE_MOVE_KEYCODE_V.end(), pressedKeys[i]) ==
                MOUSE_MOVE_KEYCODE_V.end()) {
                continue;
            }
            actionKey = pressedKeys[i];
            size_t Index1 = (i + 1) % ITEM_COUNT_3;
            size_t Index2 = (i + 2) % ITEM_COUNT_3;
            if ((std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index1]) !=
                CTRL_SHIFT_KEYCODE_V.end()) &&
                (std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index2]) !=
                CTRL_SHIFT_KEYCODE_V.end())) {
                metaKey1 = pressedKeys[Index1];
                metaKey2 = pressedKeys[Index2];
                return true;
            }
        }
    }
    return false;
}

int32_t AccessibilityMouseKey::ParseMetaKey(int32_t metaKey1, int32_t metaKey2) const
{
    HILOG_DEBUG();
    for (int32_t i = 0; i < ROW_COUNT; i++) {
        if ((metaKey1 == PRESSED_METAKEYS_TBL[i][0]) && (metaKey2 == PRESSED_METAKEYS_TBL[i][1])) {
            return PRESSED_METAKEYS_TBL[i][COLUMN_COUNT - 1];
        }
    }
    return INVALID_KEY;
}

bool AccessibilityMouseKey::ExecuteMouseKey(int32_t actionKey, int32_t metaKey1, int32_t metaKey2)
{
    HILOG_DEBUG("actionKey:%{public}d, metaKey1:%{public}d, metaKey2:%{public}d", actionKey, metaKey1, metaKey2);

    if ((actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_1) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_2) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_3) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_4) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_6) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_7) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_8) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_9)) {
        auto iter = MOUSE_MOVE_OFFSET_M.find(actionKey);
        if (iter != MOUSE_MOVE_OFFSET_M.end()) {
            int32_t offsetX = iter->second.offsetX;
            int32_t offsetY = iter->second.offsetY;
            int32_t result = ParseMetaKey(metaKey1, metaKey2);
            if ((result == INVALID_KEY) || (result == CTRL_SHIFT_KEY)) {
                return false;
            }
            if (result == CTRL_KEY) {
                offsetX = static_cast<int32_t>(iter->second.offsetX * SPEED_UP_MULTIPLE);
                offsetY = static_cast<int32_t>(iter->second.offsetY * SPEED_UP_MULTIPLE);
            } else if (result == SHIFT_KEY) {
                offsetX = static_cast<int32_t>(iter->second.offsetX * SLOW_DOWN_MULTIPLE);
                offsetY = static_cast<int32_t>(iter->second.offsetY * SLOW_DOWN_MULTIPLE);
            }
            MoveMousePointer(offsetX, offsetY);
        }
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_5) {
        SendMouseClickEvent(SINGLE_CLICK);
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE) {
        selectedKeyType_ = LEFT_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY) {
        selectedKeyType_ = BOOTH_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT) {
        selectedKeyType_ = RIGHT_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_ADD) {
        SendMouseClickEvent(DOUBLE_CLICK);
    }
    return true;
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