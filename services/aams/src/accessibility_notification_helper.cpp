/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "accessibility_notification_helper.h"
#include "accessible_ability_manager_service.h"
#include "magnification_window_proxy.h"
 
namespace OHOS {
namespace Accessibility {
 
namespace {
    const std::string INGORE_REPEAT_CLICK_KEY = "ignore_repeat_click_switch";
    const std::string IGNORE_REPEAT_CLICK_NOTIFICATION = "ignore_repeat_click_notification";
    const std::string TRANSITION_ANIMATIONS_NOTIFICATION = "transition_animations_notification";
    const std::string ANIMATION_OFF_KEY = "animation_off";
}  // namespace


void IgnoreRepeatClickNotification::CancelNotification()
{
    if (MagnificationWindowProxy::GetInstance()) {
        MagnificationWindowProxy::GetInstance()->CancelNotification();
    }
}
 
bool IgnoreRepeatClickNotification::IsSendIgnoreRepeatClickNotification()
{
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return false;
    }
    bool notificationState = true;
    bool ignoreRepeatClickState = false;
    if (accountData->GetConfig() && accountData->GetConfig()->GetDbHandle()) {
        notificationState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(IGNORE_REPEAT_CLICK_NOTIFICATION, true);
        ignoreRepeatClickState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(INGORE_REPEAT_CLICK_KEY, false);
    }
    return ignoreRepeatClickState && notificationState;
}
 
int32_t IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder()
{
    if (!IsSendIgnoreRepeatClickNotification()) {
        return 0;
    }
    if (MagnificationWindowProxy::GetInstance()) {
        return MagnificationWindowProxy::GetInstance()->PublishIgnoreRepeatClickReminder();
    }
    return -1;
}
 
int32_t IgnoreRepeatClickNotification::RegisterTimers(uint64_t beginTime)
{
    if (!IsSendIgnoreRepeatClickNotification()) {
        return 0;
    }
    if (MagnificationWindowProxy::GetInstance()) {
        return MagnificationWindowProxy::GetInstance()->RegisterTimers(beginTime);
    }
    return -1;
}
 
void IgnoreRepeatClickNotification::DestroyTimers()
{
    if (MagnificationWindowProxy::GetInstance()) {
        MagnificationWindowProxy::GetInstance()->DestroyTimers();
    }
}

int64_t IgnoreRepeatClickNotification::GetWallTimeMs()
{
    if (MagnificationWindowProxy::GetInstance()) {
        return MagnificationWindowProxy::GetInstance()->GetWallTimeMs();
    }
    return 0;
}

int32_t TransitionAnimationsNotification::PublishTransitionAnimationsReminder()
{
    if (!IsSendTransitionAnimationsNotification()) {
        return 0;
    }
    if (MagnificationWindowProxy::GetInstance()) {
        return MagnificationWindowProxy::GetInstance()->PublishTransitionAnimationsReminder();
    }
    return -1;
}
 
void TransitionAnimationsNotification::CancelNotification()
{
    if (MagnificationWindowProxy::GetInstance()) {
        MagnificationWindowProxy::GetInstance()->TransitionAnimationsCancelNotification();
    }
}
 
int32_t TransitionAnimationsNotification::RegisterTimers(uint64_t beginTime)
{
    if (!IsSendTransitionAnimationsNotification()) {
        return 0;
    }
    if (MagnificationWindowProxy::GetInstance()) {
        return MagnificationWindowProxy::GetInstance()->TransitionAnimationsRegisterTimers(beginTime);
    }
    return 0;
}
 
void TransitionAnimationsNotification::DestroyTimers()
{
    if (MagnificationWindowProxy::GetInstance()) {
        MagnificationWindowProxy::GetInstance()->TransitionAnimationsDestroyTimers();
    }
}
 
bool TransitionAnimationsNotification::IsSendTransitionAnimationsNotification()
{
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return false;
    }
    bool notificationState = true;
    bool animationOffState = false;
    if (accountData->GetConfig() && accountData->GetConfig()->GetDbHandle()) {
        notificationState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(TRANSITION_ANIMATIONS_NOTIFICATION, true);
        animationOffState =
            accountData->GetConfig()->GetDbHandle()->GetBoolValue(ANIMATION_OFF_KEY, false);
    }
    return animationOffState && notificationState;
}
}  // namespace Accessibility
}  // namespace OHOS