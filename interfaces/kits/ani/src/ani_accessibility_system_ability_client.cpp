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
#include "ani_utils.h"
#include "accessibility_utils_ani.h"
#include "hilog_wrapper.h"
#include <ani_signature_builder.h>

using namespace OHOS::Accessibility;
using namespace OHOS::AccessibilityAni;
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
std::shared_ptr<AccessibilityCaptionsObserverImpl> ANIAccessibilityClient::captionListeners_ =
    std::make_shared<AccessibilityCaptionsObserverImpl>();

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
    HILOG_INFO("state is %{public}d", state);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        observer->OnStateChanged(state);
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

void StateListenerImpl::SubscribeObserver(ani_env *env, ani_object observer)
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

    std::shared_ptr<StateListener> stateListener = std::make_shared<StateListener>(env, fnRef);
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
            HILOG_INFO("UnsubscribeObserver Observer exist");
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
            HILOG_INFO("UnsubscribeObserver Observer exist");
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
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if ((*iter)->configId_ == id) {
            (*iter)->env_->GlobalReference_Delete((*iter)->fnRef_);
        }
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
        ani_object state = ANIUtils::CreateBoolObject(tmpEnv, static_cast<ani_boolean>(callbackInfo->state_));
        if (state == nullptr) {
            HILOG_ERROR("create boolean object failed");
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
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
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
        if (ANI_OK != status) {
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
    HILOG_INFO("stateTypeStr = %{public}s", stateTypeStr.c_str());
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
        HILOG_ERROR(" not found class");
        return nullptr;
    }
    ani_method ctor;
    if (env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor) != ANI_OK) {
        HILOG_ERROR(" Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object object;
    if (env->Object_New(cls, ctor, &object) != ANI_OK) {
        HILOG_ERROR(" New object fail");
        return nullptr;
    }

    return CreateCaptionPropertyInfoInner(env, cls, object, captionProperty);
}

ani_object ANIAccessibilityClient::CreateCaptionPropertyInfoInner(ani_env *env, ani_class cls,
    ani_object &object, OHOS::AccessibilityConfig::CaptionProperty &captionProperty)
{
    if (env == nullptr || cls == nullptr || object == nullptr) {
        HILOG_ERROR(" invalid args");
        return nullptr;
    }

    if (!ANIUtils::SetStringProperty(env, object, "fontFamily", captionProperty.GetFontFamily())) {
        HILOG_ERROR("set fontFamily failed");
    }
    if (!ANIUtils::SetStringProperty(env, object, "fontEdgeType", captionProperty.GetFontEdgeType())) {
        HILOG_ERROR("set fontEdgeType failed");
    }
    if (env->Object_SetPropertyByName_Int(object, "fontScale", captionProperty.GetFontScale()) != ANI_OK) {
        HILOG_ERROR(" Set property fontScale failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "fontColor",
        ConvertColorToString(captionProperty.GetFontColor()))) {
        HILOG_ERROR(" Set property fontColor failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "backgroundColor",
        ConvertColorToString(captionProperty.GetBackgroundColor()))) {
        HILOG_ERROR(" Set property backgroundColor failed");
    }

    if (!ANIUtils::SetStringProperty(env, object, "windowColor",
        ConvertColorToString(captionProperty.GetWindowColor()))) {
        HILOG_ERROR(" Set property windowColor failed");
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
        HILOG_ERROR(" SetSyncCaptionsStyle Reference_IsUndefined");
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