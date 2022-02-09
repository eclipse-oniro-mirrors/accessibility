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
#include "accessibility_interaction_operation_callback_proxy.h"
#include "accessibility_interaction_operation_callback_stub.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_system_ability_client.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>>
    AccessibilityInteractionOperationStub::aaCallbacks_ = {};
AccessibilityInteractionOperationStub::AccessibilityInteractionOperationStub()
{
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::SEARCH_BY_ACCESSIBILITY_ID)] =
        &AccessibilityInteractionOperationStub::HandleSearchElementInfoByAccessibilityId;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::SEARCH_BY_TEXT)] =
        &AccessibilityInteractionOperationStub::HandleSearchElementInfosByText;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::FIND_FOCUSED_INFO)] =
        &AccessibilityInteractionOperationStub::HandleFindFocusedElementInfo;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::FOCUS_FIND)] =
        &AccessibilityInteractionOperationStub::HandleFocusFind;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::PERFORM_ACTION)] =
        &AccessibilityInteractionOperationStub::HandlePerformAction;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::CLEAR_FOCUS)] =
        &AccessibilityInteractionOperationStub::HandleClearFocus;
    memberFuncMap_[static_cast<uint32_t>(IAccessibilityInteractionOperation::Message::OUTSIDE_TOUCH)] =
        &AccessibilityInteractionOperationStub::HandleOutsideTouch;
}

AccessibilityInteractionOperationStub::~AccessibilityInteractionOperationStub()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int AccessibilityInteractionOperationStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
 MessageParcel &reply, MessageOption &option)
 {
    HILOG_DEBUG("AccessibilityInteractionOperationStub::OnRemoteRequest,cmd = %d,flags= %d", code, option.GetFlags());
    std::u16string descriptor = AccessibilityInteractionOperationStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();

    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto memFunc = memberFuncMap_.find(code);
    if (memFunc != memberFuncMap_.end()) {
        auto func = memFunc->second;
        if (func != nullptr) {
            return (this->*func)(data,reply);
        }
    }
    HILOG_WARN("AccessibilityInteractionOperationStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityInteractionOperationStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    long elementId = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityInteractionOperationCallback> callback =
        new AccessibilityInteractionOperationCallbackProxy(obj);
    int mode = data.ReadInt32();
    SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    long elementId = data.ReadInt32();
    std::string text = data.ReadString();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityInteractionOperationCallback> callback =
        new AccessibilityInteractionOperationCallbackProxy(obj);
    SearchElementInfosByText(elementId, text, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    long elementId = data.ReadInt32();
    int focusType = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityInteractionOperationCallback> callback =
        new AccessibilityInteractionOperationCallbackProxy(obj);
    FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    long elementId = data.ReadInt32();
    int direction = data.ReadInt32();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityInteractionOperationCallback> callback =
        new AccessibilityInteractionOperationCallbackProxy(obj);
    FocusMoveSearch(elementId, direction, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandlePerformAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);
    std::vector<std::string> argumentKey{};
    std::vector<std::string> argumentValue{};
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
    std::map<std::string, std::string> arguments{};
    for (unsigned int i = 0;i < argumentKey.size(); i++) {
        arguments.insert(std::pair<std::string, std::string>(argumentKey[i],argumentValue[i]));
    }
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> obj = data.ReadRemoteObject();

    sptr<IAccessibilityInteractionOperationCallback> callback =
        new AccessibilityInteractionOperationCallbackProxy(obj);
    PerformAction(elementId, action, arguments, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    ClearFocus();

    return NO_ERROR;
}

ErrCode AccessibilityInteractionOperationStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s" , __func__);

    OutsideTouch();

    return NO_ERROR;
}

void AccessibilityInteractionOperationStub::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback, const int mode)
{
    HILOG_DEBUG("%{public}s", __func__);

    AccessibilityInteractionOperationCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_ACCESSIBILITY_ID);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityInteractionOperationCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->SearchElementInfoByAccessibilityId(elementId, requestId, *tempCallback, mode);
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::SearchElementInfosByText(const long elementId,
    const std::string &text,
    const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s", __func__);

    AccessibilityInteractionOperationCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_TEXT);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityInteractionOperationCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->SearchElementInfosByText(elementId, text, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::FindFocusedElementInfo(const long elementId,
    const int focusType, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s", __func__);

    AccessibilityInteractionOperationCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_FIND_FOCUS);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityInteractionOperationCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->FindFocusedElementInfo(elementId, focusType, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::FocusMoveSearch(const long elementId,
    const int direction, const int requestId,
    const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s", __func__);

    AccessibilityInteractionOperationCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_BY_FOCUS_MOVE);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityInteractionOperationCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->FocusMoveSearch(elementId, direction, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::PerformAction(const long elementId,
    const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback)
{
    HILOG_DEBUG("%{public}s", __func__);

    AccessibilityInteractionOperationCallback *tempCallback = new CallbackImpl(requestId,
        CallbackImpl::CALLBACK_PERFORM_ACTION);
    aaCallbacks_.insert(
        std::pair<const int, const sptr<IAccessibilityInteractionOperationCallback>>(requestId, callback));
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->PerformAction(elementId, action, actionArguments, requestId, *tempCallback);
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::ClearFocus()
{
    HILOG_DEBUG("%{public}s", __func__);
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->ClearFocus();
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

void AccessibilityInteractionOperationStub::OutsideTouch()
{
    HILOG_DEBUG("%{public}s", __func__);
    std::shared_ptr<AccessibilityInteractionOperation> obj =
        AccessibilitySystemAbilityClient::GetInstance()->GetInteractionObject(GetWindowId());
    if (obj != nullptr) {
        obj->OutsideTouch();
    } else {
        HILOG_DEBUG("%{public}s Can not find interaction object", __func__);
    }
}

AccessibilityInteractionOperationStub::CallbackImpl::CallbackImpl()
{

}

AccessibilityInteractionOperationStub::CallbackImpl::CallbackImpl(const int requestId,
    CALL_API_NUM callNum)
{
    requestId_ = requestId;
    callNum_ = callNum;
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("%{public}s", __func__);
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetSearchElementInfoByAccessibilityIdResult(myInfos, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetSearchElementInfoByTextResult(
    const std::list<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("%{public}s", __func__);
    std::vector<AccessibilityElementInfo> myInfos = TranslateListToVector(infos);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetSearchElementInfoByTextResult(myInfos, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("%{public}s", __func__);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetFindFocusedElementInfoResult(info, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("%{public}s", __func__);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetFocusMoveSearchResult(info, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

void AccessibilityInteractionOperationStub::CallbackImpl::SetPerformActionResult(
    const bool succeeded, const int requestId)
{
    HILOG_DEBUG("%{public}s", __func__);
    auto callback = GetAACallbackList().find(requestId);
    if (callback != GetAACallbackList().end() && callback->second != nullptr) {
        callback->second->SetPerformActionResult(succeeded, requestId);
    } else {
        HILOG_DEBUG("%s Can't find the callback [requestId:%d]", __func__, requestId);
    }
    RemoveAACallbackList(requestId);
}

std::map<const int, const sptr<IAccessibilityInteractionOperationCallback>>
    AccessibilityInteractionOperationStub::CallbackImpl::GetAACallbackList()
{
    HILOG_DEBUG("%{public}s", __func__);
    return aaCallbacks_;
}

void AccessibilityInteractionOperationStub::CallbackImpl::RemoveAACallbackList(int requestId)
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

void AccessibilityInteractionOperationStub::SetWindowId(int windowId)
{
    windowId_ = windowId;
}

int AccessibilityInteractionOperationStub::GetWindowId()
{
    return windowId_;
}
} //namespace Accessibility
} //namespace OHOS