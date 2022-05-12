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

#include "accessibility_zoom_handler.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
const float DEFAULT_SCALE = 2.0f;

AccessibilityZoomHandler::AccessibilityZoomHandler(Rosen::DisplayId displayId)
{
    HILOG_DEBUG();

    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    auto display = displayMgr.GetDefaultDisplay();
    if (!display) {
        HILOG_ERROR("get display is nullptr");
        return;
    }
    int32_t displayWith = display->GetWidth();
    int32_t displayHeight = display->GetHeight();
    displayCenterX_ = displayWith / 2;
    displayCenterY_ = displayHeight / 2;
}

void AccessibilityZoomHandler::OnZoomIn(int32_t centerX, int32_t centerY)
{
    HILOG_DEBUG("centerX:%{public}d, centerY:%{public}d.", centerX, centerY);

    //setToWMS(DEFAULT_SCALE, centerX, centerY);
    // Update the following information based on the return value of wms.
    // currentCenterX_ = xxx;
    // currentCenterY_ = xxx;
    currentScale_ = DEFAULT_SCALE; // Temp deal
}

void AccessibilityZoomHandler::OnZoomOut()
{
    HILOG_DEBUG();

    //setToWMS(1, displayCenterX_, displayCenterY_);
    // Update the following information based on the return value of wms.
    // currentCenterX_ = xxx;
    // currentCenterY_ = xxx;
    // currentScale_ = xxx;
}

void AccessibilityZoomHandler::OnScroll(float offsetX, float offsetY)
{
    HILOG_DEBUG("offsetX:%{public}f, offsetY:%{public}f.", offsetX, offsetY);

    float newCenterX = (currentCenterX_ * currentScale_ - offsetX) / currentScale_;
    float newCenterY = (currentCenterY_ * currentScale_ - offsetY) / currentScale_;

    //setToWMS(DEFAULT_SCALE, centerX, centerY);
    // Update the following information based on the return value of wms.
    currentCenterX_ = newCenterX; // Temp deal
    currentCenterY_ = newCenterY; // Temp deal
    // currentScale_ = xxx;
}

void AccessibilityZoomHandler::OnScale(float scaleRatio, float focusX, float focusY)
{
    HILOG_DEBUG("scaleRatio:%{public}f, focusX:%{public}f, focusY:%{public}f.", scaleRatio, focusX, focusY);

    float offsetX = displayCenterX_ - currentCenterX_ * currentScale_;
    float offsetY = displayCenterY_ - currentCenterY_ * currentScale_;

    float originalFocusX = (focusX - offsetX) / currentScale_;
    float originalFocusY = (focusY - offsetY) / currentScale_;

    float centerOffsetX = (currentCenterX_ - originalFocusX) / scaleRatio;
    float centerOffsetY = (currentCenterY_ - originalFocusY) / scaleRatio;
    float newCenterX = originalFocusX + centerOffsetX;
    float newCenterY = originalFocusY + centerOffsetY;
    float newScale = currentScale_ * scaleRatio;
    //setToWMS(newScale, newCenterX, newCenterY);
    // Update the following information based on the return value of wms.
    currentCenterX_ = newCenterX; // Temp deal
    currentCenterY_ = newCenterY; // Temp deal
    currentScale_ = newScale; // Temp deal
}
} // namespace Accessibility
} // namespace OHOS
