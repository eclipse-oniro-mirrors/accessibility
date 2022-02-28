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
#include "accessibility_common_event_registry.h"
#include "common_event_manager.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::EventFwk;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonEventRegistryUnitTest : public ::testing::Test {
public:
    AccessibilityCommonEventRegistryUnitTest() {}
    ~AccessibilityCommonEventRegistryUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<AccessibilityCommonEventRegistry> accessibilityCommonEventRegistry_ = nullptr;
    shared_ptr<CommonEventManager> commonEventManagerMock_ = nullptr;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

void AccessibilityCommonEventRegistryUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventRegistryUnitTest Start ######################";
}

void AccessibilityCommonEventRegistryUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventRegistryUnitTest End ######################";
}

void AccessibilityCommonEventRegistryUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // 注册bundleservice
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);

    accessibilityCommonEventRegistry_ = make_shared<AccessibilityCommonEventRegistry>();
    commonEventManagerMock_ = make_shared<CommonEventManager>();
}

void AccessibilityCommonEventRegistryUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    accessibilityCommonEventRegistry_ = nullptr;
    commonEventManagerMock_ = nullptr;
    mock_ = nullptr;
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_Unittest_StartRegister_001
 * @tc.name: StartRegister
 * @tc.desc: Test function StartRegister
 */
HWTEST_F(AccessibilityCommonEventRegistryUnitTest, AccessibilityCommonEventRegistry_Unittest_StartRegister_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_StartRegister_001 start";

    EXPECT_TRUE(accessibilityCommonEventRegistry_->StartRegister());

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_StartRegister_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_Unittest_HandleRemovedUser_001
 * @tc.name: HandleRemovedUser
 * @tc.desc: Test function HandleRemovedUser
 */
HWTEST_F(AccessibilityCommonEventRegistryUnitTest, AccessibilityCommonEventRegistry_Unittest_HandleRemovedUser_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandleRemovedUser_001 start";

    accessibilityCommonEventRegistry_->StartRegister();

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandleRemovedUser_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_Unittest_HandlePresentUser_001
 * @tc.name: HandlePresentUser
 * @tc.desc: Test function HandlePresentUser
 */
HWTEST_F(AccessibilityCommonEventRegistryUnitTest, AccessibilityCommonEventRegistry_Unittest_HandlePresentUser_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePresentUser_001 start";

    accessibilityCommonEventRegistry_->StartRegister();

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_PRESENT);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePresentUser_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_Unittest_HandlePackageRemoved_001
 * @tc.name: HandlePackageRemoved
 * @tc.desc: Test function HandlePackageRemoved
 */
HWTEST_F(AccessibilityCommonEventRegistryUnitTest, AccessibilityCommonEventRegistry_Unittest_HandlePackageRemoved_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePackageRemoved_001 start";

    accessibilityCommonEventRegistry_->StartRegister();

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePackageRemoved_001 end";
}

/**
 * @tc.number: AccessibilityCommonEventRegistry_Unittest_HandlePackageChanged_001
 * @tc.name: HandlePackageChanged
 * @tc.desc: Test function HandlePackageChanged
 */
HWTEST_F(AccessibilityCommonEventRegistryUnitTest, AccessibilityCommonEventRegistry_Unittest_HandlePackageChanged_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePackageChanged_001 start";

    accessibilityCommonEventRegistry_->StartRegister();

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEventRegistry_Unittest_HandlePackageChanged_001 end";
}
} // namespace Accessibility
} // namespace OHOS