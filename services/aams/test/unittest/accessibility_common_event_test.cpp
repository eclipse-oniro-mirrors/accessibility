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
#include "common_event_manager.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace Accessibility {
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
}

void AccessibilityCommonEventUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityCommonEventUnitTest End ######################";
}

void AccessibilityCommonEventUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    Singleton<AccessibilityCommonEvent>::GetInstance().SubscriberEvent(nullptr);
    commonEventManagerMock_ = std::make_shared<CommonEventManager>();
}

void AccessibilityCommonEventUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    Singleton<AccessibilityCommonEvent>::GetInstance().UnSubscriberEvent();
    commonEventManagerMock_ = nullptr;
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserRemoved_001
 * @tc.name: HandleUserRemoved
 * @tc.desc: Test function HandleUserRemoved
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserRemoved_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserRemoved_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandleUserAdded_001
 * @tc.name: HandleUserAdded
 * @tc.desc: Test function HandleUserAdded
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandleUserAdded_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserAdded_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_USER_ADDED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandleUserAdded_001 end";
}

/**
 * @tc.number: AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001
 * @tc.name: HandlePackageRemoved
 * @tc.desc: Test function HandlePackageRemoved
 */
HWTEST_F(AccessibilityCommonEventUnitTest, AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageRemoved_001 start";

    CommonEventData data;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

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
    data.SetWant(want);

    commonEventManagerMock_->PublishCommonEvent(data);

    GTEST_LOG_(INFO) << "AccessibilityCommonEvent_Unittest_HandlePackageChanged_001 end";
}
} // namespace Accessibility
} // namespace OHOS