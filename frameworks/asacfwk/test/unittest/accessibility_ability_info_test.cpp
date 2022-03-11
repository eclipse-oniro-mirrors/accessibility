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
#include "accessibility_ability_info.h"
#include "accessibility_event_info.h"
#include "json.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
using namespace Json;

namespace OHOS {
namespace Accessibility {
const string TEST_BUNDLE_NAME = "TestBundleName";
const string TEST_MODULE_NAME = "TestModuleName";
const string TEST_NAME = "TestName";
const string TEST_DESCRIPTION = "TestDescription";

const int TIME_10000 = 10000;
const int SIZE_TWO = 2;
class AccessibilityAbilityInfoUnitTest : public ::testing::Test {
public:
    AccessibilityAbilityInfoUnitTest() {}
    ~AccessibilityAbilityInfoUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void WritefileAll(const char* fname, const char* data);
    void CreateAccessibilityConfig();
    shared_ptr<AccessibilityAbilityInfo> accessibilityAbilityInfo_ = nullptr;
};

void AccessibilityAbilityInfoUnitTest::WritefileAll(const char* fname, const char* data)
{
    FILE *fp;
    if ((fp = fopen(fname, "w")) == nullptr) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AccessibilityAbilityInfoUnitTest::CreateAccessibilityConfig()
{
    std::ostringstream os;
    Json::Value object1, targetBundleNames;
    Json::Value accessibilityAbilityTypes, accessibilityEventTypes, accessibilityCapabilities;
    string jsonStr;

    if (!remove("/system/app/dummy_accessibility_ability_config.json")) {
        GTEST_LOG_(INFO) << "remove successful";
    } else {
        GTEST_LOG_(INFO) << "remove failed";
    }

    accessibilityEventTypes[0] = "all";
    object1["accessibilityEventTypes"] = accessibilityEventTypes;
    targetBundleNames[0] = "com.example.ohos.api1";
    targetBundleNames[1] = "com.example.ohos.api2";
    object1["targetBundleNames"] = targetBundleNames;
    accessibilityAbilityTypes[0] = "spoken";
    accessibilityAbilityTypes[1] = "haptic";
    accessibilityAbilityTypes[SIZE_TWO] = "audible";
    object1["accessibilityAbilityTypes"] = accessibilityAbilityTypes;
    object1["notificationTimeout"] = 0;
    object1["uiNoninteractiveTimeout"] = 0;
    object1["uiInteractiveTimeout"] = TIME_10000;
    accessibilityCapabilities[0] = "touchGuide";
    object1["accessibilityCapabilities"] = accessibilityCapabilities;
    object1["description"] = "$string:accessibility_service_description";
    object1["settingsAbility"] = "com.example.ohos.accessibility.ServiceSettingsAbility";

    Json::StreamWriterBuilder writerBuilder;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(object1, &os);
    jsonStr = os.str();
    WritefileAll("/system/app/dummy_accessibility_ability_config.json",  jsonStr.c_str());
}

void AccessibilityAbilityInfoUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityAbilityInfoUnitTest Start ######################";
}

void AccessibilityAbilityInfoUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityAbilityInfoUnitTest End ######################";
}

void AccessibilityAbilityInfoUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    CreateAccessibilityConfig();

    AppExecFwk::ExtensionAbilityInfo abilityInfo;
    abilityInfo.bundleName = TEST_BUNDLE_NAME;
    abilityInfo.moduleName = TEST_MODULE_NAME;
    abilityInfo.name = TEST_NAME;
    abilityInfo.description = TEST_DESCRIPTION;
    accessibilityAbilityInfo_ = make_shared<AccessibilityAbilityInfo>(abilityInfo);
}

void AccessibilityAbilityInfoUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    accessibilityAbilityInfo_ = nullptr;
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetAccessibilityAbilityType_001
 * @tc.name: GetAccessibilityAbilityType
 * @tc.desc: Test function GetAccessibilityAbilityType
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetAccessibilityAbilityType_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetAccessibilityAbilityType_001 start";

    uint32_t abilityTypes = 0;
    EXPECT_EQ(accessibilityAbilityInfo_->GetAccessibilityAbilityType(), abilityTypes);

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetAccessibilityAbilityType_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetCapabilityValues_001
 * @tc.name: GetCapabilityValues
 * @tc.desc: Test function GetCapabilityValues
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetCapabilityValues_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetCapabilityValues_001 start";

    uint32_t capabilities = 0;
    EXPECT_EQ(accessibilityAbilityInfo_->GetCapabilityValues(), capabilities);

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetCapabilityValues_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetDescription_001
 * @tc.name: GetDescription
 * @tc.desc: Test function GetDescription
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetDescription_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetDescription_001 start";

    EXPECT_STREQ(accessibilityAbilityInfo_->GetDescription().c_str(), TEST_DESCRIPTION.c_str());

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetDescription_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetEventTypes_001
 * @tc.name: GetEventTypes
 * @tc.desc: Test function GetEventTypes
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetEventTypes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetEventTypes_001 start";
    uint32_t eventType = EventType::TYPES_ALL_MASK;
    EXPECT_EQ(accessibilityAbilityInfo_->GetEventTypes(), eventType);

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetEventTypes_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetId_001
 * @tc.name: GetId
 * @tc.desc: Test function GetId
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetId_001 start";

    EXPECT_STREQ(accessibilityAbilityInfo_->GetId().c_str(), (TEST_BUNDLE_NAME + "/" + TEST_NAME).c_str());

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetId_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetName_001
 * @tc.name: GetName
 * @tc.desc: Test function GetName
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetName_001 start";

    EXPECT_STREQ(accessibilityAbilityInfo_->GetName().c_str(), TEST_NAME.c_str());

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetName_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetPackageName_001
 * @tc.name: GetPackageName
 * @tc.desc: Test function GetPackageName
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetPackageName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetPackageName_001 start";

    EXPECT_STREQ(accessibilityAbilityInfo_->GetPackageName().c_str(), TEST_BUNDLE_NAME.c_str());

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetPackageName_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetFilterBundleNames_001
 * @tc.name: GetFilterBundleNames
 * @tc.desc: Test function GetFilterBundleNames
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetFilterBundleNames_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetFilterBundleNames_001 start";

    ASSERT_EQ((int)accessibilityAbilityInfo_->GetFilterBundleNames().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetFilterBundleNames_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_GetSettingsAbility_001
 * @tc.name: GetSettingsAbility
 * @tc.desc: Test function GetFilterGetSettingsAbilityBundleNames
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_GetSettingsAbility_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetSettingsAbility_001 start";

    EXPECT_STREQ(accessibilityAbilityInfo_->GetSettingsAbility().c_str(), "");

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_GetSettingsAbility_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_ReadFromParcel_001
 * @tc.name: ReadFromParcel
 * @tc.desc: Test function ReadFromParcel
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_ReadFromParcel_001 start";

    Parcel parcel;
    accessibilityAbilityInfo_->Marshalling(parcel);
    EXPECT_TRUE(accessibilityAbilityInfo_->ReadFromParcel(parcel));

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_ReadFromParcel_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_Marshalling_001
 * @tc.name: Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_Marshalling_001 start";

    Parcel parcel;
    EXPECT_TRUE(accessibilityAbilityInfo_->Marshalling(parcel));

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_Marshalling_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_Unmarshalling_001
 * @tc.name: Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_Unmarshalling_001 start";

    Parcel parcel;
    accessibilityAbilityInfo_->Marshalling(parcel);
    AccessibilityAbilityInfo *obj = accessibilityAbilityInfo_->Unmarshalling(parcel);

    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->GetAccessibilityAbilityType(), accessibilityAbilityInfo_->GetAccessibilityAbilityType());
    EXPECT_EQ(obj->GetCapabilityValues(), accessibilityAbilityInfo_->GetCapabilityValues());
    EXPECT_STREQ(obj->GetDescription().c_str(), accessibilityAbilityInfo_->GetDescription().c_str());
    EXPECT_EQ(obj->GetEventTypes(), accessibilityAbilityInfo_->GetEventTypes());
    EXPECT_STREQ(obj->GetId().c_str(), accessibilityAbilityInfo_->GetId().c_str());
    EXPECT_STREQ(obj->GetName().c_str(), accessibilityAbilityInfo_->GetName().c_str());
    EXPECT_STREQ(obj->GetPackageName().c_str(), accessibilityAbilityInfo_->GetPackageName().c_str());
    EXPECT_EQ(obj->GetFilterBundleNames().size(), accessibilityAbilityInfo_->GetFilterBundleNames().size());
    ASSERT_EQ((int)obj->GetFilterBundleNames().size(), 0);

    EXPECT_STREQ(obj->GetSettingsAbility().c_str(), accessibilityAbilityInfo_->GetSettingsAbility().c_str());

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_Unmarshalling_001 end";
}

/**
 * @tc.number: AccessibilityAbilityInfo_Unittest_SetCapabilityValues_001
 * @tc.name: SetCapabilityValues
 * @tc.desc: Test function SetCapabilityValues
 */
HWTEST_F(AccessibilityAbilityInfoUnitTest, AccessibilityAbilityInfo_Unittest_SetCapabilityValues_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_SetCapabilityValues_001 start";

    accessibilityAbilityInfo_->SetCapabilityValues(Capability::CAPABILITY_RETRIEVE);
    EXPECT_EQ(accessibilityAbilityInfo_->GetCapabilityValues(), Capability::CAPABILITY_RETRIEVE);

    GTEST_LOG_(INFO) << "AccessibilityAbilityInfo_Unittest_SetCapabilityValues_001 end";
}
} // namespace Accessibility
} // namespace OHOS
