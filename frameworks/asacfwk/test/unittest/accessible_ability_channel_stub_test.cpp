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
#include "mock_accessible_ability_channel_stub_impl.h"
#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_operator.h"
#include "accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityChannelStubUnitTest : public ::testing::Test {
public:
    AccessibleAbilityChannelStubUnitTest() {}
    ~AccessibleAbilityChannelStubUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void WriteInterfaceToken(MessageParcel &data);
    sptr<AccessibleAbilityChannelStubImplMock> stub_ = nullptr;
};

void AccessibleAbilityChannelStubUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelStubUnitTest Start ######################";
}

void AccessibleAbilityChannelStubUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelStubUnitTest End ######################";
}

void AccessibleAbilityChannelStubUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    stub_ = new AccessibleAbilityChannelStubImplMock();
}

void AccessibleAbilityChannelStubUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    stub_ = nullptr;
}

void AccessibleAbilityChannelStubUnitTest::WriteInterfaceToken(MessageParcel &data)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest WriteInterfaceToken";
    data.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_001
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleSearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_001 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int accessibilityWindowId = 0;
    long elementId = 0;
    int requestId = 0;
    int mode = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    WriteInterfaceToken(data);
    data.WriteInt32(accessibilityWindowId);
    data.WriteInt64(elementId);
    data.WriteInt32(requestId);
    data.WriteRemoteObject(callback->AsObject());
    data.WriteInt32(mode);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_002
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleSearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_002 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int accessibilityWindowId = 0;
    long elementId = 0;
    std::string text = "";
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    WriteInterfaceToken(data);
    data.WriteInt32(accessibilityWindowId);
    data.WriteInt32(elementId);
    data.WriteString(text);
    data.WriteInt32(requestId);
    data.WriteRemoteObject(callback->AsObject());

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_003
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleFindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_003 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int accessibilityWindowId = 0;
    long elementId = 0;
    int focusType = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    WriteInterfaceToken(data);
    data.WriteInt32(accessibilityWindowId);
    data.WriteInt32(elementId);
    data.WriteInt32(focusType);
    data.WriteInt32(requestId);
    data.WriteRemoteObject(callback->AsObject());

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_003 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_004
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleFocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_004 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int accessibilityWindowId = 0;
    long elementId = 0;
    int direction = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    WriteInterfaceToken(data);
    data.WriteInt32(accessibilityWindowId);
    data.WriteInt32(elementId);
    data.WriteInt32(direction);
    data.WriteInt32(requestId);
    data.WriteRemoteObject(callback->AsObject());

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_004 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_005
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_005 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int accessibilityWindowId = 0;
    long elementId = 0;
    int action = 0;
    std::map<std::string, std::string> actionArguments {};
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    WriteInterfaceToken(data);
    data.WriteInt32(accessibilityWindowId);
    data.WriteInt32(elementId);
    data.WriteInt32(action);

    vector<string> actionArgumentsKey {};
    vector<string> actionArgumentsValue {};
    for (auto iter = actionArguments.begin(); iter != actionArguments.end(); iter++) {
        actionArgumentsKey.push_back(iter->first);
        actionArgumentsValue.push_back(iter->second);
    }
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteInt32(requestId);
    data.WriteRemoteObject(callback->AsObject());

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::PERFORM_ACTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_005 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_006
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleGetWindows
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_006 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    WriteInterfaceToken(data);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_WINDOWS),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_EQ(reply.ReadInt32(), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_006 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_007
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleExecuteCommonAction
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_007 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int action = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(action);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::EXECUTE_COMMON_ACTION),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_007 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_009
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleSetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_009 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    bool handled = true;
    int sequence = 0;

    WriteInterfaceToken(data);
    data.WriteBool(handled);
    data.WriteInt32(sequence);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_009 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_010
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleGetDisplayResizeScale
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_010 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int displayId = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_SCALE), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_EQ(reply.ReadFloat(), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_010 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_011
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleGetDisplayResizeCenterX
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_011 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int displayId = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_CETER_X), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_EQ(reply.ReadFloat(), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_011 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_012
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleGetDisplayResizeCenterY
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_012 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int displayId = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_CETER_Y), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_EQ(reply.ReadFloat(), 0);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_012 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_013
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleGetDisplayResizeRect
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_013 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int displayId = 0;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_RECT), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    sptr<Rect> result = reply.ReadStrongParcelable<Rect>();
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->GetLeftTopXScreenPostion(), 1);
    EXPECT_EQ(result->GetLeftTopYScreenPostion(), 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_013 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_014
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleResetDisplayResize
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_014 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int displayId = 0;
    bool animate = true;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);
    data.WriteBool(animate);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::RESET_DISPALYRESIZE), data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_014 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_015
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleSetDisplayResizeScaleAndCenter
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_015 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int displayId = 0;
    float scale = 0;
    float centerX = 0;
    float centerY = 0;
    bool animate = true;

    WriteInterfaceToken(data);
    data.WriteInt32(displayId);
    data.WriteFloat(scale);
    data.WriteFloat(centerX);
    data.WriteFloat(centerY);
    data.WriteBool(animate);

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_DISPLAYRESIZE_SCALE_AND_CENTER),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);
    EXPECT_TRUE(reply.ReadBool());

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_015 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_016
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function HandleSendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_016 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int sequence = 0;
    std::vector<GesturePathDefine> gestureSteps {};

    WriteInterfaceToken(data);
    data.WriteInt32(sequence);
    data.WriteInt32(gestureSteps.size());
    for (auto &step : gestureSteps) {
        data.WriteParcelable(&step);
    }

    int res = stub_->OnRemoteRequest(
        static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE),
        data, reply, option);
    EXPECT_EQ(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_016 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_018
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test function OnRemoteRequest when code is error.
 */
HWTEST_F(AccessibleAbilityChannelStubUnitTest,
    AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_018 start";

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    uint32_t errorCode = 0xFFFFFFFF;

    WriteInterfaceToken(data);

    int res = stub_->OnRemoteRequest(errorCode, data, reply, option);
    EXPECT_NE(res, OHOS::Accessibility::NO_ERROR);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubUnitTest_Unittest_OnRemoteRequest_018 end";
}
} // namespace Accessibility
} // namespace OHOS
