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

#include "accessibility_element_operator_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorProxy::AccessibilityElementOperatorProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperator>(impl)
{}

AccessibilityElementOperatorProxy::~AccessibilityElementOperatorProxy()
{}

bool AccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("start");

    if (!data.WriteInterfaceToken(AccessibilityElementOperatorProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("fail, connection write parcelable elementId error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write parcelable requestId error");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write parcelable callback error");
        return;
    }

    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("fail, connection write parcelable mode error");
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::SEARCH_BY_ACCESSIBILITY_ID),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchAccessibilityInfo fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const long elementId,
    const std::string &text,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("fail, connection write parcelable elementId error");
        return;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("fail, connection write text error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId error");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::SEARCH_BY_TEXT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchElementInfosByText fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("fail, connection write elementId error");
        return;
    }

    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("fail, connection write focusType error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId error");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::FIND_FOCUSED_INFO),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("FindFocusedElementInfo fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const long elementId,
    const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("fail, connection write elementId error");
        return;
    }

    if (!data.WriteInt32(direction)) {
        HILOG_ERROR("fail, connection write focusType error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId error");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::FOCUS_FIND),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("FindFocusedElementInfo fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::ExecuteAction(const long elementId, const int action,
    const std::map<std::string, std::string> arguments, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(elementId)) {
        HILOG_ERROR("fail, connection write elementId error");
        return;
    }

    if (!data.WriteInt32(action)) {
        HILOG_ERROR("fail, connection write focusType error");
        return;
    }

    auto iter = arguments.begin();
    std::vector<std::string> argumentKey;
    std::vector<std::string> argumentValue;
    while (iter != arguments.end()) {
        argumentKey.push_back(iter->first);
        argumentValue.push_back(iter->second);
        iter++;
    }

    if (!data.WriteStringVector(argumentKey)) {
        HILOG_ERROR("fail, connection write argumentKey error");
        return;
    }

    if (!data.WriteStringVector(argumentValue)) {
        HILOG_ERROR("fail, connection write argumentValue error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId error");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(
        AccessibilityElementOperatorProxy::Message::PERFORM_ACTION),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("ExecuteAction fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    error = Remote()->SendRequest(static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::CLEAR_FOCUS),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("ExecuteAction fail, error: %d", error);
        return;
    }
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(AccessibilityElementOperatorProxy::Message::OUTSIDE_TOUCH),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("ExecuteAction fail, error: %d", error);
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS