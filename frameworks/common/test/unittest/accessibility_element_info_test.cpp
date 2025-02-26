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
#include "accessibility_def.h"
#include "accessibility_element_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityElementInfoTest : public ::testing::Test {
public:
    AccessibilityElementInfoTest()
    {}
    ~AccessibilityElementInfoTest()
    {}

    std::shared_ptr<AccessibilityElementInfo> elementInfo_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoTest Start";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoTest End";
    }

    void SetUp()
    {
        elementInfo_ = std::make_shared<AccessibilityElementInfo>();
        ASSERT_TRUE(elementInfo_);
    }

    void TearDown()
    {
        elementInfo_ = nullptr;
    }
};

class RangeInfoTest : public ::testing::Test {
public:
    RangeInfoTest()
    {}
    ~RangeInfoTest()
    {}

    std::shared_ptr<RangeInfo> rangeInfo_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityRangeInfoTest Start";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityRangeInfoTest End";
    }

    void SetUp()
    {
        rangeInfo_ = std::make_shared<RangeInfo>();
        ASSERT_TRUE(rangeInfo_);
    }

    void TearDown()
    {
        rangeInfo_ = nullptr;
    }
};

class GridInfoTest : public ::testing::Test {
public:
    GridInfoTest()
    {}
    ~GridInfoTest()
    {}

    std::shared_ptr<GridInfo> gridInfo_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "GridInfoTest Start";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "GridInfoTest End";
    }

    void SetUp()
    {
        gridInfo_ = std::make_shared<GridInfo>();
        ASSERT_TRUE(gridInfo_);
    }

    void TearDown()
    {
        gridInfo_ = nullptr;
    }
};

class GridItemInfoTest : public ::testing::Test {
public:
    GridItemInfoTest()
    {}
    ~GridItemInfoTest()
    {}

    std::shared_ptr<GridItemInfo> gridItemInfo_ = nullptr;

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "GridItemInfoTest Start";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "GridItemInfoTest End";
    }

    void SetUp()
    {
        gridItemInfo_ = std::make_shared<GridItemInfo>();
        ASSERT_TRUE(gridItemInfo_);
    }

    void TearDown()
    {
        gridItemInfo_ = nullptr;
    }
};

class AccessibleActionTest : public ::testing::Test {
public:
    AccessibleActionTest()
    {}
    ~AccessibleActionTest()
    {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibleActionTest Start";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibleActionTest End";
    }

    void SetUp()
    {}

    void TearDown()
    {}
};

/**
 * @tc.number: SetComponentId_001
 * @tc.name: SetComponentId
 * @tc.desc: Test function SetComponentId, GetAccessibilityId
 */
HWTEST_F(AccessibilityElementInfoTest, SetComponentId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetComponentId_001 start";
    elementInfo_->SetComponentId(0);
    EXPECT_EQ(elementInfo_->GetAccessibilityId(), 0);
    GTEST_LOG_(INFO) << "SetComponentId_001 end";
}

/**
 * @tc.number: GetChildId_001
 * @tc.name: GetChildId
 * @tc.desc: Test function GetChildId
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildId_001 start";
    EXPECT_EQ(elementInfo_->GetChildId(0), -1);
    GTEST_LOG_(INFO) << "GetChildId_001 end";
}

/**
 * @tc.number: GetChildId_002
 * @tc.name: GetChildId
 * @tc.desc: Test function GetChildId
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildId_002 start";
    elementInfo_->AddChild(1);
    EXPECT_EQ(elementInfo_->GetChildId(0), 1);
    GTEST_LOG_(INFO) << "GetChildId_002 end";
}

/**
 * @tc.number: GetChildId_003
 * @tc.name: GetChildId
 * @tc.desc: Test function GetChildId
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildId_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildId_003 start";
    int64_t childId = 1;
    int32_t index = 1;
    elementInfo_->AddChild(childId);
    EXPECT_EQ(elementInfo_->GetChildId(index), -1);
    GTEST_LOG_(INFO) << "GetChildId_003 end";
}

/**
 * @tc.number: GetChildId_004
 * @tc.name: GetChildId
 * @tc.desc: Test function GetChildId
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildId_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildId_004 start";
    EXPECT_EQ(elementInfo_->GetChildId(-1), -1);
    GTEST_LOG_(INFO) << "GetChildId_004 end";
}


/**
 * @tc.number: GetChildCount_001
 * @tc.name: GetChildCount
 * @tc.desc: Test function GetChildCount
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildCount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildCount_001 start";
    EXPECT_EQ(elementInfo_->GetChildCount(), 0);
    GTEST_LOG_(INFO) << "GetChildCount_001 end";
}

/**
 * @tc.number: GetChildIds_001
 * @tc.name: GetChildIds
 * @tc.desc: Test function GetChildIds
 */
HWTEST_F(AccessibilityElementInfoTest, GetChildIds_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildIds_001 start";
    EXPECT_EQ(static_cast<int>(elementInfo_->GetChildIds().size()), 0);
    GTEST_LOG_(INFO) << "GetChildIds_001 end";
}

/**
 * @tc.number: AddChild_001
 * @tc.name: AddChild
 * @tc.desc: Test function AddChild
 */
HWTEST_F(AccessibilityElementInfoTest, AddChild_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddChild_001 start";
    elementInfo_->AddChild(1);
    EXPECT_EQ(elementInfo_->GetChildCount(), 1);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetChildIds().size()), 1);
    GTEST_LOG_(INFO) << "AddChild_001 end";
}

/**
 * @tc.number: AddChild_002
 * @tc.name: AddChild
 * @tc.desc: Test function AddChild
 */
HWTEST_F(AccessibilityElementInfoTest, AddChild_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddChild_002 start";
    elementInfo_->AddChild(1);
    EXPECT_EQ(elementInfo_->GetChildCount(), 1);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetChildIds().size()), 1);
    elementInfo_->AddChild(1);
    EXPECT_EQ(elementInfo_->GetChildCount(), 1);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetChildIds().size()), 1);
    GTEST_LOG_(INFO) << "AddChild_002 end";
}

/**
 * @tc.number: RemoveChild_001
 * @tc.name: RemoveChild
 * @tc.desc: Test function RemoveChild
 */
HWTEST_F(AccessibilityElementInfoTest, RemoveChild_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RemoveChild_001 start";
    EXPECT_FALSE(elementInfo_->RemoveChild(1));
    GTEST_LOG_(INFO) << "RemoveChild_001 end";
}

/**
 * @tc.number: RemoveChild_002
 * @tc.name: RemoveChild
 * @tc.desc: Test function RemoveChild
 */
HWTEST_F(AccessibilityElementInfoTest, RemoveChild_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RemoveChild_002 start";
    elementInfo_->AddChild(1);
    EXPECT_EQ(elementInfo_->GetChildCount(), 1);
    EXPECT_TRUE(elementInfo_->RemoveChild(1));
    GTEST_LOG_(INFO) << "RemoveChild_002 end";
}

/**
 * @tc.number: GetActionList_001
 * @tc.name: GetActionList
 * @tc.desc: Test function GetActionList
 */
HWTEST_F(AccessibilityElementInfoTest, GetActionList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetActionList_001 start";
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 0);
    GTEST_LOG_(INFO) << "GetActionList_001 end";
}

/**
 * @tc.number: AddAction_001
 * @tc.name: AddAction
 * @tc.desc: Test function AddAction
 */
HWTEST_F(AccessibilityElementInfoTest, AddAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddAction_001 start";
    AccessibleAction action(ACCESSIBILITY_ACTION_FOCUS, "test");
    elementInfo_->AddAction(action);
    ASSERT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);
    EXPECT_EQ(elementInfo_->GetActionList().front().GetActionType(), ACCESSIBILITY_ACTION_FOCUS);
    EXPECT_STREQ(elementInfo_->GetActionList().front().GetDescriptionInfo().c_str(), "test");
    GTEST_LOG_(INFO) << "AddAction_001 end";
}

/**
 * @tc.number: DeleteAction_001
 * @tc.name: DeleteAction
 * @tc.desc: Test function DeleteAction
 */
HWTEST_F(AccessibilityElementInfoTest, DeleteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteAction_001 start";
    AccessibleAction action;
    elementInfo_->AddAction(action);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);

    elementInfo_->DeleteAction(action);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 0);
    GTEST_LOG_(INFO) << "DeleteAction_001 end";
}

/**
 * @tc.number: DeleteAction_002
 * @tc.name: DeleteAction
 * @tc.desc: Test function DeleteAction
 */
HWTEST_F(AccessibilityElementInfoTest, DeleteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteAction_002 start";
    ActionType action = ActionType::ACCESSIBILITY_ACTION_FOCUS;
    EXPECT_FALSE(elementInfo_->DeleteAction(action));
    GTEST_LOG_(INFO) << "DeleteAction_002 end";
}

/**
 * @tc.number: DeleteAction_003
 * @tc.name: DeleteAction
 * @tc.desc: Test function DeleteAction
 */
HWTEST_F(AccessibilityElementInfoTest, DeleteAction_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteAction_003 start";
    AccessibleAction action = AccessibleAction(
        ACCESSIBILITY_ACTION_FOCUS, std::string("accessibility_action_focus"));
    AccessibleAction actionSelect = AccessibleAction(
        ACCESSIBILITY_ACTION_SELECT, std::string("accessiblety_action_select"));
    elementInfo_->AddAction(action);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);
    elementInfo_->DeleteAction(actionSelect);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);
    GTEST_LOG_(INFO) << "DeleteAction_003 end";
}

/**
 * @tc.number: DeleteAction_004
 * @tc.name: DeleteAction
 * @tc.desc: Test function DeleteAction
 */
HWTEST_F(AccessibilityElementInfoTest, DeleteAction_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteAction_004 start";
    ActionType actionType = ActionType::ACCESSIBILITY_ACTION_FOCUS;
    AccessibleAction action = AccessibleAction(actionType, std::string("accessibility_aciton_foucus"));
    elementInfo_->AddAction(action);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);
    EXPECT_TRUE(elementInfo_->DeleteAction(actionType));
    GTEST_LOG_(INFO) << "DeleteAction_004 end";
}

/**
 * @tc.number: DeleteAllActions_001
 * @tc.name: DeleteAllActions
 * @tc.desc: Test function DeleteAllActions
 */
HWTEST_F(AccessibilityElementInfoTest, DeleteAllActions_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteAllActions_001 start";
    AccessibleAction action;
    elementInfo_->AddAction(action);
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 1);

    elementInfo_->DeleteAllActions();
    EXPECT_EQ(static_cast<int>(elementInfo_->GetActionList().size()), 0);
    GTEST_LOG_(INFO) << "DeleteAllActions_001 end";
}

/**
 * @tc.number: SetTextLengthLimit_001
 * @tc.name: SetTextLengthLimit
 * @tc.desc: Test function SetTextLengthLimit, GetTextLengthLimit
 */
HWTEST_F(AccessibilityElementInfoTest, SetTextLengthLimit_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTextLengthLimit_001 start";
    elementInfo_->SetTextLengthLimit(1);
    EXPECT_EQ(elementInfo_->GetTextLengthLimit(), 1);
    GTEST_LOG_(INFO) << "SetTextLengthLimit_001 end";
}

/**
 * @tc.number: SetWindowId_001
 * @tc.name: SetWindowId
 * @tc.desc: Test function SetWindowId, GetWindowId
 */
HWTEST_F(AccessibilityElementInfoTest, SetWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetWindowId_001 start";
    elementInfo_->SetWindowId(1);
    EXPECT_EQ(elementInfo_->GetWindowId(), 1);
    GTEST_LOG_(INFO) << "SetWindowId_001 end";
}

/**
 * @tc.number: SetParent_001
 * @tc.name: SetParent
 * @tc.desc: Test function SetParent, GetParentNodeId
 */
HWTEST_F(AccessibilityElementInfoTest, SetParent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetParent_001 start";
    elementInfo_->SetParent(1);
    EXPECT_EQ(elementInfo_->GetParentNodeId(), 1);
    GTEST_LOG_(INFO) << "SetParent_001 end";
}

/**
 * @tc.number: SetRectInScreen_001
 * @tc.name: SetRectInScreen
 * @tc.desc: Test function SetRectInScreen, GetRectInScreen
 */
HWTEST_F(AccessibilityElementInfoTest, SetRectInScreen_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetRectInScreen_001 start";
    Rect bounds(1, 1, 1, 1);
    elementInfo_->SetRectInScreen(bounds);
    EXPECT_EQ(elementInfo_->GetRectInScreen().GetLeftTopXScreenPostion(), 1);
    EXPECT_EQ(elementInfo_->GetRectInScreen().GetLeftTopYScreenPostion(), 1);
    EXPECT_EQ(elementInfo_->GetRectInScreen().GetRightBottomXScreenPostion(), 1);
    EXPECT_EQ(elementInfo_->GetRectInScreen().GetRightBottomYScreenPostion(), 1);
    GTEST_LOG_(INFO) << "SetRectInScreen_001 end";
}

/**
 * @tc.number: SetCheckable_001
 * @tc.name: SetCheckable
 * @tc.desc: Test function SetCheckable, IsCheckable
 */
HWTEST_F(AccessibilityElementInfoTest, SetCheckable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCheckable_001 start";
    elementInfo_->SetCheckable(true);
    EXPECT_TRUE(elementInfo_->IsCheckable());
    GTEST_LOG_(INFO) << "SetCheckable_001 end";
}

/**
 * @tc.number: SetChecked_001
 * @tc.name: SetChecked
 * @tc.desc: Test function SetChecked, IsChecked
 */
HWTEST_F(AccessibilityElementInfoTest, SetChecked_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetChecked_001 start";
    elementInfo_->SetChecked(true);
    EXPECT_TRUE(elementInfo_->IsChecked());
    GTEST_LOG_(INFO) << "SetChecked_001 end";
}

/**
 * @tc.number: SetFocusable_001
 * @tc.name: SetFocusable
 * @tc.desc: Test function SetFocusable, IsFocusable
 */
HWTEST_F(AccessibilityElementInfoTest, SetFocusable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFocusable_001 start";
    elementInfo_->SetFocusable(true);
    EXPECT_TRUE(elementInfo_->IsFocusable());
    GTEST_LOG_(INFO) << "SetFocusable_001 end";
}

/**
 * @tc.number: SetFocused_001
 * @tc.name: SetFocused
 * @tc.desc: Test function SetFocused, IsFocused
 */
HWTEST_F(AccessibilityElementInfoTest, SetFocused_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFocused_001 start";
    elementInfo_->SetFocused(true);
    EXPECT_TRUE(elementInfo_->IsFocused());
    GTEST_LOG_(INFO) << "SetFocused_001 end";
}

/**
 * @tc.number: SetVisible_001
 * @tc.name: SetVisible
 * @tc.desc: Test function SetVisible, IsVisible
 */
HWTEST_F(AccessibilityElementInfoTest, SetVisible_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetVisible_001 start";
    elementInfo_->SetVisible(true);
    EXPECT_TRUE(elementInfo_->IsVisible());
    GTEST_LOG_(INFO) << "SetVisible_001 end";
}

/**
 * @tc.number: SetAccessibilityFocus_001
 * @tc.name: SetAccessibilityFocus
 * @tc.desc: Test function SetAccessibilityFocus, HasAccessibilityFocus
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityFocus_001 start";
    elementInfo_->SetAccessibilityFocus(true);
    EXPECT_TRUE(elementInfo_->HasAccessibilityFocus());
    GTEST_LOG_(INFO) << "SetAccessibilityFocus_001 end";
}

/**
 * @tc.number: SetSelected_001
 * @tc.name: SetSelected
 * @tc.desc: Test function SetSelected, IsSelected
 */
HWTEST_F(AccessibilityElementInfoTest, SetSelected_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSelected_001 start";
    elementInfo_->SetSelected(true);
    EXPECT_TRUE(elementInfo_->IsSelected());
    GTEST_LOG_(INFO) << "SetSelected_001 end";
}

/**
 * @tc.number: SetClickable_001
 * @tc.name: SetClickable
 * @tc.desc: Test function SetClickable, IsClickable
 */
HWTEST_F(AccessibilityElementInfoTest, SetClickable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetClickable_001 start";
    elementInfo_->SetClickable(true);
    EXPECT_TRUE(elementInfo_->IsClickable());
    GTEST_LOG_(INFO) << "SetClickable_001 end";
}

/**
 * @tc.number: SetLongClickable_001
 * @tc.name: SetLongClickable
 * @tc.desc: Test function SetLongClickable, IsLongClickable
 */
HWTEST_F(AccessibilityElementInfoTest, SetLongClickable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetLongClickable_001 start";
    elementInfo_->SetLongClickable(true);
    EXPECT_TRUE(elementInfo_->IsLongClickable());
    GTEST_LOG_(INFO) << "SetLongClickable_001 end";
}

/**
 * @tc.number: SetEnabled_001
 * @tc.name: SetEnabled
 * @tc.desc: Test function SetEnabled, IsEnabled
 */
HWTEST_F(AccessibilityElementInfoTest, SetEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEnabled_001 start";
    elementInfo_->SetEnabled(true);
    EXPECT_TRUE(elementInfo_->IsEnabled());
    GTEST_LOG_(INFO) << "SetEnabled_001 end";
}

/**
 * @tc.number: SetPassword_001
 * @tc.name: SetPassword
 * @tc.desc: Test function SetPassword, IsPassword
 */
HWTEST_F(AccessibilityElementInfoTest, SetPassword_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPassword_001 start";
    elementInfo_->SetPassword(true);
    EXPECT_TRUE(elementInfo_->IsPassword());
    GTEST_LOG_(INFO) << "SetPassword_001 end";
}

/**
 * @tc.number: SetScrollable_001
 * @tc.name: SetScrollable
 * @tc.desc: Test function SetScrollable, IsScrollable
 */
HWTEST_F(AccessibilityElementInfoTest, SetScrollable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetScrollable_001 start";
    elementInfo_->SetScrollable(true);
    EXPECT_TRUE(elementInfo_->IsScrollable());
    GTEST_LOG_(INFO) << "SetScrollable_001 end";
}

/**
 * @tc.number: SetEditable_001
 * @tc.name: SetEditable
 * @tc.desc: Test function SetEditable, IsEditable
 */
HWTEST_F(AccessibilityElementInfoTest, SetEditable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEditable_001 start";
    elementInfo_->SetEditable(true);
    EXPECT_TRUE(elementInfo_->IsEditable());
    GTEST_LOG_(INFO) << "SetEditable_001 end";
}

/**
 * @tc.number: SetPluraLineSupported_001
 * @tc.name: SetPluraLineSupported
 * @tc.desc: Test function SetPluraLineSupported, IsPluraLineSupported
 */
HWTEST_F(AccessibilityElementInfoTest, SetPluraLineSupported_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPluraLineSupported_001 start";
    elementInfo_->SetPluraLineSupported(true);
    EXPECT_TRUE(elementInfo_->IsPluraLineSupported());
    GTEST_LOG_(INFO) << "SetPluraLineSupported_001 end";
}

/**
 * @tc.number: SetPopupSupported_001
 * @tc.name: SetPopupSupported
 * @tc.desc: Test function SetPopupSupported, IsPopupSupported
 */
HWTEST_F(AccessibilityElementInfoTest, SetPopupSupported_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPopupSupported_001 start";
    elementInfo_->SetPopupSupported(true);
    EXPECT_TRUE(elementInfo_->IsPopupSupported());
    GTEST_LOG_(INFO) << "SetPopupSupported_001 end";
}

/**
 * @tc.number: SetDeletable_001
 * @tc.name: SetDeletable
 * @tc.desc: Test function SetDeletable, IsDeletable
 */
HWTEST_F(AccessibilityElementInfoTest, SetDeletable_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDeletable_001 start";
    elementInfo_->SetDeletable(true);
    EXPECT_TRUE(elementInfo_->IsDeletable());
    GTEST_LOG_(INFO) << "SetDeletable_001 end";
}

/**
 * @tc.number: SetEssential_001
 * @tc.name: SetEssential
 * @tc.desc: Test function SetEssential, IsEssential
 */
HWTEST_F(AccessibilityElementInfoTest, SetEssential_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEssential_001 start";
    elementInfo_->SetEssential(true);
    EXPECT_TRUE(elementInfo_->IsEssential());
    GTEST_LOG_(INFO) << "SetEssential_001 end";
}

/**
 * @tc.number: SetHinting_001
 * @tc.name: SetHinting
 * @tc.desc: Test function SetHinting, IsGivingHint
 */
HWTEST_F(AccessibilityElementInfoTest, SetHinting_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHinting_001 start";
    elementInfo_->SetHinting(true);
    EXPECT_TRUE(elementInfo_->IsGivingHint());
    GTEST_LOG_(INFO) << "SetHinting_001 end";
}

/**
 * @tc.number: SetBundleName_001
 * @tc.name: SetBundleName
 * @tc.desc: Test function SetBundleName, GetBundleName
 */
HWTEST_F(AccessibilityElementInfoTest, SetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBundleName_001 start";
    elementInfo_->SetBundleName("test");
    EXPECT_STREQ(elementInfo_->GetBundleName().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBundleName_001 end";
}

/**
 * @tc.number: SetComponentType_001
 * @tc.name: SetComponentType
 * @tc.desc: Test function SetComponentType, GetComponentType
 */
HWTEST_F(AccessibilityElementInfoTest, SetComponentType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetComponentType_001 start";
    elementInfo_->SetComponentType("test");
    EXPECT_STREQ(elementInfo_->GetComponentType().c_str(), "test");
    GTEST_LOG_(INFO) << "SetComponentType_001 end";
}

/**
 * @tc.number: SetContent_001
 * @tc.name: SetContent
 * @tc.desc: Test function SetContent, GetContent
 */
HWTEST_F(AccessibilityElementInfoTest, SetContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContent_001 start";
    elementInfo_->SetContent("test");
    EXPECT_STREQ(elementInfo_->GetContent().c_str(), "test");
    GTEST_LOG_(INFO) << "SetContent_001 end";
}

/**
 * @tc.number: SetHint_001
 * @tc.name: SetHint
 * @tc.desc: Test function SetHint, GetHint
 */
HWTEST_F(AccessibilityElementInfoTest, SetHint_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHint_001 start";
    elementInfo_->SetHint("test");
    EXPECT_STREQ(elementInfo_->GetHint().c_str(), "test");
    GTEST_LOG_(INFO) << "SetHint_001 end";
}

/**
 * @tc.number: SetDescriptionInfo_001
 * @tc.name: SetDescriptionInfo
 * @tc.desc: Test function SetDescriptionInfo, GetDescriptionInfo
 */
HWTEST_F(AccessibilityElementInfoTest, SetDescriptionInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetDescriptionInfo_001 start";
    elementInfo_->SetDescriptionInfo("test");
    EXPECT_STREQ(elementInfo_->GetDescriptionInfo().c_str(), "test");
    GTEST_LOG_(INFO) << "SetDescriptionInfo_001 end";
}

/**
 * @tc.number: SetComponentResourceId_001
 * @tc.name: SetComponentResourceId
 * @tc.desc: Test function SetComponentResourceId, GetComponentResourceId
 */
HWTEST_F(AccessibilityElementInfoTest, SetComponentResourceId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetComponentResourceId_001 start";
    elementInfo_->SetComponentResourceId("test");
    EXPECT_STREQ(elementInfo_->GetComponentResourceId().c_str(), "test");
    GTEST_LOG_(INFO) << "SetComponentResourceId_001 end";
}

/**
 * @tc.number: SetLiveRegion_001
 * @tc.name: SetLiveRegion
 * @tc.desc: Test function SetLiveRegion, GetLiveRegion
 */
HWTEST_F(AccessibilityElementInfoTest, SetLiveRegion_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetLiveRegion_001 start";
    elementInfo_->SetLiveRegion(1);
    EXPECT_EQ(elementInfo_->GetLiveRegion(), 1);
    GTEST_LOG_(INFO) << "SetLiveRegion_001 end";
}

/**
 * @tc.number: SetContentInvalid_001
 * @tc.name: SetContentInvalid
 * @tc.desc: Test function SetContentInvalid, GetContentInvalid
 */
HWTEST_F(AccessibilityElementInfoTest, SetContentInvalid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentInvalid_001 start";
    elementInfo_->SetContentInvalid(false);
    EXPECT_FALSE(elementInfo_->GetContentInvalid());
    GTEST_LOG_(INFO) << "SetContentInvalid_001 end";
}

/**
 * @tc.number: SetError_001
 * @tc.name: SetError
 * @tc.desc: Test function SetError, GetError
 */
HWTEST_F(AccessibilityElementInfoTest, SetError_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetError_001 start";
    elementInfo_->SetError("test");
    EXPECT_STREQ(elementInfo_->GetError().c_str(), "test");
    GTEST_LOG_(INFO) << "SetError_001 end";
}

/**
 * @tc.number: SetLabeled_001
 * @tc.name: SetLabeled
 * @tc.desc: Test function SetLabeled, GetLabeledAccessibilityId
 */
HWTEST_F(AccessibilityElementInfoTest, SetLabeled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetLabeled_001 start";
    elementInfo_->SetLabeled(1);
    EXPECT_EQ(elementInfo_->GetLabeledAccessibilityId(), 1);
    GTEST_LOG_(INFO) << "SetLabeled_001 end";
}

/**
 * @tc.number: SetAccessibilityId_001
 * @tc.name: SetAccessibilityId
 * @tc.desc: Test function SetAccessibilityId, GetAccessibilityId
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityId_001 start";
    elementInfo_->SetAccessibilityId(1);
    EXPECT_EQ(elementInfo_->GetAccessibilityId(), 1);
    GTEST_LOG_(INFO) << "SetAccessibilityId_001 end";
}

/**
 * @tc.number: SetRange_001
 * @tc.name: SetRange
 * @tc.desc: Test function SetRange, GetRange
 */
HWTEST_F(AccessibilityElementInfoTest, SetRange_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetRange_001 start";
    RangeInfo rangeInfo(1, 1, 1);
    elementInfo_->SetRange(rangeInfo);
    EXPECT_EQ(elementInfo_->GetRange().GetMin(), 1);
    EXPECT_EQ(elementInfo_->GetRange().GetMax(), 1);
    EXPECT_EQ(elementInfo_->GetRange().GetCurrent(), 1);
    GTEST_LOG_(INFO) << "SetRange_001 end";
}

/**
 * @tc.number: SetSelectedBegin_001
 * @tc.name: SetSelectedBegin
 * @tc.desc: Test function SetSelectedBegin, GetSelectedBegin
 */
HWTEST_F(AccessibilityElementInfoTest, SetSelectedBegin_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSelectedBegin_001 start";
    elementInfo_->SetSelectedBegin(1);
    EXPECT_EQ(elementInfo_->GetSelectedBegin(), 1);
    GTEST_LOG_(INFO) << "SetSelectedBegin_001 end";
}

/**
 * @tc.number: SetSelectedEnd_001
 * @tc.name: SetSelectedEnd
 * @tc.desc: Test function SetSelectedEnd, GetSelectedEnd
 */
HWTEST_F(AccessibilityElementInfoTest, SetSelectedEnd_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSelectedEnd_001 start";
    elementInfo_->SetSelectedEnd(1);
    EXPECT_EQ(elementInfo_->GetSelectedEnd(), 1);
    GTEST_LOG_(INFO) << "SetSelectedEnd_001 end";
}

/**
 * @tc.number: SetGrid_001
 * @tc.name: SetGrid
 * @tc.desc: Test function SetGrid, GetGrid
 */
HWTEST_F(AccessibilityElementInfoTest, SetGrid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGrid_001 start";
    GridInfo grid(1, 1, 1);
    elementInfo_->SetGrid(grid);
    EXPECT_EQ(elementInfo_->GetGrid().GetRowCount(), 1);
    EXPECT_EQ(elementInfo_->GetGrid().GetColumnCount(), 1);
    EXPECT_EQ(elementInfo_->GetGrid().GetSelectionMode(), 1);
    GTEST_LOG_(INFO) << "SetGrid_001 end";
}

/**
 * @tc.number: SetGridItem_001
 * @tc.name: SetGridItem
 * @tc.desc: Test function SetGridItem, GetGridItem
 */
HWTEST_F(AccessibilityElementInfoTest, SetGridItem_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGridItem_001 start";
    GridItemInfo gridItem(1, 1, 1, 1, true, true);
    elementInfo_->SetGridItem(gridItem);
    EXPECT_EQ(elementInfo_->GetGridItem().GetColumnIndex(), 1);
    EXPECT_EQ(elementInfo_->GetGridItem().GetRowIndex(), 1);
    EXPECT_EQ(elementInfo_->GetGridItem().GetColumnSpan(), 1);
    EXPECT_EQ(elementInfo_->GetGridItem().GetRowSpan(), 1);
    EXPECT_TRUE(elementInfo_->GetGridItem().IsHeading());
    EXPECT_TRUE(elementInfo_->GetGridItem().IsSelected());
    GTEST_LOG_(INFO) << "SetGridItem_001 end";
}

/**
 * @tc.number: SetCurrentIndex_001
 * @tc.name: SetCurrentIndex
 * @tc.desc: Test function SetCurrentIndex, GetCurrentIndex
 */
HWTEST_F(AccessibilityElementInfoTest, SetCurrentIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCurrentIndex_001 start";
    elementInfo_->SetCurrentIndex(1);
    EXPECT_EQ(elementInfo_->GetCurrentIndex(), 1);
    GTEST_LOG_(INFO) << "SetCurrentIndex_001 end";
}

/**
 * @tc.number: SetBeginIndex_001
 * @tc.name: SetBeginIndex
 * @tc.desc: Test function SetBeginIndex, GetBeginIndex
 */
HWTEST_F(AccessibilityElementInfoTest, SetBeginIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBeginIndex_001 start";
    elementInfo_->SetBeginIndex(1);
    EXPECT_EQ(elementInfo_->GetBeginIndex(), 1);
    GTEST_LOG_(INFO) << "SetBeginIndex_001 end";
}

/**
 * @tc.number: SetEndIndex_001
 * @tc.name: SetEndIndex
 * @tc.desc: Test function SetEndIndex, GetEndIndex
 */
HWTEST_F(AccessibilityElementInfoTest, SetEndIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEndIndex_001 start";
    elementInfo_->SetEndIndex(1);
    EXPECT_EQ(elementInfo_->GetEndIndex(), 1);
    GTEST_LOG_(INFO) << "SetEndIndex_001 end";
}

/**
 * @tc.number: SetInputType_001
 * @tc.name: SetInputType
 * @tc.desc: Test function SetInputType, GetInputType
 */
HWTEST_F(AccessibilityElementInfoTest, SetInputType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInputType_001 start";
    elementInfo_->SetInputType(1);
    EXPECT_EQ(elementInfo_->GetInputType(), 1);
    GTEST_LOG_(INFO) << "SetInputType_001 end";
}

/**
 * @tc.number: SetValidElement_001
 * @tc.name: SetValidElement
 * @tc.desc: Test function SetValidElement, IsValidElement
 */
HWTEST_F(AccessibilityElementInfoTest, SetValidElement_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetValidElement_001 start";
    elementInfo_->SetValidElement(false);
    EXPECT_FALSE(elementInfo_->IsValidElement());
    GTEST_LOG_(INFO) << "SetValidElement_001 end";
}

/**
 * @tc.number: SetInspectorKey_001
 * @tc.name: SetInspectorKey
 * @tc.desc: Test function SetInspectorKey, GetInspectorKey
 */
HWTEST_F(AccessibilityElementInfoTest, SetInspectorKey_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetInspectorKey_001 start";
    elementInfo_->SetInspectorKey("test");
    EXPECT_STREQ(elementInfo_->GetInspectorKey().c_str(), "test");
    GTEST_LOG_(INFO) << "SetInspectorKey_001 end";
}

/**
 * @tc.number: SetPagePath_001
 * @tc.name: SetPagePath
 * @tc.desc: Test function SetPagePath, GetPagePath
 */
HWTEST_F(AccessibilityElementInfoTest, SetPagePath_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPagePath_001 start";
    elementInfo_->SetPagePath("test");
    EXPECT_STREQ(elementInfo_->GetPagePath().c_str(), "test");
    GTEST_LOG_(INFO) << "SetPagePath_001 end";
}

/**
 * @tc.number: SetPageId_001
 * @tc.name: SetPageId
 * @tc.desc: Test function SetPageId, GetPageId
 */
HWTEST_F(AccessibilityElementInfoTest, SetPageId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPageId_001 start";
    elementInfo_->SetPageId(1);
    EXPECT_EQ(elementInfo_->GetPageId(), 1);
    GTEST_LOG_(INFO) << "SetPageId_001 end";
}

/**
 * @tc.number: SetTextMovementStep_001
 * @tc.name: SetTextMovementStep
 * @tc.desc: Test function SetTextMovementStep, GetTextMovementStep
 */
HWTEST_F(AccessibilityElementInfoTest, SetTextMovementStep_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTextMovementStep_001 start";
    elementInfo_->SetTextMovementStep(STEP_CHARACTER);
    EXPECT_EQ(elementInfo_->GetTextMovementStep(), STEP_CHARACTER);
    GTEST_LOG_(INFO) << "SetTextMovementStep_001 end";
}

/**
 * @tc.number: SetItemCounts_001
 * @tc.name: SetItemCounts
 * @tc.desc: Test function SetItemCounts, GetItemCounts
 */
HWTEST_F(AccessibilityElementInfoTest, SetItemCounts_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetItemCounts_001 start";
    elementInfo_->SetItemCounts(1);
    EXPECT_EQ(elementInfo_->GetItemCounts(), 1);
    GTEST_LOG_(INFO) << "SetItemCounts_001 end";
}

/**
 * @tc.number: SetTriggerAction_001
 * @tc.name: SetTriggerAction
 * @tc.desc: Test function SetTriggerAction, GetTriggerAction
 */
HWTEST_F(AccessibilityElementInfoTest, SetTriggerAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTriggerAction_001 start";
    elementInfo_->SetTriggerAction(ACCESSIBILITY_ACTION_FOCUS);
    EXPECT_EQ(elementInfo_->GetTriggerAction(), ACCESSIBILITY_ACTION_FOCUS);
    GTEST_LOG_(INFO) << "SetTriggerAction_001 end";
}

/**
 * @tc.number: SetContentList_001
 * @tc.name: SetContentList
 * @tc.desc: Test function SetContentList, GetContentList
 */
HWTEST_F(AccessibilityElementInfoTest, SetContentList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetContentList_001 start";
    std::vector<std::string> contentList;
    contentList.push_back("test");
    elementInfo_->SetContentList(contentList);

    std::vector<std::string> result;
    elementInfo_->GetContentList(result);
    ASSERT_EQ(static_cast<int>(result.size()), 1);
    EXPECT_STREQ(result.front().c_str(), "test");
    GTEST_LOG_(INFO) << "SetContentList_001 end";
}

/**
 * @tc.number: SetLatestContent_001
 * @tc.name: SetLatestContent
 * @tc.desc: Test function SetLatestContent, GetLatestContent
 */
HWTEST_F(AccessibilityElementInfoTest, SetLatestContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetLatestContent_001 start";
    elementInfo_->SetLatestContent("test");
    EXPECT_STREQ(elementInfo_->GetLatestContent().c_str(), "test");
    GTEST_LOG_(INFO) << "SetLatestContent_001 end";
}

/**
 * @tc.number: SetMin_001
 * @tc.name: SetMin
 * @tc.desc: Test function SetMin, GetMin
 */
HWTEST_F(RangeInfoTest, SetMin_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMin_001 start";
    rangeInfo_->SetMin(1);
    EXPECT_EQ(rangeInfo_->GetMin(), 1);
    GTEST_LOG_(INFO) << "SetMin_001 end";
}

/**
 * @tc.number: SetMax_001
 * @tc.name: SetMax
 * @tc.desc: Test function SetMax, GetMax
 */
HWTEST_F(RangeInfoTest, SetMax_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMax_001 start";
    rangeInfo_->SetMax(10);
    EXPECT_EQ(rangeInfo_->GetMax(), 10);
    GTEST_LOG_(INFO) << "SetMax_001 end";
}

/**
 * @tc.number: SetCurrent_001
 * @tc.name: SetCurrent
 * @tc.desc: Test function SetCurrent, GetMax
 */
HWTEST_F(RangeInfoTest, SetCurrent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCurrent_001 start";
    rangeInfo_->SetCurrent(5);
    EXPECT_EQ(rangeInfo_->GetCurrent(), 5);
    GTEST_LOG_(INFO) << "SetCurrent_001 end";
}

/**
 * @tc.number: SetGrid_001
 * @tc.name: SetGrid
 * @tc.desc: Test function SetGrid, GetRowCount, GetColumnCount, GetSelectionMode
 */
HWTEST_F(GridInfoTest, SetGrid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGrid_001 start";
    gridInfo_->SetGrid(1, 2, 3);
    EXPECT_EQ(gridInfo_->GetRowCount(), 1);
    EXPECT_EQ(gridInfo_->GetColumnCount(), 2);
    EXPECT_EQ(gridInfo_->GetSelectionMode(), 3);
    GTEST_LOG_(INFO) << "SetGrid_001 end";
}

/**
 * @tc.number: SetGrid_002
 * @tc.name: SetGrid
 * @tc.desc: Test function SetGrid, GetRowCount, GetColumnCount, GetSelectionMode
 */
HWTEST_F(GridInfoTest, SetGrid_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGrid_002 start";
    GridInfo grid(11, 12, 13);
    gridInfo_->SetGrid(grid);
    EXPECT_EQ(gridInfo_->GetRowCount(), 11);
    EXPECT_EQ(gridInfo_->GetColumnCount(), 12);
    EXPECT_EQ(gridInfo_->GetSelectionMode(), 13);
    GTEST_LOG_(INFO) << "SetGrid_002 end";
}

/**
 * @tc.number: SetGridItemInfo_001
 * @tc.name: SetGridItemInfo
 * @tc.desc: Test function SetGridItemInfo, GetRowIndex, GetRowSpan, GetColumnIndex, GetColumnSpan, IsHeading,
 *           IsSelected
 */
HWTEST_F(GridItemInfoTest, SetGridItemInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGridItemInfo_001 start";
    gridItemInfo_->SetGridItemInfo(1, 1, 1, 1, true, true);
    EXPECT_EQ(gridItemInfo_->GetRowIndex(), 1);
    EXPECT_EQ(gridItemInfo_->GetRowSpan(), 1);
    EXPECT_EQ(gridItemInfo_->GetColumnIndex(), 1);
    EXPECT_EQ(gridItemInfo_->GetColumnSpan(), 1);
    EXPECT_TRUE(gridItemInfo_->IsHeading());
    EXPECT_TRUE(gridItemInfo_->IsSelected());
    GTEST_LOG_(INFO) << "SetGridItemInfo_001 end";
}

/**
 * @tc.number: SetGridItemInfo_002
 * @tc.name: SetGridItemInfo
 * @tc.desc: Test function SetGridItemInfo, GetRowIndex, GetRowSpan, GetColumnIndex, GetColumnSpan, IsHeading,
 *           IsSelected
 */
HWTEST_F(GridItemInfoTest, SetGridItemInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetGridItemInfo_002 start";
    GridItemInfo itemInfo(11, 12, 13, 14, false, false);
    gridItemInfo_->SetGridItemInfo(itemInfo);
    EXPECT_EQ(gridItemInfo_->GetRowIndex(), 11);
    EXPECT_EQ(gridItemInfo_->GetRowSpan(), 12);
    EXPECT_EQ(gridItemInfo_->GetColumnIndex(), 13);
    EXPECT_EQ(gridItemInfo_->GetColumnSpan(), 14);
    EXPECT_FALSE(gridItemInfo_->IsHeading());
    EXPECT_FALSE(gridItemInfo_->IsSelected());
    GTEST_LOG_(INFO) << "SetGridItemInfo_002 end";
}

/**
 * @tc.number: AccessibleAction_001
 * @tc.name: AccessibleAction
 * @tc.desc: Test function AccessibleAction, GetActionType, GetDescriptionInfo
 */
HWTEST_F(AccessibleActionTest, AccessibleAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAction_001 start";
    std::shared_ptr<AccessibleAction> action = std::make_shared<AccessibleAction>(ACCESSIBILITY_ACTION_FOCUS, "test");
    ASSERT_TRUE(action);
    EXPECT_EQ(action->GetActionType(), ACCESSIBILITY_ACTION_FOCUS);
    EXPECT_STREQ(action->GetDescriptionInfo().c_str(), "test");
    action = nullptr;
    GTEST_LOG_(INFO) << "AccessibleAction_001 end";
}

/**
 * @tc.number: AccessibleAction_002
 * @tc.name: AccessibleAction
 * @tc.desc: Test function AccessibleAction, GetActionType, GetDescriptionInfo
 */
HWTEST_F(AccessibleActionTest, AccessibleAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAction_002 start";
    std::shared_ptr<AccessibleAction> action = std::make_shared<AccessibleAction>();
    ASSERT_TRUE(action);
    EXPECT_EQ(action->GetActionType(), ACCESSIBILITY_ACTION_INVALID);
    EXPECT_STREQ(action->GetDescriptionInfo().c_str(), "");
    action = nullptr;
    GTEST_LOG_(INFO) << "AccessibleAction_002 end";
}

/**
 * @tc.number: Rect_001
 * @tc.name: Rect
 * @tc.desc: Test function Rect
 */
HWTEST_F(AccessibilityElementInfoTest, Rect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Rect_001 start";
    Rect rect(1, 1, 1, 1);
    EXPECT_EQ(rect.GetLeftTopXScreenPostion(), 1);
    EXPECT_EQ(rect.GetLeftTopYScreenPostion(), 1);
    EXPECT_EQ(rect.GetRightBottomXScreenPostion(), 1);
    EXPECT_EQ(rect.GetRightBottomYScreenPostion(), 1);
    GTEST_LOG_(INFO) << "Rect_001 end";
}

/**
 * @tc.number: SetAccessibilityText_001
 * @tc.name: SetAccessibilityText
 * @tc.desc: Test function SetAccessibilityText, GetAccessibilityText
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityText_001 start";
    elementInfo_->SetAccessibilityText("test");
    EXPECT_STREQ(elementInfo_->GetAccessibilityText().c_str(), "test");
    GTEST_LOG_(INFO) << "SetAccessibilityText_001 end";
}

/**
 * @tc.number: SetTextType_001
 * @tc.name: SetTextType
 * @tc.desc: Test function SetTextType, GetTextType
 */
HWTEST_F(AccessibilityElementInfoTest, SetTextType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTextType_001 start";
    elementInfo_->SetTextType("test");
    EXPECT_STREQ(elementInfo_->GetTextType().c_str(), "test");
    GTEST_LOG_(INFO) << "SetTextType_001 end";
}

/**
 * @tc.number: SetOffset_001
 * @tc.name: SetOffset
 * @tc.desc: Test function SetOffset, GetOffset
 */
HWTEST_F(AccessibilityElementInfoTest, SetOffset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetOffset_001 start";
    elementInfo_->SetOffset(6.6f);
    EXPECT_FLOAT_EQ(elementInfo_->GetOffset(), 6.6f);
    GTEST_LOG_(INFO) << "SetOffset_001 end";
}

/**
 * @tc.number: SetChildTreeIdAndWinId_001
 * @tc.name: SetChildTreeIdAndWinId
 * @tc.desc: Test function SetChildTreeIdAndWinId GetChildTreeId GetChildWindowId
 */
HWTEST_F(AccessibilityElementInfoTest, SetChildTreeIdAndWinId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetChildTreeIdAndWinId_001 start";
    elementInfo_->SetChildTreeIdAndWinId(1, 2);
    EXPECT_EQ(elementInfo_->GetChildTreeId(), 1);
    EXPECT_EQ(elementInfo_->GetChildWindowId(), 2);
    GTEST_LOG_(INFO) << "SetChildTreeIdAndWinId_001 end";
}

/**
 * @tc.number: SetBelongTreeId_001
 * @tc.name: SetBelongTreeId
 * @tc.desc: Test function SetBelongTreeId, GetBelongTreeId
 */
HWTEST_F(AccessibilityElementInfoTest, SetBelongTreeId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBelongTreeId_001 start";
    elementInfo_->SetBelongTreeId(1);
    EXPECT_EQ(elementInfo_->GetBelongTreeId(), 1);
    GTEST_LOG_(INFO) << "SetBelongTreeId_001 end";
}

/**
 * @tc.number: SetParentWindowId_001
 * @tc.name: SetParentWindowId
 * @tc.desc: Test function SetParentWindowId, GetParentWindowId
 */
HWTEST_F(AccessibilityElementInfoTest, SetParentWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetParentWindowId_001 start";
    elementInfo_->SetParentWindowId(1);
    EXPECT_EQ(elementInfo_->GetParentWindowId(), 1);
    GTEST_LOG_(INFO) << "SetParentWindowId_001 end";
}

/**
 * @tc.number: SetAccessibilityGroup_001
 * @tc.name: SetAccessibilityGroup
 * @tc.desc: Test function SetAccessibilityGroup, GetAccessibilityGroup
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityGroup_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityGroup_001 start";
    elementInfo_->SetAccessibilityGroup(false);
    EXPECT_FALSE(elementInfo_->GetAccessibilityGroup());
    GTEST_LOG_(INFO) << "SetAccessibilityGroup_001 end";
}

/**
 * @tc.number: SetAccessibilityLevel_001
 * @tc.name: SetAccessibilityLevel
 * @tc.desc: Test function SetAccessibilityLevel, GetAccessibilityLevel
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityLevel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityLevel_001 start";
    elementInfo_->SetAccessibilityLevel("test");
    EXPECT_STREQ(elementInfo_->GetAccessibilityLevel().c_str(), "test");
    GTEST_LOG_(INFO) << "SetAccessibilityLevel_001 end";
}

/**
 * @tc.number: SetZIndex_001
 * @tc.name: SetZIndex
 * @tc.desc: Test function SetZIndex, GetZIndex
 */
HWTEST_F(AccessibilityElementInfoTest, SetZIndex_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetZIndex_001 start";
    elementInfo_->SetZIndex(1);
    EXPECT_EQ(elementInfo_->GetZIndex(), 1);
    GTEST_LOG_(INFO) << "SetZIndex_001 end";
}

/**
 * @tc.number: SetOpacity_001
 * @tc.name: SetOpacity
 * @tc.desc: Test function SetOpacity, GetOpacity
 */
HWTEST_F(AccessibilityElementInfoTest, SetOpacity_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetOpacity_001 start";
    elementInfo_->SetOpacity(0.5f);
    EXPECT_FLOAT_EQ(elementInfo_->GetOpacity(), 0.5f);
    GTEST_LOG_(INFO) << "SetOpacity_001 end";
}

/**
 * @tc.number: SetBackgroundColor_001
 * @tc.name: SetBackgroundColor
 * @tc.desc: Test function SetBackgroundColor, GetBackgroundColor
 */
HWTEST_F(AccessibilityElementInfoTest, SetBackgroundColor_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBackgroundColor_001 start";
    elementInfo_->SetBackgroundColor("test");
    EXPECT_STREQ(elementInfo_->GetBackgroundColor().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBackgroundColor_001 end";
}

/**
 * @tc.number: SetBackgroundImage_001
 * @tc.name: SetBackgroundImage
 * @tc.desc: Test function SetBackgroundImage, GetBackgroundImage
 */
HWTEST_F(AccessibilityElementInfoTest, SetBackgroundImage_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBackgroundImage_001 start";
    elementInfo_->SetBackgroundImage("test");
    EXPECT_STREQ(elementInfo_->GetBackgroundImage().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBackgroundImage_001 end";
}

/**
 * @tc.number: SetBlur_001
 * @tc.name: SetBlur
 * @tc.desc: Test function SetBlur, GetBlur
 */
HWTEST_F(AccessibilityElementInfoTest, SetBlur_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetBlur_001 start";
    elementInfo_->SetBlur("test");
    EXPECT_STREQ(elementInfo_->GetBlur().c_str(), "test");
    GTEST_LOG_(INFO) << "SetBlur_001 end";
}

/**
 * @tc.number: SetHitTestBehavior_001
 * @tc.name: SetHitTestBehavior
 * @tc.desc: Test function SetHitTestBehavior, GetHitTestBehavior
 */
HWTEST_F(AccessibilityElementInfoTest, SetHitTestBehavior_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetHitTestBehavior_001 start";
    elementInfo_->SetHitTestBehavior("test");
    EXPECT_STREQ(elementInfo_->GetHitTestBehavior().c_str(), "test");
    GTEST_LOG_(INFO) << "SetHitTestBehavior_001 end";
}

/**
 * @tc.number: SetExtraElement_001
 * @tc.name: SetExtraElement
 * @tc.desc: Test function SetExtraElement, GetExtraElement
 */
HWTEST_F(AccessibilityElementInfoTest, SetExtraElement_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetExtraElement_001 start";
    ExtraElementInfo extraElementInfo {};
    extraElementInfo.SetExtraElementInfo("Column", "1");
    elementInfo_->SetExtraElement(extraElementInfo);
    EXPECT_TRUE(elementInfo_->GetExtraElement().GetExtraElementInfoValueStr().size() == 1);
    GTEST_LOG_(INFO) << "SetExtraElement_001 end";
}

/**
 * @tc.number: SetNavDestinationId_001
 * @tc.name: SetNavDestinationId
 * @tc.desc: Test function SetNavDestinationId, GetNavDestinationId
 */
HWTEST_F(AccessibilityElementInfoTest, SetNavDestinationId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetNavDestinationId_001 start";
    elementInfo_->SetNavDestinationId(1);
    EXPECT_TRUE(elementInfo_->GetNavDestinationId() == 1);
    GTEST_LOG_(INFO) << "SetNavDestinationId_001 end";
}

/**
 * @tc.number: SetSpanList_001
 * @tc.name: SetSpanList
 * @tc.desc: Test function AddSpan, SetSpanList
 */
HWTEST_F(AccessibilityElementInfoTest, SetSpanList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetSpanList_001 start";
    SpanInfo span;
    span.SetSpanId(1);
    EXPECT_TRUE(span.GetSpanId() == 1);
    span.SetSpanText("spanText");
    EXPECT_TRUE(span.GetSpanText() == "spanText");
    span.SetAccessibilityText("accessibilityText");
    EXPECT_TRUE(span.GetAccessibilityText() == "accessibilityText");
    span.SetAccessibilityDescription("accessibilityDescription");
    EXPECT_TRUE(span.GetAccessibilityDescription() == "accessibilityDescription");
    span.SetAccessibilityLevel("accessibilityLevel");
    EXPECT_TRUE(span.GetAccessibilityLevel() == "accessibilityLevel");

    elementInfo_->AddSpan(span);
    EXPECT_TRUE(elementInfo_->GetSpanList().size() == 1);

    std::vector<SpanInfo> spanList;
    spanList.push_back(span);
    spanList.push_back(span);
    elementInfo_->SetSpanList(spanList);
    EXPECT_TRUE(elementInfo_->GetSpanList().size() == 2);

    GTEST_LOG_(INFO) << "SetSpanList_001 end";
}

/**
 * @tc.number: SetIsActive_001
 * @tc.name: SetIsActive
 * @tc.desc: Test function SetIsActive, GetIsActive
 */
HWTEST_F(AccessibilityElementInfoTest, SetIsActive_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetIsActive_001 start";
    elementInfo_->SetIsActive(true);
    EXPECT_TRUE(elementInfo_->GetIsActive());
    GTEST_LOG_(INFO) << "SetIsActive_001 end";
}

/**
 * @tc.number: SetAccessibilityVisible_001
 * @tc.name: SetAccessibilityVisible
 * @tc.desc: Test function SetAccessibilityVisible, GetAccessibilityVisible
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityVisible_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityVisible_001 start";
    elementInfo_->SetAccessibilityVisible(true);
    EXPECT_TRUE(elementInfo_->GetAccessibilityVisible());
    GTEST_LOG_(INFO) << "SetAccessibilityVisible_001 end";
}

/**
 * @tc.number: SetClip_001
 * @tc.name: SetClip
 * @tc.desc: Test function SetClip, GetClip
 */
HWTEST_F(AccessibilityElementInfoTest, SetClip_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetClip_001 start";
    elementInfo_->SetClip(true);
    EXPECT_TRUE(elementInfo_->GetClip());
    GTEST_LOG_(INFO) << "SetClip_001 end";
}

/**
 * @tc.number: SetMainWindowId_001
 * @tc.name: SetMainWindowId
 * @tc.desc: Test function SetMainWindowId, GetMainWindowId
 */
HWTEST_F(AccessibilityElementInfoTest, SetMainWindowId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetMainWindowId_001 start";
    elementInfo_->SetMainWindowId(1);
    EXPECT_TRUE(elementInfo_->GetMainWindowId() == 1);
    GTEST_LOG_(INFO) << "SetMainWindowId_001 end";
}

/**
 * @tc.number: SetCustomComponentType_001
 * @tc.name: SetCustomComponentType
 * @tc.desc: Test function SetCustomComponentType, GetCustomComponentType
 */
HWTEST_F(AccessibilityElementInfoTest, SetCustomComponentType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCustomComponentType_001 start";
    elementInfo_->SetCustomComponentType("Button");
    EXPECT_TRUE(elementInfo_->GetCustomComponentType() == "Button");
    GTEST_LOG_(INFO) << "SetCustomComponentType_001 end";
}
 
/**
 * @tc.number: SetCustomComponentType_002
 * @tc.name: SetCustomComponentType
 * @tc.desc: Test function SetCustomComponentType, GetCustomComponentType
 */
HWTEST_F(AccessibilityElementInfoTest, SetCustomComponentType_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCustomComponentType_002 start";
    elementInfo_->SetCustomComponentType("Pictures");
    EXPECT_TRUE(elementInfo_->GetCustomComponentType() == "Pictures");
    GTEST_LOG_(INFO) << "SetCustomComponentType_002 end";
}
 
/**
 * @tc.number: SetCustomComponentType_003
 * @tc.name: SetCustomComponentType
 * @tc.desc: Test function SetCustomComponentType, GetCustomComponentType
 */
HWTEST_F(AccessibilityElementInfoTest, SetCustomComponentType_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCustomComponentType_003 start";
    elementInfo_->SetCustomComponentType("");
    EXPECT_TRUE(elementInfo_->GetCustomComponentType() == "");
    GTEST_LOG_(INFO) << "SetCustomComponentType_003 end";
}
 
/**
 * @tc.number: SetAccessibilityNextFocusId_001
 * @tc.name: SetAccessibilityNextFocusId
 * @tc.desc: Test function SetAccessibilityNextFocusId, GetAccessibilityNextFocusId
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityNextFocusId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityNextFocusId_001 start";
    elementInfo_->SetAccessibilityNextFocusId(0);
    EXPECT_TRUE(elementInfo_->GetAccessibilityNextFocusId() == 0);
    GTEST_LOG_(INFO) << "SetAccessibilityNextFocusId_001 end";
}
 
/**
 * @tc.number: SetAccessibilityNextFocusId_002
 * @tc.name: SetAccessibilityNextFocusId
 * @tc.desc: Test function SetAccessibilityNextFocusId, GetAccessibilityNextFocusId
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityNextFocusId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityNextFocusId_002 start";
    elementInfo_->SetAccessibilityNextFocusId(10);
    EXPECT_TRUE(elementInfo_->GetAccessibilityNextFocusId() == 10);
    GTEST_LOG_(INFO) << "SetAccessibilityNextFocusId_002 end";
}
 
/**
 * @tc.number: SetAccessibilityPreviousFocusId_001
 * @tc.name: SetAccessibilityPreviousFocusId
 * @tc.desc: Test function SetAccessibilityPreviousFocusId, GetAccessibilityPreviousFocusId
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityPreviousFocusId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityPreviousFocusId_001 start";
    elementInfo_->SetAccessibilityPreviousFocusId(0);
    EXPECT_TRUE(elementInfo_->GetAccessibilityPreviousFocusId() == 0);
    GTEST_LOG_(INFO) << "SetAccessibilityPreviousFocusId_001 end";
}
 
/**
 * @tc.number: SetAccessibilityPreviousFocusId_002
 * @tc.name: SetAccessibilityPreviousFocusId
 * @tc.desc: Test function SetAccessibilityPreviousFocusId, GetAccessibilityPreviousFocusId
 */
HWTEST_F(AccessibilityElementInfoTest, SetAccessibilityPreviousFocusId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityPreviousFocusId_002 start";
    elementInfo_->SetAccessibilityPreviousFocusId(10);
    EXPECT_TRUE(elementInfo_->GetAccessibilityPreviousFocusId() == 10);
    GTEST_LOG_(INFO) << "SetAccessibilityPreviousFocusId_002 end";
}
} // namespace Accessibility
} // namespace OHOS