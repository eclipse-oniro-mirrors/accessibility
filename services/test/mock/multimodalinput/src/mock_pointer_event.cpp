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

#include "mock_pointer_event.h"

namespace OHOS {
namespace MMI {
PointerEvent::PointerItem::PointerItem()
{}

PointerEvent::PointerItem::~PointerItem()
{}

int32_t PointerEvent::PointerItem::GetPointerId() const
{
    return pointerId_;
}

void PointerEvent::PointerItem::SetPointerId(int32_t pointerId)
{
    pointerId_ = pointerId;
}

int64_t PointerEvent::PointerItem::GetDownTime() const
{
    return downTime_;
}

void PointerEvent::PointerItem::SetDownTime(int64_t downTime)
{
    downTime_ = downTime;
}

bool PointerEvent::PointerItem::IsPressed() const
{
    return pressed_;
}

void PointerEvent::PointerItem::SetPressed(bool pressed)
{
    pressed_ = pressed;
}

int32_t PointerEvent::PointerItem::GetDisplayX() const
{
    return displayX_;
}

void PointerEvent::PointerItem::SetDisplayX(int32_t x)
{
    displayX_ = x;
}

int32_t PointerEvent::PointerItem::GetDisplayY() const
{
    return displayY_;
}

void PointerEvent::PointerItem::SetDisplayY(int32_t y)
{
    displayY_ = y;
}

int32_t PointerEvent::PointerItem::GetWindowX() const
{
    return windowX_;
}

void PointerEvent::PointerItem::SetWindowX(int32_t x)
{
    windowX_ = x;
}

int32_t PointerEvent::PointerItem::GetWindowY() const
{
    return windowY_;
}

void PointerEvent::PointerItem::SetWindowY(int32_t y)
{
    windowY_ = y;
}

int32_t PointerEvent::PointerItem::GetWidth() const
{
    return width_;
}

void PointerEvent::PointerItem::SetWidth(int32_t width)
{
    width_ = width;
}

int32_t PointerEvent::PointerItem::GetHeight() const
{
    return height_;
}

void PointerEvent::PointerItem::SetHeight(int32_t height)
{
    height_ = height;
}

double PointerEvent::PointerItem::GetPressure() const
{
    return pressure_;
}

void PointerEvent::PointerItem::SetPressure(double pressure)
{
    pressure_ = pressure;
}

int32_t PointerEvent::PointerItem::GetDeviceId() const
{
    return deviceId_;
}

void PointerEvent::PointerItem::SetDeviceId(int32_t deviceId)
{
    deviceId_ = deviceId;
}

PointerEvent::PointerEvent(int32_t eventType) : InputEvent(eventType)
{}

PointerEvent::PointerEvent(const PointerEvent& other)
    : InputEvent(other),
      pointerId_(other.pointerId_),
      pointers_(other.pointers_),
      pressedButtons_(other.pressedButtons_),
      sourceType_(other.sourceType_),
      pointerAction_(other.pointerAction_),
      buttonId_(other.buttonId_),
      axes_(other.axes_),
      axisValues_(other.axisValues_),
      pressedKeys_(other.pressedKeys_)
{}

PointerEvent::~PointerEvent()
{
    pointers_.clear();
    pressedButtons_.clear();
    pressedKeys_.clear();
}

std::shared_ptr<PointerEvent> PointerEvent::Create()
{
    return std::shared_ptr<PointerEvent>(new PointerEvent(InputEvent::EVENT_TYPE_POINTER));
}

int32_t PointerEvent::GetSourceType() const
{
    return sourceType_;
}

void PointerEvent::SetSourceType(int32_t sourceType)
{
    sourceType_ = sourceType;
}

int32_t PointerEvent::GetPointerAction() const
{
    return pointerAction_;
}

void PointerEvent::SetPointerAction(int32_t pointerAction)
{
    pointerAction_ = pointerAction;
}

int32_t PointerEvent::GetPointerId() const
{
    return pointerId_;
}

void PointerEvent::SetPointerId(int32_t pointerId)
{
    pointerId_ = pointerId;
}

bool PointerEvent::GetPointerItem(int32_t pointerId, PointerItem& pointerItem)
{
    for (auto& item : pointers_) {
        if (item.GetPointerId() == pointerId) {
            pointerItem = item;
            return true;
        }
    }
    return false;
}

void PointerEvent::RemovePointerItem(int32_t pointerId)
{
    for (auto it = pointers_.begin(); it != pointers_.end(); it++) {
        if (it->GetPointerId() == pointerId) {
            pointers_.erase(it);
            break;
        }
    }
}

void PointerEvent::AddPointerItem(PointerItem& pointerItem)
{
    pointers_.push_back(pointerItem);
}

std::vector<int32_t> PointerEvent::GetPointerIds() const
{
    std::vector<int32_t> pointerIdList;

    for (auto& item : pointers_) {
        pointerIdList.push_back(item.GetPointerId());
    }

    return pointerIdList;
}

void PointerEvent::Reset()
{
}

void PointerEvent::SetButtonId(int32_t buttonId)
{
    buttonId_ = buttonId;
}

void PointerEvent::SetButtonPressed(int32_t buttonId)
{
    pressedButtons_.insert(buttonId);
}

void PointerEvent::UpdatePointerItem(int32_t pointerId, PointerItem &pointerItem)
{
    for (auto &item : pointers_) {
        if (item.GetPointerId() == pointerId) {
            item = pointerItem;
            return;
        }
    }
    pointers_.push_back(pointerItem);
}
} // namespace MMI
} // namespace OHOS