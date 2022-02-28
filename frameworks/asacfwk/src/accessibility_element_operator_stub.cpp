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

#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_system_ability_client.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
using AccessibilityElementOperatorCallbacks = std::map<const int, const sptr<IAccessibilityElementOperatorCallback>>;
std::map<const int, const sptr<IAccessibilityElementOperatorCallback>>
    AccessibilityElementOperatorStub::aaCallbacks_ = {};
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
    HILOG_DEBUG("start.");
    memberFuncMap_.clear();
}

int AccessibilityElementOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibilityElementOperatorStub::OnRemoteRequest,cmd = %d,flags= %d", code, option.GetFlags());
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
    HILOG_DEBUG("start");

    long elementId = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(obj);
    int mode = data.ReadInt32();
    SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");

    long elementId = data.ReadInt32();
    std::string text = data.ReadString();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(obj);
    SearchElementInfosByText(elementId, text, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");

    long elementId = data.ReadInt32();
    int focusType = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(obj);
    FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    long elementId = data.ReadInt32();
    int direction = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(obj);
    FocusMoveSearch(elementId, direction, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");
    std::vector<std::string> argumentKey;
    std::vector<std::string> argumentValue;
    long elementId = data.ReadInt32();
    int action = data.ReadInt32();
    if (!data.ReadStringVector(&argumentKey)) {
        return ERROR;
    }
    if (!data.ReadStringVector(&argumentValue)) {
        return ERROR;
    }
    if (argumentKey.size() != argumentValue.size()) {
        return ERROR;
    }
    std::map<std::string, std::string> arguments{ };
    for (unsigned int i = 0;i < argumentKey.size(); i++) {
        arguments.insert(std::pair<std::string, std::string>(argumentKey[i],argumentValue[i]));
    }
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityElementOperatorCallback> callback =
        new AccessibilityElementOperatorCallbackProxy(obj);
    ExecuteAction(elementId, action, arguments, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    ClearFocus();

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("start");

    OutsideTouch();

    return NO_ERROR;
}

void AccessibilityElementOperatorStub::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode)
{
    HILOG_DEBUG("start");

    AccessibilityElementOperatorCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_ACCESSIBILITY_ID);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityElementOperatorCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->SearchElementInfoByAccessibilityId(elementId, requestId, *tempCallback, mode);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::SearchElementInfosByText(const long elementId,
    const std::string &text,
    const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    AccessibilityElementOperatorCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_TEXT);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityElementOperatorCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->SearchElementInfosByText(elementId, text, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    AccessibilityElementOperatorCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_FIND_FOCUS);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityElementOperatorCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->FindFocusedElementInfo(elementId, focusType, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::FocusMoveSearch(const long elementId,
    const int direction, const int requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    AccessibilityElementOperatorCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_FOCUS_MOVE);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityElementOperatorCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->FocusMoveSearch(elementId, direction, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::ExecuteAction(const long elementId,
    const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    AccessibilityElementOperatorCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_PERFORM_ACTION);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityElementOperatorCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->ExecuteAction(elementId, action, actionArguments, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::ClearFocus()
{
    HILOG_DEBUG("start");
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->ClearFocus();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

void AccessibilityElementOperatorStub::OutsideTouch()
{
    HILOG_DEBUG("start");
    std::shared_ptr<AccessibilityElementOperator> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetOperatorObject(GetWindowId());
    if (obj != nullptr) {
        obj->OutsideTouch();
    } else {
        HILOG_DEBUG("Can not find interaction object");
    }
}

AccessibilityElementOperatorStub::CallbackImpl::CallbackImpl()
{

}

AccessibilityElementOperatorStub::CallbackImpl::CallbackImpl(const int requestId,
    CALL_API_NUM callNum)
{
    requestId_ = requestId;
    callNum_ = callNum;
}

void AccessibilityElementOperatorStub::CallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("start");
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetSearchElementInfoByAccessibilityIdResult(myInfos, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityElementOperatorStub::CallbackImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("start");
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetSearchElementInfoByTextResult(myInfos, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityElementOperatorStub::CallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("start");
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetFindFocusedElementInfoResult(info, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityElementOperatorStub::CallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("start");
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetFocusMoveSearchResult(info, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityElementOperatorStub::CallbackImpl::SetExecuteActionResult(
    const bool succeeded, const int requestId)
{
    HILOG_DEBUG("start");
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetExecuteActionResult(succeeded, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

AccessibilityElementOperatorCallbacks AccessibilityElementOperatorStub::CallbackImpl::GetAACallbackList()
{
    HILOG_DEBUG("start");
    return aaCallbacks_;
}

void AccessibilityElementOperatorStub::CallbackImpl::RemoveAACallbackList(int requestId)
{
    for (auto iter = aaCallbacks_.begin(); iter != aaCallbacks_.end();) {
        if (iter->first == requestId) {
            aaCallbacks_.erase(iter++);
            return;
        } else {
            iter++;
        }
    }
    HILOG_DEBUG("%{public}s Not find requestID[%{public}d]", __func__, requestId);
}

void AccessibilityElementOperatorStub::SetWindowId(int windowId)
{
    windowId_ = windowId;
}

int AccessibilityElementOperatorStub::GetWindowId()
{
    return windowId_;
}
} // namespace Accessibility
} // namespace OHOS