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

#include "accessible_ability_manager_service_stub.h"
#include "accessibility_ability_info_parcel.h"
#include "accessibility_caption_parcel.h"
#include "accessibility_event_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceStub::AccessibleAbilityManagerServiceStub()
{
    HILOG_DEBUG("start");

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SEND_EVENT)] =
        &AccessibleAbilityManagerServiceStub::HandleSendEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::REGISTER_STATE_CALLBACK)] =
        &AccessibleAbilityManagerServiceStub::HandleRegisterStateCallback;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ABILITYLIST)] =
        &AccessibleAbilityManagerServiceStub::HandleGetAbilityList;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::REGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceStub::HandleRegisterAccessibilityElementOperator;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::DEREGISTER_INTERACTION_CONNECTION)] =
        &AccessibleAbilityManagerServiceStub::HandleDeregisterAccessibilityElementOperator;

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_CAPTION_PROPERTY)] =
        &AccessibleAbilityManagerServiceStub::HandleGetCaptionProperty;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_CAPTION_PROPERTY)] =
        &AccessibleAbilityManagerServiceStub::HandleSetCaptionProperty;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_CAPTION_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetCaptionState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ENABLED)] =
        &AccessibleAbilityManagerServiceStub::HandleGetEnabled;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_CAPTION_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetCaptionState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_TOUCH_GUIDE_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetTouchGuideState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_GESTURE_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetGestureState;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::GET_KEY_EVENT_OBSERVE_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetKeyEventObserverState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::ENABLE_ABILITIES)] =
        &AccessibleAbilityManagerServiceStub::HandleEnableAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ENABLED_OBJECT)] =
        &AccessibleAbilityManagerServiceStub::HandleGetEnabledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_INSTALLED)] =
        &AccessibleAbilityManagerServiceStub::HandleGetInstalledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::DISABLE_ABILITIES)] =
        &AccessibleAbilityManagerServiceStub::HandleDisableAbilities;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::REGISTER_CAPTION_PROPERTY_CALLBACK)] =
        &AccessibleAbilityManagerServiceStub::HandleRegisterCaptionPropertyCallback;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::ENABLE_UI_TEST_ABILITY)] =
        &AccessibleAbilityManagerServiceStub::HandleEnableUITestAbility;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::DISABLE_UI_TEST_ABILITY)] =
        &AccessibleAbilityManagerServiceStub::HandleDisableUITestAbility;
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::GET_ACTIVE_WINDOW)] =
        &AccessibleAbilityManagerServiceStub::HandleGetActiveWindow;
}

AccessibleAbilityManagerServiceStub::~AccessibleAbilityManagerServiceStub()
{
    HILOG_DEBUG("start.");
    memberFuncMap_.clear();
}

int AccessibleAbilityManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, cmd = %{public}u, flags= %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }
    HILOG_WARN("AccessibleAbilityManagerServiceStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSendEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<AccessibilityEventInfoParcel> uiEvent = data.ReadStrongParcelable<AccessibilityEventInfoParcel>();
    if (!uiEvent) {
        HILOG_DEBUG("ReadStrongParcelable<AbilityInfo> failed");
        return TRANSACTION_ERR;
    }
    int32_t userId = data.ReadInt32();
    SendEvent(*uiEvent, userId);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterStateCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerStateObserver> client = iface_cast<IAccessibleAbilityManagerStateObserver>(obj);
    int32_t userId = data.ReadInt32();
    uint64_t result = RegisterStateObserver(client, userId);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAbilityList(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    uint32_t abilityTypes = data.ReadUint32();
    int32_t stateType = data.ReadInt32();
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    abilityInfos = GetAbilityList(abilityTypes, stateType);

    int32_t abilityInfoSize = (int32_t)abilityInfos.size();
    reply.WriteInt32(abilityInfoSize);
    for (auto& abilityInfo : abilityInfos) {
        sptr<AccessibilityAbilityInfoParcel> info = new AccessibilityAbilityInfoParcel(abilityInfo);
        if (!reply.WriteStrongParcelable(info)) {
            HILOG_ERROR("WriteStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return TRANSACTION_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t windowId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperator> operation = iface_cast<IAccessibilityElementOperator>(obj);
    int32_t userId = data.ReadInt32();
    RegisterElementOperator(windowId, operation, userId);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDeregisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t windowId = data.ReadInt32();
    DeregisterElementOperator(windowId);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetCaptionProperty(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    CaptionProperty caption = GetCaptionProperty();
    CaptionPropertyParcel captionParcel(caption);

    reply.WriteParcelable(&captionParcel);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetCaptionProperty(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<CaptionPropertyParcel> caption = data.ReadStrongParcelable<CaptionPropertyParcel>();
    if (!caption) {
        HILOG_DEBUG("ReadStrongParcelable<CaptionProperty> failed");
        return TRANSACTION_ERR;
    }
    SetCaptionProperty(*caption);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetCaptionState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetCaptionState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterCaptionPropertyCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerCaptionObserver> observer =
        iface_cast<IAccessibleAbilityManagerCaptionObserver>(remote);
    int32_t userId = data.ReadInt32();
    uint64_t result = RegisterCaptionObserver(observer, userId);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetEnabled(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetEnabledState();
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetCaptionState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetCaptionState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetTouchGuideState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetTouchGuideState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetGestureState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetGestureState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetKeyEventObserverState(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetKeyEventObserverState();
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableAbilities(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    std::vector<std::string> abilities {};
    int32_t dev_num = data.ReadInt32();
    for (int32_t i = 0; i < dev_num; i++) {
        abilities.push_back(data.ReadString());
    }
    EnableAbilities(abilities);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetEnabledAbilities(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<std::string> abilities = GetEnabledAbilities();
    reply.WriteInt32(abilities.size());
    for (auto &ability : abilities) {
        if (!reply.WriteString(ability)) {
            HILOG_ERROR("ability write error: %{public}s, ", ability.c_str());
            return TRANSACTION_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetInstalledAbilities(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityAbilityInfo> it = GetInstalledAbilities();
    int32_t num = (int32_t)it.size();

    reply.WriteInt32(num);
    for (int32_t i = 0; i < num; i++) {
        sptr<AccessibilityAbilityInfoParcel> info = new AccessibilityAbilityInfoParcel(it[i]);
        bool ret = reply.WriteStrongParcelable(info);
        if (!ret) {
            HILOG_ERROR("WriteStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return TRANSACTION_ERR;
        }
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDisableAbilities(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    std::vector<std::string> abilities {};
    int32_t dev_num = data.ReadInt32();
    for (int32_t i = 0; i < dev_num; i++) {
        abilities.push_back(data.ReadString());
    }
    DisableAbilities(abilities);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableUITestAbility(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    bool result = EnableUITestAbility(obj);
    if (!reply.WriteBool(result)) {
        HILOG_ERROR("WriteBool failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetActiveWindow(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t activeWindow = GetActiveWindow();
    reply.WriteInt32(activeWindow);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDisableUITestAbility(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    bool result = DisableUITestAbility();
    if (!reply.WriteBool(result)) {
        HILOG_ERROR("WriteBool failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS