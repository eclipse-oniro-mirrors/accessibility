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

#include "display_resize_controller.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
DisplayResizeController::DisplayResizeController(uint32_t channelId, uint32_t displayId)
{
    HILOG_DEBUG("start.");
    channelId_ = channelId;
    displayId_ = displayId;
}

void DisplayResizeController::AddListener(std::shared_ptr<DisplayResizeListener>& listener)
{
    HILOG_DEBUG("start.");

    if (!listener) {
        return;
    }

    for (auto it = displayResizeListeners_.begin(); it != displayResizeListeners_.end(); ++it) {
        if (*it == listener) {
            return;
        }
    }
    displayResizeListeners_.push_back(listener);

    return;
}

bool DisplayResizeController::DeleteListener(std::shared_ptr<DisplayResizeListener>& listener)
{
    HILOG_DEBUG("start.");

    for (auto it = displayResizeListeners_.begin(); it != displayResizeListeners_.end(); ++it) {
        if (*it == listener) {
            displayResizeListeners_.erase(it);
            return true;
        }
    }

    return false;
}

float DisplayResizeController::GetCenterX()
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().GetDisplayResizeCenterX(channelId_, displayId_);
}

float DisplayResizeController::GetCenterY()
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().GetDisplayResizeCenterY(channelId_, displayId_);
}

Rect DisplayResizeController::GetDisplayResizeRect()
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().GetDisplayResizeRect(channelId_, displayId_);
}

float DisplayResizeController::GetScale()
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().GetDisplayResizeScale(channelId_, displayId_);
}

bool DisplayResizeController::Reset(bool isShouldAnimate)
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().ResetDisplayResize(channelId_, displayId_, isShouldAnimate);
}

bool DisplayResizeController::SetCenter(float centerX, float centerY, bool isShouldAnimate)
{
    HILOG_DEBUG("start.");
    return AccessibilityOperator::GetInstance().SetDisplayResizeScaleAndCenter(
        channelId_, displayId_, 1.0, centerX, centerY, isShouldAnimate);
}

bool DisplayResizeController::SetScale(float scale, bool isShouldAnimate)
{
    HILOG_DEBUG("start.");

    float invalidValue = -1;
    return AccessibilityOperator::GetInstance().SetDisplayResizeScaleAndCenter(
        channelId_, displayId_, scale, invalidValue, invalidValue, isShouldAnimate);
}

void DisplayResizeController::DispatchOnDisplayResized(const Rect& rect,
    const float scale, const float centerX, const float centerY)
{
    HILOG_DEBUG("start.");
    for (auto listener : displayResizeListeners_) {
        listener->OnDisplayResized(*this, rect, scale, centerX, centerY);
    }
}
} // namespace Accessibility
} // namespace OHOS