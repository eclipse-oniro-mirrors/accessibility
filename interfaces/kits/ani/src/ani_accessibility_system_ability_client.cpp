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
#include "accessibility_def.h"
#include "ani_accessibility_system_ability_client.h"
#include "ani_utils.h"
#include "hilog_wrapper.h"
#include <ani_signature_builder.h>

using namespace OHOS::Accessibility;
using namespace arkts::ani_signature;

constexpr int32_t ANI_SCOPE_SIZE = 16;
const std::string DEFAULT_FONT_FAMILY = "default";
constexpr uint32_t DEFAULT_FONT_SCALE = 75;
constexpr uint32_t DEFAULT_COLOR = 0xff000000;
const std::string DEFAULT_FONT_EDGE_TYPE = "none";

std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::accessibilityStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_ACCESSIBILITY_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::touchGuideStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_TOUCH_GUIDE_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::screenReaderStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_SCREEN_READER_STATE_CHANGED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::touchModeStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_TOUCH_MODE_CHANGED);
std::shared_ptr<AccessibilityCaptionsObserverImpl> ANIAccessibilityClient::captionListeners_ =
    std::make_shared<AccessibilityCaptionsObserverImpl>();
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::audioMonoStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_AUDIO_MONO);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::animationOffStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_ANIMATION_OFF);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::flashReminderSwitchStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_FLASH_REMINDER_SWITCH);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::seniorModeStateListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_ELDER_CARE_ENABLED);
std::shared_ptr<StateListenerImpl> ANIAccessibilityClient::seniorModeStateForAppListeners_ =
    std::make_shared<StateListenerImpl>(AccessibilityStateEventType::EVENT_SELF_SENIOR_MODE_STATE_CHANGE);
std::shared_ptr<FocusChangeListenerImpl> ANIAccessibilityClient::focusChangeListeners_ =
    std::make_shared<FocusChangeListenerImpl>();

void StateListenerImpl::SubscribeToFramework()
{
    HILOG_INFO("StateListenerImpl SubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->SubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("StateListenerImpl UnsubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->UnsubscribeStateObserver(shared_from_this(), type_);
    }
}

void StateListenerImpl::OnStateChanged(const bool state)
{
    HILOG_INFO("state is %{public}d, type = %{public}d", state, static_cast<int32_t>(type_));
    std::lock_guard<ffrt::mutex> lock(mutex_);
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

void AccessibilityCaptionsObserverImpl::SubscribeToFramework()
{
    HILOG_INFO("AccessibilityCaptionsObserverImpl SubscribeFromFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SubscribeConfigObserver(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE, shared_from_this(),
        false);
    instance.SubscribeConfigObserver(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE, shared_from_this(),
        false);
}

void AccessibilityCaptionsObserverImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("AccessibilityCaptionsObserverImpl UnsubscribeFromFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.UnsubscribeConfigObserver(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE,
        shared_from_this());
    instance.UnsubscribeConfigObserver(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE,
        shared_from_this());
}

void AccessibilityCaptionsObserverImpl::OnConfigChanged(const OHOS::AccessibilityConfig::CONFIG_ID id,
    const OHOS::AccessibilityConfig::ConfigValue& value)
{
    HILOG_INFO("captions OnConfigChanged, config id is %{public}d", static_cast<int32_t>(id));
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        if (observer->configId_ == id) {
            observer->OnConfigChanged(value);
        }
    }
}

void StateListenerImpl::SubscribeObserver(ani_env *env, ani_object observer, bool isBoolObserver)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_)) {
            env->GlobalReference_Delete(fnRef);
            HILOG_WARN("SubscribeObserver Observer exist");
            return;
        }
    }

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>(env, fnRef, isBoolObserver);
    observers_.emplace_back(stateListener);
    HILOG_INFO("observer size:%{public}zu", observers_.size());
}

void StateListenerImpl::UnsubscribeObserver(ani_env *env, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_)) {
            observers_.erase(iter);
            break;
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

void AccessibilityCaptionsObserverImpl::SubscribeObserver(ani_env *env, OHOS::AccessibilityConfig::CONFIG_ID id,
    ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (((*iter)->configId_ == id) && (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_))) {
            env->GlobalReference_Delete(fnRef);
            HILOG_WARN("SubscribeObserver Observer exist");
            return;
        }
    }

    std::shared_ptr<AccessibilityCaptionsObserver> captionsObserver = std::make_shared<AccessibilityCaptionsObserver>(
        env, fnRef, id);
    observers_.emplace_back(captionsObserver);
    HILOG_INFO("observer size:%{public}zu", observers_.size());
}

void AccessibilityCaptionsObserverImpl::UnsubscribeObserver(ani_env *env, OHOS::AccessibilityConfig::CONFIG_ID id,
    ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (((*iter)->configId_ == id) && (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->fnRef_))) {
            observers_.erase(iter);
            break;
        }
    }
    env->GlobalReference_Delete(fnRef);
    HILOG_WARN("UnsubscribeObserver Observer not exist");
}

void AccessibilityCaptionsObserverImpl::UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id)
{
    HILOG_INFO();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if ((*iter)->configId_ == id) {
            (*iter)->env_->GlobalReference_Delete((*iter)->fnRef_);
            iter = observers_.erase(iter);
        } else {
            iter++;
        }
    }
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
        ani_object state = ANIUtils::CreateBoolObject(tmpEnv, static_cast<ani_boolean>(callbackInfo->state_));
        if (state == nullptr) {
            HILOG_ERROR("create boolean object failed");
            tmpEnv->DestroyLocalScope();
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(state)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
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
    callbackInfo->stringValue_ = mode;
    callbackInfo->env_ = env;
    callbackInfo->fnRef_ = fnRef;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify mode changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_string state = ANIUtils::CreateAniString(tmpEnv, callbackInfo->stringValue_);
        if (state == nullptr) {
            HILOG_ERROR("create boolean object failed");
            tmpEnv->DestroyLocalScope();
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(state)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void AccessibilityCaptionsObserver::NotifyCaptionsStateToETS(ani_env *env, bool state, ani_ref fnRef)
{
    HILOG_INFO("captions state = [%{public}s]", state ? "true" : "false");
    
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->state_ = state;
    callbackInfo->env_ = env;
    callbackInfo->fnRef_ = fnRef;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify captions state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANIUtils::CreateBoolObject(tmpEnv, static_cast<ani_boolean>(callbackInfo->state_));
        if (state == nullptr) {
            HILOG_ERROR("create boolean object failed");
            tmpEnv->DestroyLocalScope();
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(state)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void AccessibilityCaptionsObserver::NotifyCaptionsStyleToETS(ani_env *env,
    OHOS::AccessibilityConfig::CaptionProperty style, ani_ref fnRef)
{
    HILOG_INFO("notify captions style");
    
    std::shared_ptr<ANICaptionCallbackInfo> callbackInfo = std::make_shared<ANICaptionCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->caption_ = style;
    callbackInfo->env_ = env;
    callbackInfo->fnRef_ = fnRef;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify captions style changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object caption = ANIAccessibilityClient::CreateAccessibilityCaptionProperty(tmpEnv, callbackInfo->caption_);
        if (caption == nullptr) {
            HILOG_ERROR("create caption style object failed");
            tmpEnv->DestroyLocalScope();
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(caption)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
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

void AccessibilityCaptionsObserver::OnConfigChanged(const OHOS::AccessibilityConfig::ConfigValue& value)
{
    if (configId_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE) {
        NotifyCaptionsStateToETS(env_, value.captionState, fnRef_);
    } else if (configId_ == OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE) {
        NotifyCaptionsStyleToETS(env_, value.captionStyle, fnRef_);
    } else {
        HILOG_ERROR("configId_ is invalid");
        return;
    }
}

void ANIAccessibilityClient::SubscribeState(ani_env *env, ani_string type, ani_object callback)
{
    std::string eventType = ANIUtils::ANIStringToStdString(env, type);
    HILOG_DEBUG("SubscribeState:%{public}s", eventType.c_str());
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeStateListeners_->SubscribeObserver(env, callback);
        touchGuideStateListeners_->SubscribeObserver(env, callback, false);
    } else if (std::strcmp(eventType.c_str(), "audioMonoStateChange") == 0) {
        audioMonoStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "animationReduceStateChange") == 0) {
        animationOffStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "flashReminderStateChange") == 0) {
        flashReminderSwitchStateListeners_->SubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "seniorModeStateChange") == 0) {
        seniorModeStateListeners_->SubscribeObserver(env, callback);
    } else {
        HILOG_ERROR("SubscribeState eventType[%{public}s] is error", eventType.c_str());
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

void ANIAccessibilityClient::UnsubscribeState(ani_env *env, ani_string type, ani_object callback)
{
    std::string eventType = ANIUtils::ANIStringToStdString(env, type);
    HILOG_DEBUG("UnsubscribeState:%{public}s", eventType.c_str());
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeStateListeners_->UnsubscribeObserver(env, callback);
        touchGuideStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "audioMonoStateChange") == 0) {
        audioMonoStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "animationReduceStateChange") == 0) {
        animationOffStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "flashReminderStateChange") == 0) {
        flashReminderSwitchStateListeners_->UnsubscribeObserver(env, callback);
    } else if (std::strcmp(eventType.c_str(), "seniorModeStateChange") == 0) {
        seniorModeStateListeners_->UnsubscribeObserver(env, callback);
    } else {
        HILOG_ERROR("UnsubscribeState eventType[%{public}s] is error", eventType.c_str());
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

void ANIAccessibilityClient::UnsubscribeStateAll(ani_env *env, ani_string type)
{
    std::string eventType = ANIUtils::ANIStringToStdString(env, type);
    HILOG_DEBUG("UnsubscribeStateAll:%{public}s", eventType.c_str());
    if (std::strcmp(eventType.c_str(), "accessibilityStateChange") == 0) {
        accessibilityStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "touchGuideStateChange") == 0) {
        touchGuideStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "screenReaderStateChange") == 0) {
        screenReaderStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "touchModeChange") == 0) {
        touchModeStateListeners_->UnsubscribeObservers();
        touchGuideStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "audioMonoStateChange") == 0) {
        audioMonoStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "animationReduceStateChange") == 0) {
        animationOffStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "flashReminderStateChange") == 0) {
        flashReminderSwitchStateListeners_->UnsubscribeObservers();
    } else if (std::strcmp(eventType.c_str(), "seniorModeStateChange") == 0) {
        seniorModeStateListeners_->UnsubscribeObservers();
    } else {
        HILOG_ERROR("UnsubscribeStateAll eventType[%{public}s] is error", eventType.c_str());
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
    }
}

void ANIAccessibilityClient::SubscribeEnableChange(ani_env *env, ani_object object, ani_object callback)
{
    HILOG_INFO();
    captionListeners_->SubscribeObserver(env, OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE, callback);
}

void ANIAccessibilityClient::SubscribeStyleChange(ani_env *env, ani_object object, ani_object callback)
{
    HILOG_INFO();
    captionListeners_->SubscribeObserver(env, OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE, callback);
}

void ANIAccessibilityClient::UnsubscribeEnableChangeWithCallback(ani_env *env, ani_object object, ani_object callback)
{
    HILOG_INFO();
    captionListeners_->UnsubscribeObserver(env, OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE, callback);
}

void ANIAccessibilityClient::UnsubscribeStyleChangeWithCallback(ani_env *env, ani_object object, ani_object callback)
{
    HILOG_INFO();
    captionListeners_->UnsubscribeObserver(env, OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE, callback);
}

void ANIAccessibilityClient::UnsubscribeEnableChangeAll(ani_env *env, ani_object object)
{
    HILOG_INFO();
    captionListeners_->UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STATE);
}

void ANIAccessibilityClient::UnsubscribeStyleChangeAll(ani_env *env, ani_object object)
{
    HILOG_INFO();
    captionListeners_->UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_CAPTION_STYLE);
}

ani_boolean ANIAccessibilityClient::IsOpenTouchGuideSync([[maybe_unused]] ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsTouchExplorationEnabled(status);
    if (ret != RET_OK) {
        HILOG_ERROR("get touch guide state failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }

    return status;
}

ani_boolean ANIAccessibilityClient::IsOpenAccessibilitySync([[maybe_unused]] ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsEnabled(status);
    if (ret != RET_OK) {
        HILOG_ERROR("get accessibility state failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }

    return status;
}

ani_boolean ANIAccessibilityClient::IsScreenReaderOpenSync([[maybe_unused]] ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsScreenReaderEnabled(status);
    if (ret != RET_OK) {
        HILOG_ERROR("get screen reader state failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }

    return status;
}

ani_string ANIAccessibilityClient::getTouchModeSync([[maybe_unused]] ani_env *env)
{
    ani_string retResult = nullptr;
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return nullptr;
    }
    std::string touchMode = "";
    asaClient->GetTouchMode(touchMode);
    env->String_NewUTF8(touchMode.c_str(), touchMode.size(), &retResult);
    return retResult;
}

ani_object ANIAccessibilityClient::CreateJsOtherInfoInner(ani_env *env, ani_class cls,
    ani_object &object, AccessibilityAbilityInfo &info)
{
    if (!ANIUtils::SetStringProperty(env, object, ABILITY_INFO_DESCRIPTION, info.GetDescription())) {
        HILOG_ERROR("set description failed");
        return nullptr;
    }
    uint32_t eventTypesValue = info.GetEventTypes();
    std::vector<std::string> eventTypes = ParseEventTypesToVec(eventTypesValue);
    if (eventTypes.size() > 0) {
        ANIUtils::SetStringArrayProperty(env, object, ABILITY_INFO_EVENT_TYPES, eventTypes);
    }
    ani_status status;
    if ((status = env->Object_SetPropertyByName_Boolean(object, ABILITY_INFO_NEED_HIDE, info.NeedHide())) != ANI_OK) {
        HILOG_ERROR("set needHide failed status=%{public}d", status);
        return nullptr;
    }

    if (!ANIUtils::SetStringProperty(env, object, ABILITY_INFO_LABEL, info.GetLabel())) {
        HILOG_ERROR("set label failed");
        return nullptr;
    }
    return object;
}

ani_object ANIAccessibilityClient::CreateJsAccessibilityAbilityInfoInner(ani_env *env, ani_class cls,
    ani_object &object, AccessibilityAbilityInfo &info)
{
    if (env == nullptr || cls == nullptr || object == nullptr) {
        HILOG_ERROR("invalid args");
        return nullptr;
    }

    if (!ANIUtils::SetStringProperty(env, object, ABILITY_INFO_ID, info.GetId())) {
        HILOG_ERROR("set id failed id");
        return nullptr;
    }
    if (!ANIUtils::SetStringProperty(env, object, ABILITY_INFO_NAME, info.GetName())) {
        HILOG_ERROR("set name failed");
        return nullptr;
    }

    if (!ANIUtils::SetStringProperty(env, object, ABILITY_INFO_BUNDLE_NAME, info.GetPackageName())) {
        HILOG_ERROR("set bundleName failed");
        return nullptr;
    }
    std::vector<std::string> filterNames = info.GetFilterBundleNames();
    if (filterNames.size() > 0) {
        ANIUtils::SetStringArrayProperty(env, object, ABILITY_INFO_TARGET_BUNDLE_NAMES, filterNames);
    }
    std::vector<std::string> abilityTypes = ParseAbilityTypesToVec(info.GetAccessibilityAbilityType());
    if (abilityTypes.size() > 0) {
        ANIUtils::SetStringArrayProperty(env, object, ABILITY_INFO_ABILITY_TYPES, abilityTypes);
    }
    std::vector<std::string> capabilities = ParseCapabilitiesToVec(info.GetStaticCapabilityValues());
    if (capabilities.size() > 0) {
        ANIUtils::SetStringArrayProperty(env, object, ABILITY_INFO_CAPABILITIES, capabilities);
    }
    return CreateJsOtherInfoInner(env, cls, object, info);
}

ani_object ANIAccessibilityClient::CreateJsAccessibilityAbilityInfo(ani_env *env,
    AccessibilityAbilityInfo &info)
{
    HILOG_DEBUG();
    ani_class cls = nullptr;
    ani_status status = ANI_ERROR;
    ani_method ctor = nullptr;
    ani_object object = nullptr;

    if ((status = env->FindClass(Builder::BuildClass("@ohos.accessibility.accessibility.AccessibilityAbilityInfoImpl")
        .Descriptor().c_str(), &cls)) != ANI_OK || cls == nullptr) {
        HILOG_ERROR("FindClass status : %{public}d or null cls", status);
        return nullptr;
    }
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    if ((status = env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor)) != ANI_OK
        || ctor == nullptr) {
        HILOG_ERROR("Class_FindMethod Constructor fail : %{public}d or null ctor", status);
        return nullptr;
    }
    if ((status = env->Object_New(cls, ctor, &object)) != ANI_OK || object == nullptr) {
        HILOG_ERROR("Object_New status : %{public}d or null cls", status);
        return nullptr;
    }

    return CreateJsAccessibilityAbilityInfoInner(env, cls, object, info);
}

ani_object ANIAccessibilityClient::ConvertAccessibleAbilityInfosToJs(ani_env *env,
    std::vector<AccessibilityAbilityInfo> &accessibleAbilityInfos)
{
    if (accessibleAbilityInfos.empty()) {
        HILOG_ERROR("convert accessible ability infos accessibleAbilityInfos empty");
        return nullptr;
    }
    ani_ref undefinedRef = nullptr;
    if (env->GetUndefined(&undefinedRef) != ANI_OK) {
        HILOG_ERROR("GetUndefined Failed.");
    }
    ani_array aniArray;
    env->Array_New(accessibleAbilityInfos.size(), undefinedRef, &aniArray);
    if (aniArray == nullptr) {
        HILOG_ERROR("ConvertAccessibleAbilityInfosToJs array is null");
        return nullptr;
    }
    ani_size index = 0;
    for (auto &abilityInfo : accessibleAbilityInfos) {
        ani_ref ani_info = CreateJsAccessibilityAbilityInfo(env, abilityInfo);
        if (ani_info == nullptr) {
            HILOG_ERROR("ConvertAccessibleAbilityInfosToJs obj is null");
            return nullptr;
        }
        auto status = env->Array_Set(aniArray, index, ani_info);
        if (status != ANI_OK) {
            HILOG_ERROR("Object_CallMethodByName_Void failed  --%{public}d ", status);
            return nullptr;
        }
        index++;
    }
    return aniArray;
}

ani_object ANIAccessibilityClient::GetAccessibilityExtensionListSync(ani_env *env, ani_string abilityType,
    ani_string stateType)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return nullptr;
    }
    std::string abilityTypeStr = ANIUtils::ANIStringToStdString(env, abilityType);
    std::string stateTypeStr = ANIUtils::ANIStringToStdString(env, stateType);

    RetError errCode = RET_OK;
    AbilityStateType stateTypes = ABILITY_STATE_INVALID;
    uint32_t abilityTypes = 0;

    HILOG_INFO("abilityTypeStr = %{public}s", abilityTypeStr.c_str());
    if (CheckAbilityType(abilityTypeStr)) {
        abilityTypes = ConvertStringToAccessibilityAbilityTypes(abilityTypeStr);
    } else {
        errCode = RET_ERR_INVALID_PARAM;
    }

    // parse ability state
    if (CheckStateType(stateTypeStr)) {
        stateTypes = ConvertStringToAbilityStateType(stateTypeStr);
    } else {
        errCode = RET_ERR_INVALID_PARAM;
    }

    std::vector<AccessibilityAbilityInfo> accessibilityAbilityList{};
    if (errCode == RET_OK) {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (asaClient) {
            errCode = asaClient->GetAbilityList(abilityTypes, stateTypes, accessibilityAbilityList);
        }
    }

    if (errCode != RET_OK) {
        HILOG_ERROR("get accessibility ability list failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return nullptr;
    }

    ani_object aniArray = ConvertAccessibleAbilityInfosToJs(env, accessibilityAbilityList);
    if (aniArray == nullptr) {
        HILOG_WARN("get accessibility extension list convert null aniArray");
    }
    return aniArray;
}

ani_object ANIAccessibilityClient::GetCaptionsManager(ani_env *env)
{
    HILOG_INFO();
    ani_class cls = nullptr;
    ani_status status = ANI_ERROR;
    ani_method ctor = nullptr;
    ani_object object = nullptr;

    if ((env->FindClass(Builder::BuildClass("@ohos.accessibility.accessibility.CaptionsManagerImpl")
        .Descriptor().c_str(), &cls)) != ANI_OK || cls == nullptr) {
        HILOG_ERROR("FindClass CaptionsManagerImpl failed");
        return nullptr;
    }
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    if ((env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor)) != ANI_OK ||
        ctor == nullptr) {
        HILOG_ERROR("Class CaptionsManagerImpl find constructor failed");
        return nullptr;
    }
    if ((env->Object_New(cls, ctor, &object)) != ANI_OK || object == nullptr) {
        HILOG_ERROR("create CaptionsManagerImpl failed");
        return nullptr;
    }
    return object;
}

ani_boolean ANIAccessibilityClient::GetAudioMonoStateSync(ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->GetAudioMonoState(status);
    if (ret != RET_OK) {
        HILOG_ERROR("GetAudioMonoState failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }
    return status;
}

ani_boolean ANIAccessibilityClient::GetAnimationOffStateSync(ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->GetAnimationOffState(status);
    if (ret != RET_OK) {
        HILOG_ERROR("GetAnimationOffState failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }
    return status;
}

ani_boolean ANIAccessibilityClient::GetFlashReminderSwitchSync(ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->GetFlashReminderSwitch(status);
    if (ret != RET_OK) {
        HILOG_ERROR("GetFlashReminderSwitch failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }
    return status;
}

ani_boolean ANIAccessibilityClient::GetSeniorModeStateSync(ani_env *env)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool status = false;
    auto ret = asaClient->GetSeniorModeState(status);
    if (ret != RET_OK) {
        HILOG_ERROR("GetSeniorModeState failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }
    return status;
}

ani_boolean ANIAccessibilityClient::GetEnabled(ani_env *env, ani_object object)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = instance.GetCaptionsState(state, false);
    if (ret != RET_OK) {
        HILOG_ERROR("GetEnabled failed, ret = %{public}d", static_cast<int32_t>(ret));
        return false;
    }
    HILOG_INFO("GetEnabled successful, state = %{public}d", static_cast<int32_t>(state));
    return static_cast<ani_boolean>(state);
}

void ANIAccessibilityClient::SetEnabled(ani_env *env, ani_object object, ani_boolean enabled)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    RetError ret = instance.SetCaptionsState(enabled, false);
    if (ret != RET_OK) {
        HILOG_ERROR("SetEnabled failed, ret = %{public}d", static_cast<int32_t>(ret));
        return;
    }
    HILOG_INFO("SetEnabled successful, state = %{public}d", static_cast<int32_t>(enabled));
    return;
}

ani_object ANIAccessibilityClient::GetStyle(ani_env *env, ani_object object)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty;
    RetError ret = instance.GetCaptionsProperty(captionProperty, false);
    if (ret != RET_OK) {
        HILOG_ERROR("GetStyle failed, ret = %{public}d", static_cast<int32_t>(ret));
        return nullptr;
    }

    HILOG_INFO("GetStyle successful");
    return CreateAccessibilityCaptionProperty(env, captionProperty);
}

ani_object ANIAccessibilityClient::CreateAccessibilityCaptionProperty(ani_env *env,
    OHOS::AccessibilityConfig::CaptionProperty &captionProperty)
{
    arkts::ani_signature::Type className =
        arkts::ani_signature::Builder::BuildClass("@ohos.accessibility.accessibility.CaptionsStyleImpl");
    ani_class cls;
    if (env->FindClass(className.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("Class CaptionsStyleImpl not found");
        return nullptr;
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor) != ANI_OK) {
        HILOG_ERROR("Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object object;
    if (env->Object_New(cls, ctor, &object) != ANI_OK) {
        HILOG_ERROR("New object fail");
        return nullptr;
    }

    return CreateCaptionPropertyInfoInner(env, cls, object, captionProperty);
}

ani_object ANIAccessibilityClient::CreateCaptionPropertyInfoInner(ani_env *env, ani_class cls,
    ani_object &object, OHOS::AccessibilityConfig::CaptionProperty &captionProperty)
{
    if (env == nullptr || cls == nullptr || object == nullptr) {
        HILOG_ERROR("invalid args");
        return nullptr;
    }

    if (!ANIUtils::SetStringProperty(env, object, "fontFamily", captionProperty.GetFontFamily())) {
        HILOG_ERROR("set fontFamily failed");
    }
    if (!ANIUtils::SetStringProperty(env, object, "fontEdgeType", captionProperty.GetFontEdgeType())) {
        HILOG_ERROR("set fontEdgeType failed");
    }
    if (env->Object_SetPropertyByName_Int(object, "fontScale", captionProperty.GetFontScale()) != ANI_OK) {
        HILOG_ERROR("Set property fontScale failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "fontColor",
        ConvertColorToString(captionProperty.GetFontColor()))) {
        HILOG_ERROR("Set property fontColor failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "backgroundColor",
        ConvertColorToString(captionProperty.GetBackgroundColor()))) {
        HILOG_ERROR("Set property backgroundColor failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "windowColor",
        ConvertColorToString(captionProperty.GetWindowColor()))) {
        HILOG_ERROR("Set property windowColor failed");
    }

    return object;
}

void ANIAccessibilityClient::SetStyle(ani_env *env, ani_object object, ani_object style)
{
    OHOS::AccessibilityConfig::CaptionProperty captionProperty;
    std::string fontFamily = DEFAULT_FONT_FAMILY;
    uint32_t fontScale = DEFAULT_FONT_SCALE;
    uint32_t fontColor = DEFAULT_COLOR;
    std::string fontEdgeType = DEFAULT_FONT_EDGE_TYPE;
    uint32_t backgroundColor = DEFAULT_COLOR;
    uint32_t windowColor = DEFAULT_COLOR;
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(style, &isUndefined) != ANI_OK) {
        HILOG_ERROR("SetSyncCaptionsStyle Reference_IsUndefined");
        return;
    }

    if (!isUndefined) {
        RETURN_IF_FALSE(ANIUtils::GetStringMember(env, style, "fontFamily", fontFamily));
        RETURN_IF_FALSE(ANIUtils::GetColorMember(env, style, "fontColor", fontColor));
        RETURN_IF_FALSE(ANIUtils::GetStringMember(env, style, "fontEdgeType", fontEdgeType));
        RETURN_IF_FALSE(ANIUtils::GetColorMember(env, style, "backgroundColor", backgroundColor));
        RETURN_IF_FALSE(ANIUtils::GetColorMember(env, style, "windowColor", windowColor));
        int styleValue = 0;
        if ((env->Object_GetPropertyByName_Int(style, "fontScale", &styleValue) != ANI_OK) || (styleValue < 0)) {
            HILOG_ERROR("Get property failed");
            return;
        }
        fontScale = static_cast<uint32_t>(styleValue);
    }

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    captionProperty.SetFontFamily(fontFamily);
    captionProperty.SetFontScale(fontScale);
    captionProperty.SetFontColor(fontColor);
    captionProperty.SetFontEdgeType(fontEdgeType);
    captionProperty.SetBackgroundColor(backgroundColor);
    captionProperty.SetWindowColor(windowColor);
    auto ret = instance.SetCaptionsProperty(captionProperty, false);
    HILOG_INFO("SetSyncCaptionsStyle ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

void ANIAccessibilityClient::SendAccessibilityEvent(ani_env *env, ani_object eventObject)
{
    AccessibilityEventInfo eventInfo {};

    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }

    bool ret = ANIUtils::ConvertEventInfoMandatoryFields(env, eventObject, eventInfo);
    if (!ret) {
        HILOG_ERROR("ConvertEventInfoMandatoryFields failed");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_INVALID_PARAM));
        return;
    }

    ANIUtils::ConvertEventInfoStringFields(env, eventObject, eventInfo);
    ANIUtils::ConvertEventInfoIntFields(env, eventObject, eventInfo);
    ANIUtils::ConvertEventInfoRefFields(env, eventObject, eventInfo);

    auto result = asaClient->SendEvent(eventInfo);
    if (result != RET_OK) {
        HILOG_ERROR("SendAccessibilityEvent failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(result));
    }
    return;
}

void ANIAccessibilityClient::OnSeniorModeStateChangeForSelfSync(ani_env *env, ani_object observer)
{
    HILOG_INFO("OnSeniorModeStateChangeForSelf");
    seniorModeStateForAppListeners_->SubscribeObserver(env, observer);
}

void ANIAccessibilityClient::OffSeniorModeStateChangeForSelfSync(ani_env *env, ani_object observer)
{
    HILOG_INFO("OffSeniorModeStateChangeForSelf");
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(observer, &isUndefined) != ANI_OK) {
        HILOG_ERROR("OffSeniorModeStateChangeForSelf Reference_IsUndefined failed");
        return;
    }

    if (!isUndefined) {
        seniorModeStateForAppListeners_->UnsubscribeObserver(env, observer);
    } else {
        seniorModeStateForAppListeners_->UnsubscribeObservers();
    }
}

ani_boolean ANIAccessibilityClient::GetSeniorModeStateForSelfSync(ani_env *env)
{
    HILOG_INFO("GetSeniorModeStateForSelf");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return false;
    }
    bool state = false;
    auto ret = asaClient->GetSeniorModeStateForApp(state);
    if (ret != RET_OK) {
        HILOG_ERROR("GetSeniorModeStateForApp failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return false;
    }
    HILOG_INFO("GetSeniorModeStateForSelf state: %{public}d", state);
    return state;
}

void ANIAccessibilityClient::SetSeniorModeStateForSelfSync(ani_env *env, ani_boolean state)
{
    HILOG_INFO("SetSeniorModeStateForSelf state: %{public}d", state);
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return;
    }
    auto ret = asaClient->SetSeniorModeStateForApp(state);
    if (ret != RET_OK) {
        HILOG_ERROR("SetSeniorModeStateForApp failed!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_FAILED));
        return;
    }
    HILOG_INFO("SetSeniorModeStateForSelf success");
}

static void SetRectToANI(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    OHOS::Accessibility::Rect rect = elementInfo.GetRectInScreen();
    ani_class rectCls = nullptr;
    ani_status status = env->FindClass(
        Builder::BuildClass("@ohos.accessibility.accessibility.UIRectImpl")
        .Descriptor().c_str(), &rectCls);
    if (rectCls == nullptr) {
        HILOG_ERROR("FindClass UIRectImpl failed, status=%{public}d", static_cast<int32_t>(status));
        return;
    }
    ani_object rectObj = ANIUtils::CreateObject(env, rectCls);
    if (rectObj == nullptr) {
        HILOG_ERROR("Failed to create UIRectImpl object");
        return;
    }
    env->Object_SetFieldByName_Int(rectObj, "left", rect.GetLeftTopXScreenPostion());
    env->Object_SetFieldByName_Int(rectObj, "top", rect.GetLeftTopYScreenPostion());
    int32_t width = rect.GetRightBottomXScreenPostion() - rect.GetLeftTopXScreenPostion();
    int32_t height = rect.GetRightBottomYScreenPostion() - rect.GetLeftTopYScreenPostion();
    env->Object_SetFieldByName_Int(rectObj, "width", width);
    env->Object_SetFieldByName_Int(rectObj, "height", height);
    env->Object_SetFieldByName_Ref(result, "rect", rectObj);
}
 
static void SetChildrenIdsToANI(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    auto childIds = elementInfo.GetChildIds();
    ani_ref undefinedRef = nullptr;
    env->GetUndefined(&undefinedRef);
    ani_array childrenArr = nullptr;
    env->Array_New(childIds.size(), undefinedRef, &childrenArr);
    for (size_t i = 0; i < childIds.size(); i++) {
        ani_object idObj;
        if (ANIUtils::CreateAniLong(env, static_cast<ani_long>(childIds[i]), idObj) != ANI_OK) {
            HILOG_ERROR("CreateAniLong failed");
            return;
        }
        env->Array_Set(childrenArr, static_cast<ani_size>(i), idObj);
    }
    env->Object_SetFieldByName_Ref(result, "childrenIds", childrenArr);
}
 
static void SetCustomActionsToANI(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    std::vector<std::string> customActions;
    elementInfo.GetCustomActionList(customActions);
    ani_ref undefinedRef = nullptr;
    env->GetUndefined(&undefinedRef);
    ani_array actionArr = nullptr;
    env->Array_New(customActions.size(), undefinedRef, &actionArr);
    for (size_t i = 0; i < customActions.size(); i++) {
        ani_string item = ANIUtils::CreateAniString(env, customActions[i]);
        env->Array_Set(actionArr, static_cast<ani_size>(i), item);
    }
    env->Object_SetFieldByName_Ref(result, "customActions", actionArr);
}
 
static void ConvertUIElementInfoToANIPart1(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    int64_t componentId = static_cast<int64_t>(elementInfo.GetAccessibilityId());
    ANIUtils::SetLongField(env, result, "componentId", componentId);
    ani_string componentType = ANIUtils::CreateAniString(env, elementInfo.GetComponentType());
    env->Object_SetFieldByName_Ref(result, "componentType", componentType);
    ani_string description = ANIUtils::CreateAniString(env, elementInfo.GetDescriptionInfo());
    env->Object_SetFieldByName_Ref(result, "accessibilityDescription", description);
    ani_object editable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsEditable()));
    env->Object_SetFieldByName_Ref(result, "editable", editable);
    ani_string error = ANIUtils::CreateAniString(env, elementInfo.GetError());
    env->Object_SetFieldByName_Ref(result, "error", error);
    ani_object focusable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsFocusable()));
    env->Object_SetFieldByName_Ref(result, "focusable", focusable);
    ani_string hintText = ANIUtils::CreateAniString(env, elementInfo.GetHint());
    env->Object_SetFieldByName_Ref(result, "hintText", hintText);
    ani_string identifier = ANIUtils::CreateAniString(env, elementInfo.GetInspectorKey());
    env->Object_SetFieldByName_Ref(result, "identifier", identifier);
    ani_object isActive = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.GetIsActive()));
    env->Object_SetFieldByName_Ref(result, "isActive", isActive);
    ani_object isEnabled = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsEnabled()));
    env->Object_SetFieldByName_Ref(result, "isEnabled", isEnabled);
    ani_object isFocused = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsFocused()));
    env->Object_SetFieldByName_Ref(result, "isFocused", isFocused);
    ani_object isVisible = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsVisible()));
    env->Object_SetFieldByName_Ref(result, "isVisible", isVisible);
    ani_object longClickable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsLongClickable()));
    env->Object_SetFieldByName_Ref(result, "longClickable", longClickable);
    env->Object_SetFieldByName_Int(result, "pageId", static_cast<ani_int>(elementInfo.GetPageId()));
}
 
static void ConvertUIElementInfoToANIPart2(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    ani_object scrollable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsScrollable()));
    env->Object_SetFieldByName_Ref(result, "scrollable", scrollable);
    ani_object selected = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsSelected()));
    env->Object_SetFieldByName_Ref(result, "selected", selected);
    ani_string text = ANIUtils::CreateAniString(env, elementInfo.GetContent());
    env->Object_SetFieldByName_Ref(result, "text", text);
    env->Object_SetFieldByName_Int(result, "textLengthLimit",
        static_cast<ani_int>(elementInfo.GetTextLengthLimit()));
    ani_object valueMax = ANIUtils::CreateDouble(env, static_cast<float>(elementInfo.GetRange().GetMax()));
    env->Object_SetFieldByName_Ref(result, "valueMax", valueMax);
    ani_object valueMin = ANIUtils::CreateDouble(env, static_cast<float>(elementInfo.GetRange().GetMin()));
    env->Object_SetFieldByName_Ref(result, "valueMin", valueMin);
    ani_object valueNow = ANIUtils::CreateDouble(env, static_cast<float>(elementInfo.GetRange().GetCurrent()));
    env->Object_SetFieldByName_Ref(result, "valueNow", valueNow);
    ani_object offset = ANIUtils::CreateDouble(env, static_cast<float>(elementInfo.GetOffset()));
    env->Object_SetFieldByName_Ref(result, "offset", offset);
    ani_string accessibilityText = ANIUtils::CreateAniString(env, elementInfo.GetAccessibilityText());
    env->Object_SetFieldByName_Ref(result, "accessibilityText", accessibilityText);
    ani_string accessibilityStateDescription = ANIUtils::CreateAniString(env,
        elementInfo.GetAccessibilityStateDescription());
    env->Object_SetFieldByName_Ref(result, "accessibilityStateDescription", accessibilityStateDescription);
    ani_string customComponentType = ANIUtils::CreateAniString(env, elementInfo.GetCustomComponentType());
    env->Object_SetFieldByName_Ref(result, "accessibilityRole", customComponentType);
    ANIUtils::SetLongField(env, result, "accessibilityNextFocusId",
        static_cast<int64_t>(elementInfo.GetAccessibilityNextFocusId()));
    ANIUtils::SetLongField(env, result, "accessibilityPreviousFocusId",
        static_cast<int64_t>(elementInfo.GetAccessibilityPreviousFocusId()));
    ani_object accessibilityScrollable = ANIUtils::CreateBoolObject(env,
        static_cast<ani_boolean>(elementInfo.GetAccessibilityScrollable()));
    env->Object_SetFieldByName_Ref(result, "accessibilityScrollable", accessibilityScrollable);
    ani_object accessibilityGroup = ANIUtils::CreateBoolObject(env,
        static_cast<ani_boolean>(elementInfo.GetAccessibilityGroup()));
    env->Object_SetFieldByName_Ref(result, "accessibilityGroup", accessibilityGroup);
    ani_string accessibilityLevel = ANIUtils::CreateAniString(env, elementInfo.GetAccessibilityLevel());
    env->Object_SetFieldByName_Ref(result, "accessibilityLevel", accessibilityLevel);
    SetRectToANI(env, result, elementInfo);
}
 
static void ConvertUIElementInfoToANIPart3(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    ani_object accessibilityVisible = ANIUtils::CreateBoolObject(env,
        static_cast<ani_boolean>(elementInfo.GetAccessibilityVisible()));
    env->Object_SetFieldByName_Ref(result, "accessibilityVisible", accessibilityVisible);
    ANIUtils::SetLongField(env, result, "parentId",
        static_cast<int64_t>(elementInfo.GetParentNodeId()));
    SetChildrenIdsToANI(env, result, elementInfo);
    SetCustomActionsToANI(env, result, elementInfo);
    ani_object checkable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsCheckable()));
    env->Object_SetFieldByName_Ref(result, "checkable", checkable);
    ani_object checked = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsChecked()));
    env->Object_SetFieldByName_Ref(result, "isChecked", checked);
    ani_object clickable = ANIUtils::CreateBoolObject(env, static_cast<ani_boolean>(elementInfo.IsClickable()));
    env->Object_SetFieldByName_Ref(result, "clickable", clickable);
    ani_object accessibilityFocused = ANIUtils::CreateBoolObject(env,
        static_cast<ani_boolean>(elementInfo.HasAccessibilityFocus()));
    env->Object_SetFieldByName_Ref(result, "accessibilityFocused", accessibilityFocused);
}
 
static void ConvertUIElementInfoToANI(ani_env *env, ani_object result,
    const OHOS::Accessibility::AccessibilityElementInfo &elementInfo)
{
    ConvertUIElementInfoToANIPart1(env, result, elementInfo);
    ConvertUIElementInfoToANIPart2(env, result, elementInfo);
    ConvertUIElementInfoToANIPart3(env, result, elementInfo);
}
 
ani_object ANIAccessibilityClient::GetFocusedUIAccessibilityElementSync(ani_env *env)
{
    HILOG_INFO("GetFocusedUIAccessibilityElementSync enter");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        HILOG_ERROR("asaClient is nullptr!");
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NULLPTR));
        return nullptr;
    }
    OHOS::Accessibility::AccessibilityElementInfo elementInfo;
    auto ret = asaClient->GetLocalFocusElement(elementInfo);
    if (ret != RET_OK) {
        HILOG_INFO("GetLocalFocusElement no focused element, ret=%{public}d", static_cast<int32_t>(ret));
        ani_ref undefinedRef = nullptr;
        env->GetUndefined(&undefinedRef);
        return reinterpret_cast<ani_object>(undefinedRef);
    }
    HILOG_INFO("GetLocalFocusElement success, componentId=%{public}lld",
        static_cast<long long>(elementInfo.GetAccessibilityId()));
    ani_class cls = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->FindClass(Builder::BuildClass("@ohos.accessibility.accessibility.UIAccessibilityElementImpl")
        .Descriptor().c_str(), &cls)) != ANI_OK || cls == nullptr) {
        HILOG_ERROR("FindClass UIAccessibilityElementImpl failed, status=%{public}d", static_cast<int32_t>(status));
        return nullptr;
    }
    ani_method ctor = nullptr;
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    if ((status = env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor)) != ANI_OK
        || ctor == nullptr) {
        HILOG_ERROR("Find ctor UIAccessibilityElementImpl failed, status=%{public}d", static_cast<int32_t>(status));
        return nullptr;
    }
    ani_object result = nullptr;
    if ((status = env->Object_New(cls, ctor, &result)) != ANI_OK || result == nullptr) {
        HILOG_ERROR("Object_New UIAccessibilityElementImpl failed, status=%{public}d", static_cast<int32_t>(status));
        return nullptr;
    }
    HILOG_INFO("UIAccessibilityElementImpl object created, begin ConvertUIElementInfoToANI");
    ConvertUIElementInfoToANI(env, result, elementInfo);
    HILOG_INFO("GetFocusedUIAccessibilityElementSync done, sourceId=%{public}lld",
        static_cast<long long>(elementInfo.GetAccessibilityId()));
    return result;
}
 
void ANIAccessibilityClient::OnFocusedUIAccessibilityElementChangedSync(ani_env *env, ani_object observer)
{
    HILOG_INFO("OnFocusedUIAccessibilityElementChangedSync");
    focusChangeListeners_->SubscribeObserver(env, observer);
}
 
void ANIAccessibilityClient::OffFocusedUIAccessibilityElementChangedSync(ani_env *env, ani_object observer)
{
    HILOG_INFO("OffFocusedUIAccessibilityElementChangedSync");
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(observer, &isUndefined) != ANI_OK) {
        HILOG_ERROR("OffFocusedUIAccessibilityElementChangedSync Reference_IsUndefined failed");
        return;
    }
    if (!isUndefined) {
        focusChangeListeners_->UnsubscribeObserver(env, observer);
    } else {
        focusChangeListeners_->UnsubscribeObservers();
    }
}
 
void ANIAccessibilityClient::SubscribeFocusChangeListenerToFramework()
{
    focusChangeListeners_->SubscribeToFramework();
}
 
void ANIAccessibilityClient::UnsubscribeFocusChangeListenerFromFramework()
{
    focusChangeListeners_->UnsubscribeFromFramework();
}
 
void FocusChangeListenerImpl::SubscribeToFramework()
{
    HILOG_INFO("FocusChangeListenerImpl SubscribeToFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->SubscribeFocusChangeObserver(shared_from_this());
    }
}
 
void FocusChangeListenerImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("FocusChangeListenerImpl UnsubscribeFromFramework");
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient) {
        asaClient->UnsubscribeFocusChangeObserver(shared_from_this());
    }
}
 
void FocusChangeListenerImpl::OnFocusChanged(
    const OHOS::Accessibility::AccessibilityElementInfo &focusedElement,
    const OHOS::Accessibility::AccessibilityElementInfo &unfocusedElement)
{
    HILOG_INFO("FocusChangeListenerImpl::OnFocusChanged");
    NotifyObservers(focusedElement, unfocusedElement);
}
 
void FocusChangeListenerImpl::SubscribeObserver(ani_env *env, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->fnRef_)) {
            HILOG_INFO("SubscribeObserver Observer exist");
            return;
        }
    }
    ani_ref ref = nullptr;
    if (env->GlobalReference_Create(observer, &ref) != ANI_OK) {
        HILOG_ERROR("Reference_New failed");
        return;
    }
    observers_.emplace_back(std::make_shared<FocusChangeCallbackListener>(env, ref));
}
 
void FocusChangeListenerImpl::UnsubscribeObserver(ani_env *env, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, observer, (*iter)->env_, (*iter)->fnRef_)) {
            DeleteObserverReference(env, *iter);
            observers_.erase(iter);
            return;
        }
    }
}
 
void FocusChangeListenerImpl::UnsubscribeObservers()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        DeleteObserverReference(observer->env_, observer);
    }
    observers_.clear();
}
 
static ani_object CreateChangeInfoObject(ani_env *env)
{
    ani_class infoCls = nullptr;
    if (env->FindClass(Builder::BuildClass(
        "@ohos.accessibility.accessibility.UIAccessibilityFocusChangeInfoImpl")
        .Descriptor().c_str(), &infoCls) != ANI_OK || infoCls == nullptr) {
        HILOG_ERROR("FindClass UIAccessibilityFocusChangeInfoImpl failed");
        return nullptr;
    }
    ani_method infoCtor = nullptr;
    std::string infoCtorName = Builder::BuildConstructorName();
    SignatureBuilder infoSb{};
    if (env->Class_FindMethod(infoCls, infoCtorName.c_str(),
        infoSb.BuildSignatureDescriptor().c_str(), &infoCtor) != ANI_OK || infoCtor == nullptr) {
        HILOG_ERROR("Find ctor UIAccessibilityFocusChangeInfoImpl failed");
        return nullptr;
    }
    ani_object obj = nullptr;
    if (env->Object_New(infoCls, infoCtor, &obj) != ANI_OK || obj == nullptr) {
        HILOG_ERROR("Object_New UIAccessibilityFocusChangeInfoImpl failed");
        return nullptr;
    }
    return obj;
}
 
void FocusChangeListenerImpl::NotifyObservers(
    const OHOS::Accessibility::AccessibilityElementInfo &focusedElement,
    const OHOS::Accessibility::AccessibilityElementInfo &unfocusedElement)
{
    bool hasUnfocused = (unfocusedElement.GetAccessibilityId() !=
                         OHOS::Accessibility::AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID);
    std::vector<std::shared_ptr<FocusChangeCallbackListener>> observersCopy;
    {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        HILOG_INFO("NotifyObservers hasUnfocused=%{public}d observers=%{public}zu",
            static_cast<int32_t>(hasUnfocused), observers_.size());
        observersCopy = observers_;
    }
    for (auto &observer : observersCopy) {
        auto callbackInfo = std::make_shared<ANIFocusChangeCallbackInfo>();
        callbackInfo->env_ = observer->env_;
        callbackInfo->fnRef_ = observer->fnRef_;
        callbackInfo->focusedElement_ = focusedElement;
        callbackInfo->unfocusedElement_ = unfocusedElement;
        auto task = [this, callbackInfo, hasUnfocused]() {
            InvokeObserverCallback(callbackInfo, hasUnfocused);
        };
        if (!ANIUtils::SendEventToMainThread(task)) {
            HILOG_ERROR("Failed to send focus change event to main thread");
        }
    }
}
 
void FocusChangeListenerImpl::InvokeObserverCallback(
    std::shared_ptr<ANIFocusChangeCallbackInfo> callbackInfo, bool hasUnfocused)
{
    ani_env *tmpEnv = callbackInfo->env_;
    ani_size nr_refs = ANI_SCOPE_SIZE;
    tmpEnv->CreateLocalScope(nr_refs);
    auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
    ani_object jsFocusedInfo = CreateChangeInfoObject(tmpEnv);
    if (jsFocusedInfo == nullptr) {
        tmpEnv->DestroyLocalScope();
        return;
    }
    ani_class elementCls = nullptr;
    ani_status elemStatus = tmpEnv->FindClass(
        Builder::BuildClass("@ohos.accessibility.accessibility.UIAccessibilityElementImpl")
        .Descriptor().c_str(), &elementCls);
    if (elementCls == nullptr) {
        tmpEnv->DestroyLocalScope();
        return;
    }
    ani_object jsFocusedElement = ANIUtils::CreateObject(tmpEnv, elementCls);
    if (jsFocusedElement != nullptr) {
        ConvertUIElementInfoToANI(tmpEnv, jsFocusedElement, callbackInfo->focusedElement_);
        ani_status focusedStat = tmpEnv->Object_SetFieldByName_Ref(jsFocusedInfo, "focusedElement", jsFocusedElement);
    } else {
        ani_ref nullRef = nullptr;
        tmpEnv->GetNull(&nullRef);
        tmpEnv->Object_SetFieldByName_Ref(jsFocusedInfo, "focusedElement", nullRef);
    }
    if (hasUnfocused) {
        ani_object jsUnfocusedElement = ANIUtils::CreateObject(tmpEnv, elementCls);
        if (jsUnfocusedElement != nullptr) {
            ConvertUIElementInfoToANI(tmpEnv, jsUnfocusedElement, callbackInfo->unfocusedElement_);
            ani_status unfocusedStat = tmpEnv->Object_SetFieldByName_Ref(jsFocusedInfo,
                "unfocusedElement", jsUnfocusedElement);
        } else {
            ani_ref undefinedRef = nullptr;
            tmpEnv->GetUndefined(&undefinedRef);
            tmpEnv->Object_SetFieldByName_Ref(jsFocusedInfo, "unfocusedElement", undefinedRef);
        }
    } else {
        ani_ref undefinedRef = nullptr;
        tmpEnv->GetUndefined(&undefinedRef);
        tmpEnv->Object_SetFieldByName_Ref(jsFocusedInfo, "unfocusedElement", undefinedRef);
    }
 
    std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(jsFocusedInfo)};
    ani_ref result;
    HILOG_INFO("NotifyObservers callback done, hasUnfocused=%{public}d",
        static_cast<int32_t>(hasUnfocused));
    tmpEnv->FunctionalObject_Call(fnObj, 1, args.data(), &result);
    tmpEnv->DestroyLocalScope();
}
 
void FocusChangeListenerImpl::DeleteObserverReference(ani_env *env,
    std::shared_ptr<FocusChangeCallbackListener> observer)
{
    auto callbackInfo = std::make_shared<ANIFocusChangeCallbackInfo>();
    callbackInfo->env_ = observer->env_;
    callbackInfo->fnRef_ = observer->fnRef_;
    auto task = [callbackInfo]() {
        ani_env *tmpEnv = callbackInfo->env_;
        tmpEnv->GlobalReference_Delete(callbackInfo->fnRef_);
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
        HILOG_ERROR("Failed to send delete reference event");
    }
}
