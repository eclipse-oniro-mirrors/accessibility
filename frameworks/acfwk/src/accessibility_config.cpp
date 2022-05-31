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

void AccessibilityConfig::SubscribeConfigObserver(const CONFIG_ID id,
    const std::shared_ptr<AccessibilityConfigObserver> &observer)
{
    CHECK_IMPL_PTR()
    pImpl_->SubscribeConfigObserver(id, observer);
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

bool AccessibilityConfig::SetScreenMagnificationState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetScreenMagnificationState(state);
}

bool AccessibilityConfig::SetShortKeyState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetShortKeyState(state);
}

bool AccessibilityConfig::SetMouseKeyState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetMouseKeyState(state);
}

bool AccessibilityConfig::SetCaptionState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetCaptionState(state);
}

bool AccessibilityConfig::SetCaptionProperty(const CaptionProperty &caption)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetCaptionProperty(caption);
}

bool AccessibilityConfig::SetMouseAutoClick(const int32_t time)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetMouseAutoClick(time);
}

bool AccessibilityConfig::SetShortkeyTarget(const std::string &name)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetShortkeyTarget(name);
}

bool AccessibilityConfig::SetHighContrastTextState(bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetHighContrastTextState(state);
}

bool AccessibilityConfig::SetInvertColorState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetInvertColorState(state);
}

bool AccessibilityConfig::SetDaltonizationColorFilter(const DALTONIZATION_TYPE type)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetDaltonizationColorFilter(type);
}

bool AccessibilityConfig::SetContentTimeout(const uint32_t timer)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetContentTimeout(timer);
}

bool AccessibilityConfig::SetAnimationOffState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetAnimationOffState(state);
}

bool AccessibilityConfig::SetBrightnessDiscount(const float brightness)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetBrightnessDiscount(brightness);
}

bool AccessibilityConfig::SetAudioMonoState(const bool state)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetAudioMonoState(state);
}

bool AccessibilityConfig::SetAudioBalance(const float balance)
{
    CHECK_IMPL_PTR(false)
    return pImpl_->SetAudioBalance(balance);
}

bool AccessibilityConfig::GetScreenMagnificationState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetScreenMagnificationState(state);
}

bool AccessibilityConfig::GetShortKeyState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetShortKeyState(state);
}

bool AccessibilityConfig::GetMouseKeyState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetMouseKeyState(state);
}

bool AccessibilityConfig::GetCaptionState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetCaptionState(state);
}

bool AccessibilityConfig::GetCaptionProperty(CaptionProperty &caption) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetCaptionProperty(caption);
}

bool AccessibilityConfig::GetMouseAutoClick(int32_t &time) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetMouseAutoClick(time);
}

bool AccessibilityConfig::GetShortkeyTarget(std::string &name) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetShortkeyTarget(name);
}

bool AccessibilityConfig::GetInvertColorState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetInvertColorState(state);
}

bool AccessibilityConfig::GetHighContrastTextState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetHighContrastTextState(state);
}

bool AccessibilityConfig::GetDaltonizationColorFilter(DALTONIZATION_TYPE &type) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetDaltonizationColorFilter(type);
}

bool AccessibilityConfig::GetContentTimeout(uint32_t &timer) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetContentTimeout(timer);
}

bool AccessibilityConfig::GetAnimationOffState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetAnimationOffState(state);
}

bool AccessibilityConfig::GetBrightnessDiscount(float &brightness) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetBrightnessDiscount(brightness);
}

bool AccessibilityConfig::GetAudioMonoState(bool &state) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetAudioMonoState(state);
}

bool AccessibilityConfig::GetAudioBalance(float &balance) const
{
    CHECK_IMPL_PTR(false)
    return pImpl_->GetAudioBalance(balance);
}
} // Accessibility
} // OHOS