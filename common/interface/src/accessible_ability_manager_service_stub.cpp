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
#include "accesstoken_kit.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace Accessibility {
using namespace Security::AccessToken;

AccessibleAbilityManagerServiceStub::AccessibleAbilityManagerServiceStub()
{
    HILOG_DEBUG();

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
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityManagerService::Message::GET_ALL_CONFIGS)] =
        &AccessibleAbilityManagerServiceStub::HandleGetAllConfigs;
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

bool AccessibleAbilityManagerServiceStub::CheckPermission(const std::string &permission)
{
    HILOG_DEBUG();

    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_INVALID) {
        HILOG_WARN("AccessToken type:%{private}d, permission:%{private}d denied!", tokenType, callerToken);
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(callerToken, permission);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID:%{private}u, permission:%{private}s denied!", callerToken, permission.c_str());
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, callerToken, permission.c_str());
    return true;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSendEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

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
    HILOG_DEBUG();

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerStateObserver> client = iface_cast<IAccessibleAbilityManagerStateObserver>(obj);
    uint64_t result = RegisterStateObserver(client);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAbilityList(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    uint32_t abilityTypes = data.ReadUint32();
    int32_t stateType = data.ReadInt32();
    std::vector<AccessibilityAbilityInfo> abilityInfos {};
    RetError result = GetAbilityList(abilityTypes, stateType, abilityInfos);

    int32_t abilityInfoSize = static_cast<int32_t>(abilityInfos.size());
    reply.WriteInt32(abilityInfoSize);
    for (auto &abilityInfo : abilityInfos) {
        sptr<AccessibilityAbilityInfoParcel> info = new(std::nothrow) AccessibilityAbilityInfoParcel(abilityInfo);
        if (!info) {
            HILOG_ERROR("Failed to create info.");
            return ERR_NULL_OBJECT;
        }
        if (!reply.WriteStrongParcelable(info)) {
            HILOG_ERROR("WriteStrongParcelable<AccessibilityAbilityInfoParcel> failed");
            return TRANSACTION_ERR;
        }
    }
    reply.WriteInt32(static_cast<int32_t>(result));
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t windowId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperator> operation = iface_cast<IAccessibilityElementOperator>(obj);
    RegisterElementOperator(windowId, operation);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDeregisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t windowId = data.ReadInt32();
    DeregisterElementOperator(windowId);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetCaptionProperty(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    AccessibilityConfig::CaptionProperty caption = {};
    RetError ret = GetCaptionProperty(caption);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        CaptionPropertyParcel captionParcel(caption);
        reply.WriteParcelable(&captionParcel);
    }

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetCaptionProperty(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    sptr<CaptionPropertyParcel> caption = data.ReadStrongParcelable<CaptionPropertyParcel>();
    if (!caption) {
        HILOG_ERROR("ReadStrongParcelable<CaptionProperty> failed");
        reply.WriteInt32(RET_ERR_IPC_FAILED);
        return TRANSACTION_ERR;
    }
    reply.WriteInt32(SetCaptionProperty(*caption));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetCaptionState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();
    reply.WriteInt32(SetCaptionState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterCaptionPropertyCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerCaptionObserver> observer =
        iface_cast<IAccessibleAbilityManagerCaptionObserver>(remote);
    uint64_t result = RegisterCaptionObserver(observer);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetEnabled(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    bool result = GetEnabledState();
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetCaptionState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetCaptionState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetTouchGuideState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    bool result = GetTouchGuideState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetGestureState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    bool result = GetGestureState();
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetKeyEventObserverState(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    bool result = GetKeyEventObserverState();
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableAbility(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    std::string name = data.ReadString();
    uint32_t capabilities = data.ReadUint32();
    RetError result = EnableAbility(name, capabilities);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetEnabledAbilities(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    std::vector<std::string> enabledAbilities;
    RetError result = GetEnabledAbilities(enabledAbilities);
    reply.WriteInt32(enabledAbilities.size());
    for (auto &ability : enabledAbilities) {
        if (!reply.WriteString(ability)) {
            HILOG_ERROR("ability write error: %{public}s, ", ability.c_str());
            return TRANSACTION_ERR;
        }
    }
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDisableAbility(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    std::string name = data.ReadString();
    RetError result = DisableAbility(name);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleEnableUITestAbility(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();

    int32_t activeWindow = GetActiveWindow();
    reply.WriteInt32(activeWindow);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDisableUITestAbility(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t result = DisableUITestAbility();
    if (!reply.WriteInt32(result)) {
        HILOG_ERROR("WriteBool failed");
        return TRANSACTION_ERR;
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetScreenMagnificationState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetScreenMagnificationState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetShortKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetShortKeyState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetMouseKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetMouseKeyState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetShortkeyTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    std::string name = data.ReadString();

    reply.WriteInt32(SetShortkeyTarget(name));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    int32_t time = data.ReadInt32();

    reply.WriteInt32(SetMouseAutoClick(time));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetInvertColorState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetInvertColorState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetHighContrastTextState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetHighContrastTextState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetAudioMonoState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetAudioMonoState(state));

    return NO_ERROR;
}


ErrCode AccessibleAbilityManagerServiceStub::HandleSetAnimationOffState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetAnimationOffState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetDaltonizationColorFilter(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    uint32_t filter = data.ReadUint32();

    reply.WriteInt32(SetDaltonizationColorFilter(filter));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetContentTimeout(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    uint32_t time = data.ReadUint32();

    reply.WriteInt32(SetContentTimeout(time));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetBrightnessDiscount(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    float discount = data.ReadFloat();

    reply.WriteInt32(SetBrightnessDiscount(discount));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetAudioBalance(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("Permission denied!");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    float balance = data.ReadFloat();

    reply.WriteInt32(SetAudioBalance(balance));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetScreenMagnificationState(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetScreenMagnificationState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetShortKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetShortKeyState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetMouseKeyState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetMouseKeyState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetShortkeyTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::string result = "";
    RetError ret = GetShortkeyTarget(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteString(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t result = 0;
    RetError ret = GetMouseAutoClick(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteInt32(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetInvertColorState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetInvertColorState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetHighContrastTextState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetHighContrastTextState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAudioMonoState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetAudioMonoState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAnimationOffState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = false;
    RetError ret = GetAnimationOffState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetDaltonizationColorFilter(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    uint32_t result = 0;
    RetError ret = GetDaltonizationColorFilter(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteUint32(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetContentTimeout(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    uint32_t result = 0;
    RetError ret = GetContentTimeout(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteUint32(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetBrightnessDiscount(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    float result = 0;
    RetError ret = GetBrightnessDiscount(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteFloat(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAudioBalance(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    float result = 0;
    RetError ret = GetAudioBalance(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteFloat(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAllConfigs(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    AccessibilityConfigData configData;
    GetAllConfigs(configData);
    CaptionPropertyParcel captionParcel(configData.captionProperty_);
    reply.WriteBool(configData.highContrastText_);
    reply.WriteBool(configData.invertColor_);
    reply.WriteBool(configData.animationOff_);
    reply.WriteBool(configData.audioMono_);
    reply.WriteBool(configData.mouseKey_);
    reply.WriteBool(configData.captionState_);
    reply.WriteBool(configData.screenMagnifier_);
    reply.WriteBool(configData.shortkey_);
    reply.WriteInt32(configData.mouseAutoClick_);
    reply.WriteUint32(configData.daltonizationColorFilter_);
    reply.WriteUint32(configData.contentTimeout_);
    reply.WriteFloat(configData.brightnessDiscount_);
    reply.WriteFloat(configData.audioBalance_);
    reply.WriteString(configData.shortkeyTarget_);
    reply.WriteParcelable(&captionParcel);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterEnableAbilityListsObserver(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityEnableAbilityListsObserver> observer =
        iface_cast<IAccessibilityEnableAbilityListsObserver>(obj);
    RegisterEnableAbilityListsObserver(observer);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterConfigCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibleAbilityManagerConfigObserver> config = iface_cast<IAccessibleAbilityManagerConfigObserver>(obj);
    uint64_t result = RegisterConfigObserver(config);
    reply.WriteUint64(result);

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS