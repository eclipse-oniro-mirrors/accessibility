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

#include <map>
#include <gtest/gtest.h>
#include "accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"
#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_operator.h"
#include "gesture_simulation.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

class AccessibleAbilityChannelProxyUnitTest : public ::testing::Test {
public:
    AccessibleAbilityChannelProxyUnitTest() {}
    ~AccessibleAbilityChannelProxyUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibleAbilityChannelProxy> proxy_ = nullptr;
    sptr<AccessibleAbilityChannelStubMock> mock_ = nullptr;
};

void AccessibleAbilityChannelProxyUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelProxyUnitTest Start ######################";
}

void AccessibleAbilityChannelProxyUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityChannelProxyUnitTest End ######################";
}

void AccessibleAbilityChannelProxyUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    mock_ = new AccessibleAbilityChannelStubMock();
    proxy_ = std::make_shared<AccessibleAbilityChannelProxy>(mock_);
}

void AccessibleAbilityChannelProxyUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    mock_ = nullptr;
    proxy_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int requestId = 0;
    int mode = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId, callback, mode);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_002
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int requestId = 0;
    int mode = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId, callback, mode);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfoByAccessibilityId_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    std::string text = "";
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_002
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    std::string text = "";
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SearchElementInfosByText_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int focusType = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_002
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int focusType = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FindFocusedElementInfo_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int direction = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_002
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int direction = 0;
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_FocusMoveSearch_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int action = 0;
    std::map<std::string, std::string> actionArguments {};
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::PERFORM_ACTION, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int accessibilityWindowId = 0;
    long elementId = 0;
    int action = 0;
    std::map<std::string, std::string> actionArguments {};
    int requestId = 0;
    sptr<AccessibilityOperator> stub = new AccessibilityOperator();
    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(stub->AsObject());

    proxy_->ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::PERFORM_ACTION, mock_->code_);

    stub = nullptr;
    callback = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteAction_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    proxy_->GetWindows();
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_WINDOWS, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    proxy_->GetWindows();
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_WINDOWS, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetWindows_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_001
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int action = 0;

    proxy_->ExecuteCommonAction(action);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::EXECUTE_COMMON_ACTION, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_002
 * @tc.name: ExecuteCommonAction
 * @tc.desc: Test function ExecuteCommonAction
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int action = 0;

    proxy_->ExecuteCommonAction(action);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::EXECUTE_COMMON_ACTION, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ExecuteCommonAction_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_001
 * @tc.name: SetOnKeyPressEventResult
 * @tc.desc: Test function SetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    bool handled = true;
    int sequence = 0;

    proxy_->SetOnKeyPressEventResult(handled, sequence);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_002
 * @tc.name: SetOnKeyPressEventResult
 * @tc.desc: Test function SetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    bool handled = true;
    int sequence = 0;

    proxy_->SetOnKeyPressEventResult(handled, sequence);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetOnKeyPressEventResult_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_001
 * @tc.name: GetDisplayResizeScale
 * @tc.desc: Test function GetDisplayResizeScale
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeScale(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_SCALE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_002
 * @tc.name: GetDisplayResizeScale
 * @tc.desc: Test function GetDisplayResizeScale
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeScale(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_SCALE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeScale_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_001
 * @tc.name: GetDisplayResizeCenterX
 * @tc.desc: Test function GetDisplayResizeCenterX
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeCenterX(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_CETER_X, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_002
 * @tc.name: GetDisplayResizeCenterX
 * @tc.desc: Test function GetDisplayResizeCenterX
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeCenterX(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_CETER_X, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterX_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_001
 * @tc.name: GetDisplayResizeCenterY
 * @tc.desc: Test function GetDisplayResizeCenterY
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeCenterY(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_CETER_Y, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_002
 * @tc.name: GetDisplayResizeCenterY
 * @tc.desc: Test function GetDisplayResizeCenterY
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeCenterY(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_CETER_Y, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeCenterY_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_001
 * @tc.name: GetDisplayResizeRect
 * @tc.desc: Test function GetDisplayResizeRect
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeRect(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_RECT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_002
 * @tc.name: GetDisplayResizeRect
 * @tc.desc: Test function GetDisplayResizeRect
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;

    proxy_->GetDisplayResizeRect(displayId);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_RECT, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_GetDisplayResizeRect_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_001
 * @tc.name: ResetDisplayResize
 * @tc.desc: Test function ResetDisplayResize
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;
    bool animate = true;

    proxy_->ResetDisplayResize(displayId, animate);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::RESET_DISPALYRESIZE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_002
 * @tc.name: ResetDisplayResize
 * @tc.desc: Test function ResetDisplayResize
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;
    bool animate = true;

    proxy_->ResetDisplayResize(displayId, animate);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::RESET_DISPALYRESIZE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_ResetDisplayResize_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001
 * @tc.name: SetDisplayResizeScaleAndCenter
 * @tc.desc: Test function SetDisplayResizeScaleAndCenter
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int displayId = 0;
    float scale = 0;
    float centerX = 0;
    float centerY = 0;
    bool animate = true;

    proxy_->SetDisplayResizeScaleAndCenter(displayId, scale, centerX, centerY, animate);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SET_DISPLAYRESIZE_SCALE_AND_CENTER, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_002
 * @tc.name: SetDisplayResizeScaleAndCenter
 * @tc.desc: Test function SetDisplayResizeScaleAndCenter
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int displayId = 0;
    float scale = 0;
    float centerX = 0;
    float centerY = 0;
    bool animate = true;

    proxy_->SetDisplayResizeScaleAndCenter(displayId, scale, centerX, centerY, animate);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SET_DISPLAYRESIZE_SCALE_AND_CENTER, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SetDisplayResizeScaleAndCenter_002 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_001
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_001 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeSendRequest));

    int requestId = 0;
    std::vector<GesturePathDefine> gestureSteps {};

    proxy_->SendSimulateGesture(requestId, gestureSteps);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_002
 * @tc.name: SendSimulateGesture
 * @tc.desc: Test function SendSimulateGesture
 */
HWTEST_F(AccessibleAbilityChannelProxyUnitTest, AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_002 start";

    EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(mock_.GetRefPtr(), &AccessibleAbilityChannelStubMock::InvokeErrorSendRequest));

    int requestId = 0;
    std::vector<GesturePathDefine> gestureSteps {};

    proxy_->SendSimulateGesture(requestId, gestureSteps);
    EXPECT_EQ((int)IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE, mock_->code_);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelProxyUnitTest_Unittest_SendSimulateGesture_002 end";
}
