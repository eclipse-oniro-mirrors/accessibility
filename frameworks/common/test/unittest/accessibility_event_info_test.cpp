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

#include <gtest/gtest.h>
#include <memory>
#include "accessibility_event_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
constexpr int64_t TIMESTAMP = 6000;
constexpr uint32_t RESOURCE_ID = 12345;
constexpr int64_t SOURCE_ID = 1001;
constexpr int32_t WINDOW_ID = 1002;
constexpr int32_t CURRENT_INDEX = 1;
constexpr int32_t BEGIN_INDEX = 1;
constexpr int32_t END_INDEX = 1;
constexpr int32_t ITEM_COUNTS = 1;
constexpr int32_t REQUEST_FOCUS_ELEMENT_ID = 1;
const std::string RESOURCE_MODULE_NAME = "accessibilityResourceModuleNameTest";
const std::string RESOURCE_BUNDLE_NAME = "accessibilityResourceBundleNameTest";
const std::string BUNDLE_NAME = "accessibilityBundleNameTest";
const std::string NOTIFICATION_CONTENT = "notificationContent";
const std::string COMPONENT_TYPE = "text";
const std::string BEFORE_TEXT = "accessibility";
const std::string LATEST_CONTENT = "lastContext";
const std::string DESCRIPTION = "descripion";
const std::string TEXT_ANNOUNCED_FOR_ACCESSIBILITY = "textAnnouncedForAccessibility";
const std::string INSPECTOR_KEY = "inspectorKey";
const std::vector<std::string> CONTENT_LIST = {"content1", "content2"};

class AccessibilityEventInfoUnitTest : public ::testing::Test {
public:
    AccessibilityEventInfoUnitTest()
    {}
    ~AccessibilityEventInfoUnitTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest SetUp() Start";
        eventInfo_ = std::make_shared<AccessibilityEventInfo>();
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest TearDown()";
        eventInfo_ = nullptr;
    }

    std::shared_ptr<AccessibilityEventInfo> eventInfo_ = nullptr;
};

/**
 * @tc.number: SetWindowChangeTypes_001
 * @tc.name: SetWindowChangeTypes
 * @tc.desc: Test function SetWindowChangeTypes
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetWindowChangeTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowChangeTypes_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetWindowChangeTypes(WindowUpdateType::WINDOW_UPDATE_ACTIVE);
        EXPECT_EQ(eventInfo_->GetWindowChangeTypes(), WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    }
    GTEST_LOG_(INFO) << "SetWindowChangeTypes_001 end";
}

/**
 * @tc.number: SetEventType_001
 * @tc.name: SetEventType
 * @tc.desc: Test function SetEventType
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetEventType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEventType_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetEventType(EventType::TYPE_PAGE_CONTENT_UPDATE);
        EXPECT_EQ(eventInfo_->GetEventType(), EventType::TYPE_PAGE_CONTENT_UPDATE);
    }
    GTEST_LOG_(INFO) << "SetEventType_001 end";
}

/**
 * @tc.number: SetWindowContentChangeTypes_001
 * @tc.name: SetWindowContentChangeTypes
 * @tc.desc: Test function SetWindowContentChangeTypes
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetWindowContentChangeTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowContentChangeTypes_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetWindowContentChangeTypes(WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
        EXPECT_EQ(eventInfo_->GetWindowContentChangeTypes(), WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
    }
    GTEST_LOG_(INFO) << "SetWindowContentChangeTypes_001 end";
}

/**
 * @tc.number: SetTimeStamp_001
 * @tc.name: SetTimeStamp
 * @tc.desc: Test function SetTimeStamp
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetTimeStamp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTimeStamp_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetTimeStamp(TIMESTAMP);
        EXPECT_EQ(eventInfo_->GetTimeStamp(), TIMESTAMP);
    }
    GTEST_LOG_(INFO) << "SetTimeStamp_001 end";
}

/**
 * @tc.number: SetResourceId_001
 * @tc.name: SetResourceId
 * @tc.desc: Test function SetResourceId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetResourceId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetResourceId_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetResourceId(RESOURCE_ID);
        EXPECT_EQ(eventInfo_->GetResourceId(), RESOURCE_ID);
    }
    GTEST_LOG_(INFO) << "SetResourceId_001 end";
}
 
/**
 * @tc.number: SetResourceModuleName_001
 * @tc.name: SetResourceModuleName
 * @tc.desc: Test function SetResourceModuleName
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetResourceModuleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetResourceModuleName_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetResourceModuleName(RESOURCE_MODULE_NAME);
        EXPECT_EQ(eventInfo_->GetResourceModuleName(), RESOURCE_MODULE_NAME);
    }
    GTEST_LOG_(INFO) << "SetResourceModuleName_001 end";
}
 
/**
 * @tc.number: SetResourceBundleName_001
 * @tc.name: SetResourceBundleName
 * @tc.desc: Test function SetResourceBundleName
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetResourceBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetResourceBundleName_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetResourceBundleName(RESOURCE_BUNDLE_NAME);
        EXPECT_EQ(eventInfo_->GetResourceBundleName(), RESOURCE_BUNDLE_NAME);
    }
    GTEST_LOG_(INFO) << "SetResourceBundleName_001 end";
}

/**
 * @tc.number: SetBundleName_001
 * @tc.name: SetBundleName
 * @tc.desc: Test function SetBundleName
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBundleName_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetBundleName(BUNDLE_NAME);
        EXPECT_EQ(eventInfo_->GetBundleName(), BUNDLE_NAME);
    }
    GTEST_LOG_(INFO) << "SetBundleName_001 end";
}

/**
 * @tc.number: SetNotificationContent_001
 * @tc.name: SetNotificationContent
 * @tc.desc: Test function SetNotificationContent
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetNotificationContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetNotificationContent_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetNotificationContent(NOTIFICATION_CONTENT);
        EXPECT_EQ(eventInfo_->GetNotificationContent(), NOTIFICATION_CONTENT);
    }
    GTEST_LOG_(INFO) << "SetNotificationContent_001 end";
}

/**
 * @tc.number: SetTextMovementStep_001
 * @tc.name: SetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetTextMovementStep_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTextMovementStep_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetTextMovementStep(TextMoveUnit::STEP_LINE);
        EXPECT_EQ(eventInfo_->GetTextMovementStep(), TextMoveUnit::STEP_LINE);
    }
    GTEST_LOG_(INFO) << "SetTextMovementStep_001 end";
}

/**
 * @tc.number: SetTriggerAction_001
 * @tc.name: SetTriggerAction
 * @tc.desc: Test function SetTriggerAction
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetTriggerAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTriggerAction_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_CLICK);
        EXPECT_EQ(eventInfo_->GetTriggerAction(), ActionType::ACCESSIBILITY_ACTION_CLICK);
    }
    GTEST_LOG_(INFO) << "SetTriggerAction_001 end";
}

/**
 * @tc.number: SetNotificationInfo_001
 * @tc.name: SetNotificationInfo
 * @tc.desc: Test function SetNotificationInfo
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetNotificationInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetNotificationInfo_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetNotificationInfo(NotificationCategory::CATEGORY_CALL);
        EXPECT_EQ(eventInfo_->GetNotificationInfo(), NotificationCategory::CATEGORY_CALL);
    }
    GTEST_LOG_(INFO) << "SetNotificationInfo_001 end";
}

/**
 * @tc.number: SetGestureType_001
 * @tc.name: SetGestureType
 * @tc.desc: Test function SetGestureType
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetGestureType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGestureType_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetGestureType(GestureType::GESTURE_SWIPE_UP);
        EXPECT_EQ(eventInfo_->GetGestureType(), GestureType::GESTURE_SWIPE_UP);
    }
    GTEST_LOG_(INFO) << "SetGestureType_001 end";
}

/**
 * @tc.number: SetPageId_001
 * @tc.name: SetPageId
 * @tc.desc: Test function SetPageId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetPageId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPageId_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetPageId(1);
        EXPECT_EQ(eventInfo_->GetPageId(), 1);
    }
    GTEST_LOG_(INFO) << "SetPageId_001 end";
}

/**
 * @tc.number: SetSource_001
 * @tc.name: SetSource
 * @tc.desc: Test function SetSource
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSource_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetSource(SOURCE_ID);
        EXPECT_EQ(eventInfo_->GetViewId(), SOURCE_ID);
        EXPECT_EQ(eventInfo_->GetAccessibilityId(), SOURCE_ID);
    }
    GTEST_LOG_(INFO) << "SetSource_001 end";
}

/**
 * @tc.number: SetWindowId_001
 * @tc.name: SetWindowId
 * @tc.desc: Test function SetWindowId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowId_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetWindowId(WINDOW_ID);
        EXPECT_EQ(eventInfo_->GetWindowId(), WINDOW_ID);
    }
    GTEST_LOG_(INFO) << "SetWindowId_001 end";
}

/**
 * @tc.number: SetCurrentIndex_001
 * @tc.name: SetCurrentIndex
 * @tc.desc: Test function SetCurrentIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetCurrentIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCurrentIndex_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetCurrentIndex(CURRENT_INDEX);
        EXPECT_EQ(eventInfo_->GetCurrentIndex(), CURRENT_INDEX);
    }
    GTEST_LOG_(INFO) << "SetCurrentIndex_001 end";
}

/**
 * @tc.number: SetBeginIndex_001
 * @tc.name: SetBeginIndex
 * @tc.desc: Test function SetBeginIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetBeginIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBeginIndex_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetBeginIndex(BEGIN_INDEX);
        EXPECT_EQ(eventInfo_->GetBeginIndex(), BEGIN_INDEX);
    }
    GTEST_LOG_(INFO) << "SetBeginIndex_001 end";
}

/**
 * @tc.number: SetEndIndex_001
 * @tc.name: SetEndIndex
 * @tc.desc: Test function SetEndIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetEndIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEndIndex_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetEndIndex(END_INDEX);
        EXPECT_EQ(eventInfo_->GetEndIndex(), END_INDEX);
    }
    GTEST_LOG_(INFO) << "SetEndIndex_001 end";
}

/**
 * @tc.number: SetItemCounts_001
 * @tc.name: SetItemCounts
 * @tc.desc: Test function SetItemCounts
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetItemCounts_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetItemCounts_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetItemCounts(ITEM_COUNTS);
        EXPECT_EQ(eventInfo_->GetItemCounts(), ITEM_COUNTS);
    }
    GTEST_LOG_(INFO) << "SetItemCounts_001 end";
}

/**
 * @tc.number: SetComponentType_001
 * @tc.name: SetComponentType
 * @tc.desc: Test function SetComponentType
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetComponentType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetComponentType_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetComponentType(COMPONENT_TYPE);
        EXPECT_EQ(eventInfo_->GetComponentType(), COMPONENT_TYPE);
    }
    GTEST_LOG_(INFO) << "SetComponentType_001 end";
}

/**
 * @tc.number: SetBeforeText_001
 * @tc.name: SetBeforeText
 * @tc.desc: Test function SetBeforeText
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetBeforeText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBeforeText_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetBeforeText(BEFORE_TEXT);
        EXPECT_EQ(eventInfo_->GetBeforeText(), BEFORE_TEXT);
    }
    GTEST_LOG_(INFO) << "SetBeforeText_001 end";
}

/**
 * @tc.number: SetLatestContent_001
 * @tc.name: SetLatestContent
 * @tc.desc: Test function SetLatestContent
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetLatestContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetLatestContent_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetLatestContent(LATEST_CONTENT);
        EXPECT_EQ(eventInfo_->GetLatestContent(), LATEST_CONTENT);
    }
    GTEST_LOG_(INFO) << "SetLatestContent_001 end";
}

/**
 * @tc.number: SetDescription_001
 * @tc.name: SetDescription
 * @tc.desc: Test function SetDescription
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDescription_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetDescription(DESCRIPTION);
        EXPECT_EQ(eventInfo_->GetDescription(), DESCRIPTION);
    }
    GTEST_LOG_(INFO) << "SetDescription_001 end";
}

/**
 * @tc.number: SetTextAnnouncedForAccessibility_001
 * @tc.name: SetTextAnnouncedForAccessibility
 * @tc.desc: Test function SetTextAnnouncedForAccessibility
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetTextAnnouncedForAccessibility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTextAnnouncedForAccessibility_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetTextAnnouncedForAccessibility(TEXT_ANNOUNCED_FOR_ACCESSIBILITY);
        EXPECT_EQ(eventInfo_->GetTextAnnouncedForAccessibility(), TEXT_ANNOUNCED_FOR_ACCESSIBILITY);
    }
    GTEST_LOG_(INFO) << "SetTextAnnouncedForAccessibility_001 end";
}

/**
 * @tc.number: SetInspectorKey_001
 * @tc.name: SetInspectorKey
 * @tc.desc: Test function SetInspectorKey
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetInspectorKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInspectorKey_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetInspectorKey(INSPECTOR_KEY);
        EXPECT_EQ(eventInfo_->GetInspectorKey(), INSPECTOR_KEY);
    }
    GTEST_LOG_(INFO) << "SetInspectorKey_001 end";
}

/**
 * @tc.number: SetRequestFocusElementId_001
 * @tc.name: SetRequestFocusElementId
 * @tc.desc: Test function SetRequestFocusElementId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetRequestFocusElementId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetRequestFocusElementId_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        eventInfo_->SetRequestFocusElementId(REQUEST_FOCUS_ELEMENT_ID);
        EXPECT_EQ(eventInfo_->GetRequestFocusElementId(), REQUEST_FOCUS_ELEMENT_ID);
    }
    GTEST_LOG_(INFO) << "SetRequestFocusElementId_001 end";
}

/**
 * @tc.number: AddContent_001
 * @tc.name: AddContent
 * @tc.desc: Test function AddContent
 */
HWTEST_F(AccessibilityEventInfoUnitTest, AddContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddContent_001 start";
    if (!eventInfo_) {
        GTEST_LOG_(INFO) << "eventInfo_ is null";
    } else {
        for (auto &content : CONTENT_LIST) {
            eventInfo_->AddContent(content);
        }
        std::vector<std::string> contentList = eventInfo_->GetContentList();
        EXPECT_FALSE(contentList.empty());
        EXPECT_EQ(contentList.size(), CONTENT_LIST.size());
        int32_t index = 0;
        for (auto &content : contentList) {
            EXPECT_EQ(content, CONTENT_LIST[index++]);
        }
    }
    GTEST_LOG_(INFO) << "AddContent_001 end";
}

/**
 * @tc.number: SetElementInfo_001
 * @tc.name: SetElementInfo
 * @tc.desc: Test function SetElement&GetElement
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetElementInfo_001 start";
    int accessibilityId = 1;
    AccessibilityElementInfo elementInfo;
    std::shared_ptr<AccessibilityEventInfo> eventInfo =
        std::make_shared<AccessibilityEventInfo>(1, WINDOW_UPDATE_FOCUSED);
    elementInfo.SetAccessibilityId(accessibilityId);
    ASSERT_TRUE(eventInfo);
    eventInfo->SetElementInfo(elementInfo);
    EXPECT_EQ(eventInfo->GetElementInfo().GetAccessibilityId(), accessibilityId);
    GTEST_LOG_(INFO) << "SetElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityEventInfo_001
 * @tc.name: AccessibilityEventInfo
 * @tc.desc: Test function AccessibilityEventInfo
 */
HWTEST_F(AccessibilityEventInfoUnitTest, AccessibilityEventInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityEventInfo_001 start";
    std::shared_ptr<AccessibilityEventInfo> eventInfo =
        std::make_shared<AccessibilityEventInfo>(1, WINDOW_UPDATE_FOCUSED);
    ASSERT_TRUE(eventInfo);
    EXPECT_EQ(eventInfo->GetWindowId(), 1);
    EXPECT_EQ(eventInfo->GetWindowChangeTypes(), WINDOW_UPDATE_FOCUSED);
    GTEST_LOG_(INFO) << "AccessibilityEventInfo_001 end";
}

/**
 * @tc.number: SetExtraEvent_001
 * @tc.name: SetExtraEvent
 * @tc.desc: Test function SetExtraEvent, GetExtraEvent
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetExtraEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetExtraEvent_001 start";
    ExtraEventInfo extraEventInfo {};
    extraEventInfo.SetExtraEventInfo("addText", "1");
    eventInfo_->SetExtraEvent(extraEventInfo);
    EXPECT_TRUE(eventInfo_->GetExtraEvent().GetExtraEventInfoValueStr().size() == 1);
    GTEST_LOG_(INFO) << "SetExtraEvent_001 end";
}
} // namespace Accessibility
} // namespace OHOS
