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

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorProxy::AccessibilityElementOperatorProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperator>(impl)
{}

bool AccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();

    if (!data.WriteInterfaceToken(AccessibilityElementOperatorProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::SendTransactCmd(IAccessibilityElementOperator::Message code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
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

void AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int32_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode)
{
    HILOG_DEBUG();
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

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
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

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::SEARCH_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("SearchElementInfoByAccessibilityId failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const int32_t elementId,
    const std::string &text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
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

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::SEARCH_BY_TEXT, data, reply, option)) {
        HILOG_ERROR("SearchElementInfosByText failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const int32_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
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

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::FIND_FOCUSED_INFO, data, reply, option)) {
        HILOG_ERROR("FindFocusedElementInfo failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
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

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::FOCUS_FIND, data, reply, option)) {
        HILOG_ERROR("FocusMoveSearch failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::ExecuteAction(const int32_t elementId, const int32_t action,
    const std::map<std::string, std::string> &arguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
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

    if (!callback) {
        HILOG_ERROR("callback is nullptr.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::PERFORM_ACTION, data, reply, option)) {
        HILOG_ERROR("ExecuteAction failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::CLEAR_FOCUS, data, reply, option)) {
        HILOG_ERROR("ClearFocus failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!SendTransactCmd(AccessibilityElementOperatorProxy::Message::OUTSIDE_TOUCH, data, reply, option)) {
        HILOG_ERROR("OutsideTouch failed");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS