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
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
std::string AccessibilityWindowInfo::GetWindowTitle() const
{
    HILOG_DEBUG("windowTitle_[%{public}s]", windowTitle_.c_str());
    return windowTitle_;
}

void AccessibilityWindowInfo::SetWindowTitle(const std::string &title)
{
    windowTitle_ = title;
    HILOG_DEBUG("windowTitle_[%{public}s]", windowTitle_.c_str());
}

WindowType AccessibilityWindowInfo::GetWindowType() const
{
    HILOG_DEBUG("windowType_[%{public}d]", windowType_);
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const WindowType type)
{
    windowType_ = type;
    HILOG_DEBUG("windowType_[%{public}d]", windowType_);
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

void AccessibilityWindowInfo::SetAnchorId(const int32_t anchorId)
{
    anchorId_ = anchorId;
    HILOG_DEBUG("anchorId_[%{public}d]", anchorId_);
}

void AccessibilityWindowInfo::SetParentId(const int32_t parentId)
{
    parentId_ = parentId;
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
}

int32_t AccessibilityWindowInfo::GetParentId() const
{
    HILOG_DEBUG("parentId_[%{public}d]", parentId_);
    return parentId_;
}

const std::vector<int32_t> &AccessibilityWindowInfo::GetChildIds() const
{
    return childIds_;
}

int32_t AccessibilityWindowInfo::GetAnchorId() const
{
    HILOG_DEBUG("anchorId_[%{public}d]", anchorId_);
    return anchorId_;
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

void AccessibilityWindowInfo::SetChannelId(const int32_t channelId)
{
    channelId_ = channelId;
    HILOG_DEBUG("channelId_[%{public}d]", channelId_);
}

int32_t AccessibilityWindowInfo::GetChannelId() const
{
    HILOG_DEBUG("channelId_[%{public}d]", channelId_);
    return channelId_;
}

Rect AccessibilityWindowInfo::GetRectInScreen() const
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
    HILOG_DEBUG("active_[%{public}d]", active_);
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{
    active_ = active;
    HILOG_DEBUG("active_[%{public}d]", active_);
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

void AccessibilityWindowInfo::SetAccessibilityFocused(const bool accessibilityfocused)
{
    accessibilityFocused_ = accessibilityfocused;
    HILOG_DEBUG("accessibilityFocused_[%{public}d]", accessibilityFocused_);
}

int32_t AccessibilityWindowInfo::GetChildNum() const
{
    HILOG_DEBUG("childNum_[%{public}d]", childNum_);
    return childNum_;
}

int32_t AccessibilityWindowInfo::GetChildIdByIndex(const int32_t index) const
{
    if (index >= childNum_ || index < 0) {
        HILOG_ERROR("index[%{public}d] is invalid", index);
        return -1;
    }

    return childIds_[index];
}

void AccessibilityWindowInfo::AddChild(const int32_t childId)
{
    childIds_.push_back(childId);
    childNum_++;
}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{
}

void AccessibilityWindowInfo::SetDisplayId(const uint64_t displayId)
{
    HILOG_DEBUG("displayId[%{public}ju]", displayId);
    displayId_ = displayId;
}

uint64_t AccessibilityWindowInfo::GetDisplayId() const
{
    HILOG_DEBUG("displayId[%{public}ju]", displayId_);
    return displayId_;
}
} // namespace Accessibility
} // namespace OHOS