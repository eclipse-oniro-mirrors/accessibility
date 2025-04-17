/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "accessibility_element_info_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityElementInfoParcelTest : public ::testing::Test {
public:
    AccessibilityElementInfoParcelTest()
    {}
    ~AccessibilityElementInfoParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoParcelTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoParcelTest SetUp() Start";
        elementInfo_ = std::make_shared<AccessibilityElementInfo>();
        elementInfoParcel_ = std::make_shared<AccessibilityElementInfoParcel>(*elementInfo_);
        spanInfo_ = std::make_shared<SpanInfo>();
        spanInfoParcel_ = std::make_shared<SpanInfoParcel>(*spanInfo_);
        action_ = std::make_shared<AccessibleAction>();
        actionParcel_ = std::make_shared<AccessibleActionParcel>(*action_);
        rangeInfo_ = std::make_shared<RangeInfo>();
        rangeInfoParcel_ = std::make_shared<RangeInfoParcel>(*rangeInfo_);
        gridInfo_ = std::make_shared<GridInfo>();
        gridInfoParcel_ = std::make_shared<GridInfoParcel>(*gridInfo_);
        gridItemInfo_ = std::make_shared<GridItemInfo>();
        gridItemInfoParcel_ = std::make_shared<GridItemInfoParcel>(*gridItemInfo_);
        rect_ = std::make_shared<Rect>();
        rectParcel_ = std::make_shared<RectParcel>(*rect_);
        extraElementInfo_ = std::make_shared<ExtraElementInfo>();
        extraElementInfoParcel_ = std::make_shared<ExtraElementInfoParcel>(*extraElementInfo_);
        GTEST_LOG_(INFO) << "AccessibilityElementInfoParcelTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityElementInfoParcelTest TearDown()";
        elementInfo_ = nullptr;
        elementInfoParcel_ = nullptr;
        spanInfo_ = nullptr;
        spanInfoParcel_ = nullptr;
        action_ = nullptr;
        actionParcel_ = nullptr;
        rangeInfo_ = nullptr;
        rangeInfoParcel_ = nullptr;
        gridInfo_ = nullptr;
        gridInfoParcel_ = nullptr;
        gridItemInfo_ = nullptr;
        gridItemInfoParcel_ = nullptr;
        rect_ = nullptr;
        rectParcel_ = nullptr;
        extraElementInfo_ = nullptr;
        extraElementInfoParcel_ = nullptr;
    }

    std::shared_ptr<AccessibleAction> action_ = nullptr;
    std::shared_ptr<AccessibleActionParcel> actionParcel_ = nullptr;
    std::shared_ptr<RangeInfo> rangeInfo_ = nullptr;
    std::shared_ptr<RangeInfoParcel> rangeInfoParcel_ = nullptr;
    std::shared_ptr<GridInfo> gridInfo_ = nullptr;
    std::shared_ptr<GridInfoParcel> gridInfoParcel_ = nullptr;
    std::shared_ptr<GridItemInfo> gridItemInfo_ = nullptr;
    std::shared_ptr<GridItemInfoParcel> gridItemInfoParcel_ = nullptr;
    std::shared_ptr<Rect> rect_ = nullptr;
    std::shared_ptr<RectParcel> rectParcel_ = nullptr;
    std::shared_ptr<AccessibilityElementInfo> elementInfo_ = nullptr;
    std::shared_ptr<AccessibilityElementInfoParcel> elementInfoParcel_ = nullptr;
    std::shared_ptr<SpanInfo> spanInfo_ = nullptr;
    std::shared_ptr<SpanInfoParcel> spanInfoParcel_ = nullptr;
    std::shared_ptr<ExtraElementInfo> extraElementInfo_ = nullptr;
    std::shared_ptr<ExtraElementInfoParcel> extraElementInfoParcel_ = nullptr;
};

/**
 * @tc.number: Element_Info__Marshalling_001
 * @tc.name: Element_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Element_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Element_Info_Marshalling_001 start";
    if (!elementInfoParcel_) {
        GTEST_LOG_(INFO) << "elementInfoParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = elementInfoParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Element_Info_Marshalling_001 end";
}

/**
 * @tc.number: Element_Info_Unmarshalling_001
 * @tc.name: Element_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Element_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Element_Info_Unmarshalling_001 start";
    if (!elementInfoParcel_) {
        GTEST_LOG_(INFO) << "elementInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<AccessibilityElementInfoParcel> elementInfoParcel = elementInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, elementInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Element_Info_Unmarshalling_001 end";
}

/**
 * @tc.number: Rect_Marshalling_001
 * @tc.name: Rect_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Rect_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Rect_Marshalling_001 start";
    if (!rectParcel_) {
        GTEST_LOG_(INFO) << "rectParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = rectParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Rect_Marshalling_001 end";
}

/**
 * @tc.number: Rect_Unmarshalling_001
 * @tc.name: Rect__Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Rect_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Rect_Unmarshalling_001 start";
    if (!rectParcel_) {
        GTEST_LOG_(INFO) << "rectParcel_ is null";
    } else {
        Parcel parcel;
        sptr<RectParcel> rectParcel = rectParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, rectParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Rect_Unmarshalling_001 end";
}

/**
 * @tc.number: Grid_Item_Info_Marshalling_001
 * @tc.name: Grid_Item_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Grid_Item_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Grid_Item_Info_Marshalling_001 start";
    if (!gridItemInfoParcel_) {
        GTEST_LOG_(INFO) << "gridItemInfoParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = gridItemInfoParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Grid_Item_Info_Marshalling_001 end";
}

/**
 * @tc.number: Grid_Item_Info_Unmarshalling_001
 * @tc.name: Grid_Item_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Grid_Item_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Grid_Item_Info_Unmarshalling_001 start";
    if (!gridItemInfoParcel_) {
        GTEST_LOG_(INFO) << "gridItemInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<GridItemInfoParcel> gridItemInfoParcel = gridItemInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, gridItemInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Grid_Item_Info_Unmarshalling_001 end";
}

/**
 * @tc.number: Action_Marshalling_001
 * @tc.name: Action_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Action_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Action_Marshalling_001 start";
    if (!actionParcel_) {
        GTEST_LOG_(INFO) << "actionParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = actionParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Action_Marshalling_001 end";
}

/**
 * @tc.number: Action_Unmarshalling_001
 * @tc.name: Action_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Action_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Action_Unmarshalling_001 start";
    if (!actionParcel_) {
        GTEST_LOG_(INFO) << "actionParcel_ is null";
    } else {
        Parcel parcel;
        sptr<AccessibleActionParcel> actionParcel = actionParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, actionParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Action_Unmarshalling_001 end";
}

/**
 * @tc.number: Range_Info_Marshalling_001
 * @tc.name: Range_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Range_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Range_Info_Marshalling_001 start";
    if (!rangeInfoParcel_) {
        GTEST_LOG_(INFO) << "rangeInfoParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = rangeInfoParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Range_Info_Marshalling_001 end";
}

/**
 * @tc.number: Range_Info_Unmarshalling_001
 * @tc.name: Range_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Range_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Range_Info_Unmarshalling_001 start";
    if (!rangeInfoParcel_) {
        GTEST_LOG_(INFO) << "rangeInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<RangeInfoParcel> rangeInfoParcel = rangeInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, rangeInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Range_Info_Unmarshalling_001 end";
}

/**
 * @tc.number: Grid_Info_Marshalling_001
 * @tc.name: Grid_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Grid_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Grid_Info_Marshalling_001 start";
    if (!gridInfoParcel_) {
        GTEST_LOG_(INFO) << "gridInfoParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = gridInfoParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Grid_Info_Marshalling_001 end";
}

/**
 * @tc.number: Grid_Info_Unmarshalling_001
 * @tc.name: Grid_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Grid_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Grid_Info_Unmarshalling_001 start";
    if (!gridInfoParcel_) {
        GTEST_LOG_(INFO) << "gridInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<GridInfoParcel> gridInfoParcel = gridInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, gridInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Grid_Info_Unmarshalling_001 end";
}

/**
 * @tc.number: Extra_Element_Info_Parcel__001
 * @tc.name: Extra_Element_Info_Parcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Extra_Element_Info_Parcel__001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Extra_Element_Info_Parcel__001 start";
    if (!extraElementInfoParcel_) {
        GTEST_LOG_(INFO) << "extraElementInfoParcel_ is null";
    } else {
        Parcel parcel;
        EXPECT_EQ(false, extraElementInfoParcel_->ReadFromParcel(parcel));
    }
    GTEST_LOG_(INFO) << "Extra_Element_Info_Parcel__001 end";
}

/**
 * @tc.number: Extra_Element_Info_Parcel__002
 * @tc.name: Extra_Element_Info_Parcel
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Extra_Element_Info_Parcel__002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Extra_Element_Info_Parcel__002 start";
    if (!extraElementInfoParcel_) {
        GTEST_LOG_(INFO) << "extraElementInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<ExtraElementInfoParcel> extraElementInfoParcel = extraElementInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, extraElementInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Extra_Element_Info_Parcel__002 end";
}

/**
 * @tc.number: Span_Info_Parcel__001
 * @tc.name: Span_Info_Parcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Span_Info_Parcel__001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Span_Info_Parcel__001 start";
    if (!spanInfoParcel_) {
        GTEST_LOG_(INFO) << "spanInfoParcel_ is null";
    } else {
        Parcel parcel;
        EXPECT_EQ(false, spanInfoParcel_->ReadFromParcel(parcel));
    }
    GTEST_LOG_(INFO) << "Span_Info_Parcel__001 end";
}

/**
 * @tc.number: Span_Info_Marshalling_001
 * @tc.name: Span_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Span_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Span_Info_Marshalling_001 start";
    if (!spanInfoParcel_) {
        GTEST_LOG_(INFO) << "spanInfoParcel_ is null";
    } else {
        Parcel parcel;
        bool ret = spanInfoParcel_->Marshalling(parcel);
        EXPECT_EQ(ret, true);
    }
    GTEST_LOG_(INFO) << "Span_Info_Marshalling_001 end";
}

/**
 * @tc.number: Span_Info_Unmarshalling__001
 * @tc.name: Span_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityElementInfoParcelTest, Span_Info_Unmarshalling__001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Span_Info_Unmarshalling__001 start";
    if (!spanInfoParcel_) {
        GTEST_LOG_(INFO) << "spanInfoParcel_ is null";
    } else {
        Parcel parcel;
        sptr<SpanInfoParcel> spanInfoParcel = spanInfoParcel_->Unmarshalling(parcel);
        EXPECT_EQ(true, spanInfoParcel == nullptr);
    }
    GTEST_LOG_(INFO) << "Span_Info_Unmarshalling__001 end";
}
} // namespace Accessibility
} // namespace OHOS