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
#include "accessibility_interaction_operation_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibilityInteractionOperationProxy::AccessibilityInteractionOperationProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityInteractionOperation>(impl)
{}

AccessibilityInteractionOperationProxy::~AccessibilityInteractionOperationProxy()
{}

bool AccessibilityInteractionOperationProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("%{public}s" , __func__);

    if (!data.WriteInterfaceToken(AccessibilityInteractionOperationProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibilityInteractionOperationProxy::SearchElementInfoByAccessibilityId(const long elementId,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("%{public}s fail, connection write parcelable elementId error", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write parcelable requestId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write parcelable callback error", __func__);
        return;
    }

    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("%{public}s fail, connection write parcelable mode error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::SEARCH_BY_ACCESSIBILITY_ID),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchAccessibilityInfo fail, error: %d", error);
        return;
    }

}

void AccessibilityInteractionOperationProxy::SearchElementInfosByText(const long elementId,
        const std::string &text,
        const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("%{public}s fail, connection write parcelable elementId error", __func__);
        return;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("%{public}s fail, connection write text error", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write callback error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::SEARCH_BY_TEXT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchElementInfosByText fail, error: %d", error);
        return;
    }

}
void AccessibilityInteractionOperationProxy::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("%{public}s fail, connection write elementId error", __func__);
        return;
    }

    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("%{public}s fail, connection write focusType error", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write callback error", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::FIND_FOCUSED_INFO),data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("FindFocusedElementInfo fail, error: %d", error);
        return;
    }

}

void AccessibilityInteractionOperationProxy::FocusMoveSearch(const long elementId, 
    const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("%{public}s fail, connection write elementId error", __func__);
        return;
    }

    if (!data.WriteInt32(direction)) {
        HILOG_ERROR("%{public}s fail, connection write focusType error", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write callback error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::FOCUS_FIND),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("FindFocusedElementInfo fail, error: %d", error);
        return;
    }
}

void AccessibilityInteractionOperationProxy::PerformAction(const long elementId, const int action,
        const std::map<std::string, std::string> arguments, const int requestId,
        const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("%{public}s fail, connection write elementId error", __func__);
        return;
    }

    if (!data.WriteInt32(action)) {
        HILOG_ERROR("%{public}s fail, connection write focusType error", __func__);
        return;
    }

    auto iter = arguments.begin();
    std::vector<std::string> argumentKey;
    std::vector<std::string> argumentValue;
    while(iter != arguments.end()) {
        argumentKey.push_back(iter->first);
        argumentValue.push_back(iter->second);
        iter++;
    }

    if (!data.WriteStringVector(argumentKey)) {
        HILOG_ERROR("%{public}s fail, connection write argumentKey error", __func__);
        return;
    }

    if (!data.WriteStringVector(argumentValue)) {
        HILOG_ERROR("%{public}s fail, connection write argumentValue error", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId error", __func__);
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("%{public}s fail, connection write callback error", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(
        AccessibilityInteractionOperationProxy::Message::PERFORM_ACTION),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("PerformAction fail, error: %d", error);
        return;
    }

}

void AccessibilityInteractionOperationProxy::ClearFocus()
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::CLEAR_FOCUS),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("PerformAction fail, error: %d", error);
        return;
    }
}

void AccessibilityInteractionOperationProxy::OutsideTouch()
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityInteractionOperationProxy::Message::OUTSIDE_TOUCH),
     data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("PerformAction fail, error: %d", error);
        return;
    }
}

} //namespace Accessibility
} //namespace OHOS