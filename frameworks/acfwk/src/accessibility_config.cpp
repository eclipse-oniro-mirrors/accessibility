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

void AccessibilityConfig::SubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer, const bool retFlag)
{
    CHECK_IMPL_PTR()
    pImpl_->SubscribeConfigObserver(id, observer, retFlag);
}

void AccessibilityConfig::UnsubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    CHECK_IMPL_PTR()
    pImpl_->UnsubscribeConfigObserver(id, observer);
}

void AccessibilityConfig::SubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    CHECK_IMPL_PTR()
    pImpl_->SubscribeEnableAbilityListsObserver(observer);
}

void AccessibilityConfig::UnsubscribeEnableAbilityListsObserver(
    const std::shared_ptr<AccessibilityEnableAbilityListsObserver> &observer)
{
    CHECK_IMPL_PTR()
    pImpl_->UnsubscribeEnableAbilityListsObserver(observer);
}

bool AccessibilityConfig::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->EnableAbility(name, capabilities);
}

bool AccessibilityConfig::DisableAbility(const std::string &name)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->DisableAbility(name);
}

void AccessibilityConfig::SetScreenMagnificationState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetScreenMagnificationState(state);
}

void AccessibilityConfig::SetShortKeyState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetShortKeyState(state);
}

void AccessibilityConfig::SetMouseKeyState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetMouseKeyState(state);
}

void AccessibilityConfig::SetCaptionsState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetCaptionsState(state);
}

void AccessibilityConfig::SetCaptionsProperty(const CaptionProperty &caption)
{
    CHECK_IMPL_PTR()
    pImpl_->SetCaptionsProperty(caption);
}

void AccessibilityConfig::SetMouseAutoClick(const int32_t time)
{
    CHECK_IMPL_PTR()
    pImpl_->SetMouseAutoClick(time);
}

void AccessibilityConfig::SetShortkeyTarget(const std::string &name)
{
    CHECK_IMPL_PTR()
    pImpl_->SetShortkeyTarget(name);
}

void AccessibilityConfig::SetHighContrastTextState(bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetHighContrastTextState(state);
}

void AccessibilityConfig::SetInvertColorState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetInvertColorState(state);
}

void AccessibilityConfig::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    CHECK_IMPL_PTR()
    pImpl_->SetDaltonizationColorFilter(type);
}

void AccessibilityConfig::SetContentTimeout(const uint32_t timer)
{
    CHECK_IMPL_PTR()
    pImpl_->SetContentTimeout(timer);
}

void AccessibilityConfig::SetAnimationOffState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetAnimationOffState(state);
}

void AccessibilityConfig::SetBrightnessDiscount(const float brightness)
{
    CHECK_IMPL_PTR()
    pImpl_->SetBrightnessDiscount(brightness);
}

void AccessibilityConfig::SetAudioMonoState(const bool state)
{
    CHECK_IMPL_PTR()
    pImpl_->SetAudioMonoState(state);
}

void AccessibilityConfig::SetAudioBalance(const float balance)
{
    CHECK_IMPL_PTR()
    pImpl_->SetAudioBalance(balance);
}

void AccessibilityConfig::GetScreenMagnificationState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetScreenMagnificationState(state);
}

void AccessibilityConfig::GetShortKeyState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetShortKeyState(state);
}

void AccessibilityConfig::GetMouseKeyState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetMouseKeyState(state);
}

void AccessibilityConfig::GetCaptionsState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetCaptionsState(state);
}

void AccessibilityConfig::GetCaptionsProperty(CaptionProperty &caption) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetCaptionsProperty(caption);
}

void AccessibilityConfig::GetMouseAutoClick(int32_t &time) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetMouseAutoClick(time);
}

void AccessibilityConfig::GetShortkeyTarget(std::string &name) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetShortkeyTarget(name);
}

void AccessibilityConfig::GetInvertColorState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetInvertColorState(state);
}

void AccessibilityConfig::GetHighContrastTextState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetHighContrastTextState(state);
}

void AccessibilityConfig::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetDaltonizationColorFilter(type);
}

void AccessibilityConfig::GetContentTimeout(uint32_t &timer) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetContentTimeout(timer);
}

void AccessibilityConfig::GetAnimationOffState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetAnimationOffState(state);
}

void AccessibilityConfig::GetBrightnessDiscount(float &brightness) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetBrightnessDiscount(brightness);
}

void AccessibilityConfig::GetAudioMonoState(bool &state) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetAudioMonoState(state);
}

void AccessibilityConfig::GetAudioBalance(float &balance) const
{
    CHECK_IMPL_PTR()
    pImpl_->GetAudioBalance(balance);
}
} // Accessibility
} // OHOS