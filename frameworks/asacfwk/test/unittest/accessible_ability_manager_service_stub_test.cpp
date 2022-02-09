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

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class ASACStubUnitTest : public ::testing::Test {
public:
    sptr<MockAccessibleAbilityManagerServiceStub> mockStub_ = nullptr;

    ASACStubUnitTest() {}
    ~ASACStubUnitTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACStubUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "ASACStubUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACStubUnitTest SetUp()";
        mockStub_ = new MockAccessibleAbilityManagerServiceStub();
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "ASACStubUnitTest TearDown()";
        mockStub_ = nullptr;
    }
    void WriteInterfaceToken(MessageParcel &data) {
        GTEST_LOG_(INFO) << "ASACStateStubUnitTest WriteInterfaceToken";
        data.WriteInterfaceToken(MockAccessibleAbilityManagerServiceStub::GetDescriptor());
    }

};

/**
 * @tc.number: OnRemoteRequest_001
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::SEND_EVENT), data, reply, option);
    EXPECT_NE(res, OHOS::Accessibility::ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "OnRemoteRequest_001 end";
}

HWTEST_F(ASACStubUnitTest, OnRemoteRequest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_STATE_CALLBACK),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_002 end";
}

/**
 * @tc.number: OnRemoteRequest_003
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_003 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_ABILITYLIST),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_003 end";
}

/**
 * @tc.number: OnRemoteRequest_004
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_004 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_INTERACTION_CONNECTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_004 end";
}

/**
 * @tc.number: OnRemoteRequest_005
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_005 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::DEREGISTER_INTERACTION_CONNECTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_005 end";
}

/**
 * @tc.number: OnRemoteRequest_006
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_006 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::INTERRUPT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::INTERRUPT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_006 end";
}

/**
 * @tc.number: OnRemoteRequest_007
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_007 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_SUGGESTED_INTERVAL),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::GET_SUGGESTED_INTERVAL),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_007 end";
}

/**
 * @tc.number: OnRemoteRequest_008
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest
 */
HWTEST_F(ASACStubUnitTest, OnRemoteRequest_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnRemoteRequest_008 start";
    //Not set descriptor
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = mockStub_->OnRemoteRequest
        (static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_ABILITY_CONNECTION_CLIENT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::ERR_INVALID_STATE);

    //set descriptor
    WriteInterfaceToken(data);
    res = mockStub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceClient::Message::REGISTER_ABILITY_CONNECTION_CLIENT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    GTEST_LOG_(INFO) << "OnRemoteRequest_008 end";
}

} //namespace Accessibility
} //namespace OHOS