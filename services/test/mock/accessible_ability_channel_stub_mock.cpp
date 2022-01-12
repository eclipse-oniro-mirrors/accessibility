/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessible_ability_channel_stub.h"
#include "parcel.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_account_data.h"
#include "accessible_ability_connection.h"



using namespace std;

namespace OHOS {
namespace Accessibility {

AccessibleAbilityChannelStub::AccessibleAbilityChannelStub() {}


AccessibleAbilityChannelStub::~AccessibleAbilityChannelStub() {}

int32_t AccessibleAbilityChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                    MessageOption &option) {

    return 0;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data, 
            MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfosByText(MessageParcel &data, 
                                                                              MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFocusMoveSearch(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandlePerformAction(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindows(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandlePerformCommonAction(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleDisableAbility(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult(MessageParcel &data, MessageParcel &reply) {

    HILOG_DEBUG("%{public}s mock AccessibleAbilityChannelStub mock ------------------ start.", __func__);

    // GTEST_LOG_(INFO) << "SetUp";
    // //new Interaction proxy
    // sptr<AccessibilityInteractionOperationStub> stub = new AccessibilityInteractionOperationStub();
    // sptr<IAccessibilityInteractionOperation> proxy =
    //     new AccessibilityInteractionOperationProxy(stub);
    // sptr<AccessibilityInteractionConnection> connection = new AccessibilityInteractionConnection(0, proxy, 
    //     "test", 0, 0);

    // //new AAconnection
    // sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo();
    // accountData_ = new AccessibilityAccountData(0);
    // accountData_->AddAccessibilityInteractionConnection(0,connection);
    // sptr<AccessibleAbilityConnection> AAConnection = new AccessibleAbilityConnection(accountData_, 0, *abilityInfo);
    // sptr<AppExecFwk::ElementName> elementName = new AppExecFwk::ElementName("1","2","3");
    // //new aastub
    // std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("test");
    // aastub_ = new AccessibleAbilityClientStubImpl(std::make_shared<AccessibleAbilityEventHandler>(runner));
    // AAConnection->OnAbilityConnectDone(*elementName, aastub_, 0);
    // //aac
    // aacs_ = std::make_shared<AccessibleAbilityChannelStubImpl>(AAConnection);



    // HILOG_INFO("AccessibleAbilityManagerService::RegisterAbilityConnectionClientTmp start");
    // currentAccountId_ = 0;
    // AppExecFwk::AbilityInfo info;
    // AAFwk::Want want;
    // AppExecFwk::ElementName name;
    // name.SetAbilityName("com.example.aalisttest.MainAbility");
    // name.SetBundleName("com.example.aalisttest");
    // want.SetElement(name);

    // HILOG_INFO("AccessibleAbilityManagerService::---------------------------- start");
    // GetBundleMgrProxy()->QueryAbilityInfo(want, info);
    // HILOG_INFO("AccessibleAbilityManagerService::---------------------------- end");


    // sptr<AccessibilityAbilityInfo> abilityInfo = new AccessibilityAbilityInfo(info);
    // AppExecFwk::ElementName elementName(info.deviceId, info.bundleName, info.name);
    // sptr<AccessibilityAccountData> accountData = GetCurrentAccountData();

    // a11yAccountsData_.insert(make_pair(0, accountData));
    // sptr<AccessibleAbilityConnection> connection = new AccessibleAbilityConnection(accountData, connectCounter_++, 
    //     *abilityInfo);
    // connection->OnAbilityConnectDone(elementName, obj, 0);
    // accountData->AddInstalledAbility(*abilityInfo);

    //bool handled = data.ReadBool();
    //int sequence = data.ReadInt32();
    // bool handled = true;
    // int sequence = 0;
    // //SetOnKeyPressEventResult(handled, sequence);

    // std::shared_ptr<AccessibleAbilityManagerService> aams_ = 
    // DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();

    // sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    // std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();

    // auto iter = connectionMaps.begin();
    // sptr<AccessibleAbilityConnection> ptr_connect = iter->second;

    // sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    // aacs_->SetOnKeyPressEventResult(handled, sequence);

    HILOG_DEBUG("%{public}s mock AccessibleAbilityChannelStub -------mock -----mock------------- end.", __func__);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeScale(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterX(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterY(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeRect(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleResetDisplayResize(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetDisplayResizeScaleAndCenter(MessageParcel &data, 
                                                                                 MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSendSimulateGesture(MessageParcel &data, MessageParcel &reply) {
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleIsFingerprintGestureDetectionValid(MessageParcel &data, 
                                                                                     MessageParcel &reply) {
    return NO_ERROR;
}


} // namespace Accessibility
} // namespace OHOS