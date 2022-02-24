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

#ifndef POINTER_EVENT_H
#define POINTER_EVENT_H

#include <array>
#include <list>
#include <memory>
#include <set>
#include <stdint.h>
#include <vector>

#include "input_event.h"

namespace OHOS {
namespace MMI {
class PointerEvent : public InputEvent {
public:
    class PointerItem {
    public:
        PointerItem();
        ~PointerItem();

    public:
        // Get or set the id of PointerItem
        int32_t GetPointerId() const;
        void SetPointerId(int32_t pointerId);

        // Get or set the time when the Pointer is pressed
        int32_t GetDownTime() const;
        void SetDownTime(int32_t downTime);

        // Get or set whether the Pointer is pressed
        bool IsPressed() const;
        void SetPressed(bool pressed);

        // Get or set the global X coordinate of Pointer.
        // For touchpad input events, it is the absolute X coordinate on the touchpad;
        // for other pointer type input events, it is the coordinate X in the target logic screen
        int32_t GetGlobalX() const;
        void SetGlobalX(int32_t globalX);

        // Get or set the global Y coordinate of Pointer.
        // For touchpad input events, it is the absolute Y coordinate on the touchpad;
        // for other pointer type input events, it is the coordinate Y in the target logic screen
        int32_t GetGlobalY() const;
        void SetGlobalY(int32_t globalY);

        // Get or set the X coordinate in the current window
        int32_t GetLocalX() const;
        void SetLocalX(int32_t x);

        // Get or set the X coordinate in the current window
        int32_t GetLocalY() const;
        void SetLocalY(int32_t y);

        // Gets or sets the width of the pressed area. For precisely pointed input events such as a mouse, it is 0
        int32_t GetWidth() const;
        void SetWidth(int32_t width);

        // Gets or sets the height of the pressed area. For precisely pointed input events such as a mouse, it is 0
        int32_t GetHeight() const;
        void SetHeight(int32_t height);

        // Get or set the pressed pressure value.
        // For unsupported devices, use the default value 0
        int32_t GetPressure() const;
        void SetPressure(int32_t pressure);

        // Get or set the device id, the default value is 0, which means non-real device
        int32_t GetDeviceId() const;
        void SetDeviceId(int32_t deviceId);

    private:
        int32_t pointerId_ = 0;
        int32_t downTime_ = 0;
        bool pressed_ = false;
        int32_t globalX_ = 0;
        int32_t globalY_ = 0;
        int32_t localX_ = 0;
        int32_t localY_ = 0;
        int32_t width_ = 0;
        int32_t height_ = 0;
        int32_t pressure_ = 0;
        int32_t deviceId_ = 0;
    };

public:
    static const int32_t POINTER_ACTION_CANCEL = 1;
    static const int32_t POINTER_ACTION_DOWN = 2;
    static const int32_t POINTER_ACTION_MOVE = 3;
    static const int32_t POINTER_ACTION_UP = 4;

    static const int32_t SOURCE_TYPE_UNKNOWN = 0;
    static const int32_t SOURCE_TYPE_MOUSE = 1;
    static const int32_t SOURCE_TYPE_TOUCHSCREEN = 2;
    static const int32_t SOURCE_TYPE_TOUCHPAD = 3;

    enum AxisType {
        // Unknown axis type, generally used to indicate the initial value
        AXIS_TYPE_UNKNOWN,
        // Scroll axis, generally used to represent the UI element where the mouse is scrolled
        AXIS_TYPE_SCROLL_VERTICAL,
        AXIS_TYPE_SCROLL_HORIZONTAL,
        // Pinch axis, generally used to represent the UI element where the mouse is zoomed
        AXIS_TYPE_PINCH,
        // This is for programing usage only, indicating the number of axis types defined.
        AXIS_TYPE_MAX
    };

    virtual ~PointerEvent();
    static std::shared_ptr<PointerEvent> Create();

    int32_t GetSourceType() const;
    void SetSourceType(int32_t sourceType);

    int32_t GetPointerAction() const;
    void SetPointerAction(int32_t pointerAction);

    int32_t GetPointerId() const;
    void SetPointerId(int32_t pointerId);

    bool GetPointerItem(int32_t pointerId, PointerItem &pointerItem);
    void AddPointerItem(PointerItem &pointerItem);
    void RemovePointerItem(int32_t pointerId);
    void UpdatePointerItem(int32_t pointerId, PointerItem &pointerItem);

    std::vector<int32_t> GetPointersIdList() const;

protected:
    explicit PointerEvent(int32_t eventType);

private:
    bool skipInspection_ = false;
    int32_t pointerId_ = 0;
    std::list<PointerItem> pointers_;
    std::set<int32_t> pressedButtons_;
    int32_t sourceType_ = 0;
    int32_t pointerAction_ = 0;
    int32_t buttonId_ = -1;
    int32_t axes_ = 0;
    std::array<double, AXIS_TYPE_MAX> axisValues_;
    std::vector<int32_t> pressedKeys_;
};
} // namespace MMI
} // namespace OHOS
#endif // POINTER_EVENT_H