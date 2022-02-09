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
#include <stdint.h>
#include <gtest/gtest.h>
#include "accessible_ability_client_proxy.h"
#include "accessibility_errorcode.h"
#include "parcel.h"
#include "accessible_ability_channel_stub.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_account_data.h"
#include "accessible_ability_connection.h"
#include "accessibility_event_info.h"

int testChannalId = -1;
int testEventType = -1;
int testInterrupt = -1;
int testGesture = -1;
int testKeyPressEvent = -1;
int testDisplayId = -1;
int testGestureSimulateResult = -1;
int testFingerprintGestureValidityChanged = -1;
int testFingerprintGesture = -1;
OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> testStub = nullptr;

namespace OHOS {
namespace Accessibility {
EventType MTeventType;
std::vector<EventType> mTeventType;
int MTgestureId = -1;

AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object): IRemoteProxy<IAccessibleAbilityClient>(object)
{}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId) {
    testStub = channel;
    testChannalId = channelId;
}

void AccessibleAbilityClientProxy::Disconnect(const int channelId) {}

void AccessibleAbilityClientProxy::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) {
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent OnAccessibilityEvent";
    testEventType = int(eventInfo.GetEventType());
    MTeventType = eventInfo.GetEventType();
    mTeventType.push_back(MTeventType);
}

void AccessibleAbilityClientProxy::OnInterrupt() {
    testInterrupt = 1;
}

void AccessibleAbilityClientProxy::OnGesture(const int gestureId) {
    testGesture = gestureId;
    MTgestureId = gestureId;
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) {

    MessageParcel data;
    MessageParcel reply;
    HILOG_DEBUG();
    HILOG_DEBUG("%{public}s start.-----------------------------zjx mock Proxy Start ", __func__);
    HILOG_DEBUG("%{public}s start.----------sequence--%{public}d ----------zjx mock Proxy Start ", __func__, sequence);
    testKeyPressEvent = sequence;

    // bool handled = true;
    // //int sequence = 0;
    // //SetOnKeyPressEventResult(handled, sequence);

    // std::shared_ptr<AccessibleAbilityManagerService> aams_ =
    // DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();

    // sptr<AccessibilityAccountData> accountData = aams_->GetCurrentAccountData();
    // std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();

    // auto iter = connectionMaps.begin();
    // sptr<AccessibleAbilityConnection> ptr_connect = iter->second;

    // sptr<AccessibleAbilityChannelStubImpl> aacs_ = new AccessibleAbilityChannelStubImpl(*ptr_connect);

    // aacs_->SetOnKeyPressEventResult(handled, sequence);

    HILOG_DEBUG("%{public}s mock AccessibleAbilityClientProxy -------mock -----mock------------- end.", __func__);


    // sptr<AccessibleAbilityChannelStub> pChannelStub = new AccessibleAbilityChannelStub();
    // pChannelStub->HandleSetOnKeyPressEventResult(data, reply);


    HILOG_DEBUG("%{public}s start.-----------------------------zjx mock Proxy end ", __func__);
}

void AccessibleAbilityClientProxy::OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale,
                                                            const float centerX, const float centerY) {
    testDisplayId = 1;
}

void AccessibleAbilityClientProxy::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) {
    testGestureSimulateResult = 1;
}

void AccessibleAbilityClientProxy::OnFingerprintGestureValidityChanged(const bool validity) {
    testFingerprintGestureValidityChanged = 1;
}

void AccessibleAbilityClientProxy::OnFingerprintGesture(const int gesture) {
    testFingerprintGesture = 1;
}

} // namespace Accessibility
} // namespace OHOS