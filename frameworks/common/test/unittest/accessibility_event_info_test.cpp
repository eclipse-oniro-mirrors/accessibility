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
        return;
    }
    eventInfo_->SetWindowChangeTypes(WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    EXPECT_EQ(eventInfo_->GetWindowChangeTypes(), WindowUpdateType::WINDOW_UPDATE_ACTIVE);
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
        return;
    }
    eventInfo_->SetEventType(EventType::TYPE_PAGE_CONTENT_UPDATE);
    EXPECT_EQ(eventInfo_->GetEventType(), EventType::TYPE_PAGE_CONTENT_UPDATE);
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
        return;
    }
    eventInfo_->SetWindowContentChangeTypes(WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
    EXPECT_EQ(eventInfo_->GetWindowContentChangeTypes(), WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
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
        return;
    }
    eventInfo_->SetTimeStamp(6000);
    EXPECT_EQ(eventInfo_->GetTimeStamp(), 6000);
    GTEST_LOG_(INFO) << "SetTimeStamp_001 end";
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
        return;
    }
    std::string bundleName = "accessibilityTest";
    eventInfo_->SetBundleName(bundleName);
    EXPECT_EQ(eventInfo_->GetBundleName(), bundleName);
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
        return;
    }
    std::string content = "notificationContent";
    eventInfo_->SetNotificationContent(content);
    EXPECT_EQ(eventInfo_->GetNotificationContent(), content);
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
        return;
    }
    eventInfo_->SetTextMovementStep(TextMoveUnit::STEP_LINE);
    EXPECT_EQ(eventInfo_->GetTextMovementStep(), TextMoveUnit::STEP_LINE);
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
        return;
    }
    eventInfo_->SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_CLICK);
    EXPECT_EQ(eventInfo_->GetTriggerAction(), ActionType::ACCESSIBILITY_ACTION_CLICK);
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
        return;
    }
    eventInfo_->SetNotificationInfo(NotificationCategory::CATEGORY_CALL);
    EXPECT_EQ(eventInfo_->GetNotificationInfo(), NotificationCategory::CATEGORY_CALL);
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
        return;
    }
    eventInfo_->SetGestureType(GestureType::GESTURE_SWIPE_UP);
    EXPECT_EQ(eventInfo_->GetGestureType(), GestureType::GESTURE_SWIPE_UP);
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
        return;
    }
    eventInfo_->SetPageId(1);
    EXPECT_EQ(eventInfo_->GetPageId(), 1);
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
        return;
    }
    eventInfo_->SetSource(1001);
    EXPECT_EQ(eventInfo_->GetViewId(), 1001);
    EXPECT_EQ(eventInfo_->GetAccessibilityId(), 1001);
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
        return;
    }
    eventInfo_->SetWindowId(1002);
    EXPECT_EQ(eventInfo_->GetWindowId(), 1002);
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
        return;
    }
    eventInfo_->SetCurrentIndex(1);
    EXPECT_EQ(eventInfo_->GetCurrentIndex(), 1);
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
        return;
    }
    eventInfo_->SetBeginIndex(1);
    EXPECT_EQ(eventInfo_->GetBeginIndex(), 1);
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
        return;
    }
    eventInfo_->SetEndIndex(1);
    EXPECT_EQ(eventInfo_->GetEndIndex(), 1);
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
        return;
    }
    eventInfo_->SetItemCounts(1);
    EXPECT_EQ(eventInfo_->GetItemCounts(), 1);
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
        return;
    }
    std::string componentType = "text";
    eventInfo_->SetComponentType(componentType);
    EXPECT_EQ(eventInfo_->GetComponentType(), componentType);
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
        return;
    }
    std::string text = "accessibility";
    eventInfo_->SetBeforeText(text);
    EXPECT_EQ(eventInfo_->GetBeforeText(), text);
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
        return;
    }
    std::string context = "lastContext";
    eventInfo_->SetLatestContent(context);
    EXPECT_EQ(eventInfo_->GetLatestContent(), context);
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
        return;
    }
    std::string descripion = "descripion";
    eventInfo_->SetDescription(descripion);
    EXPECT_EQ(eventInfo_->GetDescription(), descripion);
    GTEST_LOG_(INFO) << "SetDescription_001 end";
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
        return;
    }
    std::string content1 = "content1";
    eventInfo_->AddContent(content1);
    std::string content2 = "content2";
    eventInfo_->AddContent(content2);
    std::vector<std::string> contentLs = eventInfo_->GetContentList();
    int32_t index = 0;
    for (auto &content : contentLs) {
        if (!index) {
            EXPECT_EQ(content, content1);
        } else {
            EXPECT_EQ(content, content2);
        }
        index++;
    }
    GTEST_LOG_(INFO) << "AddContent_001 end";
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
} // namespace Accessibility
} // namespace OHOS