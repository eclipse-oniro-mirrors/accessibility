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
#include "accessibility_event_info_parcel.h"
#include "accessibility_element_info_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityEventInfoParcelTest : public ::testing::Test {
public:
    AccessibilityEventInfoParcelTest()
    {}
    ~AccessibilityEventInfoParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoParcelTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoParcelTest SetUp() Start";
        AccessibilityEventInfo eventInfo;
        eventInfoParcel_ = std::make_shared<AccessibilityEventInfoParcel>(eventInfo);
        AccessibilityElementInfo elementInfo;
        elementInfoParcel_ = std::make_shared<AccessibilityElementInfoParcel>(elementInfo);
        GTEST_LOG_(INFO) << "AccessibilityEventInfoParcelTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityEventInfoParcelTest TearDown()";
        eventInfoParcel_ = nullptr;
    }

    std::shared_ptr<AccessibilityEventInfoParcel> eventInfoParcel_ = nullptr;
    std::shared_ptr<AccessibilityElementInfoParcel> elementInfoParcel_ = nullptr;
};

/**
 * @tc.number: Event_Info_Marshalling_001
 * @tc.name: Event_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityEventInfoParcelTest, Event_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Event_Info_Marshalling_001 start";
    if (!eventInfoParcel_) {
        GTEST_LOG_(INFO) << "eventInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    bool ret = eventInfoParcel_->Marshalling(parcel);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "Event_Info_Marshalling_001 end";
}

/**
 * @tc.number: Event_Info_Unmarshalling_001
 * @tc.name: Event_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityEventInfoParcelTest, Event_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Event_Info_Unmarshalling_001 start";
    if (!eventInfoParcel_) {
        GTEST_LOG_(INFO) << "eventInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    sptr<AccessibilityEventInfoParcel> eventInfoParcel = eventInfoParcel_->Unmarshalling(parcel);
    EXPECT_EQ(true, eventInfoParcel == nullptr);
    GTEST_LOG_(INFO) << "Event_Info_Unmarshalling_001 end";
}

/**
 * @tc.number: Element_Info_Unmarshalling_001
 * @tc.name: Element_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityEventInfoParcelTest, Element_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Element_Info_Unmarshalling_001 start";
    if (!elementInfoParcel_) {
        GTEST_LOG_(INFO) << "elementInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    sptr<AccessibilityElementInfoParcel> elementInfoParcel = elementInfoParcel_->Unmarshalling(parcel);
    EXPECT_EQ(true, elementInfoParcel == nullptr);
    GTEST_LOG_(INFO) << "Element_Info_Unmarshalling_001 end";
}
} // namespace Accessibility
} // namespace OHOS
