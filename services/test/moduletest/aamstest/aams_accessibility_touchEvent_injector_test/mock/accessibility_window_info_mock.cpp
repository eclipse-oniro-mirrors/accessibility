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
#include "accessibility_operator.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
/* AccessibleAction     Parcel struct                   */
bool AccessibilityWindowInfo::ReadFromParcel(Parcel &parcel)
{
    int windowType;
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
    std::shared_ptr<Rect> boundsInScreen(parcel.ReadParcelable<Rect>());
    if (!boundsInScreen) {
        HILOG_ERROR("ReadParcelable boundsInScreen failed.");
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
    return accessibilityWindow;
}

std::string AccessibilityWindowInfo::GetWindowTitle() const
{
    return windowTitle_;
}

void AccessibilityWindowInfo::SetWindowTitle(std::string &title)
{
    windowTitle_ = title;
}

WindowType AccessibilityWindowInfo::GetWindowType() const
{
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const WindowType type)
{
    windowType_ = type;
}

int AccessibilityWindowInfo::GetWindowLayer() const
{
    return windowLayer_;
}

void AccessibilityWindowInfo::SetWindowLayer(const int layer)
{
    windowLayer_ = layer;
}

void AccessibilityWindowInfo::SetAnchorId(const int anchorId)
{
    anchorId_ = anchorId;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetParent()
{
    AccessibilityWindowInfo win {};

    return win;
}

void AccessibilityWindowInfo::SetParentId(const int parentId)
{
    parentId_ = parentId;
}

int AccessibilityWindowInfo::GetParentId() const
{
    return parentId_;
}

std::vector<int>  AccessibilityWindowInfo::GetChildIds() const
{
    return childIds_;
}

int AccessibilityWindowInfo::GetAnchorId() const
{
    return anchorId_;
}

int AccessibilityWindowInfo::GetWindowId() const
{
    return windowId_;
}

void AccessibilityWindowInfo::SetWindowId(const int id)
{
    windowId_ = id;
}

void AccessibilityWindowInfo::SetChannelId(const int channelId)
{
    channelId_ = channelId;
}

int AccessibilityWindowInfo::GetChannelId() const
{
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
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{
    active_ = active;
}

bool AccessibilityWindowInfo::IsFocused() const
{
    return true;
}

void AccessibilityWindowInfo::SetFocused(bool focused)
{
    focused_ = focused;
}

bool AccessibilityWindowInfo::IsAccessibilityFocused() const
{
    return accessibilityFocused_;
}

void AccessibilityWindowInfo::SetAccessibilityFocused(const bool accessibilityfocused)
{
    accessibilityFocused_ = accessibilityfocused;
}

int AccessibilityWindowInfo::GetChildNum() const
{
    return childNum_;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetChild(const int index)
{
    AccessibilityWindowInfo win {};
    return win;
}

void AccessibilityWindowInfo::AddChild(const int childId)
{
    childIds_.push_back(childId);
    childNum_++;
}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{}
} // namespace Accessibility
} // namespace OHOS