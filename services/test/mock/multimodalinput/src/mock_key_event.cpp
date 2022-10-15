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

#include "mock_key_event.h"

namespace OHOS {
namespace MMI {
const int32_t KeyEvent::KEYCODE_UNKNOWN = -1;
const int32_t KeyEvent::KEYCODE_VOLUME_UP = 16;
const int32_t KeyEvent::KEYCODE_VOLUME_DOWN = 17;
const int32_t KeyEvent::KEYCODE_POWER = 18;
const int32_t KeyEvent::KEY_ACTION_UNKNOWN = 0X00000000;
const int32_t KeyEvent::KEY_ACTION_CANCEL = 0X00000001;
const int32_t KeyEvent::KEY_ACTION_DOWN = 0x00000002;
const int32_t KeyEvent::KEY_ACTION_UP = 0X00000003;
const int32_t KeyEvent::KEYCODE_NUMPAD_1 = 2104;
const int32_t KeyEvent::KEYCODE_NUMPAD_2 = 2105;
const int32_t KeyEvent::KEYCODE_NUMPAD_3 = 2106;
const int32_t KeyEvent::KEYCODE_NUMPAD_4 = 2107;
const int32_t KeyEvent::KEYCODE_NUMPAD_5 = 2108;
const int32_t KeyEvent::KEYCODE_NUMPAD_6 = 2109;
const int32_t KeyEvent::KEYCODE_NUMPAD_7 = 2110;
const int32_t KeyEvent::KEYCODE_NUMPAD_8 = 2111;
const int32_t KeyEvent::KEYCODE_NUMPAD_9 = 2112;
const int32_t KeyEvent::KEYCODE_NUMPAD_DIVIDE = 2113;
const int32_t KeyEvent::KEYCODE_NUMPAD_MULTIPLY = 2114;
const int32_t KeyEvent::KEYCODE_NUMPAD_SUBTRACT = 2115;
const int32_t KeyEvent::KEYCODE_NUMPAD_ADD = 2116;
const int32_t KeyEvent::KEYCODE_SHIFT_LEFT = 2047;
const int32_t KeyEvent::KEYCODE_SHIFT_RIGHT = 2048;
const int32_t KeyEvent::KEYCODE_CTRL_LEFT = 2072;
const int32_t KeyEvent::KEYCODE_CTRL_RIGHT = 2073;

KeyEvent::KeyItem::KeyItem()
{}

KeyEvent::KeyItem::~KeyItem()
{}

int32_t KeyEvent::KeyItem::GetKeyCode() const
{
    return keyCode_;
}

void KeyEvent::KeyItem::SetKeyCode(int32_t keyCode)
{
    keyCode_ = keyCode;
}

int64_t KeyEvent::KeyItem::GetDownTime() const
{
    return downTime_;
}

void KeyEvent::KeyItem::SetDownTime(int64_t downTime)
{
    downTime_ = downTime;
}

int32_t KeyEvent::KeyItem::GetDeviceId() const
{
    return deviceId_;
}

void KeyEvent::KeyItem::SetDeviceId(int32_t deviceId)
{
    deviceId_ = deviceId;
}

bool KeyEvent::KeyItem::IsPressed() const
{
    return pressed_;
}

void KeyEvent::KeyItem::SetPressed(bool pressed)
{
    pressed_ = pressed;
}

KeyEvent::KeyEvent(int32_t eventType) : InputEvent(eventType)
{}

KeyEvent::KeyEvent(const KeyEvent& other)
    : InputEvent(other), keyCode_(other.keyCode_), keys_(other.keys_), keyAction_(other.keyAction_)
{}

KeyEvent::~KeyEvent()
{}

std::shared_ptr<KeyEvent> KeyEvent::Create()
{
    return std::shared_ptr<KeyEvent>(new KeyEvent(InputEvent::EVENT_TYPE_KEY));
}

void KeyEvent::AddKeyItem(const KeyItem& keyItem)
{
    keys_.push_back(keyItem);
}

int32_t KeyEvent::GetKeyCode() const
{
    return keyCode_;
}

void KeyEvent::SetKeyCode(int32_t keyCode)
{
    keyCode_ = keyCode;
}

bool KeyEvent::WriteToParcel(Parcel& out) const
{
    return true;
}

bool KeyEvent::ReadFromParcel(Parcel& in)
{
    return true;
}

std::vector<int32_t> KeyEvent::GetPressedKeys() const
{
    std::vector<int32_t> result;
    for (const auto &item : keys_) {
        if (item.IsPressed()) {
            result.push_back(item.GetKeyCode());
        }
    }
    return result;
}

int32_t KeyEvent::GetKeyAction() const
{
    return keyAction_;
}
} // namespace MMI
} // namespace OHOS