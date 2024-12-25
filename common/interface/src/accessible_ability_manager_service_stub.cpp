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
#include "accessibility_ipc_interface_code.h"
#include "accesstoken_kit.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"

#define SWITCH_BEGIN(code) switch (code) {
#define SWITCH_CASE(case_code, func) case case_code:\
    {\
        result_code = func(data, reply);\
        break;\
    }

#define SWITCH_END() default:\
    {\
        result_code = ERR_CODE_DEFAULT;\
        HILOG_WARN("AccessibleAbilityManagerServiceStub::OnRemoteRequest, default case, need check.");\
        break;\
    }\
}

#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_STUB_CASES() \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_SCREENMAGNIFIER_STATE, HandleSetScreenMagnificationState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_SHORTKEY_STATE, HandleSetShortKeyState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_MOUSEKEY_STATE, HandleSetMouseKeyState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_SHORTKEY_TARGET, HandleSetShortkeyTarget)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_MOUSEKEY_AUTOCLICK, HandleSetMouseAutoClick)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_INVERTCOLOR_STATE, HandleSetInvertColorState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_HIGHCONTRASTTEXT_STATE, HandleSetHighContrastTextState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_AUDIOMONO_STATE, HandleSetAudioMonoState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_ANIMATIONOFF_STATE, HandleSetAnimationOffState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_DALTONIZATION_STATE, HandleSetDaltonizationState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_DALTONIZATION_COLORFILTER, HandleSetDaltonizationColorFilter)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CONTENT_TIMEOUT, HandleSetContentTimeout)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_BRIGHTNESS_DISCOUNT, HandleSetBrightnessDiscount)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_AUDIO_BALANCE, HandleSetAudioBalance)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CAPTION_PROPERTY, HandleSetCaptionProperty)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CAPTION_STATE, HandleSetCaptionState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CLICK_RESPONSE_TIME, HandleSetClickResponseTime)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_IGNORE_REPEAT_CLICK_STATE, HandleSetIgnoreRepeatClickState)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_IGNORE_REPEAT_CLICK_TIME, HandleSetIgnoreRepeatClickTime)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_CLICK_RESPONSE_TIME, HandleGetClickResponseTime)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_IGNORE_REPEAT_CLICK_STATE, HandleGetIgnoreRepeatClickState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_IGNORE_REPEAT_CLICK_TIME, HandleGetIgnoreRepeatClickTime)\
    SWITCH_CASE(AccessibilityInterfaceCode::SET_SHORTKEY_MULTI_TARGET, HandleSetShortkeyMultiTarget)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_SCREENMAGNIFIER_STATE, HandleGetScreenMagnificationState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_SHORTKEY_STATE, HandleGetShortKeyState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_MOUSEKEY_STATE, HandleGetMouseKeyState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_SHORTKEY_TARGET, HandleGetShortkeyTarget)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_MOUSEKEY_AUTOCLICK, HandleGetMouseAutoClick)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_INVERTCOLOR_STATE, HandleGetInvertColorState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_HIGHCONTRASTTEXT_STATE, HandleGetHighContrastTextState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_AUDIOMONO_STATE, HandleGetAudioMonoState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ANIMATIONOFF_STATE, HandleGetAnimationOffState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_DALTONIZATION_STATE, HandleGetDaltonizationState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_DALTONIZATION_COLORFILTER, HandleGetDaltonizationColorFilter)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_CONTENT_TIMEOUT, HandleGetContentTimeout)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_BRIGHTNESS_DISCOUNT, HandleGetBrightnessDiscount)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_AUDIO_BALANCE, HandleGetAudioBalance)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ALL_CONFIGS, HandleGetAllConfigs)\
    SWITCH_CASE(AccessibilityInterfaceCode::REGISTER_CONFIG_CALLBACK, HandleRegisterConfigCallback)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_CAPTION_PROPERTY, HandleGetCaptionProperty)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_CAPTION_STATE, HandleGetCaptionState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_WINDOW_AND_ELEMENT_ID, HandleGetWindowAndElementId)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_SCENE_BOARD_INNER_WINDOW_ID, HandleGetSceneBoardInnerWinId)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_SHORTKEY_MULTI_TARGET, HandleGetShortkeyMultiTarget)\
    SWITCH_CASE(AccessibilityInterfaceCode::SEND_EVENT, HandleSendEvent)\
    SWITCH_CASE(AccessibilityInterfaceCode::REGISTER_STATE_CALLBACK, HandleRegisterStateCallback)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ABILITYLIST, HandleGetAbilityList)\
    SWITCH_CASE(AccessibilityInterfaceCode::REGISTER_INTERACTION_CONNECTION, HandleRegisterAccessibilityElementOperator)\
    SWITCH_CASE(AccessibilityInterfaceCode::CARDREGISTER_INTERACTION_CONNECTION, \
        HandleMultiRegisterAccessibilityElementOperator)\
    SWITCH_CASE(AccessibilityInterfaceCode::DEREGISTER_INTERACTION_CONNECTION, \
        HandleDeregisterAccessibilityElementOperator)\
    SWITCH_CASE(AccessibilityInterfaceCode::CARDDEREGISTER_INTERACTION_CONNECTION, \
        HandleMultiDeregisterAccessibilityElementOperator)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ENABLED, HandleGetEnabled)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_TOUCH_GUIDE_STATE, HandleGetTouchGuideState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_GESTURE_STATE, HandleGetGestureState)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_KEY_EVENT_OBSERVE_STATE, HandleGetKeyEventObserverState)\
    SWITCH_CASE(AccessibilityInterfaceCode::ENABLE_ABILITIES, HandleEnableAbility)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ENABLED_OBJECT, HandleGetEnabledAbilities)\
    SWITCH_CASE(AccessibilityInterfaceCode::DISABLE_ABILITIES, HandleDisableAbility)\
    SWITCH_CASE(AccessibilityInterfaceCode::REGISTER_CAPTION_PROPERTY_CALLBACK, HandleRegisterCaptionPropertyCallback)\
    SWITCH_CASE(AccessibilityInterfaceCode::ENABLE_UI_TEST_ABILITY, HandleEnableUITestAbility)\
    SWITCH_CASE(AccessibilityInterfaceCode::DISABLE_UI_TEST_ABILITY, HandleDisableUITestAbility)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ACTIVE_WINDOW, HandleGetActiveWindow)\
    SWITCH_CASE( \
        AccessibilityInterfaceCode::REGISTER_ENABLE_ABILITY_LISTS_OBSERVER, HandleRegisterEnableAbilityListsObserver)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_FOCUSED_WINDOW_ID, HandleGetFocusedWindowId)\
    SWITCH_CASE(AccessibilityInterfaceCode::REMOVE_REQUEST_ID, HandleRemoveRequestId)\
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ROOT_PARENT_ID, HandleGetRootParentId) \
    SWITCH_CASE(AccessibilityInterfaceCode::GET_ALL_TREE_ID, HandleGetAllTreeId) \

namespace OHOS {
namespace Accessibility {
using namespace Security::AccessToken;
constexpr int32_t IS_EXTERNAL = 1;
constexpr int32_t ERR_CODE_DEFAULT = -1000;

AccessibleAbilityManagerServiceStub::AccessibleAbilityManagerServiceStub()
{
}

AccessibleAbilityManagerServiceStub::~AccessibleAbilityManagerServiceStub()
{
}

int AccessibleAbilityManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest, cmd = %{public}u, flags= %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_ERROR("AccessibleAbilityManagerServiceClientStub::OnRemoteRequest,"
            "local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    ErrCode result_code = ERR_NONE;
    SWITCH_BEGIN(code)
    ACCESSIBILITY_ABILITY_MANAGER_SERVICE_STUB_CASES()
    SWITCH_END()

    if (result_code != ERR_CODE_DEFAULT) {
        return result_code;
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

bool AccessibleAbilityManagerServiceStub::CheckPermission(const std::string &permission) const
{
    HILOG_DEBUG();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_INVALID) {
        HILOG_WARN("AccessToken type invalid!");
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(callerToken, permission);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID denied!");
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, callerToken, permission.c_str());
    return true;
}

bool AccessibleAbilityManagerServiceStub::IsSystemApp() const
{
    HILOG_DEBUG();

    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType != TOKEN_HAP) {
        HILOG_INFO("Caller is not a application.");
        return true;
    }
    uint64_t accessTokenId = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApplication = TokenIdKit::IsSystemAppByFullTokenID(accessTokenId);
    return isSystemApplication;
}

bool AccessibleAbilityManagerServiceStub::IsApp() const
{
    HILOG_DEBUG();

    AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == TOKEN_HAP) {
        HILOG_DEBUG("caller is an application");
        return true;
    }
    return false;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSendEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    sptr<AccessibilityEventInfoParcel> uiEvent = data.ReadStrongParcelable<AccessibilityEventInfoParcel>();
    if (uiEvent == nullptr) {
        HILOG_DEBUG("ReadStrongParcelable<AbilityInfo> failed");
        return TRANSACTION_ERR;
    }
    SendEvent(*uiEvent, IS_EXTERNAL);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterStateCallback(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        HILOG_ERROR("obj is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibleAbilityManagerStateObserver> client = iface_cast<IAccessibleAbilityManagerStateObserver>(obj);
    if (client == nullptr) {
        HILOG_ERROR("client is nullptr");
        return ERR_INVALID_VALUE;
    }
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
        if (info == nullptr) {
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
    if (operation == nullptr) {
        HILOG_ERROR("iface_cast obj failed");
        return TRANSACTION_ERR;
    }
    bool isApp = IsApp();
    RegisterElementOperator(windowId, operation, isApp);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleMultiRegisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    Registration parameter;
    parameter.windowId = data.ReadInt32();
    parameter.parentWindowId = data.ReadInt32();
    parameter.parentTreeId = data.ReadInt32();
    parameter.elementId = data.ReadInt64();

    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperator> operation = iface_cast<IAccessibilityElementOperator>(obj);
    if (operation == nullptr) {
        HILOG_ERROR("iface_cast obj failed");
        return TRANSACTION_ERR;
    }
    bool isApp = IsApp();
    RegisterElementOperator(parameter, operation, isApp);

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

ErrCode AccessibleAbilityManagerServiceStub::HandleMultiDeregisterAccessibilityElementOperator(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t windowId = data.ReadInt32();
    int32_t treeId = data.ReadInt32();
    DeregisterElementOperator(windowId, treeId);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetCaptionProperty(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetCaptionProperty Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetCaptionProperty permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    sptr<CaptionPropertyParcel> caption = data.ReadStrongParcelable<CaptionPropertyParcel>();
    if (caption == nullptr) {
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetCaptionState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetCaptionState permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleEnableAbility Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleEnableAbility permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleDisableAbility Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleDisableAbility permission denied.");
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
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    HILOG_INFO("EnableUITestAbility called by %{public}d", clientPid);
    if (!IsSystemApp()) {
        HILOG_WARN("HandleEnableUITestAbility Permission denied");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    HILOG_INFO("DisableUITestAbility called by %{public}d", clientPid);
    if (!IsSystemApp()) {
        HILOG_WARN("HandleDisableUITestAbility Permission denied");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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
        HILOG_WARN("HandleSetScreenMagnificationState permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetShortKeyState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetShortKeyState permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetMouseKeyState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetMouseKeyState permission denied.");
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
    
    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetShortkeyTarget Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetShortkeyTarget permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    std::string name = data.ReadString();

    reply.WriteInt32(SetShortkeyTarget(name));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetShortkeyMultiTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    
    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetShortkeyMultiTarget Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetShortkeyMultiTarget permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    std::vector<std::string> name;
    data.ReadStringVector(&name);
    reply.WriteInt32(SetShortkeyMultiTarget(name));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetMouseAutoClick Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetMouseAutoClick permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetInvertColorState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetInvertColorState permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetHighContrastTextState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetHighContrastTextState permission denied.");
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
        HILOG_WARN("HandleSetAudioMonoState permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetAnimationOffState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetAnimationOffState permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetAnimationOffState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetDaltonizationState(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetDaltonizationState Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetDaltonizationState permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetDaltonizationState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetDaltonizationColorFilter(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
	
    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetDaltonizationColorFilter Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetDaltonizationColorFilter permission denied.");
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
	
    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetContentTimeout Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetContentTimeout permission denied.");
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

    if (!IsSystemApp()) {
        HILOG_WARN("HandleSetBrightnessDiscount Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetBrightnessDiscount permission denied.");
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
        HILOG_WARN("HandleSetAudioBalance permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    float balance = data.ReadFloat();

    reply.WriteInt32(SetAudioBalance(balance));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetClickResponseTime(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetClickResponseTime permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    uint32_t time = data.ReadUint32();

    reply.WriteInt32(SetClickResponseTime(time));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetIgnoreRepeatClickState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetIgnoreRepeatClickState permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    bool state = data.ReadBool();

    reply.WriteInt32(SetIgnoreRepeatClickState(state));

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSetIgnoreRepeatClickTime(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    if (!CheckPermission(OHOS_PERMISSION_WRITE_ACCESSIBILITY_CONFIG)) {
        HILOG_WARN("HandleSetIgnoreRepeatClickTime permission denied.");
        reply.WriteInt32(RET_ERR_NO_PERMISSION);
        return NO_ERROR;
    }
    uint32_t time = data.ReadUint32();

    reply.WriteInt32(SetIgnoreRepeatClickTime(time));

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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    std::string result = "";
    RetError ret = GetShortkeyTarget(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteString(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetShortkeyMultiTarget(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }

    std::vector<std::string> result;
    RetError ret = GetShortkeyMultiTarget(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteStringVector(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetMouseAutoClick(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    bool result = false;
    RetError ret = GetAnimationOffState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetDaltonizationState(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    bool result = false;
    RetError ret = GetDaltonizationState(result);
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

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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
	
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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
    
    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
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

ErrCode AccessibleAbilityManagerServiceStub::HandleGetClickResponseTime(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    uint32_t result = 0;
    RetError ret = GetClickResponseTime(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteUint32(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetIgnoreRepeatClickState(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    bool result = 0;
    RetError ret = GetIgnoreRepeatClickState(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteBool(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetIgnoreRepeatClickTime(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    uint32_t result = 0;
    RetError ret = GetIgnoreRepeatClickTime(result);
    reply.WriteInt32(ret);
    if (ret == RET_OK) {
        reply.WriteUint32(result);
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAllConfigs(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }
    reply.WriteInt32(RET_OK);
    
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
    reply.WriteBool(configData.daltonizationState_);
    reply.WriteUint32(configData.daltonizationColorFilter_);
    reply.WriteUint32(configData.contentTimeout_);
    reply.WriteFloat(configData.brightnessDiscount_);
    reply.WriteFloat(configData.audioBalance_);
    reply.WriteString(configData.shortkeyTarget_);
    reply.WriteParcelable(&captionParcel);
    reply.WriteStringVector(configData.shortkeyMultiTarget_);
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
    if (obj == nullptr) {
        HILOG_ERROR("obj is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibleAbilityManagerConfigObserver> config = iface_cast<IAccessibleAbilityManagerConfigObserver>(obj);
    if (config == nullptr) {
        HILOG_ERROR("config is nullptr");
        return ERR_INVALID_VALUE;
    }
    uint64_t result = RegisterConfigObserver(config);
    reply.WriteUint64(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetWindowAndElementId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t windowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    GetRealWindowAndElementId(windowId, elementId);
    if (!reply.WriteInt32(windowId)) {
        HILOG_ERROR("write windowId fail");
    }
    
    if (!reply.WriteInt64(elementId)) {
        HILOG_ERROR("write elementId fail");
    }

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetSceneBoardInnerWinId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t windowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    int32_t innerWid = -1;
    GetSceneBoardInnerWinId(windowId, elementId, innerWid);
    if (!reply.WriteInt32(innerWid)) {
        HILOG_ERROR("write windowId fail");
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetFocusedWindowId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t focusedWindowId = -1;
    GetFocusedWindowId(focusedWindowId);
    if (reply.WriteInt32(focusedWindowId)) {
        HILOG_ERROR("write windowId fail");
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRemoveRequestId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t requestId = data.ReadInt32();
    RemoveRequestId(requestId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetRootParentId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t windowId = data.ReadInt32();
    int32_t treeId = data.ReadInt32();
    int64_t elementId = GetRootParentId(windowId, treeId);
    reply.WriteInt64(elementId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAllTreeId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t windowId = data.ReadInt32();
    std::vector<int32_t> treeIds {};

    RetError result = GetAllTreeId(windowId, treeIds);

    reply.WriteInt32(static_cast<int32_t>(result));
    int32_t treeIdSize = static_cast<int32_t>(treeIds.size());
    reply.WriteInt32(treeIdSize);
    for (auto &treeId : treeIds) {
        if (!reply.WriteInt32(treeId)) {
            HILOG_ERROR("WriteInt32 treeId failed");
            return TRANSACTION_ERR;
        }
    }

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS
