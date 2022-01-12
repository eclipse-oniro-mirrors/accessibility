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
#include "accessible_ability_connection.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "accessibility_display_manager.h"
#include <stdio.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

extern int testChannalId;
extern int testEventType;
extern int testInterrupt;
extern int testDisplayId;
extern int testGesture;
extern int testGestureSimulateResult;
extern int testKeyPressEvent;
extern int testFingerprintGestureValidityChanged;
extern int testFingerprintGesture;

class AccessibleAbilityConnectionUnitTest : public ::testing::Test {
public:
    AccessibleAbilityConnectionUnitTest() {}
    ~AccessibleAbilityConnectionUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<AccessibleAbilityConnection> connection_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> obj_= nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AccessibleAbilityEventHandler> accessibleAbilityEventHandler_ = nullptr;
};

void AccessibleAbilityConnectionUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest Start ######################";
}

void AccessibleAbilityConnectionUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest End ######################";
}

void AccessibleAbilityConnectionUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Start AAMS
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();
    /* touchGuider->StartUp */
    AccessibilityDisplayManager& displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo wmdInfo;
    wmdInfo.dpi = 1;
    displayMgr.SetDisplay(wmdInfo);
    //new Interaction proxy
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> proxy =
        new AccessibilityInteractionOperationProxy(stub);
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(0, proxy, 0);
    // aams RegisterInteractionOperation
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->
        RegisterInteractionOperation(0, proxy, 0);
    //new AAconnection
    AppExecFwk::AbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    accountData_ = new AccessibilityAccountData(0);
    accountData_->AddAccessibilityInteractionConnection(0,connection);
    connection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1","2","3");
    runner_ = AppExecFwk::EventRunner::Create("test");
    accessibleAbilityEventHandler_ = std::make_shared<AccessibleAbilityEventHandler>(runner_);
    obj_ = new AccessibleAbilityClientStubImpl(accessibleAbilityEventHandler_);
    connection_->OnAbilityConnectDone(*elementName_, obj_, 0);
}

void AccessibleAbilityConnectionUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    // Deregister InteractionOperation
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->DeregisterInteractionOperation(0);
    connection_ ->OnAbilityDisconnectDone(*elementName_, 0);
    connection_ = nullptr;
    elementName_ = nullptr;
    testEventType = -1;
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetAbilityInfo_001
 * @tc.name: GetAbilityInfo
 * @tc.desc: Test function GetAbilityInfo
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetAbilityInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAbilityInfo_001 start";

    auto abilityInfo = connection_->GetAbilityInfo();
    auto getUiInteractiveTime = abilityInfo.GetUiInteractiveTime();
    EXPECT_EQ(int(getUiInteractiveTime), 10000);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAbilityInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetElementName_001
 * @tc.name: GetElementName
 * @tc.desc: Test function GetElementName
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetElementName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetElementName_001 start";

    auto elementName = connection_->GetElementName();
    auto ret = elementName.GetDeviceID();
    EXPECT_STREQ(ret.c_str(), "1");
    
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAbilityInfo_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetAccountData_001
 * @tc.name: GetAccountData
 * @tc.desc: Test function GetAccountData
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetAccountData_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAccountData_001 start";

    auto ret = connection_->GetAccountData();
    EXPECT_TRUE(ret.GetRefPtr());

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetAccountData_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_GetProxy_001
 * @tc.name: GetProxy
 * @tc.desc: Test function GetProxy
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_GetProxy_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 start";

    auto ret = connection_->GetProxy();
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_GetProxy_001 end";
}

// /**
//  * @tc.number: AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001
//  * @tc.name: OnAbilityConnectDone
//  * @tc.desc: Test function OnAbilityConnectDone
//  */
// HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001 start";


//     GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAbilityConnectDone_001 end";
// }

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 start";
    AccessibilityEventInfo eventInfo;
    /* EventType is white list */
    eventInfo.SetEventType(EventType::TYPE_PAGE_STATE_UPDATE);
    connection_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(int(EventType::TYPE_PAGE_STATE_UPDATE), testEventType);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 start";
    AccessibilityEventInfo eventInfo;
    /* EventType not white list */
    eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    /* invalid window */
    eventInfo.SetWindowId(3);
    connection_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(-1, testEventType);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_003
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_003 start";
    AccessibilityEventInfo eventInfo;
    /* EventType not white list */
    eventInfo.SetEventType(EventType::TYPE_VIEW_CLICKED_EVENT);
    /* valid window */
    eventInfo.SetWindowId(0);
    connection_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(int(EventType::TYPE_VIEW_CLICKED_EVENT), testEventType);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnAccessibilityEvent_003 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnInterrupt_001
 * @tc.name: OnInterrupt
 * @tc.desc: Test function OnInterrupt
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnInterrupt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnInterrupt_001 start";
    connection_->OnInterrupt();
    EXPECT_EQ(testInterrupt, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnInterrupt_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnGesture_001
 * @tc.name: OnGesture
 * @tc.desc: Test function OnGesture
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGesture_001 start";

    connection_->OnGesture(1);
    EXPECT_EQ(testGesture, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001 start";

    MMI::KeyEvent keyEvent;
    connection_->OnKeyPressEvent(keyEvent, 1);
    EXPECT_EQ(testKeyPressEvent, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnKeyPressEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnDisplayResizeChanged_001
 * @tc.name: OnDisplayResizeChanged
 * @tc.desc: Test function OnDisplayResizeChanged
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnDisplayResizeChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnDisplayResizeChanged_001 start";

    Rect rect(0,0,0,0);
    connection_->OnDisplayResizeChanged(1,rect,0,0,0);
    EXPECT_EQ(testDisplayId, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnDisplayResizeChanged_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001 start";

    Rect rect(0,0,0,0);
    connection_->OnGestureSimulateResult(1,false);
    EXPECT_EQ(testGestureSimulateResult, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnFingerprintGestureValidityChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnFingerprintGestureValidityChanged_001 start";

    connection_->OnFingerprintGestureValidityChanged(true);
    EXPECT_EQ(testFingerprintGestureValidityChanged, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnFingerprintGestureValidityChanged_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_OnFingerprintGesture_001
 * @tc.name: OnFingerprintGesture
 * @tc.desc: Test function OnFingerprintGesture
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_OnFingerprintGesture, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnFingerprintGesture_001 start";

    connection_->OnFingerprintGesture(1);
    EXPECT_EQ(testFingerprintGesture, 1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_OnFingerprintGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_Connect_001
 * @tc.name: Connect
 * @tc.desc: Test function Connect
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_Connect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Connect_001 start";

    connection_->Connect();
    auto accountData = connection_->GetAccountData();
    EXPECT_EQ(int(accountData->GetConnectingA11yAbilities().size()),1);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Connect_001 end";
}
/**
 * @tc.number: AccessibleAbilityConnection_Unittest_Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityConnectionUnitTest, AccessibleAbilityConnection_Unittest_Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Disconnect_001 start";

    connection_->Disconnect();
    EXPECT_EQ(testChannalId,0);

    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_Reset_001 end";
}

/***********************************************************************************************************/
extern int testElementId;
extern int testClearFocus;
extern int testOutsideTouch;
extern bool testKeyevent;
class AccessibleAbilityChannelStubImplUnitTest : public ::testing::Test {
public:
    AccessibleAbilityChannelStubImplUnitTest() {}
    ~AccessibleAbilityChannelStubImplUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<AccessibleAbilityChannelStubImpl> aacs_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> aastub_ = nullptr;
    sptr<AppExecFwk::ElementName> elementName_ = nullptr;
    sptr<AccessibleAbilityConnection> AAConnection_ = nullptr;
};

void AccessibleAbilityChannelStubImplUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest Start ######################";
}

void AccessibleAbilityChannelStubImplUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityConnectionUnitTest End ######################";
}

void AccessibleAbilityChannelStubImplUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    // Start AAMS
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();
    //new Interaction proxy
    sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    sptr<IAccessibilityInteractionOperation> proxy =
        new AccessibilityInteractionOperationProxy(stub);
    sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(0, proxy, 0);
    // aams RegisterInteractionOperation
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->
        RegisterInteractionOperation(0, proxy, 0);
    //new AAconnection
    AppExecFwk::AbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    accountData_ = new AccessibilityAccountData(0);
    accountData_->AddAccessibilityInteractionConnection(0,connection);
    AAConnection_ = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    elementName_ = new AppExecFwk::ElementName("1","2","3");
    //new aastub
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    aastub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    AAConnection_->OnAbilityConnectDone(*elementName_, aastub_, 0);
    //aac
    aacs_ = std::make_shared<AccessibleAbilityChannelStubImpl>(*AAConnection_);
}

void AccessibleAbilityChannelStubImplUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    AAConnection_ ->OnAbilityDisconnectDone(*elementName_, 0);
    aacs_ = nullptr;
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_SearchElementInfoByAccessibilityId_001
 * @tc.name: SearchElementInfoByAccessibilityId
 * @tc.desc: Test function SearchElementInfoByAccessibilityId
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_SearchElementInfoByAccessibilityId_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SearchElementInfoByAccessibilityId_001 start";

    aacs_->SearchElementInfoByAccessibilityId(0,0,0,nullptr,0);

    EXPECT_EQ(testElementId, 0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SearchElementInfoByAccessibilityId_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_SearchElementInfosByText_001
 * @tc.name: SearchElementInfosByText
 * @tc.desc: Test function SearchElementInfosByText
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest, 
 AccessibleAbilityChannelStubImpl_Unittest_SearchElementInfosByText_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SearchElementInfosByText_001 start";

    aacs_->SearchElementInfosByText(0,1,"0",0,nullptr);

    EXPECT_EQ(testElementId, 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityConnection_Unittest_FindFocusedElementInfo_001
 * @tc.name: FindFocusedElementInfo
 * @tc.desc: Test function FindFocusedElementInfo
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_Unittest_FindFocusedElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SearchElementInfosByText_001 start";

    aacs_->FindFocusedElementInfo(0,2,0,0,nullptr);

    EXPECT_EQ(testElementId, 2);
    GTEST_LOG_(INFO) << "AccessibleAbilityConnection_Unittest_SearchElementInfosByText_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_FocusMoveSearch_001
 * @tc.name: FocusMoveSearch
 * @tc.desc: Test function FocusMoveSearch
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_FocusMoveSearch_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_FocusMoveSearch_001 start";

    aacs_->FocusMoveSearch(0,3,0,0,nullptr);

    EXPECT_EQ(testElementId, 3);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_FocusMoveSearch_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformAction_001
 * @tc.name: PerformAction
 * @tc.desc: Test function PerformAction
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_PerformAction_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformAction_001 start";
    std::map<std::string, std::string> actionArguments;
    aacs_->PerformAction(0, 4, 0, actionArguments, 0,nullptr);

    EXPECT_EQ(testElementId, 4);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_GetWindows_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetWindows_001 start";
    auto ret = aacs_->GetWindows();
    EXPECT_EQ(int(ret.size()), 1);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetWindows_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformCommonAction_001
 * @tc.name: PerformCommonAction
 * @tc.desc: Test function PerformCommonAction
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_PerformCommonAction_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformCommonAction_001 start";
    auto ret = aacs_->PerformCommonAction(0);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_PerformCommonAction_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_SetOnKeyPressEventResult_001
 * @tc.name: SetOnKeyPressEventResult
 * @tc.desc: Test function SetOnKeyPressEventResult
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_SetOnKeyPressEventResult_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SetOnKeyPressEventResult_001 start";
    sptr<KeyEventFilter> keyEventFilter = new KeyEventFilter();
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->SetKeyEventFilter(keyEventFilter);
    aacs_->SetOnKeyPressEventResult(true,0);
    EXPECT_TRUE(testKeyevent);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SetOnKeyPressEventResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeScale_001
 * @tc.name: GetDisplayResizeScale
 * @tc.desc: Test function GetDisplayResizeScale
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_GetDisplayResizeScale_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeScale_001 start";
    auto ret = aacs_->GetDisplayResizeScale(0);
    EXPECT_EQ(ret, 0.0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeScale_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterX_001
 * @tc.name: GetDisplayResizeCenterX
 * @tc.desc: Test function GetDisplayResizeCenterX
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_GetDisplayResizeCenterX_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterX_001 start";
    auto ret = aacs_->GetDisplayResizeCenterX(0);
    EXPECT_EQ(ret, 0.0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterX_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterY_001
 * @tc.name: GetDisplayResizeCenterY
 * @tc.desc: Test function GetDisplayResizeCenterY
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_GetDisplayResizeCenterY_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterY_001 start";
    auto ret = aacs_->GetDisplayResizeCenterY(0);
    EXPECT_EQ(ret, 0.0);
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeCenterY_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeRect_001
 * @tc.name: GetDisplayResizeRect
 * @tc.desc: Test function GetDisplayResizeRect
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_GetDisplayResizeRect_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeRect_001 start";
    aacs_->GetDisplayResizeRect(0);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_GetDisplayResizeRect_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_ResetDisplayResize_001
 * @tc.name: ResetDisplayResize
 * @tc.desc: Test function ResetDisplayResize
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_ResetDisplayResize_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_ResetDisplayResize_001 start";
    auto ret = aacs_->ResetDisplayResize(0, true);
    EXPECT_FALSE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_ResetDisplayResize_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001
 * @tc.name: SetDisplayResizeScaleAndCenter
 * @tc.desc: Test function SetDisplayResizeScaleAndCenter
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_SetDisplayResizeScaleAndCenter_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001 start";
    auto ret = aacs_->SetDisplayResizeScaleAndCenter(0, 0, 0, 0, true);
    EXPECT_FALSE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_SetDisplayResizeScaleAndCenter_001
 * @tc.name: SetDisplayResizeScaleAndCenter
 * @tc.desc: Test function SetDisplayResizeScaleAndCenter
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_SendSimulateGesture_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SendSimulateGesture_001 start";
    std::vector<GesturePathDefine> gestureSteps;
    aacs_->SendSimulateGesture(0, gestureSteps);
    EXPECT_EQ(testGestureSimulateResult,1);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_SendSimulateGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityChannelStubImplUnitTest_Unittest_IsFingerprintGestureDetectionValid_001
 * @tc.name: IsFingerprintGestureDetectionValid
 * @tc.desc: Test function IsFingerprintGestureDetectionValid
 */
HWTEST_F(AccessibleAbilityChannelStubImplUnitTest,
 AccessibleAbilityChannelStubImpl_IsFingerprintGestureDetectionValid_Reset_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_IsFingerprintGestureDetectionValid_001 start";
    auto ret = aacs_->IsFingerprintGestureDetectionValid();
    EXPECT_FALSE(ret);

    GTEST_LOG_(INFO) << "AccessibleAbilityChannelStubImplUnitTest_Unittest_IsFingerprintGestureDetectionValid_001 end";
}