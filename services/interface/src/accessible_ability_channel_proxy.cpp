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

bool AccessibleAbilityChannelProxy::SendTransactCmd(IAccessibleAbilityChannel::Message code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG();

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}

bool AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}d, ", elementId);
        return false;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return false;
    }
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return false;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return false;
    }
    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("mode write error: %{public}d, ", mode);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("fail to find elementInfo by elementId");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}d, ", elementId);
        return false;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("text write error: %{public}s, ", text.c_str());
        return false;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return false;
    }
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return false;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT,
        data, reply, option)) {
        HILOG_ERROR("fail to find elementInfo by text");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}d, ", elementId);
        return false;
    }
    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("focusType write error: %{public}d, ", focusType);
        return false;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return false;
    }
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return false;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO, data, reply, option)) {
        HILOG_ERROR("fail to gain focus");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}d, ", elementId);
        return false;
    }
    if (!data.WriteInt32(direction)) {
        HILOG_ERROR("direction write error: %{public}d, ", direction);
        return false;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return false;
    }
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return false;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH, data, reply, option)) {
        HILOG_ERROR("fail to search focus");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}d, ", elementId);
        return false;
    }
    if (!data.WriteInt32(action)) {
        HILOG_ERROR("action write error: %{public}d, ", action);
        return false;
    }

    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};
    for (auto iter = actionArguments.begin(); iter != actionArguments.end(); iter++) {
        actionArgumentsKey.push_back(iter->first);
        actionArgumentsValue.push_back(iter->second);
    }
    if (!data.WriteStringVector(actionArgumentsKey)) {
        HILOG_ERROR("actionArgumentsKey write error");
        return false;
    }
    if (!data.WriteStringVector(actionArgumentsValue)) {
        HILOG_ERROR("actionArgumentsValue write error");
        return false;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return false;
    }
    if (!callback || !data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("callback write error");
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::PERFORM_ACTION,
        data, reply, option)) {
        HILOG_ERROR("fail to perform accessibility action");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }

    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("windowId write error: %{public}d, ", windowId);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_WINDOW, data, reply, option)) {
        HILOG_ERROR("fail to get window");
        return false;
    }

    sptr<AccessibilityWindowInfoParcel> windowInfoParcel = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
    if (!windowInfoParcel) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
        return false;
    }
    windowInfo = *windowInfoParcel;

    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_WINDOWS, data, reply, option)) {
        HILOG_ERROR("fail to get windows");
        return false;
    }

    int32_t windowsSize = reply.ReadInt32();
    for (int32_t i = 0; i < windowsSize; i++) {
        sptr<AccessibilityWindowInfoParcel> window = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
        if (!window) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
            return false;
        }
        windows.emplace_back(*window);
    }

    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }

    if (!data.WriteUint64(displayId)) {
        HILOG_ERROR("displayId write error: %{public}" PRIu64 ", ", displayId);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_WINDOWS_BY_DISPLAY_ID, data, reply, option)) {
        HILOG_ERROR("fail to get windows");
        return false;
    }

    int32_t windowsSize = reply.ReadInt32();
    for (int32_t i = 0; i < windowsSize; i++) {
        sptr<AccessibilityWindowInfoParcel> window = reply.ReadStrongParcelable<AccessibilityWindowInfoParcel>();
        if (!window) {
            HILOG_ERROR("ReadStrongParcelable<AccessibilityWindowInfoParcel> failed");
            return false;
        }
        windows.emplace_back(*window);
    }

    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::ExecuteCommonAction(const int32_t action)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(action)) {
        HILOG_ERROR("action write error: %{public}d, ", action);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::EXECUTE_COMMON_ACTION, data, reply, option)) {
        HILOG_ERROR("fail to perform common action");
        return false;
    }
    return reply.ReadBool();
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
    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT,
        data, reply, option)) {
        HILOG_ERROR("fail to set onKeyPressEvent result");
    }
}

void AccessibleAbilityChannelProxy::SendSimulateGesture(const int32_t requestId,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    sptr<AccessibilityGestureInjectPathParcel> path =
        new(std::nothrow) AccessibilityGestureInjectPathParcel(*gesturePath);
    if (!path) {
        HILOG_ERROR("Failed to create path.");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
        return;
    }

    if (!data.WriteStrongParcelable(path)) {
        HILOG_ERROR("WriteStrongParcelable<AccessibilityGestureInjectPathParcel> failed");
        return;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE_PATH, data, reply, option)) {
        HILOG_ERROR("fail to send simulation gesture path");
    }
}

bool AccessibleAbilityChannelProxy::SetEventTypeFilter(const uint32_t filter)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }

    if (!data.WriteUint32(filter)) {
        HILOG_ERROR("filter write error: %{public}d, ", filter);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SET_EVENT_TYPE_FILTER, data, reply, option)) {
        HILOG_ERROR("fail to set event type filter");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(targetBundleNames.size())) {
        HILOG_ERROR("targetBundleNames.size() write error: %{public}zu, ", targetBundleNames.size());
        return false;
    }
    for (auto &targetBundleName : targetBundleNames) {
        if (!data.WriteString(targetBundleName)) {
            HILOG_ERROR("targetBundleName write error: %{public}s, ", targetBundleName.c_str());
            return false;
        }
    }
    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SET_TARGET_BUNDLE_NAME, data, reply, option)) {
        HILOG_ERROR("fail to set target bundle name filter");
        return false;
    }
    return reply.ReadBool();
}
} // namespace Accessibility
} // namespace OHOS