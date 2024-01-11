/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include <string>
#include <hitrace_meter.h>
#include "accessibility_account_data.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_settings.h"
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
#include "accessibility_power_manager.h"
#endif
#include "hilog_wrapper.h"
#include "parameter.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string GRAPHIC_ANIMATION_SCALE_NAME = "persist.sys.graphic.animationscale";
    const std::string ARKUI_ANIMATION_SCALE_NAME = "persist.sys.arkui.animationscale";
    const std::string SCREEN_READER_BUNDLE_ABILITY_NAME = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
}

void AccessibilitySettings::RegisterSettingsHandler(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    HILOG_DEBUG();
    handler_ = handler;
}

RetError AccessibilitySettings::SetScreenMagnificationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetScreenMagnificationState:" + std::to_string(state));

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetScreenMagnificationState(state);
    UpdateConfigState();
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
    return ret;
}

RetError AccessibilitySettings::SetShortKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetShortKeyState:" + std::to_string(state));

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetShortKeyState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
        }), "TASK_SET_SHORTKEY_STATE");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetMouseKeyState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetMouseKeyState(state);
    UpdateConfigState();
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
    return ret;
}

RetError AccessibilitySettings::SetMouseAutoClick(const int32_t time)
{
    HILOG_INFO("time = [%{public}d]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetMouseAutoClick(time);
        syncPromise.set_value(ret);
        UpdateMouseAutoClick();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
        }), "TASK_SET_MOUSE_AUTOCLICK");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetShortkeyTarget(const std::string &name)
{
    HILOG_INFO("name = [%{public}s]", name.c_str());
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetShortkeyTarget:" + name);

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetShortkeyTarget(name);
        syncPromise.set_value(ret);
        UpdateShortkeyTarget();
        }), "TASK_SET_SHORTKEY_TARGET");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetShortkeyMultiTarget(const std::vector<std::string> &name)
{
    HILOG_DEBUG();
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetShortkeyMultiTarget");

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetShortkeyMultiTarget(name);
        syncPromise.set_value(ret);
        UpdateShortkeyMultiTarget();
        }), "TASK_SET_SHORTKEY_MULTI_TARGET");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetHighContrastTextState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetHighContrastTextState:" + std::to_string(state));

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetHighContrastTextState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibilitySettings::SetDaltonizationState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetDaltonizationState:" + std::to_string(state));

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetDaltonizationState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibilitySettings::SetInvertColorState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetInvertColorState:" + std::to_string(state));

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetInvertColorState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibilitySettings::SetAnimationOffState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetAnimationOffState(state);
    UpdateConfigState();
    int setGraphicParamRes = -1;
    int setArkuiParamRes = -1;
    if (state) {
        setGraphicParamRes = SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "0");
        setArkuiParamRes = SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "0");
    } else {
        setGraphicParamRes = SetParameter(GRAPHIC_ANIMATION_SCALE_NAME.c_str(), "1");
        setArkuiParamRes = SetParameter(ARKUI_ANIMATION_SCALE_NAME.c_str(), "1");
    }
    HILOG_INFO("SetParameter results are %{public}d and %{public}d", setGraphicParamRes, setArkuiParamRes);
    return ret;
}

RetError AccessibilitySettings::SetAudioMonoState(const bool state)
{
    HILOG_INFO("state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetAudioMonoState:" + std::to_string(state));

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetAudioMonoState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibilitySettings::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_INFO("filter = [%{public}u]", filter);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetDaltonizationColorFilter:" + std::to_string(filter));
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, filter]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetDaltonizationColorFilter(filter);
        syncPromise.set_value(ret);
        UpdateDaltonizationColorFilter();
        }), "TASK_SET_DALTONIZATION_COLORFILTER");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetContentTimeout(const uint32_t time)
{
    HILOG_INFO("time = [%{public}u]", time);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetContentTimeout(time);
        syncPromise.set_value(ret);
        UpdateContentTimeout();
        }), "TASK_SET_CONTENT_TIMEOUT");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetBrightnessDiscount(const float discount)
{
    HILOG_INFO("discount = [%{public}f]", discount);
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    if (!Singleton<AccessibilityPowerManager>::GetInstance().DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
        return Accessibility::RET_ERR_FAILED;
    }
#endif
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, discount]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetBrightnessDiscount(discount);
        syncPromise.set_value(ret);
        UpdateBrightnessDiscount();
        }), "TASK_SET_BRIGHTNESS_DISCOUNT");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetAudioBalance(const float balance)
{
    HILOG_INFO("balance = [%{public}f]", balance);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetAudioBalance:" + std::to_string(balance));

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, balance]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetAudioBalance(balance);
        syncPromise.set_value(ret);
        UpdateAudioBalance();
        }), "TASK_SET_AUDIO_BALANCE");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetClickResponseTime(const uint32_t time)
{
    HILOG_INFO("click response time = [%{public}u]", time);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetClickResponseTime:" + std::to_string(time));

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetClickResponseTime(time);
        syncPromise.set_value(ret);
        UpdateClickResponseTime();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
        }), "TASK_SET_CLICK_RESPONSE_TIME");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetIgnoreRepeatClickState(const bool state)
{
    HILOG_INFO("ignore repeat click state = [%{public}s]", state ? "True" : "False");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetIgnoreRepeatClickState:" + std::to_string(state));

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, state]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetIgnoreRepeatClickState(state);
        syncPromise.set_value(ret);
        UpdateConfigState();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
        }), "TASK_SET_IGNORE_REPEAT_CLICK_STATE");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetIgnoreRepeatClickTime(const uint32_t time)
{
    HILOG_INFO("click response time = [%{public}u]", time);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SetIgnoreRepeatClickTime:" + std::to_string(time));

    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, time]() -> void {
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetIgnoreRepeatClickTime(time);
        syncPromise.set_value(ret);
        UpdateIgnoreRepeatClickTime();
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
        }), "TASK_SET_IGNORE_REPEAT_CLICK_TIME");
    return syncFuture.get();
}

void AccessibilitySettings::UpdateSettingsInAtoHosStatePart(ConfigValueAtoHosUpdate &atoHosValue)
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    // set
    if (atoHosValue.daltonizationState) {
        accountData->GetConfig()->SetDaltonizationState(atoHosValue.daltonizationState);
    }
    if (atoHosValue.invertColor) {
        accountData->GetConfig()->SetInvertColorState(atoHosValue.invertColor);
    }
    if (atoHosValue.audioMono) {
        accountData->GetConfig()->SetAudioMonoState(atoHosValue.audioMono);
    }
    if (atoHosValue.highContrastText) {
        accountData->GetConfig()->SetHighContrastTextState(atoHosValue.highContrastText);
    }
    if (atoHosValue.ignoreRepeatClickState) {
        accountData->GetConfig()->SetIgnoreRepeatClickState(atoHosValue.ignoreRepeatClickState);
    }
    UpdateConfigState();
}

void AccessibilitySettings::UpdateSettingsInAtoHos()
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ConfigValueAtoHosUpdate atoHosValue;
    accountData->GetConfigValueAtoHos(atoHosValue);

    HILOG_INFO("daltonizationState(%{public}d), invertColor(%{public}d), \
        audioMono(%{public}d), audioBalance(%{public}f), highContrastText(%{public}d), \
        isScreenReaderEnabled(%{public}d), ignoreRepeatClickState(%{public}d), \
        clickResponseTime(%{public}d), ignoreRepeatClickTime(%{public}d), displayDaltonizer(%{public}d).",
        atoHosValue.daltonizationState, atoHosValue.invertColor, atoHosValue.audioMono, atoHosValue.audioBalance,
        atoHosValue.highContrastText, atoHosValue.isScreenReaderEnabled, atoHosValue.ignoreRepeatClickState,
        atoHosValue.clickResponseTime, atoHosValue.ignoreRepeatClickTime, atoHosValue.displayDaltonizer);

    UpdateSettingsInAtoHosStatePart(atoHosValue);

    if (atoHosValue.audioBalance != 0.0) {
        accountData->GetConfig()->SetAudioBalance(atoHosValue.audioBalance);
        UpdateAudioBalance();
    }
    if (atoHosValue.clickResponseTime != 0) {
        accountData->GetConfig()->SetClickResponseTime(static_cast<uint32_t>(atoHosValue.clickResponseTime));
        UpdateClickResponseTime();
    }
    if (atoHosValue.ignoreRepeatClickState && atoHosValue.ignoreRepeatClickTime != 0) {
        accountData->GetConfig()->SetIgnoreRepeatClickTime(static_cast<uint32_t>(atoHosValue.ignoreRepeatClickTime));
        UpdateIgnoreRepeatClickTime();
    }
    if (atoHosValue.daltonizationState && atoHosValue.displayDaltonizer != 0) {
        accountData->GetConfig()->SetDaltonizationColorFilter(static_cast<uint32_t>(atoHosValue.displayDaltonizer));
        UpdateDaltonizationColorFilter();
    }
    
    if (atoHosValue.isScreenReaderEnabled) {
        uint32_t capabilities = CAPABILITY_GESTURE | CAPABILITY_KEY_EVENT_OBSERVER | CAPABILITY_RETRIEVE |
            CAPABILITY_TOUCH_GUIDE | CAPABILITY_ZOOM;
        accountData->EnableAbility(SCREEN_READER_BUNDLE_ABILITY_NAME, capabilities);
    }

    accountData->GetConfig()->SetStartFromAtoHosState(false);
}

RetError AccessibilitySettings::GetScreenMagnificationState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetScreenMagnificationState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SCREENMAGNIFIER_STATE");
    return syncFuture.get();
}

RetError AccessibilitySettings::GetShortKeyState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetShortKeyState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SHORTKEY_STATE");
    return syncFuture.get();
}

RetError AccessibilitySettings::GetMouseKeyState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetMouseKeyState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_MOUSEKEY_STATE");
    return syncFuture.get();
}

RetError AccessibilitySettings::GetMouseAutoClick(int32_t &time)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &time]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        time = accountData->GetConfig()->GetMouseAutoClick();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_MOUSE_AUTOCLICK");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetShortkeyTarget(std::string &name)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        name = accountData->GetConfig()->GetShortkeyTarget();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SHORTKEY_TARGET");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetShortkeyMultiTarget(std::vector<std::string> &name)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &name]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        name = accountData->GetConfig()->GetShortkeyMultiTarget();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_SHORTKEY_MULTI_TARGET");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetHighContrastTextState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetHighContrastTextState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_HIGHCONTRASTTEXT_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetDaltonizationState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetDaltonizationState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_DALTONIZATIONSTATE_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetInvertColorState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetInvertColorState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_INVERTCOLOR_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetAnimationOffState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetAnimationOffState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_ANIMATIONOFF_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetAudioMonoState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetAudioMonoState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_AUDIOMONO_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetDaltonizationColorFilter(uint32_t &type)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &type]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        type = accountData->GetConfig()->GetDaltonizationColorFilter();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_DALTONIZATION_COLORFILTER");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetContentTimeout(uint32_t &timer)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &timer]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        timer = accountData->GetConfig()->GetContentTimeout();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CONTENT_TIMEOUT");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetBrightnessDiscount(float &brightness)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &brightness]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        brightness = accountData->GetConfig()->GetBrightnessDiscount();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_BRIGHTNESS_DISCOUNT");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetAudioBalance(float &balance)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &balance]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        balance = accountData->GetConfig()->GetAudioBalance();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_AUDIO_BALANCE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetClickResponseTime(uint32_t &time)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &time]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        time = accountData->GetConfig()->GetClickResponseTime();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CLICK_RESPONSE_TIME");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetIgnoreRepeatClickState(bool &state)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetIgnoreRepeatClickState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_IGNORE_REPEAT_CLICK_STATE");

    return syncFuture.get();
}

RetError AccessibilitySettings::GetIgnoreRepeatClickTime(uint32_t &time)
{
    HILOG_DEBUG();
    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &time]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        time = accountData->GetConfig()->GetIgnoreRepeatClickTime();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_IGNORE_REPEAT_CLICK_TIME");

    return syncFuture.get();
}

void AccessibilitySettings::UpdateConfigState()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateConfigState.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t state = accountData->GetConfig()->GetConfigState();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnConfigStateChanged(state);
            }
        }
        }), "UpdateConfigState");
}

void AccessibilitySettings::UpdateAudioBalance()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateAudioBalance.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        float audioBalance = accountData->GetConfig()->GetAudioBalance();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnAudioBalanceChanged(audioBalance);
            }
        }
        }), "UpdateAudioBalance");
}

void AccessibilitySettings::UpdateBrightnessDiscount()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateBrightnessDiscount.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        float brightnessDiscount = accountData->GetConfig()->GetBrightnessDiscount();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnBrightnessDiscountChanged(brightnessDiscount);
            }
        }
        }), "UpdateBrightnessDiscount");
}

void AccessibilitySettings::UpdateContentTimeout()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateContentTimeout.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t contentTimeout = accountData->GetConfig()->GetContentTimeout();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnContentTimeoutChanged(contentTimeout);
            }
        }
        }), "UpdateContentTimeout");
}

void AccessibilitySettings::UpdateDaltonizationColorFilter()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateDaltonizationColorFilter.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t daltonizationColorFilter = accountData->GetConfig()->GetDaltonizationColorFilter();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnDaltonizationColorFilterChanged(daltonizationColorFilter);
            }
        }
        }), "UpdateDaltonizationColorFilter");
}

void AccessibilitySettings::UpdateMouseAutoClick()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateMouseAutoClick.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        int32_t mouseAutoClick = accountData->GetConfig()->GetMouseAutoClick();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnMouseAutoClickChanged(mouseAutoClick);
            }
        }
        }), "UpdateMouseAutoClick");
}

void AccessibilitySettings::UpdateShortkeyTarget()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateShortkeyTarget.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        std::string shortkeyTarget = accountData->GetConfig()->GetShortkeyTarget();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnShortkeyTargetChanged(shortkeyTarget);
            }
        }
        }), "UpdateShortkeyTarget");
}

void AccessibilitySettings::UpdateShortkeyMultiTarget()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateShortkeyMultiTarget.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        std::vector<std::string> shortkeyMultiTarget = accountData->GetConfig()->GetShortkeyMultiTarget();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnShortkeyMultiTargetChanged(shortkeyMultiTarget);
            }
        }
        }), "UpdateShortkeyMultiTarget");
}

void AccessibilitySettings::UpdateClickResponseTime()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateClickResponseTime.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t time = accountData->GetConfig()->GetClickResponseTime();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnClickResponseTimeChanged(time);
            }
        }
        }), "UpdateClickResponseTime");
}

void AccessibilitySettings::UpdateIgnoreRepeatClickTime()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_INFO("UpdateIgnoreRepeatClickTime.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        uint32_t time = accountData->GetConfig()->GetIgnoreRepeatClickTime();
        for (auto &callback : accountData->GetConfigCallbacks()) {
            if (callback) {
                callback->OnIgnoreRepeatClickTimeChanged(time);
            }
        }
        }), "UpdateIgnoreRepeatClickTime");
}

RetError AccessibilitySettings::GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &caption]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        caption = accountData->GetConfig()->GetCaptionProperty();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CAPTION_PROPERTY");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &caption]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr.");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        RetError ret = accountData->GetConfig()->SetCaptionProperty(caption);
        syncPromise.set_value(ret);
        UpdateCaptionProperty();
        }), "TASK_SET_CAPTION_PROPERTY");
    return syncFuture.get();
}

RetError AccessibilitySettings::SetCaptionState(const bool state)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return RET_ERR_NULLPTR;
    }
    RetError ret = accountData->GetConfig()->SetCaptionState(state);
    UpdateConfigState();
    return ret;
}

RetError AccessibilitySettings::GetCaptionState(bool &state)
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::promise<RetError> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    handler_->PostTask(std::bind([this, &syncPromise, &state]() -> void {
        HILOG_DEBUG();
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("accountData is nullptr");
            syncPromise.set_value(RET_ERR_NULLPTR);
            return;
        }
        state = accountData->GetConfig()->GetCaptionState();
        syncPromise.set_value(RET_OK);
        }), "TASK_GET_CAPTION_STATE");
    return syncFuture.get();
}

void AccessibilitySettings::UpdateCaptionProperty()
{
    handler_->PostTask(std::bind([this]() -> void {
        HILOG_DEBUG("UpdateCaptionProperty.");
        sptr<AccessibilityAccountData> accountData =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
        if (!accountData) {
            HILOG_ERROR("Account data is null");
            return;
        }

        AccessibilityConfig::CaptionProperty caption = accountData->GetConfig()->GetCaptionProperty();
        for (auto &callback : accountData->GetCaptionPropertyCallbacks()) {
            if (callback) {
                callback->OnPropertyChanged(caption);
            }
        }
        }), "UpdateCaptionProperty");
}

void AccessibilitySettings::UpdateAllSetting()
{
    HILOG_DEBUG();
    if (!handler_) {
        HILOG_ERROR("UpdateAllSetting: handler is nullptr!");
        return;
    }
    handler_->PostTask(std::bind([this]() -> void {
        UpdateConfigState();
        UpdateShortkeyTarget();
        UpdateShortkeyMultiTarget();
        UpdateMouseAutoClick();
        UpdateDaltonizationColorFilter();
        UpdateContentTimeout();
        UpdateBrightnessDiscount();
        UpdateAudioBalance();
        UpdateClickResponseTime();
        UpdateIgnoreRepeatClickTime();
        }), "UPDATE_ALL_SETTING");
}
} // namespace Accessibility
} // namespace OHOS