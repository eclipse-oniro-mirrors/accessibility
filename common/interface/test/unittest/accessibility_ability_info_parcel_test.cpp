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
#include "accessibility_ability_info_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityAbilityInfoParcelTest : public ::testing::Test {
public:
    AccessibilityAbilityInfoParcelTest()
    {}
    ~AccessibilityAbilityInfoParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoParcelTest End";
    }

    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoParcelTest SetUp() Start";
        AccessibilityAbilityInitParams params;
        params.bundleName = "bundleName";
        params.moduleName = "moduleName";
        params.name = "name";
        params.description = "description";
        params.rationale = "rationale";
        params.settingsAbility = "settingsAbility";
        params.staticCapabilities = 1;
        params.abilityTypes = ACCESSIBILITY_ABILITY_TYPE_SPOKEN;
        params.isImportant = true;
        params.needHide = false;
        abilityInfo_ = std::make_shared<AccessibilityAbilityInfo>(params);
        abilityInfoParcel_ = std::make_shared<AccessibilityAbilityInfoParcel>(*abilityInfo_);
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoParcelTest SetUp() End";
    };
    
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoParcelTest TearDown()";
        abilityInfo_ = nullptr;
        abilityInfoParcel_ = nullptr;
    }

    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo_ = nullptr;
    std::shared_ptr<AccessibilityAbilityInfoParcel> abilityInfoParcel_ = nullptr;
};

/**
 * @tc.number: Marshalling_001
 * @tc.name: Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityAbilityInfoParcelTest, Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Marshalling_001 start";
    if (!abilityInfoParcel_) {
        GTEST_LOG_(INFO) << "abilityInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    bool ret = abilityInfoParcel_->Marshalling(parcel);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "Marshalling_001 end";
}

/**
 * @tc.number: Unmarshalling_001
 * @tc.name: Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityAbilityInfoParcelTest, Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Unmarshalling_001 start";
    if (!abilityInfoParcel_) {
        GTEST_LOG_(INFO) << "abilityInfoParcel_ is null";
        return;
    }

    Parcel parcel;
    sptr<AccessibilityAbilityInfoParcel> infoParcel = abilityInfoParcel_->Unmarshalling(parcel);
    EXPECT_EQ(true, infoParcel == nullptr);
}
} // namespace Accessibility
} // namespace OHOS