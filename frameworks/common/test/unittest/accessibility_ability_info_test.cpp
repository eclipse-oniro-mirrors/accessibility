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
#include "accessibility_ability_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityAbilityInfoUnitTest : public ::testing::Test {
public:
    AccessibilityAbilityInfoUnitTest()
    {}
    ~AccessibilityAbilityInfoUnitTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoUnitTest SetUp() Start";
        AccessibilityAbilityInitParams params;
        params.bundleName = "bundleName";
        params.description = "description";
        params.moduleName = "moduleName";
        params.name = "name";
        params.rationale = "rationale";
        params.settingsAbility = "settingsAbility";
        params.staticCapabilities = 1;
        params.abilityTypes = ACCESSIBILITY_ABILITY_TYPE_SPOKEN;
        params.isImportant = true;
        abilityInfo_ = std::make_shared<AccessibilityAbilityInfo>(params);
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoUnitTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityAbilityInfoUnitTest TearDown()";
        abilityInfo_ = nullptr;
    }

    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo_ = nullptr;
};

/**
 * @tc.number: AccessibilityAbilityInfo_001
 * @tc.name: AccessibilityAbilityInfo
 * @tc.desc: Test function AccessibilityAbilityInfo
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_001 start";
    std::shared_ptr<AccessibilityAbilityInfo> info = std::make_shared<AccessibilityAbilityInfo>();
    EXPECT_TRUE(info);
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_001 end";
}

/**
 * @tc.number: SetPackageName_001
 * @tc.name: SetPackageName
 * @tc.desc: Test function SetPackageName
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, SetPackageName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetPackageName_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    std::string packageName = "packageName";
    abilityInfo_->SetPackageName(packageName);
    EXPECT_STREQ(abilityInfo_->GetPackageName().c_str(), packageName.c_str());
    GTEST_LOG_(INFO) << "SetPackageName_001 end";
}

/**
 * @tc.number: SetAccessibilityAbilityType_001
 * @tc.name: SetAccessibilityAbilityType
 * @tc.desc: Test function SetAccessibilityAbilityType
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, SetAccessibilityAbilityType_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetAccessibilityAbilityType_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    abilityInfo_->SetAccessibilityAbilityType(1);
    EXPECT_EQ(abilityInfo_->GetAccessibilityAbilityType(), 1);
    GTEST_LOG_(INFO) << "SetAccessibilityAbilityType_001 end";
}

/**
 * @tc.number: SetCapabilityValues_001
 * @tc.name: SetCapabilityValues
 * @tc.desc: Test function SetCapabilityValues
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, SetCapabilityValues_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCapabilityValues_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    abilityInfo_->SetCapabilityValues(1);
    EXPECT_EQ(abilityInfo_->GetCapabilityValues(), 1);
    GTEST_LOG_(INFO) << "SetCapabilityValues_001 end";
}

/**
 * @tc.number: SetEventTypes_001
 * @tc.name: SetEventTypes
 * @tc.desc: Test function SetEventTypes
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, SetEventTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetEventTypes_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    abilityInfo_->SetEventTypes(1);
    EXPECT_EQ(abilityInfo_->GetEventTypes(), 1);
    GTEST_LOG_(INFO) << "SetEventTypes_001 end";
}

/**
 * @tc.number: IsImportant_001
 * @tc.name: IsImportant
 * @tc.desc: Test function IsImportant
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, IsImportant_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsImportant_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_TRUE(abilityInfo_->IsImportant());
    GTEST_LOG_(INFO) << "IsImportant_001 end";
}

/**
 * @tc.number: GetDescription_001
 * @tc.name: GetDescription
 * @tc.desc: Test function GetDescription
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetDescription_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_STREQ(abilityInfo_->GetDescription().c_str(), "description");
    GTEST_LOG_(INFO) << "GetDescription_001 end";
}

/**
 * @tc.number: GetId_001
 * @tc.name: GetId
 * @tc.desc: Test function GetId
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetId_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    std::string id = "bundleName/name";
    EXPECT_STREQ(abilityInfo_->GetId().c_str(), id.c_str());
    GTEST_LOG_(INFO) << "GetId_001 end";
}

/**
 * @tc.number: GetName_001
 * @tc.name: GetName
 * @tc.desc: Test function GetName
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetName_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_STREQ(abilityInfo_->GetName().c_str(), "name");
    GTEST_LOG_(INFO) << "GetName_001 end";
}

/**
 * @tc.number: GetModuleName_001
 * @tc.name: GetModuleName
 * @tc.desc: Test function GetModuleName
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetModuleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetModuleName_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_STREQ(abilityInfo_->GetModuleName().c_str(), "moduleName");
    GTEST_LOG_(INFO) << "GetModuleName_001 end";
}

/**
 * @tc.number: SetFilterBundleNames_001
 * @tc.name: SetFilterBundleNames
 * @tc.desc: Test function SetFilterBundleNames
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, SetFilterBundleNames_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetFilterBundleNames_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    std::vector<std::string> names = {"BundleName1", "BundleName2"};
    abilityInfo_->SetFilterBundleNames(names);
    int32_t index = 0;
    for (auto &name : abilityInfo_->GetFilterBundleNames()) {
        if (!index) {
            EXPECT_STREQ(name.c_str(), "BundleName1");
        } else {
            EXPECT_STREQ(name.c_str(), "BundleName2");
        }
        index++;
    }
    GTEST_LOG_(INFO) << "SetFilterBundleNames_001 end";
}

/**
 * @tc.number: GetSettingsAbility_001
 * @tc.name: GetSettingsAbility
 * @tc.desc: Test function GetSettingsAbility
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetSettingsAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSettingsAbility_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_STREQ(abilityInfo_->GetSettingsAbility().c_str(), "settingsAbility");
    GTEST_LOG_(INFO) << "GetSettingsAbility_001 end";
}

/**
 * @tc.number: GetStaticCapabilityValues_001
 * @tc.name: GetStaticCapabilityValues
 * @tc.desc: Test function GetStaticCapabilityValues
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, GetStaticCapabilityValues_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetStaticCapabilityValues_001 start";
    if (!abilityInfo_) {
        GTEST_LOG_(INFO) << "abilityInfo_ is null";
        return;
    }
    EXPECT_EQ(abilityInfo_->GetStaticCapabilityValues(), 1);
    GTEST_LOG_(INFO) << "GetStaticCapabilityValues_001 end";
}
} // namespace Accessibility
} // namespace OHOS