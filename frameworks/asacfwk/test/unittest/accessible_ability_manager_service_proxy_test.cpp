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

#include <memory>
#include <gtest/gtest.h>
#include "mock_accessible_ability_manager_service_stub.h"
#include "accessible_ability_manager_service_proxy.h"

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

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACProxyUnitTest SetUp()";
        mockStub_ = new MockAccessibleAbilityManagerServiceStub();
        mockProxy_ = std::make_shared<AccessibleAbilityManagerServiceClientProxy>(mockStub_);
        GTEST_LOG_(INFO) << "ASACProxyUnitTest SetUp() end";
    };
    void TearDown() {
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
 * @tc.number: RegisterInteractionOperation_001
 * @tc.name: RegisterInteractionOperation
 * @tc.desc: Test function RegisterInteractionOperation
 */
HWTEST_F(ASACProxyUnitTest, RegisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_001 start";
    mockProxy_->RegisterInteractionOperation(1, nullptr, 1);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_001 end";
}

/**
 * @tc.number: DeregisterInteractionOperation_001
 * @tc.name: DeregisterInteractionOperation
 * @tc.desc: Test function DeregisterInteractionOperation
 */
HWTEST_F(ASACProxyUnitTest, DeregisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterInteractionOperation_001 start";
    mockProxy_->DeregisterInteractionOperation(1);
    GTEST_LOG_(INFO) << "DeregisterInteractionOperation_001 end";
}

/**
 * @tc.number: Interrupt_001
 * @tc.name: Interrupt
 * @tc.desc: Test function Interrupt
 */
HWTEST_F(ASACProxyUnitTest, Interrupt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Interrupt_001 start";
    mockProxy_->Interrupt(0);
    GTEST_LOG_(INFO) << "Interrupt_001 end";
}

/**
 * @tc.number: GetSuggestedInterval_001
 * @tc.name: GetSuggestedInterval
 * @tc.desc: Test function GetSuggestedInterval
 */
HWTEST_F(ASACProxyUnitTest, GetSuggestedInterval_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSuggestedInterval_001 start";
    mockProxy_->GetSuggestedInterval();
    GTEST_LOG_(INFO) << "GetSuggestedInterval_001 end";
}

/**
 * @tc.number: RegisterAbilityConnectionClientTmp_001
 * @tc.name: RegisterAbilityConnectionClientTmp
 * @tc.desc: Test function RegisterAbilityConnectionClientTmp
 */
HWTEST_F(ASACProxyUnitTest, RegisterAbilityConnectionClientTmp_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityConnectionClientTmp_001 start";
    MessageParcel data {};
    mockProxy_->RegisterAbilityConnectionClientTmp(nullptr);
    GTEST_LOG_(INFO) << "RegisterAbilityConnectionClientTmp_001 end";
}

} //namespace Accessibility
} //namespace OHOS