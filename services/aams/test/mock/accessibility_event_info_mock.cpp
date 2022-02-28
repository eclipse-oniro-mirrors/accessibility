/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_event_info.h"
#include "accessibility_operator.h"

namespace OHOS {
namespace Accessibility {
bool AccessibilityEventInfo::Marshalling(Parcel &parcel) const
{
    return true;
}

bool AccessibilityMemo::Marshalling(Parcel &parcel) const
{
    return true;
}

EventType AccessibilityEventInfo::GetEventType() const
{
    return eventType_;
}

void AccessibilityEventInfo::SetEventType(const EventType eventType)
{
    eventType_ = eventType;
}

AccessibilityEventInfo::AccessibilityEventInfo(
            int windowId, WindowUpdateType windowChangeTypes)
{
    eventType_ = TYPE_WINDOW_UPDATE;
}

AccessibilityEventInfo::AccessibilityEventInfo(EventType eventType)
{
    eventType_ = eventType;
}

void AccessibilityEventInfo::SetTimeStamp(const long long eventTime)
{
    timeStamp_ = eventTime;
}

int AccessibilityMemo::GetWindowId() const
{
    return windowId_;
}

void AccessibilityMemo::SetChannelId(const int channelId)
{
    channelId_ = channelId;
}

WindowUpdateType AccessibilityEventInfo::GetWindowChangeTypes() const
{
    return windowChangeTypes_;
}

WindowsContentChangeTypes AccessibilityEventInfo::GetWindowContentChangeTypes() const
{
    return windowContentChangeTypes_;
}

std::string AccessibilityEventInfo::GetBundleName() const
{
    return bundleName_;
}

void AccessibilityMemo::SetWindowId(const int windowId)
{
    windowId_ = windowId;
}

void AccessibilityEventInfo::SetGestureType(const GestureType gestureType)
{
    gestureType_ = gestureType;
}

GestureType AccessibilityEventInfo::GetGestureType() const
{
    return gestureType_;
}
} // namespace Accessibility
} // namespace OHOS