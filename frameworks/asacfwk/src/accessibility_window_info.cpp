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

#include "accessibility_window_info.h"
#include "accessibility_operator.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

/* AccessibleAction     Parcel struct                   */
bool AccessibilityWindowInfo::ReadFromParcel(Parcel &parcel)
{
    int windowType = TYPE_WINDOW_INVALID;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowType);
    windowType_ = static_cast<WindowType>(windowType);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowLayer_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, windowTitle_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, &childIds_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, anchorId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childNum_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, channelId_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, active_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    sptr<Rect> boundsInScreen = parcel.ReadStrongParcelable<Rect>();
    if (!boundsInScreen) {
        HILOG_ERROR("ReadStrongParcelable boundsInScreen failed.");
        return false;
    }
    boundsInScreen_ = *boundsInScreen;

    return true;
}

bool AccessibilityWindowInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int>(windowType_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowLayer_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, windowId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, parentId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String, parcel, windowTitle_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32Vector, parcel, childIds_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, anchorId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, childNum_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, channelId_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, active_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, focused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, accessibilityFocused_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &boundsInScreen_);

    return true;
};

AccessibilityWindowInfo* AccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    AccessibilityWindowInfo *accessibilityWindow = new AccessibilityWindowInfo();
    if (!accessibilityWindow->ReadFromParcel(parcel)) {
        delete accessibilityWindow;
        accessibilityWindow = nullptr;
    }
    return accessibilityWindow;
}

std::string AccessibilityWindowInfo::GetWindowTitle() const
{
    HILOG_DEBUG("[%{public}s] windowTitle_[%{public}s]", __func__, windowTitle_.c_str());
    return windowTitle_;
}

void AccessibilityWindowInfo::SetWindowTitle(std::string &title)
{
    windowTitle_ = title;
    HILOG_DEBUG("[%{public}s] windowTitle_[%{public}s]", __func__, windowTitle_.c_str());
}

WindowType AccessibilityWindowInfo::GetWindowType() const
{
    HILOG_DEBUG("[%{public}s] windowType_[%{public}d]", __func__, windowType_);
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const WindowType type)
{
    windowType_ = type;
    HILOG_DEBUG("[%{public}s] windowType_[%{public}d]", __func__, windowType_);
}

int AccessibilityWindowInfo::GetWindowLayer() const
{
    HILOG_DEBUG("[%{public}s] windowLayer_[%{public}d]", __func__, windowLayer_);
    return windowLayer_;
}

void AccessibilityWindowInfo::SetWindowLayer(const int layer)
{
    windowLayer_ = layer;
    HILOG_DEBUG("[%{public}s] windowLayer_[%{public}d]", __func__, windowLayer_);
}

bool AccessibilityWindowInfo::GetRootAccessibilityInfo(AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d]",
        __func__, channelId_, windowId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();

    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_, windowId_, -1, 0, elementInfos);
        if (elementInfos.empty()) {
            result = false;
        } else {
            elementInfo = elementInfos.front();
        }
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

void AccessibilityWindowInfo::SetAnchorId(const int anchorId)
{
    anchorId_ = anchorId;
    HILOG_DEBUG("[%{public}s] anchorId_[%{public}d]", __func__, anchorId_);
}

bool AccessibilityWindowInfo::GetAnchor(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("GetAnchor of windowInfo is not support!");
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], windowId_[%{public}d], anchorId_[%{public}d]",
        __func__, channelId_, windowId_, anchorId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();

    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    if (instance != nullptr) {
        result = instance->SearchElementInfosByAccessibilityId(channelId_, windowId_, anchorId_, 0, elementInfos);
        if (elementInfos.empty()) {
            result = false;
        } else {
            elementInfo = elementInfos.front();
        }
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return result;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetParent()
{
    HILOG_DEBUG("GetParent of windowInfo is not support!");
    AccessibilityWindowInfo win {};
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], parentId_[%{public}d]",
        __func__, channelId_, parentId_);
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    if (instance != nullptr) {
        for (auto window : instance->GetWindows(channelId_)) {
            if (window.GetWindowId() == parentId_) {
                return window;
            }
        }
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return win;
}

void AccessibilityWindowInfo::SetParentId(const int parentId)
{
    parentId_ = parentId;
    HILOG_DEBUG("[%{public}s] parentId_[%{public}d]", __func__, parentId_);
}

int AccessibilityWindowInfo::GetParentId() const
{
    HILOG_DEBUG("[%{public}s] parentId_[%{public}d]", __func__, parentId_);
    return parentId_;
}

std::vector<int>  AccessibilityWindowInfo::GetChildIds() const
{
    return childIds_;
}

int AccessibilityWindowInfo::GetAnchorId() const
{
    HILOG_DEBUG("[%{public}s] anchorId_[%{public}d]", __func__, anchorId_);
    return anchorId_;
}
int AccessibilityWindowInfo::GetWindowId() const
{
    HILOG_DEBUG("[%{public}s] windowId_[%{public}d]", __func__, windowId_);
    return windowId_;
}

void AccessibilityWindowInfo::SetWindowId(const int id)
{
    windowId_ = id;
    HILOG_DEBUG("[%{public}s] windowId_[%{public}d]", __func__, windowId_);
}

void AccessibilityWindowInfo::SetChannelId(const int channelId)
{
    channelId_ = channelId;
    HILOG_DEBUG("[%{public}s] channelId_[%{public}d]", __func__, channelId_);
}

int AccessibilityWindowInfo::GetChannelId() const
{
    HILOG_DEBUG("[%{public}s] channelId_[%{public}d]", __func__, channelId_);
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
    HILOG_DEBUG("[%{public}s] active_[%{public}d]", __func__, active_);
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{
    active_ = active;
    HILOG_DEBUG("[%{public}s] active_[%{public}d]", __func__, active_);
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

int AccessibilityWindowInfo::GetChildNum() const
{
    HILOG_DEBUG("[%{public}s] childNum_[%{public}d]", __func__, childNum_);
    return childNum_;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetChild(const int index)
{
    HILOG_DEBUG("GetChild of windowInfo is not support!");
    AccessibilityWindowInfo win {};
    HILOG_INFO("[%{public}s] called] channelId_[%{public}d], childNum_[%{public}d], index[%{public}d]",
        __func__, channelId_, childNum_, index);
    if (index >= childNum_ || index < 0) {
        HILOG_ERROR("[%{public}s] called] index[%{public}d] is invalid", __func__, index);
        return win;
    }
    AccessibilityOperator *instance = &AccessibilityOperator::GetInstance();
    if (instance != nullptr) {
        for (auto window : instance->GetWindows(channelId_)) {
        if (window.GetWindowId() == childIds_[index]) {
            return window;
        }
    }
    } else {
        HILOG_INFO("[%{public}s] called] Can't get AccessibilityOperator instance", __func__);
    }
    return win;
}

void AccessibilityWindowInfo::AddChild(const int childId)
{
    childIds_.push_back(childId);
    childNum_++;
}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{

}

} // namespace Accessibility
} // namespace OHOS