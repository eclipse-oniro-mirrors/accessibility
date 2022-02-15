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
#include "accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"
#include "accessible_ability_client_proxy.h"
#include "mock_accessible_ability_client_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

class AccessibleAbilityClientProxyUnitTest : public ::testing::Test {
public:
    AccessibleAbilityClientProxyUnitTest() {}
    ~AccessibleAbilityClientProxyUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibleAbilityClientProxy> proxy_ = nullptr;
    sptr<AccessibleAbilityClientStubMock> mock_ = nullptr;
};

void AccessibleAbilityClientProxyUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientProxyUnitTest Start ######################";
}

void AccessibleAbilityClientProxyUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientProxyUnitTest End ######################";
}

void AccessibleAbilityClientProxyUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    mock_ = new AccessibleAbilityClientStubMock();
    proxy_ = std::make_shared<AccessibleAbilityClientProxy>(mock_);
}

void AccessibleAbilityClientProxyUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    mock_ = nullptr;
    proxy_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_Init_001
 * @tc.name: Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest, AccessibleAbilityClientProxyUnitTest_Unittest_Init_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Init_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;
    proxy_->Init(channel, channelId);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::INIT, mock_->code_);

    stub = nullptr;
    channel = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Init_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_Init_002
 * @tc.name: Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest, AccessibleAbilityClientProxyUnitTest_Unittest_Init_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Init_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;
    proxy_->Init(channel, channelId);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::INIT, mock_->code_);

    stub = nullptr;
    channel = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Init_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    int channelId = 0;
    proxy_->Disconnect(channelId);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::DISCONNECT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_002
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    int channelId = 0;
    proxy_->Disconnect(channelId);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::DISCONNECT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_Disconnect_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    AccessibilityEventInfo eventInfo;
    proxy_->OnAccessibilityEvent(eventInfo);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    AccessibilityEventInfo eventInfo;
    proxy_->OnAccessibilityEvent(eventInfo);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    int sequence = 0;
    proxy_->OnKeyPressEvent(*keyEvent, sequence);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_002
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    int sequence = 0;
    proxy_->OnKeyPressEvent(*keyEvent, sequence);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnKeyPressEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_001
 * @tc.name: OnDisplayResized
 * @tc.desc: Test function OnDisplayResized
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    int displayId = 0;
    Rect rect {};
    float scale = 1;
    float centerX = 0;
    float centerY = 0;
    proxy_->OnDisplayResized(displayId, rect, scale, centerX, centerY);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_002
 * @tc.name: OnDisplayResized
 * @tc.desc: Test function OnDisplayResized
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    int displayId = 0;
    Rect rect {};
    float scale = 1;
    float centerX = 0;
    float centerY = 0;
    proxy_->OnDisplayResized(displayId, rect, scale, centerX, centerY);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnDisplayResized_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeSendRequest));

    int sequence = 0;
    bool completedSuccessfully = true;
    proxy_->OnGestureSimulateResult(sequence, completedSuccessfully);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_002
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityClientProxyUnitTest,
    AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityClientStubMock::InvokeErrorSendRequest));

    int sequence = 0;
    bool completedSuccessfully = true;
    proxy_->OnGestureSimulateResult(sequence, completedSuccessfully);

    EXPECT_EQ((int)IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxyUnitTest_Unittest_OnGestureSimulateResult_002 end";
}