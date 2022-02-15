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

bool AccessibilityWindowInfo::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool AccessibilityWindowInfo::Marshalling(Parcel &parcel) const
{
    return true;
};

AccessibilityWindowInfo* AccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    return nullptr;
}

std::string AccessibilityWindowInfo::GetWindowTitle() const
{
    return windowTitle_;
}

void AccessibilityWindowInfo::SetWindowTitle(std::string &title)
{}

WindowType AccessibilityWindowInfo::GetWindowType() const
{
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const WindowType type)
{}

int AccessibilityWindowInfo::GetWindowLayer() const
{
    return windowLayer_;
}

void AccessibilityWindowInfo::SetWindowLayer(const int layer)
{}

bool AccessibilityWindowInfo::GetRootAccessibilityInfo(AccessibilityElementInfo &elementInfo)
{
    return true;
}

void AccessibilityWindowInfo::SetAnchorId(const int anchorId)
{}

bool AccessibilityWindowInfo::GetAnchor(AccessibilityElementInfo &elementInfo)
{
    return true;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetParent()
{
    AccessibilityWindowInfo accessibilityWindow {};
    return accessibilityWindow;
}

void AccessibilityWindowInfo::SetParentId(const int parentId)
{}

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
{}

void AccessibilityWindowInfo::SetChannelId(const int channelId)
{}

int AccessibilityWindowInfo::GetChannelId() const
{
    return channelId_;
}

Rect AccessibilityWindowInfo::GetRectInScreen() const
{
    return boundsInScreen_;
}

void AccessibilityWindowInfo::SetRectInScreen(const Rect &bounds)
{}

bool AccessibilityWindowInfo::IsActive() const
{
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{}

bool AccessibilityWindowInfo::IsFocused() const
{
    return focused_;
}

void AccessibilityWindowInfo::SetFocused(bool focused)
{}

bool AccessibilityWindowInfo::IsAccessibilityFocused() const
{
    return accessibilityFocused_;
}

void AccessibilityWindowInfo::SetAccessibilityFocused(const bool accessibilityfocused)
{}

int AccessibilityWindowInfo::GetChildNum() const
{
    return childNum_;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetChild(const int index)
{
    AccessibilityWindowInfo accessibilityWindow {};
    return accessibilityWindow;
}

void AccessibilityWindowInfo::AddChild(const int childId)
{}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{}

} // namespace Accessibility
} // namespace OHOS