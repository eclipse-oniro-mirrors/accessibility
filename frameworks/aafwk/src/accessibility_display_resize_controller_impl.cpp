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

#include "accessibility_display_resize_controller_impl.h"
#include "hilog_wrapper.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
DisplayResizeControllerImpl::DisplayResizeControllerImpl(const int32_t displayId,
    const std::shared_ptr<AccessibleAbilityChannelClient> &client)
    : displayId_(displayId), channelClient_(client)
{
    HILOG_DEBUG("start.");
}

void DisplayResizeControllerImpl::AddListener(std::shared_ptr<DisplayResizeListener> &listener)
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
}

void DisplayResizeControllerImpl::DeleteListener(std::shared_ptr<DisplayResizeListener> &listener)
{
    HILOG_DEBUG("start.");

    for (auto it = displayResizeListeners_.begin(); it != displayResizeListeners_.end(); ++it) {
        if (*it == listener) {
            displayResizeListeners_.erase(it);
            return;
        }
    }
}

float DisplayResizeControllerImpl::GetCenterX()
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->GetDisplayResizeCenterX(displayId_);
    } else {
        return 1.0f;
    }
}

float DisplayResizeControllerImpl::GetCenterY()
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->GetDisplayResizeCenterY(displayId_);
    } else {
        return 1.0f;
    }
}

Rect DisplayResizeControllerImpl::GetDisplayResizeRect()
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->GetDisplayResizeRect(displayId_);
    } else {
        Rect temp {};
        return temp;
    }
}

float DisplayResizeControllerImpl::GetScale()
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->GetDisplayResizeScale(displayId_);
    } else {
        return 1.0f;
    }
}

bool DisplayResizeControllerImpl::Reset(bool isShouldAnimate)
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->ResetDisplayResize(displayId_, isShouldAnimate);
    } else {
        return false;
    }
}

bool DisplayResizeControllerImpl::SetCenter(float centerX, float centerY, bool isShouldAnimate)
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        return channelClient_->SetDisplayResizeScaleAndCenter(displayId_, 1.0, centerX, centerY, isShouldAnimate);
    } else {
        return false;
    }
}

bool DisplayResizeControllerImpl::SetScale(float scale, bool isShouldAnimate)
{
    HILOG_DEBUG("start.");
    float invalidValue = -1;
    if (channelClient_) {
        return channelClient_->SetDisplayResizeScaleAndCenter(displayId_, scale,
            invalidValue, invalidValue, isShouldAnimate);
    } else {
        return false;
    }
}

void DisplayResizeControllerImpl::DispatchOnDisplayResized(const Rect &rect,
    const float scale, const float centerX, const float centerY)
{
    HILOG_DEBUG("start.");
    for (auto listener : displayResizeListeners_) {
        if (listener) {
            listener->OnDisplayResized(rect, scale, centerX, centerY);
        }
    }
}
} // namespace Accessibility
} // namespace OHOS