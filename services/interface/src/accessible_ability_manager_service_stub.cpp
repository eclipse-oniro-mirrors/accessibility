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
        &AccessibleAbilityManagerServiceStub::HandleEnableAbility;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ENABLED_OBJECT)] =
        &AccessibleAbilityManagerServiceStub::HandleGetEnabledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_INSTALLED)] =
        &AccessibleAbilityManagerServiceStub::HandleGetInstalledAbilities;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::DISABLE_ABILITIES)] =
        &AccessibleAbilityManagerServiceStub::HandleDisableAbility;
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
    memberFuncMap_[static_cast<uint32_t>(
        IAccessibleAbilityManagerService::Message::REGISTER_ENABLE_ABILITY_LISTS_OBSERVER)] =
        &AccessibleAbilityManagerServiceStub::HandleRegisterEnableAbilityListsObserver;

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_SCREENMAGNIFIER_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetScreenMagnificationState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_SHORTKEY_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetShortKeyState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_MOUSEKEY_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetMouseKeyState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_SHORTKEY_TARGET)] =
        &AccessibleAbilityManagerServiceStub::HandleSetShortkeyTarget;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_MOUSEKEY_AUTOCLICK)] =
        &AccessibleAbilityManagerServiceStub::HandleSetMouseAutoClick;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_INVERTCOLOR_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetInvertColorState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_HIGHCONTRASTTEXT_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetHighContrastTextState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_AUDIOMONO_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetAudioMonoState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_ANIMATIONOFF_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetAnimationOffState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_DALTONIZATION_COLORFILTER)] =
        &AccessibleAbilityManagerServiceStub::HandleSetDaltonizationColorFilter;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_CONTENT_TIMEOUT)] =
        &AccessibleAbilityManagerServiceStub::HandleSetContentTimeout;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_BRIGHTNESS_DISCOUNT)] =
        &AccessibleAbilityManagerServiceStub::HandleSetBrightnessDiscount;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::SET_AUDIO_BALANCE)] =
        &AccessibleAbilityManagerServiceStub::HandleSetAudioBalance;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_SCREENMAGNIFIER_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetScreenMagnificationState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_SHORTKEY_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetShortKeyState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_MOUSEKEY_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetMouseKeyState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_SHORTKEY_TARGET)] =
        &AccessibleAbilityManagerServiceStub::HandleGetShortkeyTarget;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_MOUSEKEY_AUTOCLICK)] =
        &AccessibleAbilityManagerServiceStub::HandleGetMouseAutoClick;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_INVERTCOLOR_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetInvertColorState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_HIGHCONTRASTTEXT_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetHighContrastTextState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_AUDIOMONO_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetAudioMonoState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ANIMATIONOFF_STATE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetAnimationOffState;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_DALTONIZATION_COLORFILTER)] =
        &AccessibleAbilityManagerServiceStub::HandleGetDaltonizationColorFilter;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_CONTENT_TIMEOUT)] =
        &AccessibleAbilityManagerServiceStub::HandleGetContentTimeout;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_BRIGHTNESS_DISCOUNT)] =
        &AccessibleAbilityManagerServiceStub::HandleGetBrightnessDiscount;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_AUDIO_BALANCE)] =
        &AccessibleAbilityManagerServiceStub::HandleGetAudioBalance;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::REGISTER_CONFIG_CALLBACK)] =
        &AccessibleAbilityManagerServiceStub::HandleRegisterConfigCallback;
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
    SendEvent(*uiEvent);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterStateCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerStateObserver> client = iface_cast<IAccessibleAbilityManagerStateObserver>(obj);
    uint64_t result = RegisterStateObserver(client);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAbilityList(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    uint32_t abilityTypes = data.ReadUint32();
    int32_t stateType = data.ReadInt32();
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    bool result = GetAbilityList(abilityTypes, stateType, abilityInfos);

    int32_t abilityInfoSize = static_cast<int32_t>(abilityInfos.size());
    reply.WriteInt32(abilityInfoSize);
    for (auto& abilityInfo : abilityInfos) {
        sptr<AccessibilityAbilityInfoParcel> info = new AccessibilityAbilityInfoParcel(abilityInfo);
        if (!reply.WriteStrongParcelable(info)) {
            HILOG_ERROR("WriteStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return TRANSACTION_ERR;
        }
    }
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t windowId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperator> operation = iface_cast<IAccessibilityElementOperator>(obj);
    RegisterElementOperator(windowId, operation);

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

    AccessibilityConfig::CaptionProperty caption = GetCaptionProperty();
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
    uint64_t result = RegisterCaptionObserver(observer);
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

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableAbility(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    std::string name = data.ReadString();
    uint32_t capabilities = data.ReadUint32();
    bool result = EnableAbility(name, capabilities);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetEnabledAbilities(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<std::string> enabledAbilities;
    bool result = GetEnabledAbilities(enabledAbilities);
    reply.WriteInt32(enabledAbilities.size());
    for (auto &ability : enabledAbilities) {
        if (!reply.WriteString(ability)) {
            HILOG_ERROR("ability write error: %{public}s, ", ability.c_str());
            return TRANSACTION_ERR;
        }
    }
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetInstalledAbilities(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityAbilityInfo> installedAbilities;
    bool result = GetInstalledAbilities(installedAbilities);
    int32_t num = static_cast<int32_t>(installedAbilities.size());

    reply.WriteInt32(num);
    for (int32_t i = 0; i < num; i++) {
        sptr<AccessibilityAbilityInfoParcel> info = new AccessibilityAbilityInfoParcel(installedAbilities[i]);
        bool result = reply.WriteStrongParcelable(info);
        if (!result) {
            HILOG_ERROR("WriteStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return TRANSACTION_ERR;
        }
    }
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDisableAbility(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    std::string name = data.ReadString();
    bool result = DisableAbility(name);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableUITestAbility(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    int32_t result = EnableUITestAbility(obj);
    if (!reply.WriteInt32(result)) {
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

ErrCode AccessibleAbilityManagerServiceStub::HandleSetScreenMagnificationState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetScreenMagnificationState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetShortKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetShortKeyState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetMouseKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetMouseKeyState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetShortkeyTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::string name = data.ReadString();

    SetShortkeyTarget(name);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t time = data.ReadInt32();

    SetMouseAutoClick(time);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetInvertColorState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetInvertColorState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetHighContrastTextState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetHighContrastTextState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetAudioMonoState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetAudioMonoState(state);

    return NO_ERROR;
}


ErrCode AccessibleAbilityManagerServiceStub::HandleSetAnimationOffState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool state = data.ReadBool();

    SetAnimationOffState(state);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetDaltonizationColorFilter(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    uint32_t filter = data.ReadUint32();

    SetDaltonizationColorFilter(filter);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetContentTimeout(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    uint32_t time = data.ReadUint32();

    SetContentTimeout(time);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetBrightnessDiscount(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    float discount = data.ReadFloat();

    SetBrightnessDiscount(discount);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetAudioBalance(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    float balance = data.ReadFloat();

    SetAudioBalance(balance);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetScreenMagnificationState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    bool result = GetScreenMagnificationState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetShortKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetShortKeyState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetMouseKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetMouseKeyState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetShortkeyTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    std::string result = GetShortkeyTarget();
    reply.WriteString(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    int32_t result = GetMouseAutoClick();
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetInvertColorState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetInvertColorState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetHighContrastTextState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetHighContrastTextState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAudioMonoState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetAudioMonoState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAnimationOffState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    bool result = GetAnimationOffState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetDaltonizationColorFilter(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG("start");

    uint32_t result = GetDaltonizationColorFilter();
    reply.WriteUint32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetContentTimeout(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    uint32_t result = GetContentTimeout();
    reply.WriteUint32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetBrightnessDiscount(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    float result = GetBrightnessDiscount();
    reply.WriteFloat(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAudioBalance(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    float result = GetAudioBalance();
    reply.WriteFloat(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterEnableAbilityListsObserver(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        iface_cast<IAccessibilityEnableAbilityListsObserver>(obj);
    RegisterEnableAbilityListsObserver(observer);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterConfigCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerConfigObserver> config = iface_cast<IAccessibleAbilityManagerConfigObserver>(obj);
    uint64_t result = RegisterConfigObserver(config);
    reply.WriteUint64(result);

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS