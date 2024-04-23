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

#include "accessible_ability_channel_proxy.h"

#include <cinttypes>

#include "accessibility_element_info_parcel.h"
#include "accessibility_gesture_inject_path_parcel.h"
#include "accessibility_ipc_interface_code.h"
#include "accessibility_window_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelProxy::AccessibleAbilityChannelProxy(
    const sptr<IRemoteObject> &object): IRemoteProxy<IAccessibleAbilityChannel>(object)
{
}

bool AccessibleAbilityChannelProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();

    if (!data.WriteInterfaceToken(AccessibleAbilityChannelProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> remoteChannelProxy = Remote();
    if (!remoteChannelProxy) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t resultChannelProxy = remoteChannelProxy->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (resultChannelProxy != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", resultChannelProxy, code);
        return false;
    }
    return true;
}

RetError AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter)
{
    HILOG_DEBUG();
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_INVALID_PARAM;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("mode write error: %{public}d, ", mode);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteBool(isFilter)) {
        HILOG_ERROR("isFilter write error: %{public}d, ", isFilter);
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("fail to find elementInfo by elementId");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_INVALID_PARAM;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("text write error: %{public}s, ", text.c_str());
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_ELEMENTINFOS_BY_TEXT,
        data, reply, option)) {
        HILOG_ERROR("fail to find elementInfo by text");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_INVALID_PARAM;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("focusType write error: %{public}d, ", focusType);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::FIND_FOCUSED_ELEMENTINFO, data, reply, option)) {
        HILOG_ERROR("fail to gain focus");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_INVALID_PARAM;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(direction)) {
        HILOG_ERROR("direction write error: %{public}d, ", direction);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::FOCUS_MOVE_SEARCH, data, reply, option)) {
        HILOG_ERROR("fail to search focus");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::EnableScreenCurtain(bool isEnable)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteBool(isEnable)) {
        HILOG_ERROR("isEnable write error: %{public}d, ", isEnable);
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CURTAIN_SCREEN,
        data, reply, option)) {
        HILOG_ERROR("fail to enable screen curtain");
        return RET_ERR_IPC_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(action)) {
        HILOG_ERROR("action write error: %{public}d, ", action);
        return RET_ERR_IPC_FAILED;
    }

    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};
    for (auto iter = actionArguments.begin(); iter != actionArguments.end(); iter++) {
        actionArgumentsKey.push_back(iter->first);
        actionArgumentsValue.push_back(iter->second);
    }
    if (!data.WriteStringVector(actionArgumentsKey)) {
        HILOG_ERROR("actionArgumentsKey write error");
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteStringVector(actionArgumentsValue)) {
        HILOG_ERROR("actionArgumentsValue write error");
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!callback || !data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::PERFORM_ACTION,
        data, reply, option)) {
        HILOG_ERROR("fail to perform accessibility action");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("windowId write error: %{public}d, ", windowId);
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_WINDOW, data, reply, option)) {
        HILOG_ERROR("fail to get window");
        return RET_ERR_IPC_FAILED;
    }

    sptr<AccessibilityWindowInfoParcel> windowInfoParcel = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
    if (!windowInfoParcel) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
        return RET_ERR_IPC_FAILED;
    }
    windowInfo = *windowInfoParcel;

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_WINDOWS, data, reply, option)) {
        HILOG_ERROR("fail to get windows");
        return RET_ERR_IPC_FAILED;
    }

    int32_t windowsSize = reply.ReadInt32();
    for (int32_t i = 0; i < windowsSize; i++) {
        sptr<AccessibilityWindowInfoParcel> window = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
        if (!window) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
            return RET_ERR_IPC_FAILED;
        }
        windows.emplace_back(*window);
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteUint64(displayId)) {
        HILOG_ERROR("displayId write error: %{public}" PRIu64 ", ", displayId);
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_WINDOWS_BY_DISPLAY_ID, data, reply, option)) {
        HILOG_ERROR("fail to get windows");
        return RET_ERR_IPC_FAILED;
    }

    int32_t windowsSize = reply.ReadInt32();
    for (int32_t i = 0; i < windowsSize; i++) {
        sptr<AccessibilityWindowInfoParcel> window = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
        if (!window) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
            return RET_ERR_IPC_FAILED;
        }
        windows.emplace_back(*window);
    }

    return static_cast<RetError>(reply.ReadInt32());
}

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteBool(handled)) {
        HILOG_ERROR("handled write error: %{public}d, ", handled);
        return;
    }
    if (!data.WriteInt32(sequence)) {
        HILOG_ERROR("sequence write error: %{public}d, ", sequence);
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_ON_KEY_PRESS_EVENT_RESULT,
        data, reply, option)) {
        HILOG_ERROR("fail to set onKeyPressEvent result");
    }
}

RetError AccessibleAbilityChannelProxy::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!WriteInterfaceToken(data)) {
        return RET_ERR_FAILED;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return RET_ERR_FAILED;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error");
        return RET_ERR_FAILED;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return RET_ERR_IPC_FAILED;
    }
    if (!callback) {
        HILOG_ERROR("callback is null");
        return RET_ERR_FAILED;
    }
    if (callback->AsObject() == nullptr) {
        HILOG_ERROR("callback->AsObject() is null");
        return RET_ERR_FAILED;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return RET_ERR_IPC_FAILED;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::GET_CURSOR_POSITION,
        data, reply, option)) {
        HILOG_ERROR("fail to set onKeyPressEvent result");
        return RET_ERR_FAILED;
    }

    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    sptr<AccessibilityGestureInjectPathParcel> path =
        new(std::nothrow) AccessibilityGestureInjectPathParcel(*gesturePath);
    if (!path) {
        HILOG_ERROR("Failed to create path.");
        return RET_ERR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }

    if (!data.WriteStrongParcelable(path)) {
        HILOG_ERROR("WriteStrongParcelable<AccessibilityGestureInjectPathParcel> failed");
        return RET_ERR_IPC_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEND_SIMULATE_GESTURE_PATH, data, reply, option)) {
        HILOG_ERROR("fail to send simulation gesture path");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}

RetError AccessibleAbilityChannelProxy::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return RET_ERR_IPC_FAILED;
    }
    if (!data.WriteInt32(targetBundleNames.size())) {
        HILOG_ERROR("targetBundleNames.size() write error: %{public}zu, ", targetBundleNames.size());
        return RET_ERR_IPC_FAILED;
    }
    for (auto &targetBundleName : targetBundleNames) {
        if (!data.WriteString(targetBundleName)) {
            HILOG_ERROR("targetBundleName write error: %{public}s, ", targetBundleName.c_str());
            return RET_ERR_IPC_FAILED;
        }
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_TARGET_BUNDLE_NAME, data, reply, option)) {
        HILOG_ERROR("fail to set target bundle name filter");
        return RET_ERR_IPC_FAILED;
    }
    return static_cast<RetError>(reply.ReadInt32());
}
} // namespace Accessibility
} // namespace OHOS