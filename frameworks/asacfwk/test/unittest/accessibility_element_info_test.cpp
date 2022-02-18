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

#include "accessibility_element_info.h"
#include "accessibility_operator.h"
#include "mock_accessible_ability_channel_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityElementInfoUnitTest : public ::testing::Test {
public:
    AccessibilityElementInfoUnitTest() {}
    ~AccessibilityElementInfoUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoUnitTest SetUp()";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoUnitTest TearDown()";
    }
};

/**
 * @tc.number: GetElementProperty_001
 * @tc.name: Test the type property is bool/int
 * @tc.desc: Test the type property is bool/int
 */
HWTEST_F(AccessibilityElementInfoUnitTest, GetElementProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetElementProperty_001 start";
    AccessibilityElementInfo element {};
    element.SetComponentId(1);
    EXPECT_TRUE(element.GetAccessibilityId() == 1);

    element.AddChild(1);
    EXPECT_TRUE(element.GetChildId(0) == 1);
    EXPECT_TRUE(element.GetChildCount() == 1);
    EXPECT_TRUE(element.GetChildIds()[0] == 1);

    element.AddChild(2);
    EXPECT_TRUE(element.GetChildId(1) == 2);
    EXPECT_TRUE(element.GetChildCount() == 2);
    EXPECT_TRUE(element.GetChildIds()[0] == 1);
    EXPECT_TRUE(element.GetChildIds()[1] == 2);

    element.RemoveChild(1);
    EXPECT_TRUE(element.GetChildCount() == 1);
    EXPECT_TRUE(element.GetChildIds()[0] == 2);

    AccessibleAction action(ActionType::ACCESSIBILITY_ACTION_CLICK, "test");
    element.AddAction(action);
    EXPECT_TRUE(element.GetActionList()[0].GetActionType() == ActionType::ACCESSIBILITY_ACTION_CLICK);
    EXPECT_TRUE(strcmp(element.GetActionList()[0].GetDescriptionInfo().c_str(), "test") == 0);

    GTEST_LOG_(INFO) << "GetElementProperty_001 Delete Action start";
    element.DeleteAction(action);
    GTEST_LOG_(INFO) << "GetElementProperty_001 Delete Action1  end";
    element.AddAction(action);
    ActionType act = ActionType::ACCESSIBILITY_ACTION_CLICK;
    EXPECT_TRUE(element.DeleteAction(act));
    GTEST_LOG_(INFO) << "GetElementProperty_001 Delete Action2  end";
    element.AddAction(action);
    element.DeleteAllActions();
    GTEST_LOG_(INFO) << "GetElementProperty_001 Delete all Action end";

    element.SetTextLengthLimit(100);
    EXPECT_TRUE(element.GetTextLengthLimit() == 100);

    element.SetWindowId(100);
    EXPECT_TRUE(element.GetWindowId() == 100);

    element.SetParent(100);
    EXPECT_TRUE(element.GetParentNodeId() == 100);

    Rect bounds(1, 2, 3, 4);
    element.SetRectInScreen(bounds);
    EXPECT_TRUE(element.GetRectInScreen().GetLeftTopXScreenPostion() == 1);
    EXPECT_TRUE(element.GetRectInScreen().GetLeftTopYScreenPostion() == 2);
    EXPECT_TRUE(element.GetRectInScreen().GetRightBottomXScreenPostion() == 3);
    EXPECT_TRUE(element.GetRectInScreen().GetRightBottomYScreenPostion() == 4);

    element.SetCheckable(true);
    EXPECT_TRUE(element.IsCheckable());

    element.SetChecked(true);
    EXPECT_TRUE(element.IsChecked());

    element.SetFocusable(true);
    EXPECT_TRUE(element.IsFocusable());

    element.SetFocused(true);
    EXPECT_TRUE(element.IsFocused());

    element.SetVisible(true);
    EXPECT_TRUE(element.IsVisible());

    element.SetAccessibilityFocus(true);
    EXPECT_TRUE(element.HasAccessibilityFocus());

    element.SetSelected(true);
    EXPECT_TRUE(element.IsSelected());

    element.SetClickable(true);
    EXPECT_TRUE(element.IsClickable());

    element.SetLongClickable(true);
    EXPECT_TRUE(element.IsLongClickable());

    element.SetEnabled(true);
    EXPECT_TRUE(element.IsEnabled());

    element.SetPassword(true);
    EXPECT_TRUE(element.IsPassword());

    element.SetScrollable(true);
    EXPECT_TRUE(element.IsScrollable());

    element.SetCurrentIndex(1);
    EXPECT_TRUE(element.GetCurrentIndex() == 1);

    element.SetBeginIndex(1);
    EXPECT_TRUE(element.GetBeginIndex() == 1);

    element.SetEndIndex(1);
    EXPECT_TRUE(element.GetEndIndex() == 1);

    element.SetInputType(1);
    EXPECT_TRUE(element.GetInputType() == 1);

    element.SetEditable(true);
    EXPECT_TRUE(element.IsEditable());

    element.SetPluraLineSupported(true);
    EXPECT_TRUE(element.IsPluraLineSupported());

    element.SetPopupSupported(true);
    EXPECT_TRUE(element.IsPopupSupported());

    element.SetDeletable(true);
    EXPECT_TRUE(element.IsDeletable());

    element.SetEssential(true);
    EXPECT_TRUE(element.IsEssential());

    element.SetHinting(true);
    EXPECT_TRUE(element.IsGivingHint());

    element.SetSelectedBegin(1);
    EXPECT_TRUE(element.GetSelectedBegin() == 1);

    element.SetSelectedEnd(1);
    EXPECT_TRUE(element.GetSelectedEnd() == 1);

    element.SetAccessibilityGroup(true);
    EXPECT_TRUE(element.GetAccessibilityGroup());

    element.SetLiveRegion(1);
    EXPECT_TRUE(element.GetLiveRegion() == 1);

    element.SetContentInvalid(true);
    EXPECT_TRUE(element.GetContentInvalid());

    element.SetLabeled(1);
    EXPECT_TRUE(element.GetLabeledAccessibilityId() == 1);

    element.SetChannelId(1);
    EXPECT_TRUE(element.GetChannelId() == 1);

    element.SetAccessibilityId(1);
    EXPECT_TRUE(element.GetAccessibilityId() == 1);

    RangeInfo rangeInfo(1, 5, 3);
    element.SetRange(rangeInfo);
    EXPECT_TRUE(element.GetRange().GetMin() == 1);
    EXPECT_TRUE(element.GetRange().GetMax() == 5);
    EXPECT_TRUE(element.GetRange().GetCurrent() == 3);

    GridInfo grid(2, 2, 0);
    element.SetGrid(grid);
    EXPECT_TRUE(element.GetGrid().GetRowCount() == 2);
    EXPECT_TRUE(element.GetGrid().GetColumnCount() == 2);
    EXPECT_TRUE(element.GetGrid().GetSelectionMode() == 0);

    GridItemInfo gridItem(0, 2, 0, 2, true, true);
    element.SetGridItem(gridItem);
    EXPECT_TRUE(element.GetGridItem().GetRowIndex() == 0);
    EXPECT_TRUE(element.GetGridItem().GetRowSpan() == 2);
    EXPECT_TRUE(element.GetGridItem().GetColumnIndex() == 0);
    EXPECT_TRUE(element.GetGridItem().GetColumnSpan() == 2);
    EXPECT_TRUE(element.GetGridItem().IsHeading());
    EXPECT_TRUE(element.GetGridItem().IsSelected());

    GTEST_LOG_(INFO) << "GetElementProperty_001 end";
}

/**
 * @tc.number: GetElementStr_001
 * @tc.name: Test the property of string
 * @tc.desc: Test the property of string
 */
HWTEST_F(AccessibilityElementInfoUnitTest, GetElementStr_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetElementStr_001 start";
    AccessibilityElementInfo element {};
    element.SetBundleName("test");
    EXPECT_TRUE(strcmp(element.GetBundleName().c_str(), "test") == 0);

    element.SetComponentType("test");
    EXPECT_TRUE(strcmp(element.GetComponentType().c_str(), "test") == 0);

    element.SetContent("test");
    EXPECT_TRUE(strcmp(element.GetContent().c_str(), "test") == 0);

    element.SetAccessibilityContent("test");
    EXPECT_TRUE(strcmp(element.GetAccessibilityContent().c_str(), "test") == 0);

    element.SetAccessibilityDescription("test");
    EXPECT_TRUE(strcmp(element.GetAccessibilityDescription().c_str(), "test") == 0);

    element.SetHint("test");
    EXPECT_TRUE(strcmp(element.GetHint().c_str(), "test") == 0);

    element.SetDescriptionInfo("test");
    EXPECT_TRUE(strcmp(element.GetDescriptionInfo().c_str(), "test") == 0);

    element.SetComponentResourceId("test");
    EXPECT_TRUE(strcmp(element.GetComponentResourceId().c_str(), "test") == 0);

    element.SetError("test");
    EXPECT_TRUE(strcmp(element.GetError().c_str(), "test") == 0);
    GTEST_LOG_(INFO) << "GetElementStr_001 end";
}

/**
 * @tc.number: ReadFromParcel_001
 * @tc.name: ReadFromParcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityElementInfoUnitTest, ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ReadFromParcel_001 start";

    Parcel parcel;
    AccessibilityElementInfo element {};
    element.Marshalling(parcel);
    EXPECT_TRUE(element.ReadFromParcel(parcel));

    AccessibleAction action {};
    action.Marshalling(parcel);
    EXPECT_TRUE(action.ReadFromParcel(parcel));

    RangeInfo range {};
    range.Marshalling(parcel);
    EXPECT_TRUE(range.ReadFromParcel(parcel));

    GridInfo grid {};
    grid.Marshalling(parcel);
    EXPECT_TRUE(grid.ReadFromParcel(parcel));

    GridItemInfo gridItem {};
    gridItem.Marshalling(parcel);
    EXPECT_TRUE(gridItem.ReadFromParcel(parcel));

    Rect rect {};
    rect.Marshalling(parcel);
    EXPECT_TRUE(rect.ReadFromParcel(parcel));
    GTEST_LOG_(INFO) << "ReadFromParcel_001 end";
}

/**
 * @tc.number: Marshalling_001
 * @tc.name: Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoUnitTest, Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Marshalling_001 start";

    Parcel parcel;
    AccessibilityElementInfo element {};
    EXPECT_TRUE(element.Marshalling(parcel));

    AccessibleAction action {};
    EXPECT_TRUE(action.Marshalling(parcel));

    RangeInfo range {};
    EXPECT_TRUE(range.Marshalling(parcel));

    GridInfo grid {};
    EXPECT_TRUE(grid.Marshalling(parcel));

    GridItemInfo gridItem {};
    EXPECT_TRUE(gridItem.Marshalling(parcel));

    Rect rect {};
    EXPECT_TRUE(rect.Marshalling(parcel));
    GTEST_LOG_(INFO) << "Marshalling_001 end";
}

} // namespace Accessibility
} // namespace OHOS