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
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_system_ability_client.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class ASACStateStubUnitTest : public ::testing::Test {
public:
    sptr<AccessibleAbilityManagerServiceStateStub> mockStub_ = nullptr;

    ASACStateStubUnitTest() {}
    ~ASACStateStubUnitTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest SetUp()";
        mockStub_ = new AccessibleAbilityManagerServiceStateStub();
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest TearDown()";
        mockStub_ = nullptr;
    }

    void WriteInterfaceToken(MessageParcel &data) {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest WriteInterfaceToken";
        data.WriteInterfaceToken(AccessibleAbilityManagerServiceStateStub::GetDescriptor());
    }
};

/**
 * @tc.number: OnRemoteRequest_001
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStateStubUnitTest, OnRemoteRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;


    int res = mockStub_->OnRemoteRequest(static_cast<uint32_t>(IAccessibleAbilityManagerServiceState::Message::ON_STATE_CHANGED), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 end";
}

/**
 * @tc.number: OnRemoteRequest_002
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStateStubUnitTest, OnRemoteRequest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 start";
    //set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    WriteInterfaceToken(data);
    mockStub_->OnRemoteRequest(static_cast<uint32_t>(IAccessibleAbilityManagerServiceState::Message::ON_STATE_CHANGED), data, reply, option);
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 end";
}

/**
 * @tc.number: OnStateChanged_001
 * @tc.name: OnStateChanged
 * @tc.desc: Test function OnStateChanged
 */
HWTEST_F(ASACStateStubUnitTest, OnStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnStateChanged_001 start";
    uint32_t stateType = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED;
    mockStub_->OnStateChanged(stateType);

    stateType = AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED;
    mockStub_->OnStateChanged(stateType);

    stateType = AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED;
    mockStub_->OnStateChanged(stateType);

    stateType = AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED;
    mockStub_->OnStateChanged(stateType);

    stateType = AccessibilitySystemAbilityClient::STATE_EXPLORATION_DISABLED |
        AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_DISABLED;
    mockStub_->OnStateChanged(stateType);
    GTEST_LOG_(INFO) << "OnStateChanged_001 end";
}

} //namespace Accessibility
} //namespace OHOS