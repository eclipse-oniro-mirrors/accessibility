/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include <memory>
#include <vector>

#include "input_event.h"
#include "parcel.h"

namespace OHOS {
namespace MMI {
class KeyEvent : public InputEvent {
public:
    static const int32_t KEYCODE_VOLUME_UP;
    static const int32_t KEYCODE_VOLUME_DOWN;

    virtual ~KeyEvent();
    static std::shared_ptr<KeyEvent> Create();
    int32_t GetKeyCode() const;
    void SetKeyCode(int32_t keyCode);

public:
    class KeyItem {
    public:
        KeyItem();
        virtual ~KeyItem();

        // Get or set the key code.
        // The key code is the number that identifies the key
        int32_t GetKeyCode() const;
        void SetKeyCode(int32_t keyCode);

        // Get or set the key press time
        int32_t GetDownTime() const;
        void SetDownTime(int32_t downTime);

        // Get or set the unique identifier of the device reporting this button. i
        // The default value is 0, which means that the non-real device reports.
        int32_t GetDeviceId() const;
        void SetDeviceId(int32_t deviceId);

        // Gets or sets whether the key is currently pressed.
        // The default value is true, which means it is in a pressed state.
        bool IsPressed() const;
        void SetPressed(bool pressed);

    private:
        bool pressed_;
        int32_t downTime_;
        int32_t deviceId_;
        int32_t keyCode_;
    };

    void AddKeyItem(const KeyItem& keyItem);

public:
    bool WriteToParcel(Parcel &out) const;
    bool ReadFromParcel(Parcel &in);

protected:
    explicit KeyEvent(int32_t eventType);

private:
    int32_t keyCode_;
    std::vector<KeyItem> keys_;
};
}
}
#endif // KEY_EVENT_H