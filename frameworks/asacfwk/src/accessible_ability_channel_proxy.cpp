/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "accessibility_errorcode.h"
#include "parcel.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

AccessibleAbilityChannelProxy::AccessibleAbilityChannelProxy(
    const sptr<IRemoteObject> &object): IRemoteProxy<IAccessibleAbilityChannel>(object)
{

}

bool AccessibleAbilityChannelProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (!data.WriteInterfaceToken(AccessibleAbilityChannelProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityChannelProxy::SendTransactCmd(IAccessibleAbilityChannel::Message code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("%{public}s start.", __func__);

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

bool AccessibleAbilityChannelProxy::SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
    const long elementId, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int mode)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}ld, ", elementId);
        return false;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("requestId write error: %{public}d, ", requestId);
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

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID,
                        data, reply, option)) {
        HILOG_ERROR("fail to find elementInfo by elementId");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::SearchElementInfosByText(const int accessibilityWindowId,
    const long elementId, const std::string &text, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}ld, ", elementId);
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

bool AccessibleAbilityChannelProxy::FindFocusedElementInfo(const int accessibilityWindowId,
    const long elementId, const int focusType, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}ld, ", elementId);
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

bool AccessibleAbilityChannelProxy::FocusMoveSearch(const int accessibilityWindowId, const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}ld, ", elementId);
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

bool AccessibleAbilityChannelProxy::ExecuteAction(const int accessibilityWindowId, const long elementId,
    const int action, std::map<std::string, std::string> &actionArguments, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(accessibilityWindowId)) {
        HILOG_ERROR("accessibilityWindowId write error: %{public}d, ", accessibilityWindowId);
        return false;
    }
    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("elementId write error: %{public}ld, ", elementId);
        return false;
    }
    if (!data.WriteInt32(action)) {
        HILOG_ERROR("action write error: %{public}d, ", action);
        return false;
    }

    vector<string> actionArgumentsKey{};
    vector<string> actionArgumentsValue{};
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
    if (!data.WriteRemoteObject(callback->AsObject())) {
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

vector<AccessibilityWindowInfo> AccessibleAbilityChannelProxy::GetWindows()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    vector<AccessibilityWindowInfo> windowsError;
    vector<AccessibilityWindowInfo> windows;

    if (!WriteInterfaceToken(data)) {
        return windowsError;
    }
    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_WINDOWS, data, reply, option)) {
        HILOG_ERROR("fail to get windows");
        return windowsError;
    }

    int32_t windowsSize = reply.ReadInt32();
    for (int32_t i = 0; i < windowsSize; i++) {
        unique_ptr<AccessibilityWindowInfo> window(reply.ReadParcelable<AccessibilityWindowInfo>());
        if (!window) {
            HILOG_ERROR("ReadParcelable<AccessibilityWindowInfo> failed");
            return windowsError;
        }
        windows.emplace_back(*window);
    }

    return windows;
}

bool AccessibleAbilityChannelProxy::ExecuteCommonAction(const int action)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

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

void AccessibleAbilityChannelProxy::SetOnKeyPressEventResult(const bool handled, const int sequence)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

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

float AccessibleAbilityChannelProxy::GetDisplayResizeScale(const int displayId)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return 0;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return 0;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_SCALE, data, reply, option)) {
        HILOG_ERROR("fail to get displayResize scale");
        return 0;
    }
    return reply.ReadFloat();
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterX(const int displayId)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return 0;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return 0;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_CETER_X, data, reply, option)) {
        HILOG_ERROR("fail to get displayResize centerX");
        return 0;
    }
    return reply.ReadFloat();
}

float AccessibleAbilityChannelProxy::GetDisplayResizeCenterY(const int displayId)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return 0;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return 0;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_CETER_Y, data, reply, option)) {
        HILOG_ERROR("fail to get displayResize centerY");
        return 0;
    }
    return reply.ReadFloat();
}

Rect AccessibleAbilityChannelProxy::GetDisplayResizeRect(const int displayId)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    Rect rect(0, 0, 0, 0);

    if (!WriteInterfaceToken(data)) {
        return rect;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return rect;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_RECT, data, reply, option)) {
        HILOG_ERROR("fail to get displayResize rect");
        return rect;
    }

    sptr<Rect> result = reply.ReadStrongParcelable<Rect>();
    if (!result) {
        HILOG_ERROR("ReadStrongParcelable<Rect> failed");
        return rect;
    }
    return *result;
}

bool AccessibleAbilityChannelProxy::ResetDisplayResize(const int displayId, const bool animate)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return false;
    }
    if (!data.WriteBool(animate)) {
        HILOG_ERROR("animate write error: %{public}d, ", animate);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::RESET_DISPALYRESIZE, data, reply, option)) {
        HILOG_ERROR("fail to reset displayResize");
        return false;
    }
    return reply.ReadBool();
}

bool AccessibleAbilityChannelProxy::SetDisplayResizeScaleAndCenter(const int displayId, const float scale,
                                        const float centerX, const float centerY, const bool animate)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        return false;
    }
    if (!data.WriteInt32(displayId)) {
        HILOG_ERROR("displayId write error: %{public}d, ", displayId);
        return false;
    }
    if (!data.WriteFloat(scale)) {
        HILOG_ERROR("scale write error: %{public}f, ", scale);
        return false;
    }
    if (!data.WriteFloat(centerX)) {
        HILOG_ERROR("centerX write error: %{public}f, ", centerX);
        return false;
    }
    if (!data.WriteFloat(centerY)) {
        HILOG_ERROR("centerY write error: %{public}f, ", centerY);
        return false;
    }
    if (!data.WriteBool(animate)) {
        HILOG_ERROR("animate write error: %{public}d, ", animate);
        return false;
    }

    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SET_DISPLAYRESIZE_SCALE_AND_CENTER,
                            data, reply, option)) {
        HILOG_ERROR("fail to set displayResize scale and center");
        return false;
    }
    return reply.ReadBool();
}

void AccessibleAbilityChannelProxy::SendSimulateGesture(const int requestId,
                                                        const std::vector<GesturePathDefine> &gestureSteps)
{
    HILOG_DEBUG("%{public}s start.", __func__);

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
    if (!data.WriteInt32(gestureSteps.size())) {
        HILOG_ERROR("gestureSteps.size() write error: %{public}d, ", gestureSteps.size());
        return;
    }
    for (auto &step : gestureSteps) {
        if (!data.WriteParcelable(&step)) {
            HILOG_ERROR("gestureSteps write error");
            return;
        }
    }
    if (!SendTransactCmd(IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE, data, reply, option)) {
        HILOG_ERROR("fail to send simulation gesture");
    }
}

} // namespace Accessibility
} // namespace OHOS