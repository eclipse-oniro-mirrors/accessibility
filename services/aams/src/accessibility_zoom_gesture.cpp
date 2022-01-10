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

#include "accessibility_zoom_gesture.h"
#include <cmath>

namespace OHOS {
namespace Accessibility {
/**
 * Triple: DOWN UP DOWN UP DOWN
 */
const int DOWN_COUNT = 3;
const int UP_COUNT = 2;

AccessibilityZoomGesture::AccessibilityZoomGesture()
{
    Initialize();
}

AccessibilityZoomGesture::~AccessibilityZoomGesture()
{
}

void AccessibilityZoomGesture::Initialize()
{
    distance_ = GetSysDistance();
    timeout_ = GetSysTimeout();
}

int AccessibilityZoomGesture::GetSysDistance() const
{
    return 0;
}

int AccessibilityZoomGesture::GetSysTimeout() const
{
    return 0;
}

bool AccessibilityZoomGesture::ValidDown(TouchEvent &event)
{
    if (downCount_ == 0) {
        lastDown_ = event;
    } else {
        if (event.GetMultimodalEvent()->GetOccurredTime() - lastDown_.GetMultimodalEvent()->GetOccurredTime() > timeout_) {
            Reset(event);
            return false;
        }
        int dist = sqrt(pow(event.GetPointerScreenPosition(event.GetIndex()).GetX() - lastDown_.GetPointerScreenPosition(lastDown_.GetIndex()).GetX(), 2) + pow(event.GetPointerScreenPosition(event.GetIndex()).GetY() - lastDown_.GetPointerScreenPosition(lastDown_.GetIndex()).GetY(), 2));
        if (dist > distance_) {
            Reset(event);
            return false;
        }
        lastDown_ = event;
    }
    downCount_ ++;
    return true;
}

bool AccessibilityZoomGesture::Triple(TouchEvent &event)
{
    if (ValidDown(event) && downCount_ >= DOWN_COUNT && upCount_ >= UP_COUNT) {
        return true;
    }
    return false;
}

void AccessibilityZoomGesture::Up()
{
    upCount_ ++;
}

void AccessibilityZoomGesture::Reset(const TouchEvent &event)
{
    upCount_ = 0;
    downCount_ = 1;
    lastDown_ = event;
}

void AccessibilityZoomGesture::Clear()
{
    upCount_ = 0;
    downCount_ = 0;
}
}  // namespace accessibility
}  // namespace OHOS
