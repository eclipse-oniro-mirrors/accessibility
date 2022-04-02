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
#include "parcel.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelStub::AccessibleAbilityChannelStub()
{
    HILOG_DEBUG("start.");

    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID)] =
        &AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEARCH_ELEMENTINFOS_BY_TEXT)] =
        &AccessibleAbilityChannelStub::HandleSearchElementInfosByText;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FIND_FOCUSED_ELEMENTINFO)] =
        &AccessibleAbilityChannelStub::HandleFindFocusedElementInfo;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::FOCUS_MOVE_SEARCH)] =
        &AccessibleAbilityChannelStub::HandleFocusMoveSearch;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::PERFORM_ACTION)] =
        &AccessibleAbilityChannelStub::HandleExecuteAction;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_WINDOWS)] =
        &AccessibleAbilityChannelStub::HandleGetWindows;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::EXECUTE_COMMON_ACTION)] =
        &AccessibleAbilityChannelStub::HandleExecuteCommonAction;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_ON_KEY_PRESS_EVENT_RESULT)] =
        &AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_SCALE)] =
        &AccessibleAbilityChannelStub::HandleGetDisplayResizeScale;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPALYRESIZE_CETER_X)] =
        &AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterX;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_CETER_Y)] =
        &AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterY;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::GET_DISPLAYRESIZE_RECT)] =
        &AccessibleAbilityChannelStub::HandleGetDisplayResizeRect;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::RESET_DISPALYRESIZE)] =
        &AccessibleAbilityChannelStub::HandleResetDisplayResize;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SET_DISPLAYRESIZE_SCALE_AND_CENTER)] =
        &AccessibleAbilityChannelStub::HandleSetDisplayResizeScaleAndCenter;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityChannel::Message::SEND_SIMULATE_GESTURE)] =
        &AccessibleAbilityChannelStub::HandleSendSimulateGesture;
}

AccessibleAbilityChannelStub::~AccessibleAbilityChannelStub()
{
    HILOG_DEBUG("start.");
    memberFuncMap_.clear();
}

int32_t AccessibleAbilityChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityChannelStub::OnRemoteRequest, cmd = %{public}d, flags= %{public}d",
        code, option.GetFlags());
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
    HILOG_DEBUG("start.");

    int accessibilityWindowId = data.ReadInt32();
    int elementId = data.ReadInt64();
    int requestId = data.ReadInt32();

    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (!object) {
        HILOG_ERROR("object is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(object);
    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    int mode = data.ReadInt32();
    bool result = SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId, callback, mode);
    HILOG_DEBUG("SearchElementInfoByAccessibilityId ret = %{public}d", result);
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int accessibilityWindowId = data.ReadInt32();
    long elementId = data.ReadInt32();
    string text = data.ReadString();
    int requestId = data.ReadInt32();

    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(object);

    bool result = SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, callback);
    HILOG_DEBUG("SearchElementInfosByText ret = %{public}d", result);
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int accessibilityWindowId = data.ReadInt32();
    long elementId = data.ReadInt32();
    int focusType = data.ReadInt32();
    int requestId = data.ReadInt32();

    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(object);

    bool result = FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, callback);
    HILOG_DEBUG("FindFocusedElementInfo ret = %{public}d", result);
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFocusMoveSearch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int accessibilityWindowId = data.ReadInt32();
    long elementId = data.ReadInt32();
    int direction = data.ReadInt32();
    int requestId = data.ReadInt32();

    sptr<IRemoteObject> object = data.ReadRemoteObject();
    sptr<IAccessibilityElementOperatorCallback> callback =
        iface_cast<IAccessibilityElementOperatorCallback>(object);

    bool result = FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, callback);
    HILOG_DEBUG("FocusMoveSearch ret = %{public}d", result);
    reply.WriteBool(result);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int accessibilityWindowId = data.ReadInt32();
    long elementId = data.ReadInt32();
    int action = data.ReadInt32();

    vector<string> actionArgumentsKey;
    vector<string> actionArgumentsValue;
    map<string, string> actionArguments;

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
    for (unsigned int i = 0; i < actionArgumentsKey.size(); i++) {
        actionArguments.insert(make_pair(actionArgumentsKey[i], actionArgumentsValue[i]));
    }

    int requestId = data.ReadInt32();

    auto callback = iface_cast<IAccessibilityElementOperatorCallback>(data.ReadRemoteObject());
    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    bool result = ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, callback);
    HILOG_DEBUG("ExecuteAction ret = %{public}d", result);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindows(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    vector<AccessibilityWindowInfo> windows = GetWindows();
    if (!reply.WriteInt32((int32_t)windows.size())) {
        HILOG_ERROR("windows.size() write error: %{public}zu, ", windows.size());
        return ERR_INVALID_VALUE;
    }
    for (auto &window : windows) {
        if (!reply.WriteParcelable(&window)) {
            HILOG_ERROR("write windows failed");
            return ERR_INVALID_VALUE;
        }
    }
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteCommonAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int action = data.ReadInt32();
    bool result = ExecuteCommonAction(action);

    HILOG_DEBUG("ExecuteCommonAction ret = %{public}d", result);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    bool handled = data.ReadBool();
    int sequence = data.ReadInt32();
    SetOnKeyPressEventResult(handled, sequence);

    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeScale(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    float result = GetDisplayResizeScale(displayId);

    HILOG_DEBUG("GetDisplayResizeScale ret = %{public}f", result);
    reply.WriteFloat(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterX(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    float result = GetDisplayResizeCenterX(displayId);

    HILOG_DEBUG("GetDisplayResizeCenterX ret = %{public}f", result);
    reply.WriteFloat(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterY(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    float result = GetDisplayResizeCenterY(displayId);

    HILOG_DEBUG("GetDisplayResizeCenterY ret = %{public}f", result);
    reply.WriteFloat(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeRect(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    Rect rect = GetDisplayResizeRect(displayId);

    HILOG_DEBUG("GetDisplayResizeRect");
    reply.WriteParcelable(&rect);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleResetDisplayResize(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    bool animate = data.ReadBool();
    bool result = ResetDisplayResize(displayId, animate);

    HILOG_DEBUG("ResetDisplayResize ret = %{public}d", result);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetDisplayResizeScaleAndCenter(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int displayId = data.ReadInt32();
    float scale = data.ReadFloat();
    float centerX = data.ReadFloat();
    float centerY = data.ReadFloat();
    bool animate = data.ReadBool();
    bool result = SetDisplayResizeScaleAndCenter(displayId, scale, centerX, centerY, animate);

    HILOG_DEBUG("SetDisplayResizeScaleAndCenter ret = %{public}d", result);
    reply.WriteBool(result);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSendSimulateGesture(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start.");

    int32_t requestId = data.ReadInt32();

    vector<GesturePathDefine> gestureSteps;
    int32_t stepSize = data.ReadInt32();
    if (!stepSize) {
        HILOG_ERROR("stepSize is 0");
        return ERROR;
    }
    for (int32_t i = 0; i < stepSize; i++) {
        std::shared_ptr<GesturePathDefine> gestureStep(data.ReadParcelable<GesturePathDefine>());
        if (!gestureStep) {
            HILOG_ERROR("ReadParcelable<GesturePathDefine> failed");
            return ERR_INVALID_VALUE;
        }
        gestureSteps.emplace_back(*gestureStep);
    }
    SendSimulateGesture(requestId, gestureSteps);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS
