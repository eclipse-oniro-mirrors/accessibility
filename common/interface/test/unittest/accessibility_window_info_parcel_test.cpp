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
#include "accessibility_window_info_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowInfoParcelTest : public ::testing::Test {
public:
    AccessibilityWindowInfoParcelTest()
    {}
    ~AccessibilityWindowInfoParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoParcelTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoParcelTest SetUp() Start";
        AccessibilityWindowInfo windowInfo;
        windowInfoParcel_ = std::make_shared<AccessibilityWindowInfoParcel>(windowInfo);
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoParcelTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityWindowInfoParcelTest TearDown()";
        windowInfoParcel_ = nullptr;
    }

    std::shared_ptr<AccessibilityWindowInfoParcel> windowInfoParcel_ = nullptr;
};

/**
 * @tc.number: Window_Info_Marshalling_001
 * @tc.name: Window_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityWindowInfoParcelTest, Window_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Window_Info_Marshalling_001 start";
    if (!windowInfoParcel_) {
        GTEST_LOG_(INFO) << "captionPropertyParcel_ is null";
        return;
    }

    Parcel parcel;
    bool ret = windowInfoParcel_->Marshalling(parcel);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "Window_Info_Marshalling_001 end";
}

/**
 * @tc.number: Window_Info_Unmarshalling_001
 * @tc.name: Window_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityWindowInfoParcelTest, Window_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Window_Info_Unmarshalling_001 start";
    if (!windowInfoParcel_) {
        GTEST_LOG_(INFO) << "windowInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    sptr<AccessibilityWindowInfoParcel> windowInfoParcel = windowInfoParcel_->Unmarshalling(parcel);
    EXPECT_EQ(true, windowInfoParcel == nullptr);
    GTEST_LOG_(INFO) << "Window_Info_Unmarshalling_001 end";
}
} // namespace Accessibility
} // namespace OHOS