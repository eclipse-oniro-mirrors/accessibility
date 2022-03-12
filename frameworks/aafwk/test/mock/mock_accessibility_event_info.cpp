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

#include "accessibility_event_info.h"
#include "accessibility_operator.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
bool AccessibilityMemo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool AccessibilityMemo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
}

AccessibilityMemo* AccessibilityMemo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    AccessibilityMemo* accessibilityRecord = new AccessibilityMemo();
    return accessibilityRecord;
}

void AccessibilityMemo::SetChannelId(const int channelId)
{
    (void)channelId;
}

int AccessibilityMemo::GetChannelId() const
{
    return channelId_;
}

void AccessibilityMemo::SetSource(const int componentId)
{
    (void)componentId;
}

bool AccessibilityMemo::GetSource(AccessibilityElementInfo &elementInfo) const
{
    (void)elementInfo;
    return true;
}

int AccessibilityMemo::GetViewId() const
{
    return componentId_;
}

int AccessibilityMemo::GetAccessibilityId() const
{
    return elementId_;
}

void AccessibilityMemo::SetWindowId(const int windowId)
{
    (void)windowId;
}

int AccessibilityMemo::GetWindowId() const
{
    return windowId_;
}

int AccessibilityMemo::GetCurrentIndex() const
{
    return currentIndex_;
}

void AccessibilityMemo::SetCurrentIndex(const int index)
{
    (void)index;
}

int AccessibilityMemo::GetBeginIndex() const
{
    return beginIndex_;
}

void AccessibilityMemo::SetBeginIndex(const int index)
{
    (void)index;
}

int AccessibilityMemo::GetEndIndex() const
{
    return endIndex_;
}

void AccessibilityMemo::SetEndIndex(const int index)
{
    (void)index;
}

std::string AccessibilityMemo::GetComponentType() const
{
    return componentType_;
}

void AccessibilityMemo::SetComponentType(const std::string &className)
{
    (void)className;
}

std::string AccessibilityMemo::GetBeforeText() const
{
    return beforeText_;
}

void AccessibilityMemo::SetBeforeText(const std::string &beforeText)
{
    (void)beforeText;
}

void AccessibilityMemo::AddContent(const std::string &content)
{
    (void)content;
}

std::vector<std::string> AccessibilityMemo::GetContentList() const
{
    return contents_;
}

std::string AccessibilityMemo::GetLatestContent() const
{
    return latestConent_;
}

void AccessibilityMemo::SetLatestContent(const std::string &content)
{
    (void)content;
}

std::string AccessibilityMemo::GetDescription() const
{
    return description_;
}

void AccessibilityMemo::SetDescription(const std::string &contentDescription)
{
    (void)contentDescription;
}

void AccessibilityMemo::SetItemCounts(const int itemCounts)
{
    (void)itemCounts;
}

int AccessibilityMemo::GetItemCounts() const
{
    return itemCounts_;
}

bool AccessibilityEventInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return true;
}

bool AccessibilityEventInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return true;
};

AccessibilityEventInfo *AccessibilityEventInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    AccessibilityEventInfo *accessibilityEventInfo = new AccessibilityEventInfo();
    return accessibilityEventInfo;
}

int AccessibilityEventInfo::GetRecordCount() const
{
    return recordsCount_;
}

void AccessibilityEventInfo::SetRecordCount(const int recordCount)
{
    (void)recordCount;
}

void AccessibilityEventInfo::AddRecord(const AccessibilityEventInfo &record)
{
    (void)record;
}

AccessibilityEventInfo AccessibilityEventInfo::GetRecord(const int index)
{
    return records_[index];
}

std::vector<AccessibilityEventInfo> AccessibilityEventInfo::GetRecords()
{
    return records_;
}

EventType AccessibilityEventInfo::GetEventType() const
{
    return eventType_;
}

WindowsContentChangeTypes AccessibilityEventInfo::GetWindowContentChangeTypes() const
{
    return windowContentChangeTypes_;
}

void AccessibilityEventInfo::SetWindowContentChangeTypes(const WindowsContentChangeTypes changeTypes)
{
    (void)changeTypes;
}

WindowUpdateType AccessibilityEventInfo::GetWindowChangeTypes() const
{
    return windowChangeTypes_;
}

void AccessibilityEventInfo::SetWindowChangeTypes(const WindowUpdateType changeTypes)
{
    (void)changeTypes;
}

void AccessibilityEventInfo::SetEventType(const EventType eventType)
{
    eventType_ = eventType;
}

int64_t AccessibilityEventInfo::GetTimeStamp() const
{
    return timeStamp_;
}

void AccessibilityEventInfo::SetTimeStamp(const int64_t eventTime)
{
    (void)eventTime;
}

std::string AccessibilityEventInfo::GetBundleName() const
{
    return bundleName_;
}

void AccessibilityEventInfo::SetBundleName(const std::string &bundleName)
{
    (void)bundleName;
}

void AccessibilityEventInfo::SetTextMovementStep(const TextMoveUnit granularity)
{
    (void)granularity;
}

TextMoveUnit AccessibilityEventInfo::GetTextMovementStep() const
{
    return textMoveStep_;
}

void AccessibilityEventInfo::SetTriggerAction(const ActionType action)
{
    (void)action;
}

ActionType AccessibilityEventInfo::GetTriggerAction() const
{
    return triggerAction_;
}

AccessibilityEventInfo::AccessibilityEventInfo(int windowId, WindowUpdateType windowChangeTypes)
{
    (void)windowId;
    (void)windowChangeTypes;
}

AccessibilityEventInfo::AccessibilityEventInfo(EventType eventType)
{
    (void)eventType;
}

void AccessibilityEventInfo::SetNotificationInfo(const NotificationCategory category)
{
    (void)category;
}

NotificationCategory AccessibilityEventInfo::GetNotificationInfo() const
{
    return category_;
}

GestureType AccessibilityEventInfo::GetGestureType() const
{
    return gestureType_;
}
} // namespace Accessibility
} // namespace OHOS