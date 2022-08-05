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

#include "accessibility_element_operator_stub.h"
#include "accessibility_element_operator_callback_proxy.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorStub::AccessibilityElementOperatorStub()
{
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::SEARCH_BY_ACCESSIBILITY_ID)] =
        &AccessibilityElementOperatorStub::HandleSearchElementInfoByAccessibilityId;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::SEARCH_BY_TEXT)] =
        &AccessibilityElementOperatorStub::HandleSearchElementInfosByText;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::FIND_FOCUSED_INFO)] =
        &AccessibilityElementOperatorStub::HandleFindFocusedElementInfo;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::FOCUS_FIND)] =
        &AccessibilityElementOperatorStub::HandleFocusFind;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::PERFORM_ACTION)] =
        &AccessibilityElementOperatorStub::HandleExecuteAction;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::CLEAR_FOCUS)] =
        &AccessibilityElementOperatorStub::HandleClearFocus;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityElementOperator::Message::OUTSIDE_TOUCH)] =
        &AccessibilityElementOperatorStub::HandleOutsideTouch;
}

AccessibilityElementOperatorStub::~AccessibilityElementOperatorStub()
{
    HILOG_DEBUG();
    memberFuncMap_.clear();
}

int AccessibilityElementOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}u, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibilityElementOperatorStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data, reply);
        }
    }
    HILOG_WARN("AccessibilityElementOperatorStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t elementId = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    int32_t mode = data.ReadInt32();
    SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t elementId = data.ReadInt32();
    std::string text = data.ReadString();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    SearchElementInfosByText(elementId, text, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t elementId = data.ReadInt32();
    int32_t focusType = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int32_t elementId = data.ReadInt32();
    int32_t direction = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    FocusMoveSearch(elementId, direction, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<std::string> argumentKey;
    std::vector<std::string> argumentValue;
    int32_t elementId = data.ReadInt32();
    int32_t action = data.ReadInt32();
    if (!data.ReadStringVector(&argumentKey)) {
        return ERR_TRANSACTION_FAILED;
    }
    if (!data.ReadStringVector(&argumentValue)) {
        return ERR_TRANSACTION_FAILED;
    }
    if (argumentKey.size() != argumentValue.size()) {
        return ERR_TRANSACTION_FAILED;
    }
    std::map<std::string, std::string> arguments;
    for (size_t i = 0;i < argumentKey.size(); i++) {
        arguments.insert(std::pair<std::string, std::string>(argumentKey[i], argumentValue[i]));
    }
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    ExecuteAction(elementId, action, arguments, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    ClearFocus();

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    OutsideTouch();

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS