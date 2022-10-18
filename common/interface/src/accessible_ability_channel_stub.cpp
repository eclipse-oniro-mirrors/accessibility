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
#include "accessibility_window_info_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelStub::AccessibleAbilityChannelStub()
{
    HILOG_DEBUG();

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID)] =
        &AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT)] =
        &AccessibleAbilityChannelStub::HandleSearchElementInfosByText;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO)] =
        &AccessibleAbilityChannelStub::HandleFindFocusedElementInfo;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH)] =
        &AccessibleAbilityChannelStub::HandleFocusMoveSearch;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::PERFORM_ACTION)] =
        &AccessibleAbilityChannelStub::HandleExecuteAction;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_WINDOW)] =
        &AccessibleAbilityChannelStub::HandleGetWindow;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_WINDOWS)] =
        &AccessibleAbilityChannelStub::HandleGetWindows;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_WINDOWS_BY_DISPLAY_ID)] =
        &AccessibleAbilityChannelStub::HandleGetWindowsByDisplayId;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT)] =
        &AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE_PATH)] =
        &AccessibleAbilityChannelStub::HandleSendSimulateGesturePath;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_TARGET_BUNDLE_NAME)] =
        &AccessibleAbilityChannelStub::HandleSetTargetBundleName;
}

AccessibleAbilityChannelStub::~AccessibleAbilityChannelStub()
{
    HILOG_DEBUG();
    memberFuncMap_.clear();
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

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    HILOG_WARN("AbilityManagerStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int32_t elementId = data.ReadInt32();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (!remote) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    int32_t mode = data.ReadInt32();
    RetError result = SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId, callback, mode);
    HILOG_DEBUG("SearchElementInfoByAccessibilityId ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int32_t elementId = data.ReadInt32();
    std::string text = data.ReadString();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);

    RetError result = SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, callback);
    HILOG_DEBUG("SearchElementInfosByText ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int32_t elementId = data.ReadInt32();
    int32_t focusType = data.ReadInt32();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);

    RetError result = FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, callback);
    HILOG_DEBUG("FindFocusedElementInfo ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFocusMoveSearch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int32_t elementId = data.ReadInt32();
    int32_t direction = data.ReadInt32();
    int32_t requestId = data.ReadInt32();

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(remote);

    RetError result = FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, callback);
    HILOG_DEBUG("FocusMoveSearch ret = %{public}d", result);
    reply.WriteInt32(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t accessibilityWindowId = data.ReadInt32();
    int32_t elementId = data.ReadInt32();
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

    auto callback = iface_cast<IAccessibilityElementOperatorCallback>(data.ReadRemoteObject());
    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    RetError result = ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
    HILOG_DEBUG("ExecuteAction ret = %{public}d", result);
    reply.WriteInt32(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindow(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t windowId = data.ReadInt32();
    sptr<AccessibilityWindowInfoParcel> windowInfoParcel = new(std::nothrow) AccessibilityWindowInfoParcel();
    if (!windowInfoParcel) {
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
        if (!windowInfo) {
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
        if (!windowInfo) {
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
    if (!positions) {
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
    for (int32_t i = 0; i < size; i++) {
        std::string temp = data.ReadString();
        targetBundleNames.emplace_back(temp);
    }
    RetError result = SetTargetBundleName(targetBundleNames);
    reply.WriteInt32(result);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS