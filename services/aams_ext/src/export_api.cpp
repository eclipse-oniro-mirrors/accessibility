/*
* Copyright (C) 2025-2026 Huawei Device Co., Ltd.
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
// LCOV_EXCL_START
#include "accessibility_notification_helper.h"
#include "visibility.h"
#include "time_service_client.h"
#include "extend_service_manager.h"
#include "accessibility_extend_power_manager.h"
#include "magnification_manager.h"
 
OHOS::Accessibility::ExtendServiceManager &serviceManagerInstance =
  OHOS::Singleton<OHOS::Accessibility::ExtendServiceManager>::GetInstance();

extern "C" {
// set sa callback function
API_EXPORT void SetSendAccessibilityEventToAACallback(
    OHOS::Accessibility::SendAccessibilityEventToAACallback cb)
{
    serviceManagerInstance.sendAccessibilityEventToAACallback = cb;
}
API_EXPORT void SetGetDelayTimeCallback(OHOS::Accessibility::GetDelayTime cb)
{
    serviceManagerInstance.getDelayTime = cb;
}
API_EXPORT void SetGetMagnificationStateCallback(OHOS::Accessibility::GetMagnificationState cb)
{
    serviceManagerInstance.getMagnificationState = cb;
}
API_EXPORT void SetFindFocusedElementCallback(
    OHOS::Accessibility::FindFocusedElementCallback cb)
{
    serviceManagerInstance.findFocusedElementCallback = cb;
}
API_EXPORT void SetExecuteActionOnAccessibilityFocusedCallback(
    OHOS::Accessibility::ExecuteActionOnAccessibilityFocusedCallback cb)
{
    serviceManagerInstance.executeActionOnAccessibilityFocusedCallback = cb;
}
API_EXPORT void SetGetFocusedWindowIdCallback(
    OHOS::Accessibility::GetFocusedWindowIdCallback cb)
{
    serviceManagerInstance.getFocusedWindowIdCallback = cb;
}
API_EXPORT void SetGetActiveWindowIdCallback(
    OHOS::Accessibility::GetActiveWindowIdCallback cb)
{
    serviceManagerInstance.getActiveWindowIdCallback = cb;
}
API_EXPORT void SetGetAccessibilityWindowCallback(
    OHOS::Accessibility::GetAccessibilityWindowCallback cb)
{
    serviceManagerInstance.getAccessibilityWindowCallback = cb;
}
API_EXPORT void SetSendPointerEventForHoverCallback(
    OHOS::Accessibility::SendPointerEventForHoverCallback cb)
{
    serviceManagerInstance.sendPointerEventForHoverCallback = cb;
}
API_EXPORT void SetCheckDisplayIdCallback(
    OHOS::Accessibility::CheckDisplayIdCallback cb)
{
    serviceManagerInstance.checkDisplayIdCallback = cb;
}
// method export To sa
API_EXPORT int32_t PublishIgnoreRepeatClickReminder()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
}
API_EXPORT void CancelNotification()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::CancelNotification();
}
API_EXPORT int32_t RegisterTimers(uint64_t beginTime)
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::RegisterTimers(beginTime);
}
API_EXPORT void DestroyTimers()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::DestroyTimers();
}
API_EXPORT int64_t GetWallTimeMs()
{
    if (OHOS::MiscServices::TimeServiceClient::GetInstance()) {
        return OHOS::MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
    }
    return 0;
}
API_EXPORT int32_t PublishTransitionAnimationsReminder()
{
    return OHOS::Accessibility::TransitionAnimationsNotification::PublishTransitionAnimationsReminder();
}
API_EXPORT void TransitionAnimationsCancelNotification()
{
    return OHOS::Accessibility::TransitionAnimationsNotification::CancelNotification();
}
API_EXPORT int32_t TransitionAnimationsRegisterTimers(uint64_t beginTime)
{
    return OHOS::Accessibility::TransitionAnimationsNotification::RegisterTimers(beginTime);
}
API_EXPORT void TransitionAnimationsDestroyTimers()
{
    return OHOS::Accessibility::TransitionAnimationsNotification::DestroyTimers();
}
API_EXPORT void UpdateInputFilter(uint32_t state)
{
    serviceManagerInstance.UpdateInputFilter(state);
}
API_EXPORT void GetClickPosition(int32_t &xPos, int32_t &yPos)
{
    serviceManagerInstance.GetClickPosition(xPos, yPos);
}
API_EXPORT OHOS::Accessibility::RetError SetCurtainScreenUsingStatus(bool isEnable)
{
    return serviceManagerInstance.SetCurtainScreenUsingStatus(isEnable);
}
API_EXPORT uint64_t GetDefaultDisplayId()
{
    return serviceManagerInstance.GetDefaultDisplayId();
}
API_EXPORT void SetMagnificationScale(float scale)
{
    serviceManagerInstance.SetMagnificationScale(scale);
}
API_EXPORT void SetMagnificationMode(uint32_t mode)
{
    serviceManagerInstance.SetMagnificationMode(mode);
}
API_EXPORT void SetMagnificationType(uint32_t type)
{
    serviceManagerInstance.SetMagnificationType(type);
}
API_EXPORT OHOS::Accessibility::RetError SetClickConfig(OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime,
    bool state, OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME responseTime)
{
    return serviceManagerInstance.SetClickConfig(clickTime, state, responseTime);
}
API_EXPORT OHOS::Accessibility::RetError SetMouseAutoClick(int32_t time)
{
    return serviceManagerInstance.SetMouseAutoClick(time);
}
API_EXPORT void SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum)
{
    serviceManagerInstance.SetServiceOnKeyEventResult(connectionId, isHandled, sequenceNum);
}
API_EXPORT OHOS::Accessibility::RetError InjectEvents(
    const std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>& gesturePath)
{
    return serviceManagerInstance.InjectEvents(gesturePath);
}
API_EXPORT void OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType)
{
    HILOG_INFO();
    return serviceManagerInstance.OnScreenMagnificationTypeChanged(screenMagnificationType);
}
API_EXPORT void OnScreenMagnificationStateChanged()
{
    HILOG_INFO();
    return serviceManagerInstance.OnScreenMagnificationStateChanged();
}
API_EXPORT void UnregisterDisplayListener()
{
    HILOG_INFO();
    serviceManagerInstance.UnregisterDisplayListener();
}
API_EXPORT void ExtendGetMagnificationModeCallback(OHOS::Accessibility::GetMagnificationModeCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.getMagnificationModeCallback = cb;
}
API_EXPORT void ExtendGetMagnificationScaleCallback(OHOS::Accessibility::GetMagnificationScaleCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.getMagnificationScaleCallback = cb;
}
API_EXPORT void ExtendUpdateInputFilterCallback(OHOS::Accessibility::UpdateInputFilterCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.updateInputFilterCallback = cb;
}
API_EXPORT void SetMagnificationModeCallback(OHOS::Accessibility::MagnificationModeCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.magnificationModeCallback = cb;
}
API_EXPORT void GetMagnificationTypeCallback(OHOS::Accessibility::MagnificationTypeCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.magnificationTypeCallback = cb;
}
API_EXPORT void ExtendAnnouncedForMagnificationCallback(OHOS::Accessibility::AnnouncedForMagnificationCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.announcedForMagnificationCallback = cb;
}
API_EXPORT void SetDispatchKeyEventCallback(OHOS::Accessibility::DispatchKeyEventCallback cb)
{
    serviceManagerInstance.dispatchKeyEventCallback = cb;
}
API_EXPORT bool DiscountBrightness(const float discount)
{
#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    return OHOS::Singleton<OHOS::Accessibility::AccessibilityExtendPowerManager>::GetInstance().DiscountBrightness(
        discount);
#else
    return true;
#endif
}
API_EXPORT void SetMagnificationScaleCallback(OHOS::Accessibility::MagnificationScaleCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.magnificationScaleCallback = cb;
}
API_EXPORT void ExtendGetAccessibilityWindowsCallback(OHOS::Accessibility::GetAccessibilityWindowsCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.getAccessibilityWindowsCallback = cb;
}
API_EXPORT void FollowFocuseElement(const int32_t centerX, const int32_t centerY)
{
    HILOG_INFO();
    OHOS::Singleton<OHOS::Accessibility::MagnificationManager>::GetInstance().FollowFocuseElement(centerX, centerY);
}
API_EXPORT void OffZoomGesture()
{
    HILOG_INFO();
    serviceManagerInstance.OffZoomGesture();
}
API_EXPORT void ExtendSubscribeOsAccountCallback(OHOS::Accessibility::SubscribeOsAccountCallback cb)
{
    HILOG_INFO();
    serviceManagerInstance.subscribeOsAccountCallback = cb;
}
API_EXPORT void SetMagnificationState(bool state, uint32_t type, uint32_t mode)
{
    HILOG_INFO();
    serviceManagerInstance.SetMagnificationState(state, type, mode);
}
}
// LCOV_EXCL_STOP