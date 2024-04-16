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

bool AccessibilityElementOperatorProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG();
    sptr<IRemoteObject> remoteObj = Remote();
    if (!remoteObj) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t result = remoteObj->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}

void AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write parcelable element id failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }

    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }

    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("connection write parcelable mode failed");
        return;
    }

    if (!data.WriteBool(isFilter)) {
        HILOG_ERROR("connection write parcelable isFilter failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("search element info by accessibility id failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const int64_t elementId,
    const std::string &text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write parcelable elementId failed");
        return;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("connection write text failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_TEXT, data, reply, option)) {
        HILOG_ERROR("search element infos by text failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("connection write focusType failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write requestId failed");
        return;
    }

    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::FIND_FOCUSED_INFO, data, reply, option)) {
        HILOG_ERROR("find focused element info failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const int64_t elementId,
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

    if (!data.WriteInt64(elementId)) {
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

    if (!SendTransactCmd(AccessibilityInterfaceCode::FOCUS_FIND, data, reply, option)) {
        HILOG_ERROR("focus move search failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::ExecuteAction(const int64_t elementId, const int32_t action,
    const std::map<std::string, std::string> &arguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write element id failed");
        return;
    }

    if (!data.WriteInt32(action)) {
        HILOG_ERROR("connection write focus type failed");
        return;
    }

    auto iter = arguments.begin();
    std::vector<std::string> keys;
    std::vector<std::string> values;
    while (iter != arguments.end()) {
        keys.push_back(iter->first);
        values.push_back(iter->second);
        iter++;
    }

    if (!data.WriteStringVector(keys)) {
        HILOG_ERROR("connection write argument keys failed");
        return;
    }

    if (!data.WriteStringVector(values)) {
        HILOG_ERROR("connection write argument values failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::PERFORM_ACTION_ELEMENT, data, reply, option)) {
        HILOG_ERROR("execute action failed");
        return;
    }
}


void AccessibilityElementOperatorProxy::GetCursorPosition(const int64_t elementId, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write requestId failed");
        return;
    }

    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::CURSOR_POSITION, data, reply, option)) {
        HILOG_ERROR("find cursor position info failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::CLEAR_FOCUS, data, reply, option)) {
        HILOG_ERROR("clear focus failed");
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
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::OUTSIDE_TOUCH, data, reply, option)) {
        HILOG_ERROR("outside touch failed");
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS