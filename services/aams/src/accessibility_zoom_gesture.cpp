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

#include "accessibility_zoom_gesture.h"
#include <complex>

namespace OHOS {
namespace Accessibility {
/*
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

int64_t AccessibilityZoomGesture::GetSysTimeout() const
{
    return 0;
}

bool AccessibilityZoomGesture::ValidDown(MMI::PointerEvent &event)
{
    MMI::PointerEvent::PointerItem currentPointerItem;
    MMI::PointerEvent::PointerItem lastPointerItem;
    if (!downCount_) {
        pLastDown_ = std::make_shared<MMI::PointerEvent>(event);
    } else {
        if (!pLastDown_) {
            HILOG_ERROR("pLastDown_ is null");
            return false;
        }
        if ((event.GetActionTime() - pLastDown_->GetActionTime()) > timeout_) {
            Reset(event);
            return false;
        }
        if (!event.GetPointerItem(event.GetPointerId(), currentPointerItem)) {
            HILOG_ERROR("get current GetPointerItem(%d) failed", event.GetPointerId());
            return false;
        }
        if (!pLastDown_->GetPointerItem(pLastDown_->GetPointerId(), lastPointerItem)) {
            HILOG_ERROR("get last GetPointerItem(%d) failed", pLastDown_->GetPointerId());
            return false;
        }
        int dist = std::sqrt(std::pow(currentPointerItem.GetGlobalX() - lastPointerItem.GetGlobalX(), 2)
            + std::pow(currentPointerItem.GetGlobalY() - lastPointerItem.GetLocalY(), 2));
        if (dist > distance_) {
            Reset(event);
            return false;
        }
        pLastDown_ = std::make_shared<MMI::PointerEvent>(event);
    }
    downCount_++;
    return true;
}

bool AccessibilityZoomGesture::Triple(MMI::PointerEvent &event)
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

void AccessibilityZoomGesture::Reset(const MMI::PointerEvent &event)
{
    upCount_ = 0;
    downCount_ = 1;
    pLastDown_ = std::make_shared<MMI::PointerEvent>(event);
}

void AccessibilityZoomGesture::Clear()
{
    upCount_ = 0;
    downCount_ = 0;
}
} // namespace Accessibility
} // namespace OHOS
