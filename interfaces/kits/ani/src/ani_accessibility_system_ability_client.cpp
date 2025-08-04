/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <array>
#include <iostream>
#include "ani_accessibility_system_ability_client.h"
#include "ani_accessibility_common.h"
#include "accessibility_utils_ani.h"
#include "hilog_wrapper.h"

using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityAni;

constexpr int32_t ANI_SCOPE_SIZE = 16;

std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::accessibilityStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::touchGuideStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::screenReaderStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::touchModeListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_TOUCH_MODE_CHANGED);

void StateListenerImpl::SubscribeToFramework()
{
    HILOG_INFO("SubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->SubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("UnsubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->UnsubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::OnStateChanged(const bool state)
{
    HILOG_INFO("state is %{public}d", state);
    std::string touchMode = "";
    if (type_ == AccessibilityStateEventType::EVENT_TOUCH_MODE_CHANGED) {
        for (auto &observer : observers_) {
            touchMode = state ? "singleTouchMode" : "doubleTouchMode";
            observer->OnStateChanged(touchMode);
        }
        return;
    }
 
    for (auto &observer : observers_) {
        if (observer->isBoolObserver_) {
            observer->OnStateChanged(state);
        } else if (!state) {
            // notify the touch mode change
            touchMode = "none";
            observer->OnStateChanged(touchMode);
        }
    }
}

void StateListenerImpl::SubscribeObserver(ani_env *env, ani_object observer, bool isBoolObserver)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if (ANICommon::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_)) {
            HILOG_WARN("SubscribeObserver Observer exist");
            return;
        } else {
            iter++;
        }
    }

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>(env, fnRef, isBoolObserver);
    observers_.emplace_back(stateListener);
    HILOG_INFO("observer size%{public}zu", observers_.size());
}

void StateListenerImpl::UnsubscribeObserver(ani_env *env, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if (ANICommon::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_)) {
            HILOG_INFO("UnsubscribeObserver Observer exist");
            env->GlobalReference_Delete(fnRef);
            observers_.erase(iter);
            return;
        } else {
            iter++;
        }
    }
    env->GlobalReference_Delete(fnRef);
    HILOG_WARN("UnsubscribeObserver Observer not exist");
}

void StateListenerImpl::UnsubscribeObservers()
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        (*iter)->env_->GlobalReference_Delete((*iter)->fnRef_);
    }
    observers_.clear();
}

void StateListener::NotifyETS(ani_env *env, bool state, ani_ref fnRef)
{
    HILOG_INFO("state = [%{public}s]", state ? "true" : "false");
    
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->state_ = state;
    callbackInfo->env_ = env;
    callbackInfo->fnRef_ = fnRef;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANICommon::CreateBoolObject(tmpEnv, static_cast<ani_boolean>(callbackInfo->state_));
        if (state == nullptr) {
            HILOG_ERROR("create boolean object failed");
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(state)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANICommon::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void StateListener::NotifyETS(ani_env *env, std::string mode, ani_ref fnRef)
{
    HILOG_INFO("mode = [%{public}s]", mode.c_str());
    
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->mode_ = mode;
    callbackInfo->env_ = env;
    callbackInfo->fnRef_ = fnRef;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify mode changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_string modeStr;
        tmpEnv->String_NewUTF8(callbackInfo->mode_.c_str(), callbackInfo->mode_.length(), &modeStr);
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(modeStr)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANICommon::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void StateListener::OnStateChanged(const bool state)
{
    NotifyETS(env_, state, fnRef_);
}

void StateListener::OnStateChanged(const std::string mode)
{
    NotifyETS(env_, mode, fnRef_);
}

void ANIAccessibilityClient::SubscribeState(ani_env *env, ani_string type, ani_object callback)
{
    std::string eventType = ANICommon::ANIStringToStdString(env, type);
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeListeners_->SubscribeObserver(env, callback);
        touchGuideStateListeners_->SubscribeObserver(env, callback, false);
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

void ANIAccessibilityClient::UnsubscribeState(ani_env *env, ani_string type, ani_object callback)
{
    std::string eventType = ANICommon::ANIStringToStdString(env, type);
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeListeners_->UnsubscribeObserver(env, callback);
        touchGuideStateListeners_->UnsubscribeObserver(env, callback);
    } else {
        HILOG_ERROR("UnsubscribeState eventType[%{public}s] is error", eventType.c_str());
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

void ANIAccessibilityClient::UnsubscribeStateAll(ani_env *env, ani_string type)
{
    std::string eventType = ANICommon::ANIStringToStdString(env, type);
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeListeners_->UnsubscribeObservers();
        touchGuideStateListeners_->UnsubscribeObservers();
    } else {
        HILOG_ERROR("UnsubscribeStateAll eventType[%{public}s] is error", eventType.c_str());
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

ani_boolean ANIAccessibilityClient::IsOpenTouchGuideSync([[maybe_unused]] ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsTouchExplorationEnabled(status);
    if (ret != RET_OK) {
        HILOG_ERROR("get touch guide state failed!");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_FAILED));
        return false;
    }

    return status;
}

ani_boolean ANIAccessibilityClient::IsOpenAccessibilitySync([[maybe_unused]] ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsEnabled(status);
    if (ret != RET_OK) {
        HILOG_ERROR("get accessibility state failed!");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_FAILED));
        return false;
    }

    return status;
}

void ANIAccessibilityClient::SendAccessibilityEvent(ani_env *env, ani_object eventObject)
{
    AccessibilityEventInfo eventInfo {};

    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }

    bool ret = ANICommon::ConvertEventInfoMandatoryFields(env, eventObject, eventInfo);
    if (!ret) {
        HILOG_ERROR("ConvertEventInfoMandatoryFields failed");
        ThrowBusinessError(env, QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }

    ANICommon::ConvertEventInfoStringFields(env, eventObject, eventInfo);
    ANICommon::ConvertEventInfoIntFields(env, eventObject, eventInfo);

    auto result = asaClient->SendEvent(eventInfo);
    if (result != RET_OK) {
        HILOG_ERROR("SendAccessibilityEvent failed!");
        ThrowBusinessError(env, QueryRetMsg(result));
    }
    return;
}