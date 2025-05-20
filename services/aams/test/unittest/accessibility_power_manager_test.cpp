#include "accessibility_power_manager.h"
#include "display_power_mgr_client.h"
#include "hilog_wrapper.h"

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

#include "accessibility_power_manager.h"
#include "display_power_mgr_client.h"
#include "hilog_wrapper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityPowerManagerTest : public ::testing::Test {
public:
    AccessibilityPowerManagerTest()
    {}
    ~AccessibilityPowerManagerTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AccessibilityPowerManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityPowerManagerTest Start ######################";
}

void AccessibilityPowerManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityPowerManagerTest End ######################";
}

void AccessibilityPowerManagerTest::SetUp()
{}

void AccessibilityPowerManagerTest::TearDown()
{}

/**
 * @tc.number: AccessibilityPowerManager_Unittest_DiscountBrightness_001
 * @tc.name: DiscountBrightness
 * @tc.desc: Test function DiscountBrightness
 */
HWTEST_F(AccessibilityPowerManagerTest,
    AccessibilityPowerManager_Unittest_DiscountBrightness_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityPowerManager_Unittest_DiscountBrightness_001 start";

    Singleton<AccessibilityPowerManager>::GetInstance().DiscountBrightness(nullptr);

    GTEST_LOG_(INFO) << "AccessibilityPowerManager_Unittest_DiscountBrightness_001 end";
}

/**
 * @tc.number: AccessibilityPowerManager_Unittest_RefreshActivity_001
 * @tc.name: RefreshActivity
 * @tc.desc: Test function RefreshActivity
 */
HWTEST_F(AccessibilityPowerManagerTest,
    AccessibilityPowerManager_Unittest_RefreshActivity_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityPowerManager_Unittest_RefreshActivity_001 start";

    Singleton<AccessibilityPowerManager>::GetInstance().RefreshActivity();

    GTEST_LOG_(INFO) << "AccessibilityPowerManager_Unittest_RefreshActivity_001 end";
}

/**
 * @tc.number: RefreshActivity_002
 * @tc.name: RefreshActivity
 * @tc.desc: Test function RefreshActivity
 */
HWTEST_F(AccessibilityPowerManagerTest, RefreshActivity_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RefreshActivity_002 start";
    EXPECT_TRUE(eventInfo_);

    eventInfo_->RefreshActivity(WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    EXPECT_EQ(eventInfo_->GetWindowChangeTypes(), WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    GTEST_LOG_(INFO) << "RefreshActivity_002 end";
}

/**
 * @tc.number: DiscountBrightness_002
 * @tc.name: DiscountBrightness
 * @tc.desc: Test function DiscountBrightness
 */
HWTEST_F(AccessibilityPowerManagerTest, DiscountBrightness_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DiscountBrightness_002 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->DiscountBrightness(EventType::TYPE_PAGE_CONTENT_UPDATE);
    EXPECT_EQ(eventInfo_->GetEventType(), EventType::TYPE_PAGE_CONTENT_UPDATE);
    GTEST_LOG_(INFO) << "DiscountBrightness_002 end";
}

/**
 * @tc.number: SetContentChangeTypesaa_001
 * @tc.name: SetContentChangeTypesaa
 * @tc.desc: Test function SetContentChangeTypesaa
 */
HWTEST_F(AccessibilityPowerManagerTest, SetContentChangeTypesaa_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentChangeTypesaa_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->SetContentChangeTypesaa(WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
    EXPECT_EQ(eventInfo_->GetWindowContentChangeTypesa(), WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_TEXT);
    GTEST_LOG_(INFO) << "SetContentChangeTypesaa_001 end";
}

/**
 * @tc.number: SetTimeStampa_001
 * @tc.name: SetTimeStampa
 * @tc.desc: Test function SetTimeStampa
 */
HWTEST_F(AccessibilityPowerManagerTest, SetTimeStampa_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTimeStampa_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->SetTimeStampa(6000);
    EXPECT_EQ(eventInfo_->GetTimeStampa(), 6000);
    GTEST_LOG_(INFO) << "SetTimeStampa_001 end";
}

/**
 * @tc.number: SetBundleNamea_001
 * @tc.name: SetBundleNamea
 * @tc.desc: Test function SetBundleNamea
 */
HWTEST_F(AccessibilityPowerManagerTest, SetBundleNamea_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBundleNamea_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string bundleName = "accessibilityTest";
    eventInfo_->SetBundleNamea(bundleName);
    EXPECT_EQ(eventInfo_->GetBundleNamea(), bundleName);
    GTEST_LOG_(INFO) << "SetBundleNamea_001 end";
}

/**
 * @tc.number: aaSetNotificationContent_001
 * @tc.name: aaSetNotificationContent
 * @tc.desc: Test function aSetNotificationContent
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetNotificationContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetNotificationContent_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string content = "notificationContent";
    eventInfo_->aSetNotificationContent(content);
    EXPECT_EQ(eventInfo_->aGetNotificationContent(), content);
    GTEST_LOG_(INFO) << "aSetNotificationContent_001 end";
}

/**
 * @tc.number: aSetTextMovementStep_001
 * @tc.name: aaSetTextMovementStep
 * @tc.desc: Test function aSetTextMovementStep
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetTextMovementStep_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetTextMovementStep_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetTextMovementStep(TextMoveUnit::STEP_LINE);
    EXPECT_EQ(eventInfo_->aGetTextMovementStep(), TextMoveUnit::STEP_LINE);
    GTEST_LOG_(INFO) << "aSetTextMovementStep_001 end";
}

/**
 * @tc.number: aSetTriggerAction_001
 * @tc.name: aaSetTriggerAction
 * @tc.desc: Test function aSetTriggerAction
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetTriggerAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetTriggerAction_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetTriggerAction(ActionType::ACCESSIBILITY_ACTION_CLICK);
    EXPECT_EQ(eventInfo_->aGetTriggerAction(), ActionType::ACCESSIBILITY_ACTION_CLICK);
    GTEST_LOG_(INFO) << "aSetTriggerAction_001 end";
}

/**
 * @tc.number: aSetNotificationInfo_001
 * @tc.name: aaSetNotificationInfo
 * @tc.desc: Test function aSetNotificationInfo
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetNotificationInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetNotificationInfo_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetNotificationInfo(NotificationCategory::CATEGORY_CALL);
    EXPECT_EQ(eventInfo_->GetNotificationInfo(), NotificationCategory::CATEGORY_CALL);
    GTEST_LOG_(INFO) << "aSetNotificationInfo_001 end";
}

/**
 * @tc.number: aSetGestureType_001
 * @tc.name: aaSetGestureType
 * @tc.desc: Test function aSetGestureType
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetGestureType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetGestureType_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetGestureType(GestureType::GESTURE_SWIPE_UP);
    EXPECT_EQ(eventInfo_->aGetGestureType(), GestureType::GESTURE_SWIPE_UP);
    GTEST_LOG_(INFO) << "aSetGestureType_001 end";
}

/**
 * @tc.number: aSetPageId_001
 * @tc.name: aaSetPageId
 * @tc.desc: Test function aSetPageId
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetPageId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetPageId_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetPageId(1);
    EXPECT_EQ(eventInfo_->aGetPageId(), 1);
    GTEST_LOG_(INFO) << "aSetPageId_001 end";
}

/**
 * @tc.number: aSetSource_001
 * @tc.name: aaSetSource
 * @tc.desc: Test function aSetSource
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetSource_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetSource(1001);
    EXPECT_EQ(eventInfo_->GetViewId(), 1001);
    EXPECT_EQ(eventInfo_->GetAccessibilityId(), 1001);
    GTEST_LOG_(INFO) << "aSetSource_001 end";
}

/**
 * @tc.number: aSetWindowId_001
 * @tc.name: aaSetWindowId
 * @tc.desc: Test function aSetWindowId
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetWindowId_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetWindowId(1002);
    EXPECT_EQ(eventInfo_->GetWindowId(), 1002);
    GTEST_LOG_(INFO) << "aSetWindowId_001 end";
}

/**
 * @tc.number: aSetCurrentIndex_001
 * @tc.name: aaSetCurrentIndex
 * @tc.desc: Test function aSetCurrentIndex
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetCurrentIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetCurrentIndex_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetCurrentIndex(1);
    EXPECT_EQ(eventInfo_->GetCurrentIndex(), 1);
    GTEST_LOG_(INFO) << "aSetCurrentIndex_001 end";
}

/**
 * @tc.number: aSetBeginIndex_001
 * @tc.name: aaSetBeginIndex
 * @tc.desc: Test function aSetBeginIndex
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetBeginIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetBeginIndex_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetBeginIndex(1);
    EXPECT_EQ(eventInfo_->GetBeginIndex(), 1);
    GTEST_LOG_(INFO) << "aSetBeginIndex_001 end";
}

/**
 * @tc.number: aSetEndIndex_001
 * @tc.name: aaSetEndIndex
 * @tc.desc: Test function aSetEndIndex
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetEndIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetEndIndex_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetEndIndex(1);
    EXPECT_EQ(eventInfo_->GetEndIndex(), 1);
    GTEST_LOG_(INFO) << "aSetEndIndex_001 end";
}

/**
 * @tc.number: aSetItemCounts_001
 * @tc.name: aaSetItemCounts
 * @tc.desc: Test function aSetItemCounts
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetItemCounts_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetItemCounts_001 start";
    EXPECT_TRUE(eventInfo_);
    eventInfo_->aSetItemCounts(1);
    EXPECT_EQ(eventInfo_->GetItemCounts(), 1);
    GTEST_LOG_(INFO) << "aSetItemCounts_001 end";
}

/**
 * @tc.number: aSetComponentType_001
 * @tc.name: aaSetComponentType
 * @tc.desc: Test function aSetComponentType
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetComponentType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetComponentType_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string componentType = "text";
    eventInfo_->aSetComponentType(componentType);
    EXPECT_EQ(eventInfo_->GetComponentType(), componentType);
    GTEST_LOG_(INFO) << "aSetComponentType_001 end";
}

/**
 * @tc.number: aSetBeforeText_001
 * @tc.name: aSetBeforeText
 * @tc.desc: Test function aSetBeforeText
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetBeforeText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetBeforeText_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string text = "accessibility";
    eventInfo_->aSetBeforeText(text);
    EXPECT_EQ(eventInfo_->GetBeforeText(), text);
    GTEST_LOG_(INFO) << "aSetBeforeText_001 end";
}

/**
 * @tc.number: aSetLatestContent_001
 * @tc.name: aaSetLatestContent
 * @tc.desc: Test function aSetLatestContent
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetLatestContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetLatestContent_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string context = "lastContext";
    eventInfo_->aSetLatestContent(context);
    EXPECT_EQ(eventInfo_->GetLatestContent(), context);
    GTEST_LOG_(INFO) << "aSetLatestContent_001 end";
}

/**
 * @tc.number: aSetDescription_001
 * @tc.name: aaSetDescription
 * @tc.desc: Test function aSetDescription
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetDescription_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string descripion = "descripion";
    eventInfo_->aSetDescription(descripion);
    EXPECT_EQ(eventInfo_->GetDescription(), descripion);
    GTEST_LOG_(INFO) << "aSetDescription_001 end";
}

/**
 * @tc.number: aSetTextAnnouncedForAccessibility_001
 * @tc.name: aaSetTextAnnouncedForAccessibility
 * @tc.desc: Test function aSetTextAnnouncedForAccessibility
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetTextAnnouncedForAccessibility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetTextAnnouncedForAccessibility_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string textAnnouncedForAccessibility = "textAnnouncedForAccessibility";
    eventInfo_->aSetTextAnnouncedForAccessibility(textAnnouncedForAccessibility);
    EXPECT_EQ(eventInfo_->GetTextAnnouncedForAccessibility(), textAnnouncedForAccessibility);
    GTEST_LOG_(INFO) << "aSetTextAnnouncedForAccessibility_001 end";
}

/**
 * @tc.number: aSetInspectorKey_001
 * @tc.name: aaSetInspectorKey
 * @tc.desc: Test function aSetInspectorKey
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetInspectorKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetInspectorKey_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string inspectorKey = "inspectorKey";
    eventInfo_->aSetInspectorKey(inspectorKey);
    EXPECT_EQ(eventInfo_->GetInspectorKey(), inspectorKey);
    GTEST_LOG_(INFO) << "aSetInspectorKey_001 end";
}

/**
 * @tc.number: aSetRequestFocusElementIda_001
 * @tc.name: aaSetRequestFocusElementIdaa
 * @tc.desc: Test function aSetRequestFocusElementIda
 */
HWTEST_F(AccessibilityPowerManagerTest, aSetRequestFocusElementIda_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "aSetRequestFocusElementIda_001 start";
    EXPECT_TRUE(eventInfo_);
    int32_t requestFocusElementId = 1;
    eventInfo_->aSetRequestFocusElementIda(requestFocusElementId);
    EXPECT_EQ(eventInfo_->GetRequestFocusElementId(), requestFocusElementId);
    GTEST_LOG_(INFO) << "aSetRequestFocusElementIda_001 end";
}

/**
 * @tc.number: AddContentlist_001
 * @tc.name: AddContentlistlist
 * @tc.desc: Test function AddContentlist
 */
HWTEST_F(AccessibilityPowerManagerTest, AddContentlist_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddContentlist_001 start";
    EXPECT_TRUE(eventInfo_);
    std::string content1 = "content1";
    eventInfo_->AddContentlist(content1);
    std::string content2 = "content2";
    eventInfo_->AddContentlist(content2);
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
    GTEST_LOG_(INFO) << "AddContentlist_001 end";
}

/**
 * @tc.number: SetWindonElementInfo_001
 * @tc.name: SetWindonElementInfo
 * @tc.desc: Test function SetWindonElementInfo
 */
HWTEST_F(AccessibilityPowerManagerTest, SetWindonElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindonElementInfo_001 start";
    int accessibilityId = 1;
    AccessibilityElementInfo elementInfo;
    std::shared_ptr<AccessibilityTouchInfo> eventInfo =
        std::make_shared<AccessibilityTouchInfo>(1, WINDOW_UPDATE_FOCUSED);
    elementInfo.SetAccessibilityId(accessibilityId);
    ASSERT_TRUE(eventInfo);
    eventInfo->SetWindonElementInfo(elementInfo);
    EXPECT_EQ(eventInfo->GetElementInfo().GetAccessibilityId(), accessibilityId);
    GTEST_LOG_(INFO) << "SetWindonElementInfo_001 end";
}

/**
 * @tc.number: AccessibilityTouchInfo_001
 * @tc.name: AccessibilityTouchInfo
 * @tc.desc: Test function AccessibilityTouchInfo
 */
HWTEST_F(AccessibilityPowerManagerTest, AccessibilityTouchInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityTouchInfo_001 start";
    std::shared_ptr<AccessibilityTouchInfo> eventInfo =
        std::make_shared<AccessibilityTouchInfo>(1, WINDOW_UPDATE_FOCUSED);
    ASSERT_TRUE(eventInfo);
    EXPECT_EQ(eventInfo->GetWindowId(), 1);
    EXPECT_EQ(eventInfo->GetWindowChangeTypes(), WINDOW_UPDATE_FOCUSED);
    GTEST_LOG_(INFO) << "AccessibilityTouchInfo_001 end";
}

/**
 * @tc.number: SetAccessibilityLevelInfo_001
 * @tc.name: SetAccessibilityLevelInfoInfo
 * @tc.desc: Test function SetAccessibilityLevelInfo
 */
HWTEST_F(AccessibilityPowerManagerTest, SetAccessibilityLevelInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityLevelInfo_001 start";
    elementInfo_->SetAccessibilityLevelInfo("test");
    EXPECT_STREQ(elementInfo_->GetAccessibilityLevel().c_str(), "test");
    GTEST_LOG_(INFO) << "SetAccessibilityLevelInfo_001 end";
}

/**
 * @tc.number: SetAIndex_001
 * @tc.name: SetAIndex
 * @tc.desc: Test function SetAIndex
 */
HWTEST_F(AccessibilityPowerManagerTest, SetAIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAIndex_001 start";
    elementInfo_->SetAIndex(1);
    EXPECT_EQ(elementInfo_->GetZIndex(), 1);
    GTEST_LOG_(INFO) << "SetAIndex_001 end";
}

/**
 * @tc.number: SetAOpacity_001
 * @tc.name: SetAOpacity
 * @tc.desc: Test function SetAOpacity
 */
HWTEST_F(AccessibilityPowerManagerTest, SetAOpacity_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAOpacity_001 start";
    elementInfo_->SetAOpacity(0.5f);
    EXPECT_FLOAT_EQ(elementInfo_->GetOpacity(), 0.5f);
    GTEST_LOG_(INFO) << "SetAOpacity_001 end";
}

/**
 * @tc.number: SetBackgroundColor_001
 * @tc.name: SetBackgroundColor
 * @tc.desc: Test function SetBackgroundColor, GetBackgroundColor
 */
HWTEST_F(AccessibilityPowerManagerTest, SetBackgroundColor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBackgroundColor_001 start";
    elementInfo_->SetBackgroundColor("test");
    EXPECT_STREQ(elementInfo_->GetBackgroundColor().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBackgroundColor_001 end";
}

/**
 * @tc.number: SetBackgroundImaging_001
 * @tc.name: SetBackgroundImaging
 * @tc.desc: Test function SetBackgroundImaging, GetBackgroundImage
 */
HWTEST_F(AccessibilityPowerManagerTest, SetBackgroundImaging_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBackgroundImaging_001 start";
    elementInfo_->SetBackgroundImaging("test");
    EXPECT_STREQ(elementInfo_->GetBackgroundImage().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBackgroundImaging_001 end";
}

/**
 * @tc.number: ASetBlur_001
 * @tc.name: AASetBlur
 * @tc.desc: Test function ASetBlur
 */
HWTEST_F(AccessibilityPowerManagerTest, ASetBlur_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ASetBlur_001 start";
    elementInfo_->ASetBlur("test");
    EXPECT_STREQ(elementInfo_->GetBlur().c_str(), "test");
    GTEST_LOG_(INFO) << "ASetBlur_001 end";
}

/**
 * @tc.number: ASetHitTestBehavior_001
 * @tc.name: AASetHitTestBehavior
 * @tc.desc: Test function ASetHitTestBehavior
 */
HWTEST_F(AccessibilityPowerManagerTest, ASetHitTestBehavior_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ASetHitTestBehavior_001 start";
    elementInfo_->ASetHitTestBehavior("test");
    EXPECT_STREQ(elementInfo_->GetHitTestBehavior().c_str(), "test");
    GTEST_LOG_(INFO) << "ASetHitTestBehavior_001 end";
}

/**
 * @tc.number: ASetExtraElement_001
 * @tc.name: AASetExtraElement
 * @tc.desc: Test function ASetExtraElement
 */
HWTEST_F(AccessibilityPowerManagerTest, ASetExtraElement_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ASetExtraElement_001 start";
    ExtraElementInfo extraElementInfo {};
    extraElementInfo.ASetExtraElementInfo("Column", "1");
    elementInfo_->ASetExtraElement(extraElementInfo);
    EXPECT_TRUE(elementInfo_->GetExtraElement().GetExtraElementInfoValueStr().size() == 1);
    GTEST_LOG_(INFO) << "ASetExtraElement_001 end";
}

/**
 * @tc.number: ASetNavDestinationId_001
 * @tc.name: AASetNavDestinationId
 * @tc.desc: Test function ASetNavDestinationId
 */
HWTEST_F(AccessibilityPowerManagerTest, ASetNavDestinationId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ASetNavDestinationId_001 start";
    elementInfo_->ASetNavDestinationId(1);
    EXPECT_TRUE(elementInfo_->GetNavDestinationId() == 1);
    GTEST_LOG_(INFO) << "ASetNavDestinationId_001 end";
}
} // namespace Accessibility
} // namespace OHOS