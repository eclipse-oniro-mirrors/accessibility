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
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

#define SWITCH_BEGIN(code) switch (code) {
#define SWITCH_CASE(case_code, func) case case_code:\
    {\
        result_code = func(data, reply);\
        break;\
    }

#define SWITCH_END() default:\
    {\
        result_code = ERR_CODE_DEFAULT;\
        HILOG_WARN("AccessibleAbilityClientStub::OnRemoteRequest, default case, need check.");\
        break;\
    }\
}

#define ACCESSIBLE_ABILITY_CLIENT_STUB_CASES() \
    SWITCH_CASE(AccessibilityInterfaceCode::INIT, HandleInit)\
    SWITCH_CASE(AccessibilityInterfaceCode::DISCONNECT, HandleDisconnect)\
    SWITCH_CASE(AccessibilityInterfaceCode::ON_ACCESSIBILITY_EVENT, HandleOnAccessibilityEvent)\
    SWITCH_CASE(AccessibilityInterfaceCode::ON_KEY_PRESS_EVENT, HandleOnKeyPressEvent)\

namespace OHOS {
namespace Accessibility {
constexpr int32_t ERR_CODE_DEFAULT = -1000;

AccessibleAbilityClientStub::AccessibleAbilityClientStub()
{
    HILOG_DEBUG();
    memberFuncMap_[static_cast<uint32_t>(AccessibilityInterfaceCode::INIT)] =
        &AccessibleAbilityClientStub::HandleInit;
    memberFuncMap_[static_cast<uint32_t>(AccessibilityInterfaceCode::DISCONNECT)] =
        &AccessibleAbilityClientStub::HandleDisconnect;
    memberFuncMap_[static_cast<uint32_t>(AccessibilityInterfaceCode::ON_ACCESSIBILITY_EVENT)] =
        &AccessibleAbilityClientStub::HandleOnAccessibilityEvent;
    memberFuncMap_[static_cast<uint32_t>(AccessibilityInterfaceCode::ON_KEY_PRESS_EVENT)] =
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
    std::u16string remote = data.ReadInterfaceToken();
    if (descriptor != remote) {
        HILOG_ERROR("local descriptor is not equal to remote descriptor");
        return ERR_INVALID_STATE;
    }

    ErrCode result_code = ERR_NONE;
    SWITCH_BEGIN(code)
    ACCESSIBLE_ABILITY_CLIENT_STUB_CASES()
    SWITCH_END()

    if (result_code != ERR_CODE_DEFAULT) {
        return result_code;
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityClientStub::HandleInit(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("object is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibleAbilityChannel> channel = iface_cast<IAccessibleAbilityChannel>(remote);
    if (channel == nullptr) {
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
    if (keyEvent == nullptr) {
        HILOG_ERROR("keyEvent is nullptr");
        return ERR_INVALID_VALUE;
    }
    
    if (!keyEvent->ReadFromParcel(data)) {
        HILOG_ERROR("keyEvent ReadFromParcel failed");
        return ERR_INVALID_VALUE;
    }
    OnKeyPressEvent(*keyEvent, sequence);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS