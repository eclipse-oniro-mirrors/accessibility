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
    (void)parcel;
    return true;
}

bool AccessibilityWindowInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

AccessibilityWindowInfo* AccessibilityWindowInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    return nullptr;
}

std::string AccessibilityWindowInfo::GetWindowTitle() const
{
    return windowTitle_;
}

void AccessibilityWindowInfo::SetWindowTitle(std::string &title)
{
    (void)title;
}

WindowType AccessibilityWindowInfo::GetWindowType() const
{
    return windowType_;
}

void AccessibilityWindowInfo::SetWindowType(const WindowType type)
{
    (void)type;
}

int AccessibilityWindowInfo::GetWindowLayer() const
{
    return windowLayer_;
}

void AccessibilityWindowInfo::SetWindowLayer(const int layer)
{
    (void)layer;
}

bool AccessibilityWindowInfo::GetRootAccessibilityInfo(AccessibilityElementInfo &elementInfo)
{
    (void)elementInfo;
    return true;
}

void AccessibilityWindowInfo::SetAnchorId(const int anchorId)
{}

bool AccessibilityWindowInfo::GetAnchor(AccessibilityElementInfo &elementInfo)
{
    (void)elementInfo;
    return true;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetParent()
{
    AccessibilityWindowInfo accessibilityWindow {};
    return accessibilityWindow;
}

void AccessibilityWindowInfo::SetParentId(const int parentId)
{
    (void)parentId;
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
    (void)id;
}

void AccessibilityWindowInfo::SetChannelId(const int channelId)
{
    (void)channelId;
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
    (void)bounds;
}

bool AccessibilityWindowInfo::IsActive() const
{
    return active_;
}

void AccessibilityWindowInfo::SetActive(bool active)
{
    (void)active;
}

bool AccessibilityWindowInfo::IsFocused() const
{
    return focused_;
}

void AccessibilityWindowInfo::SetFocused(bool focused)
{
    (void)focused;
}

bool AccessibilityWindowInfo::IsAccessibilityFocused() const
{
    return accessibilityFocused_;
}

void AccessibilityWindowInfo::SetAccessibilityFocused(const bool accessibilityfocused)
{
    (void)accessibilityfocused;
}

int AccessibilityWindowInfo::GetChildNum() const
{
    return childNum_;
}

AccessibilityWindowInfo AccessibilityWindowInfo::GetChild(const int index)
{
    (void)index;
    AccessibilityWindowInfo accessibilityWindow {};
    return accessibilityWindow;
}

void AccessibilityWindowInfo::AddChild(const int childId)
{
    (void)childId;
}

AccessibilityWindowInfo::AccessibilityWindowInfo()
{}
} // namespace Accessibility
} // namespace OHOS