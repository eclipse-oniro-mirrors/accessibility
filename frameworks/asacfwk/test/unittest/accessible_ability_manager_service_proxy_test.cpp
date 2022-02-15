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
#include <memory>

#include "accessible_ability_manager_service_proxy.h"
#include "mock_accessible_ability_manager_service_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class ASACProxyUnitTest : public ::testing::Test {
public:
    sptr<MockAccessibleAbilityManagerServiceStub> mockStub_ = nullptr;
    std::shared_ptr<AccessibleAbilityManagerServiceClientProxy> mockProxy_ = nullptr;
    ASACProxyUnitTest() {}
    ~ASACProxyUnitTest() {}

    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest SetUp()";
        mockStub_ = new MockAccessibleAbilityManagerServiceStub();
        mockProxy_ = std::make_shared<AccessibleAbilityManagerServiceClientProxy>(mockStub_);
        GTEST_LOG_(INFO) << "ASACProxyUnitTest SetUp() end";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest TearDown()";
        mockStub_ = nullptr;
        mockProxy_ = nullptr;
    }
};

/**
 * @tc.number: SendEvent_001
 * @tc.name: WriteInterfaceToken
 * @tc.desc: Test function WriteInterfaceToken
 */
HWTEST_F(ASACProxyUnitTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendEvent_001 start";
    AccessibilityEventInfo event {};
    mockProxy_->SendEvent(event, 0);
    GTEST_LOG_(INFO) << "SendEvent_001 end";
}

/**
 * @tc.number: RegisterStateCallback_001
 * @tc.name: RegisterStateCallback
 * @tc.desc: Test function RegisterStateCallback
 */
HWTEST_F(ASACProxyUnitTest, RegisterStateCallback_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterStateCallback_001 start";
    MessageParcel data {};
    mockProxy_->RegisterStateCallback(nullptr, 0);
    GTEST_LOG_(INFO) << "RegisterStateCallback_001 end";
}

/**
 * @tc.number: GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(ASACProxyUnitTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAbilityList_001 start";
    mockProxy_->GetAbilityList(1, 1);
    GTEST_LOG_(INFO) << "GetAbilityList_001 end";
}

/**
 * @tc.number: RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(ASACProxyUnitTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 start";
    mockProxy_->RegisterElementOperator(1, nullptr, 1);
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 end";
}

/**
 * @tc.number: DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(ASACProxyUnitTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 start";
    mockProxy_->DeregisterElementOperator(1);
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 end";
}
} // namespace Accessibility
} // namespace OHOS