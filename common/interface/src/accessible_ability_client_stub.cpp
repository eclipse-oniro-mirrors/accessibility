/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "accessible_ability_client_stub.h"
#include "accessibility_element_info_parcel.h"
#include "accessibility_event_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityClientStub::AccessibleAbilityClientStub()
{
    HILOG_DEBUG();
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::INIT)] =
        &AccessibleAbilityClientStub::HandleInit;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::DISCONNECT)] =
        &AccessibleAbilityClientStub::HandleDisconnect;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT)] =
        &AccessibleAbilityClientStub::HandleOnAccessibilityEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT)] =
        &AccessibleAbilityClientStub::HandleOnKeyPressEvent;
}

AccessibleAbilityClientStub::~AccessibleAbilityClientStub()
{
    HILOG_DEBUG();
    memberFuncMap_.clear();
}

int AccessibleAbilityClientStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityClientStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityClientStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOG_WARN("AccessibleAbilityClientStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityClientStub::HandleInit(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (!remote) {
        HILOG_ERROR("object is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibleAbilityChannel> channel = iface_cast<IAccessibleAbilityChannel>(remote);
    if (!channel) {
        HILOG_ERROR("channel is nullptr.");
        return ERR_INVALID_VALUE;
    }
    int32_t channelId = data.ReadInt32();

    Init(channel, channelId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleDisconnect(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t channelId = data.ReadInt32();
    Disconnect(channelId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnAccessibilityEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<AccessibilityEventInfoParcel> eventInfo = data.ReadStrongParcelable<AccessibilityEventInfoParcel>();
    if (!eventInfo) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityEventInfo> failed");
        return ERR_INVALID_VALUE;
    }

    OnAccessibilityEvent(*eventInfo);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnKeyPressEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t sequence = data.ReadInt32();

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    if (!keyEvent->ReadFromParcel(data)) {
        HILOG_ERROR("keyEvent ReadFromParcel failed");
        return ERR_INVALID_VALUE;
    }
    OnKeyPressEvent(*keyEvent, sequence);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS