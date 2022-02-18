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
#include "accessible_ability_client_stub.h"
#include "accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_client_stub_impl.h"
#include "mock_accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

class AccessibleAbilityClientStubUnitTest : public ::testing::Test {
public:
    AccessibleAbilityClientStubUnitTest() {}
    ~AccessibleAbilityClientStubUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void WriteInterfaceToken(MessageParcel &data) const;
    sptr<AccessibleAbilityClientStubImplMock> stub_ = nullptr;
};

void AccessibleAbilityClientStubUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientStubUnitTest Start ######################";
}

void AccessibleAbilityClientStubUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientStubUnitTest End ######################";
}

void AccessibleAbilityClientStubUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    stub_ = new AccessibleAbilityClientStubImplMock();
}

void AccessibleAbilityClientStubUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    stub_ = nullptr;
}

void AccessibleAbilityClientStubUnitTest::WriteInterfaceToken(MessageParcel &data) const
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest WriteInterfaceToken";
    data.WriteInterfaceToken(AccessibleAbilityClientStub::GetDescriptor());
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_001
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleInit
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_001 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    WriteInterfaceToken(data);
    data.WriteRemoteObject(channel->AsObject());
    data.WriteInt32(channelId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::INIT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    stub = nullptr;
    channel = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_002
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleOnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_002 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    AccessibilityEventInfo eventInfo {};

    WriteInterfaceToken(data);
    data.WriteParcelable(&eventInfo);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_003
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleOnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_003 start";

#if 0 // TODO: fix me -> keyEvent
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    int sequence = 1;

    WriteInterfaceToken(data);
    data.WriteParcelable(&keyEvent);
    data.WriteInt32(sequence);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

#endif
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_003 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_004
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleOnDisplayResized
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_004 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int displayId = 1;
    Rect rect {};
    float scale = 1;
    float centerX = 1;
    float centerY = 1;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);
    data.WriteParcelable(&rect);
    data.WriteFloat(scale);
    data.WriteFloat(centerX);
    data.WriteFloat(centerY);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_004 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_005
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleOnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_005 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int sequence = 1;
    bool completedSuccessfully = true;

    WriteInterfaceToken(data);
    data.WriteInt32(sequence);
    data.WriteBool(completedSuccessfully);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_005 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_006
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest when code is error.
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_006 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t errorCode = 0xFFFFFFFF;

    WriteInterfaceToken(data);

    int res = stub_->OnRemoteRequest(errorCode, data, reply, option);
    EXPECT_NE(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_006 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_007
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleDisconnect.
 */
HWTEST_F(AccessibleAbilityClientStubUnitTest,
    AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_007 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int channelId = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(channelId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityClient::Message::DISCONNECT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubUnitTest_Unittest_OnRemoteRequest_007 end";
}
