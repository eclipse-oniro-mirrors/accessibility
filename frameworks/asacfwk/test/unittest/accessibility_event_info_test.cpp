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

#include <gtest/gtest.h>
#include <memory>

#include "accessibility_event_info.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityEventInfoUnitTest : public ::testing::Test {
public:
    AccessibilityEventInfoUnitTest() {}
    ~AccessibilityEventInfoUnitTest() {}

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
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoUnitTest TearDown()";
    }
};

/**
 * @tc.number: SetChannelId_001
 * @tc.name: SetChannelId/GetChannelId
 * @tc.desc: Test function SetChannelId/GetChannelId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, SetChannelId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetChannelId_001 start";
    AccessibilityMemo record {};
    record.SetChannelId(1);
    EXPECT_TRUE(record.GetChannelId() == 1);
    GTEST_LOG_(INFO) << "SetChannelId_001 end";
}

/**
 * @tc.number: GetViewId_001
 * @tc.name: GetViewId
 * @tc.desc: Test function GetViewId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetViewId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetViewId_001 start";
    AccessibilityMemo record {};
    record.SetSource(1);
    EXPECT_TRUE(record.GetViewId() == 1);
    GTEST_LOG_(INFO) << "GetViewId_001 end";
}

/**
 * @tc.number: GetAccessibilityId_001
 * @tc.name: GetAccessibilityId
 * @tc.desc: Test function GetAccessibilityId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAccessibilityId_001 start";
    AccessibilityMemo record {};
    record.SetSource(1);

    EXPECT_TRUE(record.GetAccessibilityId() == 1);
    GTEST_LOG_(INFO) << "GetAccessibilityId_001 end";
}

/**
 * @tc.number: GetWindowId_001
 * @tc.name: SetWindowId/GetWindowId
 * @tc.desc: Test function SetWindowId/GetWindowId
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowId_001 start";
    AccessibilityMemo record {};
    record.SetWindowId(1);
    EXPECT_TRUE(record.GetWindowId() == 1);
    GTEST_LOG_(INFO) << "GetWindowId_001 end";
}

/**
 * @tc.number: GetPresentIndex_001
 * @tc.name: SetCurrentIndex/GetCurrentIndex
 * @tc.desc: Test function SetCurrentIndex/GetCurrentIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetPresentIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetPresentIndex_001 start";
    AccessibilityMemo record {};
    record.SetCurrentIndex(1);
    EXPECT_TRUE(record.GetCurrentIndex() == 1);
    GTEST_LOG_(INFO) << "GetPresentIndex_001 end";
}

/**
 * @tc.number: GetStartIndex_001
 * @tc.name: SetBeginIndex/GetBeginIndex
 * @tc.desc: Test function SetBeginIndex/GetBeginIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetStartIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetStartIndex_001 start";
    AccessibilityMemo record {};
    record.SetBeginIndex(1);
    EXPECT_TRUE(record.GetBeginIndex() == 1);
    GTEST_LOG_(INFO) << "GetStartIndex_001 end";
}

/**
 * @tc.number: GetEndIndex_001
 * @tc.name: SetEndIndex/GetEndIndex
 * @tc.desc: Test function SetEndIndex/GetEndIndex
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetEndIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetEndIndex_001 start";
    AccessibilityMemo record {};
    record.SetEndIndex(1);
    EXPECT_TRUE(record.GetEndIndex() == 1);
    GTEST_LOG_(INFO) << "GetEndIndex_001 end";
}

/**
 * @tc.number: GetClassName_001
 * @tc.name: SetComponentType/GetComponentType
 * @tc.desc: Test function SetComponentType/GetComponentType
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetClassName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetClassName_001 start";
    AccessibilityMemo record {};
    record.SetComponentType("test");
    EXPECT_TRUE(!strcmp(record.GetComponentType().c_str(), "test"));
    GTEST_LOG_(INFO) << "GetClassName_001 end";
}

/**
 * @tc.number: GetBeforeText_001
 * @tc.name: SetBeforeText/GetBeforeText
 * @tc.desc: Test function SetBeforeText/GetBeforeText
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetBeforeText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetBeforeText_001 start";
    AccessibilityMemo record {};
    record.SetBeforeText("test");
    EXPECT_TRUE(!strcmp(record.GetBeforeText().c_str(), "test"));
    GTEST_LOG_(INFO) << "GetBeforeText_001 end";
}

/**
 * @tc.number: GetContentList_001
 * @tc.name: AddContent/GetContentList
 * @tc.desc: Test function AddContent/GetContentList
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetContentList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetContentList_001 start";
    AccessibilityMemo record {};
    record.AddContent("test");
    EXPECT_TRUE(!strcmp(record.GetContentList()[0].c_str(), "test"));
    GTEST_LOG_(INFO) << "GetContentList_001 end";
}

/**
 * @tc.number: GetLatestContent_001
 * @tc.name: SetLatestContent/GetLatestContent
 * @tc.desc: Test function SetLatestContent/GetLatestContent
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetLatestContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetLatestContent_001 start";
    AccessibilityMemo record {};
    record.SetLatestContent("test");
    EXPECT_TRUE(!strcmp(record.GetLatestContent().c_str(), "test"));
    GTEST_LOG_(INFO) << "GetLatestContent_001 end";
}

/**
 * @tc.number: GetDescription_001
 * @tc.name: SetDescription/GetDescription
 * @tc.desc: Test function SetDescription/GetDescription
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetDescription_001 start";
    AccessibilityMemo record {};
    record.SetDescription("test");
    EXPECT_TRUE(!strcmp(record.GetDescription().c_str(), "test"));
    GTEST_LOG_(INFO) << "GetDescription_001 end";
}

/**
 * @tc.number: GetItemCounts_001
 * @tc.name: SetItemCounts/GetItemCounts
 * @tc.desc: Test function SetItemCounts/GetItemCounts
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetItemCounts_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetItemCounts_001 start";
    AccessibilityMemo record {};
    record.SetItemCounts(1);
    EXPECT_TRUE(record.GetItemCounts() == 1);
    GTEST_LOG_(INFO) << "GetItemCounts_001 end";
}

/**
 * @tc.number: GetRecordCount_001
 * @tc.name: SetRecordCount/GetRecordCount
 * @tc.desc: Test function SetRecordCount/GetRecordCount
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetRecordCount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRecordCount_001 start";
    AccessibilityEventInfo event {};
    event.SetRecordCount(1);
    EXPECT_TRUE(event.GetRecordCount() == 1);
    GTEST_LOG_(INFO) << "GetRecordCount_001 end";
}

/**
 * @tc.number: GetRecord_001
 * @tc.name: AddRecord/GetRecord
 * @tc.desc: Test function AddRecord/GetRecord
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetRecord_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRecord_001 start";
    AccessibilityEventInfo event {};
    event.SetRecordCount(1);
    event.AddRecord(event);
    EXPECT_TRUE(event.GetRecord(0).GetRecordCount() == 1);
    GTEST_LOG_(INFO) << "GetRecord_001 end";
}

/**
 * @tc.number: GetEventType_001
 * @tc.name: SetEventType/GetEventType
 * @tc.desc: Test function SetEventType/GetEventType
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetEventType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetEventType_001 start";
    AccessibilityEventInfo event {};
    event.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    EXPECT_TRUE(event.GetEventType() == EventType::TYPE_VIEW_CLICKED_EVENT);
    GTEST_LOG_(INFO) << "GetEventType_001 end";
}

/**
 * @tc.number: GetRecordCount_001
 * @tc.name: SetWindowContentChangeTypes/GetWindowContentChangeTypes
 * @tc.desc: Test function SetWindowContentChangeTypes/GetWindowContentChangeTypes
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetWindowContentChangeTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowContentChangeTypes_001 start";
    AccessibilityEventInfo event {};
    event.SetWindowContentChangeTypes(WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_SUBTREE);
    EXPECT_TRUE(event.GetWindowContentChangeTypes() == WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_SUBTREE);
    GTEST_LOG_(INFO) << "GetWindowContentChangeTypes_001 end";
}

/**
 * @tc.number: GetWindowChangeTypes_001
 * @tc.name: SetWindowChangeTypes/GetWindowChangeTypes
 * @tc.desc: Test function SetWindowChangeTypes/GetWindowChangeTypes
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetWindowChangeTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowChangeTypes_001 start";
    AccessibilityEventInfo event {};
    event.SetWindowChangeTypes(WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    EXPECT_TRUE(event.GetWindowChangeTypes() == WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    GTEST_LOG_(INFO) << "GetWindowChangeTypes_001 end";
}

/**
 * @tc.number: GetEventTime_001
 * @tc.name: SetTimeStamp/GetTimeStamp
 * @tc.desc: Test function SetTimeStamp/GetTimeStamp
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetEventTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindowContentChangeTypes_001 start";
    AccessibilityEventInfo event {};
    event.SetTimeStamp(100);
    EXPECT_TRUE(event.GetTimeStamp() == 100);
    GTEST_LOG_(INFO) << "GetWindowContentChangeTypes_001 end";
}

/**
 * @tc.number: GetBundleName_001
 * @tc.name: SetBundleName/GetBundleName
 * @tc.desc: Test function SetBundleName/GetBundleName
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetDescription_001 start";
    AccessibilityEventInfo event {};
    event.SetBundleName("test");
    EXPECT_TRUE(!strcmp(event.GetBundleName().c_str(), "test"));
    GTEST_LOG_(INFO) << "GetDescription_001 end";
}

/**
 * @tc.number: GetTextMovementStep_001
 * @tc.name: SetTextMovementStep/GetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep/GetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTextMovementStep_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTextMovementStep_001 start";
    AccessibilityEventInfo event {};
    event.SetTextMovementStep(TextMoveUnit::STEP_CHARACTER);
    EXPECT_TRUE(event.GetTextMovementStep() == TextMoveUnit::STEP_CHARACTER);
    GTEST_LOG_(INFO) << "GetTextMovementStep_001 end";
}

/**
 * @tc.number: GetTextMovementStep_002
 * @tc.name: SetTextMovementStep/GetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep/GetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTextMovementStep_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTextMovementStep_002 start";
    AccessibilityEventInfo event {};
    event.SetTextMovementStep(TextMoveUnit::STEP_WORD);
    EXPECT_TRUE(event.GetTextMovementStep() == TextMoveUnit::STEP_WORD);
    GTEST_LOG_(INFO) << "GetTextMovementStep_002 end";
}

/**
 * @tc.number: GetTextMovementStep_003
 * @tc.name: SetTextMovementStep/GetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep/GetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTextMovementStep_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTextMovementStep_003 start";
    AccessibilityEventInfo event {};
    event.SetTextMovementStep(TextMoveUnit::STEP_LINE);
    EXPECT_TRUE(event.GetTextMovementStep() == TextMoveUnit::STEP_LINE);
    GTEST_LOG_(INFO) << "GetTextMovementStep_003 end";
}

/**
 * @tc.number: GetTextMovementStep_004
 * @tc.name: SetTextMovementStep/GetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep/GetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTextMovementStep_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTextMovementStep_004 start";
    AccessibilityEventInfo event {};
    event.SetTextMovementStep(TextMoveUnit::STEP_PAGE);
    EXPECT_TRUE(event.GetTextMovementStep() == TextMoveUnit::STEP_PAGE);
    GTEST_LOG_(INFO) << "GetTextMovementStep_004 end";
}

/**
 * @tc.number: GetTextMovementStep_005
 * @tc.name: SetTextMovementStep/GetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep/GetTextMovementStep
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTextMovementStep_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTextMovementStep_005 start";
    AccessibilityEventInfo event {};
    event.SetTextMovementStep(TextMoveUnit::STEP_PARAGRAPH);
    EXPECT_TRUE(event.GetTextMovementStep() == TextMoveUnit::STEP_PARAGRAPH);
    GTEST_LOG_(INFO) << "GetTextMovementStep_005 end";
}

/**
 * @tc.number: GetTriggerAction_001
 * @tc.name: SetTriggerAction/GetTriggerAction
 * @tc.desc: Test function SetTriggerAction/GetTriggerAction
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTriggerAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTriggerAction_001 start";
    AccessibilityEventInfo event {};
    event.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_FOCUS);
    EXPECT_TRUE(event.GetTriggerAction() == ActionType::ACCESSIBILITY_ACTION_FOCUS);
    GTEST_LOG_(INFO) << "GetTriggerAction_001 end";
}

/**
 * @tc.number: GetTriggerAction_002
 * @tc.name: SetTriggerAction/GetTriggerAction
 * @tc.desc: Test function SetTriggerAction/GetTriggerAction
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTriggerAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTriggerAction_002 start";
    AccessibilityEventInfo event {};
    event.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_INVALID);
    EXPECT_TRUE(event.GetTriggerAction() == ActionType::ACCESSIBILITY_ACTION_INVALID);
    GTEST_LOG_(INFO) << "GetTriggerAction_002 end";
}

/**
 * @tc.number: GetTriggerAction_003
 * @tc.name: SetTriggerAction/GetTriggerAction
 * @tc.desc: Test function SetTriggerAction/GetTriggerAction
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTriggerAction_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTriggerAction_003 start";
    AccessibilityEventInfo event {};
    event.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_DELETED);
    EXPECT_TRUE(event.GetTriggerAction() == ActionType::ACCESSIBILITY_ACTION_DELETED);
    GTEST_LOG_(INFO) << "GetTriggerAction_003 end";
}

/**
 * @tc.number: GetTriggerAction_004
 * @tc.name: SetTriggerAction/GetTriggerAction
 * @tc.desc: Test function SetTriggerAction/GetTriggerAction
 */
HWTEST_F(AccessibilityEventInfoUnitTest, GetTriggerAction_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetTriggerAction_004 start";
    AccessibilityEventInfo event {};
    event.SetTriggerAction(ActionType::ACCESSIBILITY_ACTION_TYPE_MASK);
    EXPECT_TRUE(event.GetTriggerAction() == ActionType::ACCESSIBILITY_ACTION_TYPE_MASK);
    GTEST_LOG_(INFO) << "GetTriggerAction_004 end";
}

/**
 * @tc.number: ReadFromParcel_001
 * @tc.name: ReadFromParcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityEventInfoUnitTest, ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ReadFromParcel_001 start";

    Parcel parcel;
    AccessibilityEventInfo eventInfo {};
    eventInfo.Marshalling(parcel);
    EXPECT_TRUE(eventInfo.ReadFromParcel(parcel));

    AccessibilityMemo memo {};
    memo.Marshalling(parcel);
    EXPECT_TRUE(memo.ReadFromParcel(parcel));

    GTEST_LOG_(INFO) << "ReadFromParcel_001 end";
}
} // namespace Accessibility
} // namespace OHOS