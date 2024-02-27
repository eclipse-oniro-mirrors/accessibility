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
#include "accessibility_caption_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityCaptionParcelTest : public ::testing::Test {
public:
    AccessibilityCaptionParcelTest()
    {}
    ~AccessibilityCaptionParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityCaptionParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityCaptionParcelTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityCaptionParcelTest SetUp() Start";
        AccessibilityConfig::CaptionProperty captionProperty;
        captionPropertyParcel_ = std::make_shared<CaptionPropertyParcel>(captionProperty);
        GTEST_LOG_(INFO) << "AccessibilityCaptionParcelTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityCaptionParcelTest TearDown()";
        captionPropertyParcel_ = nullptr;
    }

    std::shared_ptr<CaptionPropertyParcel> captionPropertyParcel_ = nullptr;
};

/**
 * @tc.number: Caption_Marshalling_001
 * @tc.name: Caption_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityCaptionParcelTest, Caption_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Caption_Marshalling_001 start";
    if (!captionPropertyParcel_) {
        GTEST_LOG_(INFO) << "captionPropertyParcel_ is null";
        return;
    }

    Parcel parcel;
    bool ret = captionPropertyParcel_->Marshalling(parcel);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "Caption_Marshalling_001 end";
}

/**
 * @tc.number: Caption_Unmarshalling_001
 * @tc.name: Caption_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityCaptionParcelTest, Caption_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Caption_Unmarshalling_001 start";
    if (!captionPropertyParcel_) {
        GTEST_LOG_(INFO) << "captionPropertyParcel_ is null";
        return;
    }

    Parcel parcel;
    sptr<CaptionPropertyParcel> captionParcel = captionPropertyParcel_->Unmarshalling(parcel);
    EXPECT_EQ(true, captionParcel == nullptr);
    GTEST_LOG_(INFO) << "Caption_Unmarshalling_001 end";
}
} // namespace Accessibility
} // namespace OHOS