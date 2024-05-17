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

#include "accessibility_window_info.h"

#include <cinttypes>

#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityWindowType AccessibilityWindowInfo::GetAccessibilityWindowType() const
{
    HILOG_DEBUG("accessibilityWindowType_[%{public}d]", accessibilityWindowType_);
    return accessibilityWindowType_;
}

void AccessibilityWindowInfo::SetAccessibilityWindowType(const AccessibilityWindowType type)
{
    accessibilityWindowType_ = type;
    HILOG_DEBUG("accessibilityWindowType_[%{public}d]", accessibilityWindowType_);
}

int32_t AccessibilityWindowInfo::GetWindowLayer() const
{
    HILOG_DEBUG("window(%{public}d)Layer_[%{public}d]", windowId_, windowLayer_);
    return windowLayer_;
}

void AccessibilityWindowInfo::SetWindowLayer(const int32_t layer)
{
    windowLayer_ = layer;
    HILOG_DEBUG("window(%{public}d)Layer[%{public}d]", windowId_, windowLayer_);
}

int32_t AccessibilityWindowInfo::GetWindowId() const
{
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
    return windowId_;
}

void AccessibilityWindowInfo::SetWindowId(const int32_t id)
{
    windowId_ = id;
    HILOG_DEBUG("windowId_[%{public}d]", windowId_);
}

const Rect &AccessibilityWindowInfo::GetRectInScreen() const
{
    return boundsInScreen_;
}

void AccessibilityWindowInfo::SetRectInScreen(const Rect &bounds)
{
    boundsInScreen_.SetLeftTopScreenPostion(const_cast<Rect &>(bounds).GetLeftTopXScreenPostion(),
        const_cast<Rect &>(bounds).GetLeftTopYScreenPostion());
    boundsInScreen_.SetRightBottomScreenPostion(const_cast<Rect &>(bounds).GetRightBottomXScreenPostion(),
        const_cast<Rect &>(bounds).GetRightBottomYScreenPostion());
}

bool AccessibilityWindowInfo::IsActive() const
{
    HILOG_DEBUG("active_[%{public}d] windowId_[%{public}d]", active_, windowId_);
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{
    active_ = active;
    HILOG_DEBUG("active_[%{public}d] windowId_[%{public}d]", active_, windowId_);
}

bool AccessibilityWindowInfo::IsFocused() const
{
    HILOG_DEBUG("focused_[%{public}d]", focused_);
    return focused_;
}

void AccessibilityWindowInfo::SetFocused(bool focused)
{
    focused_ = focused;
    HILOG_DEBUG("focused_[%{public}d]", focused_);
}

bool AccessibilityWindowInfo::IsAccessibilityFocused() const
{
    HILOG_DEBUG("accessibilityFocused_[%{public}d]", accessibilityFocused_);
    return accessibilityFocused_;
}

void AccessibilityWindowInfo::SetAccessibilityFocused(const bool accessibilityFocused)
{
    accessibilityFocused_ = accessibilityFocused;
    HILOG_DEBUG("accessibilityFocused_[%{public}d]", accessibilityFocused_);
}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{
}

void AccessibilityWindowInfo::SetDisplayId(const uint64_t displayId)
{
    HILOG_DEBUG("displayId:%{public}" PRIu64 "", displayId);
    displayId_ = displayId;
}

uint64_t AccessibilityWindowInfo::GetDisplayId() const
{
    HILOG_DEBUG("displayId:%{public}" PRIu64 "", displayId_);
    return displayId_;
}

uint32_t AccessibilityWindowInfo::GetWindowType() const
{
    HILOG_DEBUG("windowType_[%{public}d]", windowType_);
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const uint32_t type)
{
    windowType_ = type;
    HILOG_DEBUG("windowType_[%{public}d]", windowType_);
}

uint32_t AccessibilityWindowInfo::GetWindowMode() const
{
    HILOG_DEBUG("windowMode_[%{public}d]", windowMode_);
    return windowMode_;
}

void AccessibilityWindowInfo::SetWindowMode(const uint32_t mode)
{
    windowMode_ = mode;
    HILOG_DEBUG("windowMode_[%{public}d]", windowMode_);
}

bool AccessibilityWindowInfo::IsDecorEnable() const
{
    HILOG_DEBUG("isDecorEnable_[%{public}d]", isDecorEnable_);
    return isDecorEnable_;
}

void AccessibilityWindowInfo::SetDecorEnable(const bool isDecorEnable)
{
    isDecorEnable_ = isDecorEnable;
    HILOG_DEBUG("isDecorEnable_[%{public}d]", isDecorEnable_);
}

void AccessibilityWindowInfo::SetInnerWid(const int32_t innerWid)
{
    innerWid_ = innerWid;
    HILOG_DEBUG("innerWid_[%{public}d]", innerWid_);
}

int32_t AccessibilityWindowInfo::GetInnerWid() const
{
    HILOG_DEBUG("innerWid_[%{public}d]", innerWid_);
    return innerWid_;
}

void AccessibilityWindowInfo::SetUiNodeId(const int64_t nodeId)
{
    uiNodeId_ = nodeId;
    HILOG_DEBUG("uiNodeId_[%{public}" PRId64 "]", uiNodeId_);
}

int64_t AccessibilityWindowInfo::GetUiNodeId() const
{
    HILOG_DEBUG("uiNodeId_[%{public}" PRId64 "]", uiNodeId_);
    return uiNodeId_;
}

void AccessibilityWindowInfo::SetScaleVal(const float scaleVal)
{
    scaleVal_ = scaleVal;
    HILOG_DEBUG("set scaleVal_ [%{public}f]", scaleVal_);
}

float AccessibilityWindowInfo::GetScaleVal() const
{
    HILOG_DEBUG("get scaleVal_ [%{public}f]", scaleVal_);
    return scaleVal_;
}

void AccessibilityWindowInfo::SetScaleX(const float scaleX)
{
    scaleX_ = scaleX;
    HILOG_DEBUG("set scaleX_ [%{public}f]", scaleX_);
}

float AccessibilityWindowInfo::GetScaleX() const
{
    HILOG_DEBUG("get scaleX_ [%{public}f]", scaleX_);
    return scaleX_;
}

void AccessibilityWindowInfo::SetScaleY(const float scaleY)
{
    scaleY_ = scaleY;
    HILOG_DEBUG("set scaleY_ [%{public}f]", scaleY_);
}

float AccessibilityWindowInfo::GetScaleY() const
{
    HILOG_DEBUG("get scaleY_ [%{public}f]", scaleY_);
    return scaleY_;
}

void AccessibilityWindowInfo::SetBundleName(const std::string bundleName)
{
    bundleName_ = bundleName;
    HILOG_DEBUG("set bundleName_ [%{public}s]", bundleName_.c_str());
}

std::string AccessibilityWindowInfo::GetBundleName()
{
    HILOG_DEBUG("get bundleName_ [%{public}s]", bundleName_.c_str());
    return bundleName_;
}

void AccessibilityWindowInfo::SetTouchHotAreas(const std::vector<Rect> &touchHotAreas)
{
    touchHotAreas_ = touchHotAreas;
    HILOG_DEBUG("set touchHotAreas_ size(%{public}zu)", touchHotAreas_.size());
}

std::vector<Rect> AccessibilityWindowInfo::GetTouchHotAreas()
{
    HILOG_DEBUG("get touchHotAreas_size(%{public}zu)", touchHotAreas_.size());
    return touchHotAreas_;
}

} // namespace Accessibility
} // namespace OHOS