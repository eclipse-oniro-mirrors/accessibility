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

#include "accessible_ability_channel_stub.h"
#include "accessibility_element_info_parcel.h"
#include "accessibility_gesture_inject_path_parcel.h"
#include "accessibility_ipc_interface_code.h"
#include "accessibility_permission.h"
#include "accessibility_window_info_parcel.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

#define SWITCH_BEGIN(code) switch (code) {
#define SWITCH_CASE(case_code, func)     \
    case case_code: {                    \
        result_code = func(data, reply); \
        break;                           \
    }

#define SWITCH_END()                                                                            \
    default: {                                                                                  \
        result_code = ERR_CODE_DEFAULT;                                                         \
        HILOG_WARN("AccessibleAbilityChannelStub::OnRemoteRequest, default case, need check."); \
        break;                                                                                  \
    }                                                                                           \
}

#define ACCESSIBLE_ABILITY_CHANNEL_STUB_CASES()                                                                       \
    SWITCH_CASE(                                                                                                      \
        AccessibilityInterfaceCode::SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID, HandleSearchElementInfoByAccessibilityId) \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_ELEMENTINFOS_BY_TEXT, HandleSearchElementInfosByText)              \
    SWITCH_CASE(AccessibilityInterfaceCode::FIND_FOCUSED_ELEMENTINFO, HandleFindFocusedElementInfo)                   \
    SWITCH_CASE(AccessibilityInterfaceCode::FOCUS_MOVE_SEARCH, HandleFocusMoveSearch)                                 \
    SWITCH_CASE(AccessibilityInterfaceCode::PERFORM_ACTION, HandleExecuteAction)                                      \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CURTAIN_SCREEN, HandleEnableScreenCurtain)                            \
    SWITCH_CASE(AccessibilityInterfaceCode::HOLD_RUNNING_LOCK, HandleHoldRunningLock)                                 \
    SWITCH_CASE(AccessibilityInterfaceCode::UNHOLD_RUNNING_LOCK, HandleUnholdRunningLock)                             \
    SWITCH_CASE(AccessibilityInterfaceCode::GET_WINDOW, HandleGetWindow)                                              \
    SWITCH_CASE(AccessibilityInterfaceCode::GET_WINDOWS, HandleGetWindows)                                            \
    SWITCH_CASE(AccessibilityInterfaceCode::GET_WINDOWS_BY_DISPLAY_ID, HandleGetWindowsByDisplayId)                   \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_ON_KEY_PRESS_EVENT_RESULT, HandleSetOnKeyPressEventResult)            \
    SWITCH_CASE(AccessibilityInterfaceCode::SEND_SIMULATE_GESTURE_PATH, HandleSendSimulateGesturePath)                \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_TARGET_BUNDLE_NAME, HandleSetTargetBundleName)                        \
    SWITCH_CASE(AccessibilityInterfaceCode::GET_CURSOR_POSITION, HandleGetCursorPosition)                             \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_DEFAULTFOCUSED_BY_WINDOW_ID, HandleSearchDefaultFocusedByWindowId) \
    SWITCH_CASE(                                                                                                      \
        AccessibilityInterfaceCode::SET_IS_REGISTER_DISCONNECT_CALLBACK, HandleSetIsRegisterDisconnectCallback)       \
    SWITCH_CASE(AccessibilityInterfaceCode::NOTIFY_DISCONNECT, HandleNotifyDisconnect)                                \
    SWITCH_CASE(AccessibilityInterfaceCode::CONFIGURE_EVENTS, HandleConfigureEvents)

namespace OHOS {
namespace Accessibility {
constexpr int32_t ERR_CODE_DEFAULT = -1000;

AccessibleAbilityChannelStub::AccessibleAbilityChannelStub()
{
}

AccessibleAbilityChannelStub::~AccessibleAbilityChannelStub()
{
}

int AccessibleAbilityChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityChannelStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    ErrCode result_code = ERR_NONE;
    SWITCH_BEGIN(code)
    ACCESSIBLE_ABILITY_CHANNEL_STUB_CASES()
    SWITCH_END()

    if (result_code != ERR_CODE_DEFAULT) {
        return result_code;
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    ElementBasicInfo elementBasicInfo {};
    elementBasicInfo.windowId = data.ReadInt32();
    elementBasicInfo.treeId = data.ReadInt32();
    elementBasicInfo.elementId = data.ReadInt64();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    int32_t mode = data.ReadInt32();
    if (mode == PREFETCH_RECURSIVE_CHILDREN) {
        if (!Permission::CheckCallingPermission(OHOS_PERMISSION_QUERY_ACCESSIBILITY_ELEMENT) &&
            !Permission::CheckCallingPermission(OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY) &&
            !Permission::IsStartByHdcd()) {
            HILOG_ERROR("no get element permission");
            reply.WriteInt32(RET_ERR_NO_CONNECTION);
            return NO_ERROR;
        }
    }

    if (mode == GET_SOURCE_MODE) {
        mode = PREFETCH_RECURSIVE_CHILDREN;
    }
    bool isFilter = data.ReadBool();
    RetError result = SearchElementInfoByAccessibilityId(elementBasicInfo, requestId, callback, mode,
        isFilter);
    HILOG_DEBUG("SearchElementInfoByAccessibilityId ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchDefaultFocusedByWindowId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
 
    ElementBasicInfo elementBasicInfo {};
    elementBasicInfo.windowId = data.ReadInt32();
    elementBasicInfo.elementId = data.ReadInt64();
    elementBasicInfo.treeId = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
 
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }
 
    int32_t mode = data.ReadInt32();
    if (mode == PREFETCH_RECURSIVE_CHILDREN) {
        if (!Permission::CheckCallingPermission(OHOS_PERMISSION_QUERY_ACCESSIBILITY_ELEMENT) &&
            !Permission::CheckCallingPermission(OHOS_PERMISSION_ACCESSIBILITY_EXTENSION_ABILITY) &&
            !Permission::IsStartByHdcd()) {
            HILOG_ERROR("no get element permission");
            reply.WriteInt32(RET_ERR_NO_CONNECTION);
            return NO_ERROR;
        }
    }
 
    if (mode == GET_SOURCE_MODE) {
        mode = PREFETCH_RECURSIVE_CHILDREN;
    }
    bool isFilter = data.ReadBool();
    RetError result = SearchDefaultFocusedByWindowId(elementBasicInfo, requestId, callback, mode,
        isFilter);
    HILOG_DEBUG("SearchDefaultFocusedByWindowId ret = %{public}d", result);
    reply.WriteInt32(result);
 
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    std::string text = data.ReadString();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    RetError result = SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, callback);
    HILOG_DEBUG("SearchElementInfosByText ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    int32_t focusType = data.ReadInt32();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    RetError result = FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, callback);
    HILOG_DEBUG("FindFocusedElementInfo ret = %{public}d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFocusMoveSearch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    int32_t direction = data.ReadInt32();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    RetError result = FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, callback);
    HILOG_DEBUG("FocusMoveSearch ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    int32_t action = data.ReadInt32();

    std::vector<std::string> actionArgumentsKey;
    std::vector<std::string> actionArgumentsValue;
    std::map<std::string, std::string> actionArguments;

    if (!data.ReadStringVector(&actionArgumentsKey)) {
        HILOG_ERROR("ReadStringVector actionArgumentsKey failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.ReadStringVector(&actionArgumentsValue)) {
        HILOG_ERROR("ReadStringVector actionArgumentsValue failed");
        return ERR_INVALID_VALUE;
    }
    if (actionArgumentsKey.size() != actionArgumentsValue.size()) {
        HILOG_ERROR("Read actionArguments failed.");
        return ERR_INVALID_VALUE;
    }
    for (size_t i = 0; i < actionArgumentsKey.size(); i++) {
        actionArguments.insert(make_pair(actionArgumentsKey[i], actionArgumentsValue[i]));
    }

    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    auto callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    RetError result = ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
    HILOG_DEBUG("ExecuteAction ret = %{public}d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleEnableScreenCurtain(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!Permission::IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return NO_ERROR;
    }

    bool isEnable = data.ReadBool();
    RetError result = EnableScreenCurtain(isEnable);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleHoldRunningLock(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
 
    if (!Permission::IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return RET_ERR_NOT_SYSTEM_APP;
    }
 
    RetError result = HoldRunningLock();
    reply.WriteInt32(result);
    return NO_ERROR;
}
 
ErrCode AccessibleAbilityChannelStub::HandleUnholdRunningLock(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
 
    if (!Permission::IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return RET_ERR_NOT_SYSTEM_APP;
    }
 
    RetError result = UnholdRunningLock();
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetCursorPosition(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int64_t elementId = data.ReadInt64();
    int32_t requestId = data.ReadInt32();
    HILOG_INFO("AccessibleAbilityChannelStub::HandleGetCursorPosition   The execution was successful");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    auto callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    RetError result = GetCursorPosition(accessibilityWindowId, elementId, requestId, callback);
    HILOG_DEBUG("GetCursorPosition ret = %{public}d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindow(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t windowId = data.ReadInt32();
    sptr<AccessibilityWindowInfoParcel> windowInfoParcel = new(std::nothrow) AccessibilityWindowInfoParcel();
    if (windowInfoParcel == nullptr) {
        HILOG_ERROR("Failed to create windowInfoParcel.");
        return ERR_NULL_OBJECT;
    }

    RetError result = GetWindow(windowId, *windowInfoParcel);
    if (!reply.WriteStrongParcelable(windowInfoParcel)) {
        HILOG_ERROR("WriteStrongParcelable windows failed");
        return ERR_INVALID_VALUE;
    }

    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindows(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<AccessibilityWindowInfo> windows;
    RetError result = GetWindows(windows);
    if (!reply.WriteInt32(static_cast<int32_t>(windows.size()))) {
        HILOG_ERROR("windows.size() write error: %{public}zu, ", windows.size());
        return ERR_INVALID_VALUE;
    }
    for (auto &window : windows) {
        sptr<AccessibilityWindowInfoParcel> windowInfo = new(std::nothrow) AccessibilityWindowInfoParcel(window);
        if (windowInfo == nullptr) {
            HILOG_ERROR("Failed to create windowInfo.");
            return ERR_NULL_OBJECT;
        }
        if (!reply.WriteStrongParcelable(windowInfo)) {
            HILOG_ERROR("WriteStrongParcelable windows failed");
            return ERR_INVALID_VALUE;
        }
    }
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindowsByDisplayId(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    uint64_t displayId = data.ReadUint64();
    std::vector<AccessibilityWindowInfo> windows;
    RetError result = GetWindowsByDisplayId(displayId, windows);
    if (!reply.WriteInt32(static_cast<int32_t>(windows.size()))) {
        HILOG_ERROR("windows.size() write error: %{public}zu, ", windows.size());
        return ERR_INVALID_VALUE;
    }
    for (auto &window : windows) {
        sptr<AccessibilityWindowInfoParcel> windowInfo = new(std::nothrow) AccessibilityWindowInfoParcel(window);
        if (windowInfo == nullptr) {
            HILOG_ERROR("Failed to create windowInfo.");
            return ERR_NULL_OBJECT;
        }
        if (!reply.WriteStrongParcelable(windowInfo)) {
            HILOG_ERROR("WriteStrongParcelable windows failed");
            return ERR_INVALID_VALUE;
        }
    }
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    bool handled = data.ReadBool();
    int32_t sequence = data.ReadInt32();
    SetOnKeyPressEventResult(handled, sequence);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSendSimulateGesturePath(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    sptr<AccessibilityGestureInjectPathParcel> positions =
        data.ReadStrongParcelable<AccessibilityGestureInjectPathParcel>();
    if (positions == nullptr) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityGestureInjectPathParcel> failed");
        return ERR_INVALID_VALUE;
    }
 
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
        std::make_shared<AccessibilityGestureInjectPath>(*positions);
    RetError result = SendSimulateGesture(gesturePath);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetTargetBundleName(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<std::string> targetBundleNames;
    int32_t size = data.ReadInt32();
    bool verifyResult = ContainerSecurityVerify(data, size, targetBundleNames.max_size());
    if (!verifyResult) {
        return TRANSACTION_ERR;
    }
    if (size < 0 || size > MAX_ALLOW_SIZE) {
        return TRANSACTION_ERR;
    }
    for (int32_t i = 0; i < size; i++) {
        std::string temp = data.ReadString();
        targetBundleNames.emplace_back(temp);
    }
    RetError result = SetTargetBundleName(targetBundleNames);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetIsRegisterDisconnectCallback(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!Permission::IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return RET_ERR_NOT_SYSTEM_APP;
    }

    bool isRegister = data.ReadBool();
    RetError result = SetIsRegisterDisconnectCallback(isRegister);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleNotifyDisconnect(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    if (!Permission::IsSystemApp()) {
        HILOG_WARN("Not system app");
        reply.WriteInt32(RET_ERR_NOT_SYSTEM_APP);
        return RET_ERR_NOT_SYSTEM_APP;
    }

    RetError result = NotifyDisconnect();
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleConfigureEvents(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<uint32_t> needEvents;
    uint32_t size = data.ReadUint32();
    bool verifyResult = ContainerSecurityVerify(data, size, needEvents.max_size());
    if (!verifyResult) {
        return TRANSACTION_ERR;
    }

    if (size < 0 || size > MAX_ALLOW_SIZE) {
        return TRANSACTION_ERR;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        uint32_t temp = data.ReadUint32();
        needEvents.emplace_back(temp);
    }
    RetError result = ConfigureEvents(needEvents);
    reply.WriteInt32(result);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS