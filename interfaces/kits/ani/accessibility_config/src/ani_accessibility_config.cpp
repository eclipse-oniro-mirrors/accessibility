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
#include "ani_accessibility_config.h"
#include "ani_utils.h"
#include "ipc_skeleton.h"
#include "tokenid_kit.h"
#include "accesstoken_kit.h"
#include "hilog_wrapper.h"
#include <ani_signature_builder.h>

using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace Security::AccessToken;
using CONFIG_ID = OHOS::AccessibilityConfig::CONFIG_ID;
using ConfigValue = OHOS::AccessibilityConfig::ConfigValue;

constexpr int32_t ANI_SCOPE_SIZE = 16;
const std::string DEFAULT_FONT_FAMILY = "default";
constexpr uint32_t DEFAULT_FONT_SCALE = 75;
constexpr uint32_t DEFAULT_COLOR = 0xff000000;
const std::string DEFAULT_FONT_EDGE_TYPE = "none";

std::shared_ptr<EnableAbilityListsObserverImpl> ANIAccessibilityConfig::enableAbilityListsObservers_ =
    std::make_shared<EnableAbilityListsObserverImpl>();
std::shared_ptr<ANIAccessibilityConfigObserverImpl> ANIAccessibilityConfig::configObservers_ =
    std::make_shared<ANIAccessibilityConfigObserverImpl>();

void EnableAbilityListsObserver::OnEnableAbilityListsStateChanged()
{
    HILOG_DEBUG();
    std::shared_ptr<AccessibilityCallbackInfo> callbackInfo = std::make_shared<AccessibilityCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = callback_;
    auto task = [callbackInfo]() {
        HILOG_INFO("on enable ability lists state changed");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, 0, nullptr, &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void EnableAbilityListsObserverImpl::OnEnableAbilityListsStateChanged()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnEnableAbilityListsStateChanged();
        } else {
            HILOG_ERROR("observer is null");
        }
    }
}

void EnableAbilityListsObserverImpl::OnInstallAbilityListsStateChanged()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : installAbilityListsObservers_) {
        if (observer) {
            observer->OnEnableAbilityListsStateChanged();
        } else {
            HILOG_ERROR("observer is null");
        }
    }
}

void EnableAbilityListsObserverImpl::SubscribeObserver(ani_env *env, ani_object observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->callback_)) {
            env->GlobalReference_Delete(fnRef);
            HILOG_DEBUG("SubscribeObserver Observer exist");
            return;
        }
    }
    std::shared_ptr<EnableAbilityListsObserver> observerPtr = std::make_shared<EnableAbilityListsObserver>(env, fnRef);
    enableAbilityListsObservers_.emplace_back(observerPtr);
    HILOG_DEBUG("enableAbilityListsObservers size%{public}zu", enableAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::SubscribeInstallObserver(ani_env *env, ani_object observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = installAbilityListsObservers_.begin(); iter != installAbilityListsObservers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->callback_)) {
            env->GlobalReference_Delete(fnRef);
            HILOG_DEBUG("SubscribeObserver Observer exist");
            return;
        }
    }
    std::shared_ptr<EnableAbilityListsObserver> observerPtr = std::make_shared<EnableAbilityListsObserver>(env, fnRef);
    installAbilityListsObservers_.emplace_back(observerPtr);
    HILOG_DEBUG("installAbilityListsObservers_ size%{public}zu", installAbilityListsObservers_.size());
}

void EnableAbilityListsObserverImpl::UnsubscribeObserver(ani_env *env, ani_object observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->callback_)) {
            env->GlobalReference_Delete((*iter)->callback_);
            enableAbilityListsObservers_.erase(iter);
            break;
        }
    }
    env->GlobalReference_Delete(fnRef);
}

void EnableAbilityListsObserverImpl::UnsubscribeObservers()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = enableAbilityListsObservers_.begin(); iter != enableAbilityListsObservers_.end(); iter++) {
        ((*iter)->env_)->GlobalReference_Delete((*iter)->callback_);
    }
    enableAbilityListsObservers_.clear();
}

void EnableAbilityListsObserverImpl::UnsubscribeInstallObserver(ani_env *env, ani_object observer)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = installAbilityListsObservers_.begin(); iter != installAbilityListsObservers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->callback_)) {
            env->GlobalReference_Delete((*iter)->callback_);
            installAbilityListsObservers_.erase(iter);
            break;
        }
    }
    env->GlobalReference_Delete(fnRef);
}

void EnableAbilityListsObserverImpl::UnsubscribeInstallObservers()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = installAbilityListsObservers_.begin(); iter != installAbilityListsObservers_.end(); iter++) {
        ((*iter)->env_)->GlobalReference_Delete((*iter)->callback_);
    }
    installAbilityListsObservers_.clear();
}

void EnableAbilityListsObserverImpl::SubscribeToFramework()
{
    HILOG_INFO("SubscribeToFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.SubscribeEnableAbilityListsObserver(shared_from_this());
}

void EnableAbilityListsObserverImpl::UnsubscribeFromFramework()
{
    HILOG_INFO("UnsubscribeFromFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    instance.UnsubscribeEnableAbilityListsObserver(shared_from_this());
}

void ANIAccessibilityConfigObserver::OnConfigChangedExtra(const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));
    if (configId_ == CONFIG_ID::CONFIG_CONTENT_TIMEOUT) {
        NotifyIntChangedToJs(static_cast<int32_t>(value.contentTimeout));
    } else if (configId_ == CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT) {
        NotifyFloatChangedToJs(value.brightnessDiscount);
    } else if (configId_ == CONFIG_ID::CONFIG_AUDIO_BALANCE) {
        NotifyFloatChangedToJs(value.audioBalance);
    } else if (configId_ ==  CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
        NotifyStateChangedToJs(value.highContrastText);
    } else if (configId_ ==  CONFIG_ID::CONFIG_DALTONIZATION_STATE) {
        NotifyStateChangedToJs(value.daltonizationState);
    } else if (configId_ == CONFIG_ID::CONFIG_INVERT_COLOR) {
        NotifyStateChangedToJs(value.invertColor);
    } else if (configId_ == CONFIG_ID::CONFIG_ANIMATION_OFF) {
        NotifyStateChangedToJs(value.animationOff);
    } else if (configId_ == CONFIG_ID::CONFIG_AUDIO_MONO) {
        NotifyStateChangedToJs(value.audioMono);
    } else if (configId_ == CONFIG_ID::CONIFG_CLICK_RESPONSE_TIME) {
        NotifyStringChangedToJs(ConvertClickResponseTimeTypeToString(value.clickResponseTime));
    } else if (configId_ == CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_TIME) {
        NotifyStringChangedToJs(ConvertIgnoreRepeatClickTimeTypeToString(value.ignoreRepeatClickTime));
    } else if (configId_ == CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        NotifyStateChangedToJs(value.ignoreRepeatClickState);
    }
}

void ANIAccessibilityConfigObserver::OnConfigChanged(const ConfigValue &value)
{
    HILOG_INFO("id = [%{public}d]", static_cast<int32_t>(configId_));
    if (configId_ == CONFIG_ID::CONFIG_CAPTION_STATE) {
        NotifyStateChangedToJs(value.captionState);
    } else if (configId_ == CONFIG_ID::CONFIG_CAPTION_STYLE) {
        NotifyPropertyChangedToJs(value.captionStyle);
    } else if (configId_ == CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION) {
        NotifyStateChangedToJs(value.screenMagnifier);
    } else if (configId_ == CONFIG_ID::CONFIG_MOUSE_KEY) {
        NotifyStateChangedToJs(value.mouseKey);
    } else if (configId_ == CONFIG_ID::CONFIG_SHORT_KEY) {
        NotifyStateChangedToJs(value.shortkey);
    } else if (configId_ == CONFIG_ID::CONFIG_SHORT_KEY_TARGET) {
        NotifyStringChangedToJs(value.shortkey_target);
    } else if (configId_ == CONFIG_ID::CONFIG_SHORT_KEY_MULTI_TARGET) {
        NotifyStringVectorChangedToJs(value.shortkeyMultiTarget);
    } else if (configId_ == CONFIG_ID::CONFIG_MOUSE_AUTOCLICK) {
        NotifyIntChangedToJs(value.mouseAutoClick);
    } else if (configId_ == CONFIG_ID::CONFIG_DALTONIZATION_COLOR_FILTER) {
        OnDaltonizationColorFilterConfigChanged();
    } else {
        OnConfigChangedExtra(value);
    }
}

void ANIAccessibilityConfigObserver::OnDaltonizationColorFilterConfigChanged()
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::DALTONIZATION_TYPE type = OHOS::AccessibilityConfig::DALTONIZATION_TYPE::Normal;
    RetError ret = instance.GetDaltonizationColorFilter(type);
    NotifyStringChangedToJs(ConvertDaltonizationTypeToString(type));
    if (ret != RET_OK) {
        HILOG_ERROR("get DaltonizationColorFilter failed: %{public}d", ret);
    }
}

void ANIAccessibilityConfigObserver::NotifyStateChangedToJs(bool enabled)
{
    HILOG_INFO(" enabled = [%{public}s]", enabled ? "true" : "false");
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->state_ = enabled;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
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

void ANIAccessibilityConfigObserver::NotifyPropertyChangedToJs(
    const OHOS::AccessibilityConfig::CaptionProperty &caption)
{
    std::shared_ptr<ANICaptionCallbackInfo> callbackInfo = std::make_shared<ANICaptionCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->caption_ = caption;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
    auto task = [callbackInfo] () {
        if (callbackInfo == nullptr) {
            return;
        }
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANIAccessibilityConfig::CreateJsAccessibilityCaptionProperty(tmpEnv, callbackInfo->caption_);
        if (state == nullptr) {
            HILOG_ERROR("create CaptionProperty object failed");
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

void ANIAccessibilityConfigObserver::NotifyStringChangedToJs(const std::string& value)
{
    HILOG_INFO("ani value = [%{public}s]", value.c_str());
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->stringValue_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANIUtils::CreateAniString(callbackInfo->env_, callbackInfo->stringValue_);
        if (state == nullptr) {
            HILOG_ERROR("create CreateAniString object failed");
            return;
        }
        std::vector<ani_ref> args = {reinterpret_cast<ani_ref>(state)};
        ani_ref result;
        tmpEnv->FunctionalObject_Call(fnObj, args.size(), args.data(), &result);
        tmpEnv->DestroyLocalScope();
    };
    if (!ANIUtils::SendEventToMainThread(task)) {
        HILOG_ERROR("failed to send event");
    }
}

void ANIAccessibilityConfigObserver::NotifyStringVectorChangedToJs(std::vector<std::string> value)
{
    HILOG_INFO();
    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->stringVector_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANIUtils::CreateArray(tmpEnv, callbackInfo->stringVector_);
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
        HILOG_ERROR(" failed to send event");
    }
}

void ANIAccessibilityConfigObserver::NotifyIntChangedToJs(int32_t value)
{
    HILOG_INFO("value = [%{public}d]", value);

    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->int32Value_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state;
        if (ANIUtils::CreateAniInt(tmpEnv, callbackInfo->int32Value_, state) != ANI_OK) {
            HILOG_ERROR("create int32_t object failed");
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

void ANIAccessibilityConfigObserver::NotifyFloatChangedToJs(float value)
{
    HILOG_INFO("id = [%{public}d] value = [%{public}f]", static_cast<int32_t>(configId_), value);

    std::shared_ptr<ANIStateCallbackInfo> callbackInfo = std::make_shared<ANIStateCallbackInfo>();
    if (callbackInfo == nullptr) {
        HILOG_ERROR("Failed to create callbackInfo");
        return;
    }
    callbackInfo->floatValue_ = value;
    callbackInfo->env_ = env_;
    callbackInfo->fnRef_ = handlerRef_;
    auto task = [callbackInfo]() {
        HILOG_INFO("notify state changed to ets");
        ani_env *tmpEnv = callbackInfo->env_;
        ani_size nr_refs = ANI_SCOPE_SIZE;
        tmpEnv->CreateLocalScope(nr_refs);
        auto fnObj = reinterpret_cast<ani_fn_object>(callbackInfo->fnRef_);
        ani_object state = ANIUtils::CreateDouble(tmpEnv, callbackInfo->floatValue_);
        if (state == nullptr) {
            HILOG_ERROR("create uint32_t object failed");
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

void ANIAccessibilityConfigObserverImpl::SubscribeObserver(ani_env* env,
    OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->handlerRef_)) {
            env->GlobalReference_Delete(fnRef);
            HILOG_INFO(" SubscribeObserver Observer exist");
            return;
        }
    }
    std::shared_ptr<ANIAccessibilityConfigObserver> observerPtr =
        std::make_shared<ANIAccessibilityConfigObserver>(env, fnRef, id);
    observers_.emplace_back(observerPtr);
}

void ANIAccessibilityConfigObserverImpl::UnsubscribeObserver(ani_env* env,
    OHOS::AccessibilityConfig::CONFIG_ID id, ani_object observer)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    ani_ref fnRef;
    env->GlobalReference_Create(observer, &fnRef);
    for (auto iter = observers_.begin(); iter != observers_.end(); iter++) {
        if ((*iter)->configId_ == id) {
            if (ANIUtils::CheckObserverEqual(env, fnRef, (*iter)->env_, (*iter)->handlerRef_)) {
                HILOG_INFO(" UnsubscribeObserver, ID:%{public}d", id);
                env->GlobalReference_Delete((*iter)->handlerRef_);
                observers_.erase(iter);
                break;
            }
        }
    }
    env->GlobalReference_Delete(fnRef);
    HILOG_INFO(" UnsubscribeObserver END");
}


void ANIAccessibilityConfigObserverImpl::SubscribeToFramework()
{
    HILOG_INFO(" ani SubscribeToFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID::CONFIG_ID_MAX); index ++) {
        instance.SubscribeConfigObserver(static_cast<CONFIG_ID>(index), shared_from_this(), false);
    }
}

void ANIAccessibilityConfigObserverImpl::UnsubscribeFromFramework()
{
    HILOG_INFO(" ani UnsubscribeFromFramework");
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    for (int32_t index = 0; index < static_cast<int32_t>(CONFIG_ID::CONFIG_ID_MAX); index ++) {
        instance.UnsubscribeConfigObserver(static_cast<CONFIG_ID>(index), shared_from_this());
    }
}

void ANIAccessibilityConfigObserverImpl::UnsubscribeObservers(OHOS::AccessibilityConfig::CONFIG_ID id)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = observers_.begin(); iter != observers_.end();) {
        if ((*iter)->configId_ == id) {
            iter = observers_.erase(iter);
        } else {
            iter++;
        }
    }
}

void ANIAccessibilityConfigObserverImpl::OnConfigChanged(
    const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto &observer : observers_) {
        if (observer && observer->configId_ == id) {
            observer->OnConfigChanged(value);
        }
    }
}

void ANIAccessibilityConfig::SetSyncboolean(ani_env *env, ani_object object, ani_enum_item id, ani_boolean state)
{
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    HILOG_DEBUG("configId= %{public}d", configId);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = RET_OK;
    if (configId == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
        ret = instance.SetHighContrastTextState(state);
    } else if (configId == CONFIG_ID::CONFIG_DALTONIZATION_STATE) {
        ret = instance.SetDaltonizationState(state);
    } else if (configId == CONFIG_ID::CONFIG_INVERT_COLOR) {
        ret = instance.SetInvertColorState(state);
    } else if (configId == CONFIG_ID::CONFIG_ANIMATION_OFF) {
        ret = instance.SetAnimationOffState(state);
    } else if (configId == CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION) {
        ret = instance.SetScreenMagnificationState(state);
    } else if (configId == CONFIG_ID::CONFIG_AUDIO_MONO) {
        ret = instance.SetAudioMonoState(state);
    } else if (configId == CONFIG_ID::CONFIG_MOUSE_KEY) {
        ret = instance.SetMouseKeyState(state);
    } else if (configId == CONFIG_ID::CONFIG_SHORT_KEY) {
        ret = instance.SetShortKeyState(state);
    } else if (configId == CONFIG_ID::CONFIG_CAPTION_STATE) {
        ret = instance.SetCaptionsState(state);
    } else if (configId == CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        ret = instance.SetIgnoreRepeatClickState(state);
    }
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncboolean ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_boolean ANIAccessibilityConfig::GetSyncboolean(ani_env *env, ani_object object, ani_enum_item id)
{
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return false;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    HILOG_DEBUG("configId= %{public}d", configId);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = RET_ERR_FAILED;

    if (configId == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT) {
        ret = instance.GetHighContrastTextState(state);
    } else if (configId == CONFIG_ID::CONFIG_DALTONIZATION_STATE) {
        ret = instance.GetDaltonizationState(state);
    } else if (configId == CONFIG_ID::CONFIG_INVERT_COLOR) {
        ret = instance.GetInvertColorState(state);
    } else if (configId == CONFIG_ID::CONFIG_ANIMATION_OFF) {
        ret = instance.GetAnimationOffState(state);
    } else if (configId == CONFIG_ID::CONFIG_SCREEN_MAGNIFICATION) {
        ret = instance.GetScreenMagnificationState(state);
    } else if (configId == CONFIG_ID::CONFIG_AUDIO_MONO) {
        ret = instance.GetAudioMonoState(state);
    } else if (configId == CONFIG_ID::CONFIG_MOUSE_KEY) {
        ret = instance.GetMouseKeyState(state);
    } else if (configId == CONFIG_ID::CONFIG_SHORT_KEY) {
        ret = instance.GetShortKeyState(state);
    } else if (configId == CONFIG_ID::CONFIG_CAPTION_STATE) {
        ret = instance.GetCaptionsState(state);
    } else if (configId == CONFIG_ID::CONFIG_IGNORE_REPEAT_CLICK_STATE) {
        ret = instance.GetIgnoreRepeatClickState(state);
    }

    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("GetSyncboolean ret = %{public}d", static_cast<int32_t>(ret));
    return static_cast<ani_boolean>(state);
}

void ANIAccessibilityConfig::SetSyncDouble(ani_env *env, ani_object object, ani_enum_item id, ani_double value)
{
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = RET_OK;
    if (configId == CONFIG_ID::CONFIG_AUDIO_BALANCE) {
        ret = instance.SetAudioBalance(static_cast<float>(value));
    } else if (configId == CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT) {
        ret = instance.SetBrightnessDiscount(static_cast<float>(value));
    }

    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncDouble ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_double ANIAccessibilityConfig::GetSyncDouble(ani_env *env, ani_object object, ani_enum_item id)
{
    float floatData = 0;
    RetError ret = RET_ERR_FAILED;
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return false;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (configId == CONFIG_ID::CONFIG_AUDIO_BALANCE) {
        ret = instance.GetAudioBalance(floatData);
    } else if (configId == CONFIG_ID::CONFIG_BRIGHTNESS_DISCOUNT) {
        ret = instance.GetBrightnessDiscount(floatData);
    }

    HILOG_INFO("getSyncNumber ret = %{public}d", static_cast<int32_t>(ret));
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }

    return static_cast<ani_double>(floatData);
}

void ANIAccessibilityConfig::SetSyncInt(ani_env *env, ani_object object, ani_enum_item id, ani_int value)
{
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    HILOG_DEBUG(" SetSyncInt configId= %{public}d, %{public}d", configId, static_cast<uint32_t>(value));
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = RET_ERR_FAILED;
    if (configId == CONFIG_ID::CONFIG_CONTENT_TIMEOUT) {
        ret = instance.SetContentTimeout(static_cast<uint32_t>(value));
    } else if (configId == CONFIG_ID::CONFIG_MOUSE_AUTOCLICK) {
        ret = instance.SetMouseAutoClick(static_cast<uint32_t>(value));
    }
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncInt ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_int ANIAccessibilityConfig::GetSyncInt(ani_env *env, ani_object object, ani_enum_item id)
{
    float floatData = 0;
    int32_t int32Data = 0;
    RetError ret = RET_ERR_FAILED;
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return -1;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (configId == CONFIG_ID::CONFIG_CONTENT_TIMEOUT) {
        uint32_t timeout = 0;
        ret = instance.GetContentTimeout(timeout);
        int32Data = static_cast<int32_t>(timeout);
    } else if (configId == CONFIG_ID::CONFIG_MOUSE_AUTOCLICK) {
        ret = instance.GetMouseAutoClick(int32Data);
    }

    HILOG_INFO("getSyncNumber ret = %{public}d", static_cast<int32_t>(ret));
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }

    return static_cast<ani_int>(int32Data);
}

void ANIAccessibilityConfig::SetSyncString(ani_env *env, ani_object object, ani_enum_item id, ani_string value)
{
    (void)id;
    std::string nameStr = ANIUtils::ANIStringToStdString(env, value);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.SetShortkeyTarget(nameStr);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO(" SetSyncString ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_string ANIAccessibilityConfig::GetSyncString(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    ani_string retResult = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string result = "";
    auto ret = instance.GetShortkeyTarget(result);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    env->String_NewUTF8(result.c_str(), result.size(), &retResult);
    return retResult;
}

void ANIAccessibilityConfig::SetSyncVectorString(ani_env *env, ani_object object, ani_enum_item id, ani_object value)
{
    (void)id;
    std::vector<std::string> valueList;
    if (!ANIUtils::ParseStringArray(env, value, valueList)) {
        HILOG_INFO(" ParseStringArray fail");
        return;
    }
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.SetShortkeyMultiTarget(valueList);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncVectorString ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_object ANIAccessibilityConfig::GetSyncVectorString(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    ani_string retResult = nullptr;
    std::vector<std::string> multiList;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.GetShortkeyMultiTarget(multiList);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }

    return ANIUtils::CreateArray(env, multiList);
}

void ANIAccessibilityConfig::SetSyncDaltonizationColorFilter(ani_env *env, ani_object object,
    ani_enum_item id, ani_string value)
{
    (void)id;
    std::string nameStr = ANIUtils::ANIStringToStdString(env, value);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto filter = ConvertStringToDaltonizationTypes(nameStr);
    auto ret = instance.SetDaltonizationColorFilter(filter);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncDaltonizationColorFilter ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_string ANIAccessibilityConfig::GetSyncDaltonizationColorFilter(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    ani_string retResult = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::DALTONIZATION_TYPE type;
    auto ret = instance.GetDaltonizationColorFilter(type);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    std::string result = ConvertDaltonizationTypeToString(type);
    env->String_NewUTF8(result.c_str(), result.size(), &retResult);
    return retResult;
}

void ANIAccessibilityConfig::SetSyncClickResponseTime(ani_env *env, ani_object object,
    ani_enum_item id, ani_string value)
{
    (void)id;
    std::string nameStr = ANIUtils::ANIStringToStdString(env, value);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto time = ConvertStringToClickResponseTimeTypes(nameStr);
    auto ret = instance.SetClickResponseTime(time);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncClickResponseTime ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_string ANIAccessibilityConfig::GetSyncClickResponseTime(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    ani_string retResult = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();

    OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME time;
    auto ret = instance.GetClickResponseTime(time);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    std::string result = ConvertClickResponseTimeTypeToString(time);
    env->String_NewUTF8(result.c_str(), result.size(), &retResult);
    return retResult;
}

void ANIAccessibilityConfig::SetSyncRepeatClickInterval(ani_env *env, ani_object object,
    ani_enum_item id, ani_string value)
{
    (void)id;
    std::string nameStr = ANIUtils::ANIStringToStdString(env, value);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto time = ConvertStringToIgnoreRepeatClickTimeTypes(nameStr);
    auto ret = instance.SetIgnoreRepeatClickTime(time);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncRepeatClickInterval ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_string ANIAccessibilityConfig::GetSyncRepeatClickInterval(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    ani_string retResult = nullptr;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();

    OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME time;
    auto ret = instance.GetIgnoreRepeatClickTime(time);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    std::string result = ConvertIgnoreRepeatClickTimeTypeToString(time);
    env->String_NewUTF8(result.c_str(), result.size(), &retResult);
    return retResult;
}

bool ANIAccessibilityConfig::GetColorMember(ani_env *env, ani_object object, const char* name, uint32_t &color)
{
    color = 0;
    ani_ref ref;
    if (ANI_OK != env->Object_GetPropertyByName_Ref(object, name, &ref)) {
        HILOG_ERROR("Get property '%{public}s' failed", name);
        return false;
    }

    ani_boolean isUndefined;
    if (ANI_OK != env->Reference_IsUndefined(ref, &isUndefined)) {
        HILOG_ERROR("Reference IsUndefined failed");
        return false;
    }
    if (isUndefined) {
        return false;
    }
    ani_class stringClass;
    env->FindClass("std.core.String", &stringClass);
    ani_boolean isString;
    env->Object_InstanceOf(static_cast<ani_object>(ref), stringClass, &isString);
    if (isString) {
        auto stringContent = ANIUtils::ANIStringToStdString(env, static_cast<ani_string>(ref));
        color = ConvertColorStringToNumber(stringContent);
        HILOG_INFO("color stringContent = %{public}s, color = %{public}u", stringContent.c_str(), color);
    } else {
        ani_int valueInt;
        if (ANI_OK != env->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "toInt", ":i", &valueInt)) {
            HILOG_ERROR(" Unbox Int failed");
            return false;
        }
        color = static_cast<uint32_t>(valueInt);
        HILOG_INFO("color intContent = %{public}u", color);
    }
    return true;
}

void ANIAccessibilityConfig::SetSyncCaptionsStyle(ani_env *env, ani_object object, ani_enum_item id, ani_object value)
{
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    OHOS::AccessibilityConfig::CaptionProperty captionProperty;

    std::string fontFamily = DEFAULT_FONT_FAMILY;
    uint32_t fontScale = DEFAULT_FONT_SCALE;
    uint32_t fontColor = DEFAULT_COLOR;
    std::string fontEdgeType = DEFAULT_FONT_EDGE_TYPE;
    uint32_t backgroundColor = DEFAULT_COLOR;
    uint32_t windowColor = DEFAULT_COLOR;
    ani_boolean isUndefined = true;
    if (ANI_OK != env->Reference_IsUndefined(value, &isUndefined)) {
        HILOG_ERROR(" SetSyncCaptionsStyle Reference_IsUndefined");
        return;
    }

    if (!isUndefined) {
        ANIUtils::GetStringMember(env, value, "fontFamily", fontFamily);
        GetColorMember(env, value, "fontColor", fontColor);
        ANIUtils::GetStringMember(env, value, "fontEdgeType", fontEdgeType);
        GetColorMember(env, value, "backgroundColor", backgroundColor);
        GetColorMember(env, value, "windowColor", windowColor);
        int value1;
        if (ANI_OK != env->Object_GetPropertyByName_Int(value, "fontScale", &value1)) {
            HILOG_ERROR(" Get property value1  failed");
        }
        fontScale = static_cast<uint32_t>(value1);
    }

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = RET_OK;
    if (configId == CONFIG_ID::CONFIG_CAPTION_STYLE) {
        captionProperty.SetFontFamily(fontFamily);
        captionProperty.SetFontScale(fontScale);
        captionProperty.SetFontColor(fontColor);
        captionProperty.SetFontEdgeType(fontEdgeType);
        captionProperty.SetBackgroundColor(backgroundColor);
        captionProperty.SetWindowColor(windowColor);
        ret = instance.SetCaptionsProperty(captionProperty);
    }
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetSyncCaptionsStyle ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_object ANIAccessibilityConfig::CreateJsCaptionPropertyInfoInner(ani_env *env, ani_class cls,
    ani_object &object, OHOS::AccessibilityConfig::CaptionProperty &captionProperty)
{
    if (env == nullptr || cls == nullptr || object == nullptr) {
        HILOG_ERROR(" invalid args");
        return nullptr;
    }
    if (!ANIUtils::SetStringProperty(env, object, "fontFamily", captionProperty.GetFontFamily())) {
        HILOG_ERROR(" set fontFamily failed");
    }
    if (!ANIUtils::SetStringProperty(env, object, "fontEdgeType", captionProperty.GetFontEdgeType())) {
        HILOG_ERROR(" set fontEdgeType failed");
    }
    if (ANI_OK != env->Object_SetPropertyByName_Int(object, "fontScale", captionProperty.GetFontScale())) {
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

ani_object ANIAccessibilityConfig::CreateJsAccessibilityCaptionProperty(ani_env *env,
    OHOS::AccessibilityConfig::CaptionProperty &captionProperty)
{
    HILOG_DEBUG();
    arkts::ani_signature::Type className =
        arkts::ani_signature::Builder::BuildClass("@ohos.accessibility.accessibility.CaptionsStyleImpl");
    ani_class cls;
    if (ANI_OK != env->FindClass(className.Descriptor().c_str(), &cls)) {
        HILOG_ERROR(" not found class");
        return nullptr;
    }
    ani_method ctor;
    if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR(" Find method '<ctor>' failed");
        return nullptr;
    }

    ani_object object;
    if (ANI_OK != env->Object_New(cls, ctor, &object)) {
        HILOG_ERROR(" New object fail");
        return nullptr;
    }

    return CreateJsCaptionPropertyInfoInner(env, cls, object, captionProperty);
}

ani_object ANIAccessibilityConfig::GetSyncCaptionsStyle(ani_env *env, ani_object object, ani_enum_item id)
{
    (void)id;
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    OHOS::AccessibilityConfig::CaptionProperty captionProperty;
    auto ret = instance.GetCaptionsProperty(captionProperty);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }

    return CreateJsAccessibilityCaptionProperty(env, captionProperty);
}


void ANIAccessibilityConfig::EnableAbilitySync(ani_env *env, ani_string name, ani_array capability)
{
    std::string nameStr = ANIUtils::ANIStringToStdString(env, name);

    if (capability == nullptr) {
        HILOG_INFO("capability is null");
        return;
    }
    ani_size length;
    ani_status status = env->Array_GetLength(capability, &length);
    if (status != ANI_OK) {
        HILOG_INFO(" Object_GetPropertyByName_Double faild. status : %{public}d", status);
        return;
    }

    std::vector<std::string> strings;
    for (ani_size i = 0; i < length; i++) {
        ani_ref stringRef;
        auto signature = arkts::ani_signature::SignatureBuilder().AddInt().SetReturnUndefined()
            .BuildSignatureDescriptor();
        if (ANI_OK != env->Array_Get(capability, i, &stringRef)) {
            HILOG_ERROR("Object_CallMethodByName_Ref Failed");
            return;
        }
        strings.emplace_back(ANIUtils::ANIStringToStdString(env, static_cast<ani_string>(stringRef)));
    }
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.EnableAbility(nameStr, ParseCapabilitiesFromVec(strings));
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("EnableAbilitySync ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

void ANIAccessibilityConfig::DisableAbilitySync(ani_env *env, ani_string name)
{
    std::string nameStr = ANIUtils::ANIStringToStdString(env, name);
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.DisableAbility(nameStr);
    if (ret != RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("DisableAbilitySync ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

bool ANIAccessibilityConfig::CheckReadPermission(const std::string &permission)
{
    HILOG_DEBUG();
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    int result = TypePermissionState::PERMISSION_GRANTED;
    ATokenTypeEnum tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenType == TOKEN_INVALID) {
        HILOG_WARN("AccessToken type invalid!");
        return false;
    } else {
        result = AccessTokenKit::VerifyAccessToken(tokenId, permission);
    }
    if (result == TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID denied!");
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, tokenId, permission.c_str());
    return true;
}

bool ANIAccessibilityConfig::IsAvailable(ani_env *env)
{
    HILOG_DEBUG();
    if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NOT_SYSTEM_APP));
        HILOG_ERROR("is not system app");
        return false;
    }

    if (!CheckReadPermission(OHOS_PERMISSION_READ_ACCESSIBILITY_CONFIG)) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(RET_ERR_NO_PERMISSION));
        HILOG_ERROR("have no read permission");
        return false;
    }

    return true;
}

void ANIAccessibilityConfig::SubscribeState(ani_env *env, ani_string type, ani_object observer)
{
    if (!IsAvailable(env)) {
        return;
    }
    std::string typeStr = ANIUtils::ANIStringToStdString(env, type);
    if (enableAbilityListsObservers_ == nullptr) {
        HILOG_ERROR("enableAbilityListsObservers_ is null");
        return;
    }
    if (std::strcmp(typeStr.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
        enableAbilityListsObservers_->SubscribeObserver(env, observer);
    } else if (std::strcmp(typeStr.c_str(), "installedAccessibilityListChange") == 0) {
        enableAbilityListsObservers_->SubscribeInstallObserver(env, observer);
    }
}

void ANIAccessibilityConfig::UnsubscribeState(ani_env *env, ani_string type, ani_object observer)
{
    if (!IsAvailable(env)) {
        return;
    }
    std::string typeStr = ANIUtils::ANIStringToStdString(env, type);
    HILOG_INFO("OffObserver:%{public}s", typeStr.c_str());
    if (enableAbilityListsObservers_ == nullptr) {
        HILOG_ERROR("enableAbilityListsObservers_ is null");
        return;
    }
    if (std::strcmp(typeStr.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
        enableAbilityListsObservers_->UnsubscribeObserver(env, observer);
    } else if (std::strcmp(typeStr.c_str(), "installedAccessibilityListChange") == 0) {
        enableAbilityListsObservers_->UnsubscribeInstallObserver(env, observer);
    }
}

void ANIAccessibilityConfig::UnsubscribeStates(ani_env *env, ani_string type)
{
    if (!IsAvailable(env)) {
        return;
    }
    std::string typeStr = ANIUtils::ANIStringToStdString(env, type);
    HILOG_INFO("OffObservers:%{public}s", typeStr.c_str());
    if (enableAbilityListsObservers_ == nullptr) {
        HILOG_ERROR("enableAbilityListsObservers_ is null");
        return;
    }
    if (std::strcmp(typeStr.c_str(), "enabledAccessibilityExtensionListChange") == 0) {
        enableAbilityListsObservers_->UnsubscribeObservers();
    } else if (std::strcmp(typeStr.c_str(), "installedAccessibilityListChange") == 0) {
        enableAbilityListsObservers_->UnsubscribeInstallObservers();
    }
}

void ANIAccessibilityConfig::SubscribeConfigObserver(ani_env *env, ani_object object,
    ani_enum_item id, ani_object callback)
{
    if (!IsAvailable(env)) {
        return;
    }
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    configObservers_->SubscribeObserver(env, configId, callback);
}

void ANIAccessibilityConfig::UnSubscribeConfigObservers(ani_env *env, ani_object object, ani_enum_item id)
{
    if (!IsAvailable(env)) {
        return;
    }
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    configObservers_->UnsubscribeObservers(configId);
}

void ANIAccessibilityConfig::UnSubscribeConfigObserver(ani_env *env, ani_object object,
    ani_enum_item id, ani_object observer)
{
    if (!IsAvailable(env)) {
        return;
    }
    ani_int itemEnum;
    if (ANI_OK != env->EnumItem_GetValue_Int(id, &itemEnum)) {
        HILOG_INFO("get value int ERROR");
        return;
    }
    CONFIG_ID configId = static_cast<CONFIG_ID>(itemEnum);
    configObservers_->UnsubscribeObserver(env, configId, observer);
}
