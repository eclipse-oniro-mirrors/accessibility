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

#include "accessibility_config.h"
#include "accessibility_config_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AccessibilityConfig {
#define CHECK_IMPL_PTR(ret_expr)            \
    if (!pImpl_) {                          \
        HILOG_ERROR("pImpl_ is nullptr");   \
        return ret_expr;                    \
    }

ACCESSIBILITY_DEFINE_SINGLETON(AccessibilityConfig)

AccessibilityConfig::AccessibilityConfig() : pImpl_(std::make_unique<Impl>())
{
    if (!pImpl_) {
        HILOG_ERROR("Create AccessibilityConfigImpl Failed");
    }
}

AccessibilityConfig::~AccessibilityConfig()
{
}

bool AccessibilityConfig::InitializeContext()
{
    CHECK_IMPL_PTR(false)
    return pImpl_->InitializeContext();
}

Accessibility::RetError AccessibilityConfig::SubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer, const bool retFlag)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SubscribeConfigObserver(id, observer, retFlag);
}

Accessibility::RetError AccessibilityConfig::UnsubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->UnsubscribeConfigObserver(id, observer);
}

Accessibility::RetError AccessibilityConfig::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SubscribeEnableAbilityListsObserver(observer);
}

Accessibility::RetError AccessibilityConfig::UnsubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->UnsubscribeEnableAbilityListsObserver(observer);
}

Accessibility::RetError AccessibilityConfig::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->EnableAbility(name, capabilities);
}

Accessibility::RetError AccessibilityConfig::DisableAbility(const std::string &name)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->DisableAbility(name);
}

Accessibility::RetError AccessibilityConfig::SetScreenMagnificationState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetScreenMagnificationState(state);
}

Accessibility::RetError AccessibilityConfig::SetShortKeyState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetShortKeyState(state);
}

Accessibility::RetError AccessibilityConfig::SetMouseKeyState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetMouseKeyState(state);
}

Accessibility::RetError AccessibilityConfig::SetCaptionsState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetCaptionsState(state);
}

Accessibility::RetError AccessibilityConfig::SetCaptionsProperty(const CaptionProperty &caption)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetCaptionsProperty(caption);
}

Accessibility::RetError AccessibilityConfig::SetMouseAutoClick(const int32_t time)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetMouseAutoClick(time);
}

Accessibility::RetError AccessibilityConfig::SetShortkeyTarget(const std::string &name)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetShortkeyTarget(name);
}

Accessibility::RetError AccessibilityConfig::SetHighContrastTextState(bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetHighContrastTextState(state);
}

Accessibility::RetError AccessibilityConfig::SetInvertColorState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetInvertColorState(state);
}

Accessibility::RetError AccessibilityConfig::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetDaltonizationColorFilter(type);
}

Accessibility::RetError AccessibilityConfig::SetContentTimeout(const uint32_t timer)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetContentTimeout(timer);
}

Accessibility::RetError AccessibilityConfig::SetAnimationOffState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetAnimationOffState(state);
}

Accessibility::RetError AccessibilityConfig::SetBrightnessDiscount(const float brightness)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetBrightnessDiscount(brightness);
}

Accessibility::RetError AccessibilityConfig::SetAudioMonoState(const bool state)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetAudioMonoState(state);
}

Accessibility::RetError AccessibilityConfig::SetAudioBalance(const float balance)
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->SetAudioBalance(balance);
}

Accessibility::RetError AccessibilityConfig::GetScreenMagnificationState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetScreenMagnificationState(state);
}

Accessibility::RetError AccessibilityConfig::GetShortKeyState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetShortKeyState(state);
}

Accessibility::RetError AccessibilityConfig::GetMouseKeyState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetMouseKeyState(state);
}

Accessibility::RetError AccessibilityConfig::GetCaptionsState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetCaptionsState(state);
}

Accessibility::RetError AccessibilityConfig::GetCaptionsProperty(CaptionProperty &caption) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetCaptionsProperty(caption);
}

Accessibility::RetError AccessibilityConfig::GetMouseAutoClick(int32_t &time) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetMouseAutoClick(time);
}

Accessibility::RetError AccessibilityConfig::GetShortkeyTarget(std::string &name) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetShortkeyTarget(name);
}

Accessibility::RetError AccessibilityConfig::GetInvertColorState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetInvertColorState(state);
}

Accessibility::RetError AccessibilityConfig::GetHighContrastTextState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetHighContrastTextState(state);
}

Accessibility::RetError AccessibilityConfig::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetDaltonizationColorFilter(type);
}

Accessibility::RetError AccessibilityConfig::GetContentTimeout(uint32_t &timer) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetContentTimeout(timer);
}

Accessibility::RetError AccessibilityConfig::GetAnimationOffState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetAnimationOffState(state);
}

Accessibility::RetError AccessibilityConfig::GetBrightnessDiscount(float &brightness) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetBrightnessDiscount(brightness);
}

Accessibility::RetError AccessibilityConfig::GetAudioMonoState(bool &state) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetAudioMonoState(state);
}

Accessibility::RetError AccessibilityConfig::GetAudioBalance(float &balance) const
{
    CHECK_IMPL_PTR(Accessibility::RET_ERR_NULLPTR)
    return pImpl_->GetAudioBalance(balance);
}
} // Accessibility
} // OHOS