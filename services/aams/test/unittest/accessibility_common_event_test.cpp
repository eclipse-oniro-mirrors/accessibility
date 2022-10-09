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
#include "accessibility_common_event.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t USERID_1 = 1;
    constexpr int32_t SLEEP_TIME = 1;
    const std::string BUNDLE_NAME = "test";
}
class AccessibilityCommonEventUnitTest : public ::testing::Test {
public:
    AccessibilityCommonEventUnitTest()
    {}
    ~AccessibilityCommonEventUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<CommonEventManager> commonEventManagerMock_ = nullptr;
};

void AccessibilityCommonEventUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityCommonEventUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventUnitTest End ######################";
    Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityCommonEventUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
}

void AccessibilityCommonEventUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserRemoved_001
 * @tc.name: HandleUserRemoved
 * @tc.desc: Test function HandleUserRemoved(remove fail)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserRemoved_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    data.SetWant(want);
    data.SetCode(-1);
    CommonEventManager::PublishCommonEvent(data);
    sleep(SLEEP_TIME);
    std::vector<int32_t> userIds;
    AccessibilityAbilityHelper::GetInstance().GetUserIds(userIds);
    EXPECT_EQ(0, userIds.size());

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserRemoved_002
 * @tc.name: HandleUserRemoved
 * @tc.desc: Test function HandleUserRemoved(add/remove success)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserRemoved_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_002 start";

    CommonEventData data1;
    Want want1;
    want1.SetAction(CommonEventSupport::COMMON_EVENT_USER_ADDED);
    data1.SetWant(want1);
    data1.SetCode(USERID_1);
    CommonEventManager::PublishCommonEvent(data1);
    sleep(SLEEP_TIME);
    std::vector<int32_t> userIds;
    AccessibilityAbilityHelper::GetInstance().GetUserIds(userIds);
    EXPECT_EQ(1, userIds.size());

    CommonEventData data2;
    Want want2;
    want2.SetAction(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    data2.SetWant(want2);
    data2.SetCode(USERID_1);
    CommonEventManager::PublishCommonEvent(data2);
    sleep(SLEEP_TIME);
    userIds.clear();
    AccessibilityAbilityHelper::GetInstance().GetUserIds(userIds);
    EXPECT_EQ(0, userIds.size());

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_002 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserAdded_001
 * @tc.name: HandleUserAdded
 * @tc.desc: Test function HandleUserAdded(add fail)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserAdded_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserAdded_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_ADDED);
    data.SetWant(want);
    data.SetCode(-1);
    CommonEventManager::PublishCommonEvent(data);
    sleep(SLEEP_TIME);
    std::vector<int32_t> userIds;
    AccessibilityAbilityHelper::GetInstance().GetUserIds(userIds);
    EXPECT_EQ(0, userIds.size());

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserAdded_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserSwitched_001
 * @tc.name: HandleUserSwitched
 * @tc.desc: Test function HandleUserSwitched(switch success)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserSwitched_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserSwitched_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    data.SetWant(want);
    data.SetCode(USERID_1);
    CommonEventManager::PublishCommonEvent(data);
    sleep(SLEEP_TIME);
    int32_t userId = AccessibilityAbilityHelper::GetInstance().GetCurrentUserId();
    EXPECT_EQ(USERID_1, userId);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserSwitched_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserSwitched_002
 * @tc.name: HandleUserSwitched
 * @tc.desc: Test function HandleUserSwitched(switch fail)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserSwitched_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserSwitched_002 start";

    AccessibilityAbilityHelper::GetInstance().SetCurrentUserId(USERID_1);
    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    data.SetWant(want);
    data.SetCode(-1);
    CommonEventManager::PublishCommonEvent(data);
    sleep(SLEEP_TIME);
    int32_t userId = AccessibilityAbilityHelper::GetInstance().GetCurrentUserId();
    EXPECT_EQ(USERID_1, userId);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserSwitched_002 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001
 * @tc.name: HandlePackageRemoved
 * @tc.desc: Test function HandlePackageRemoved(add/remove success)
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001 start";

    CommonEventData data1;
    Want want1;
    want1.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want1.SetBundle(BUNDLE_NAME);
    data1.SetWant(want1);
    CommonEventManager::PublishCommonEvent(data1);
    sleep(SLEEP_TIME);
    std::vector<std::string> packages;
    AccessibilityAbilityHelper::GetInstance().GetPackages(packages);
    EXPECT_EQ(1, packages.size());

    CommonEventData data2;
    Want want2;
    want2.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want2.SetBundle(BUNDLE_NAME);
    data2.SetWant(want2);
    CommonEventManager::PublishCommonEvent(data2);
    sleep(SLEEP_TIME);
    packages.clear();
    AccessibilityAbilityHelper::GetInstance().GetPackages(packages);
    EXPECT_EQ(0, packages.size());

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandlePackageChanged_001
 * @tc.name: HandlePackageChanged
 * @tc.desc: Test function HandlePackageChanged
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandlePackageChanged_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageChanged_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle(BUNDLE_NAME);
    data.SetWant(want);
    CommonEventManager::PublishCommonEvent(data);
    sleep(SLEEP_TIME);
    EXPECT_TRUE(AccessibilityAbilityHelper::GetInstance().GetChangePackageFlag());

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageChanged_001 end";
}
} // namespace Accessibility
} // namespace OHOS