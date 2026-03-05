/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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

#include "mock_accessible_extend_manager_service_proxy.h"

namespace OHOS {
namespace Accessibility {
namespace {
}

ExtendManagerServiceProxy::ExtendManagerServiceProxy()
{
}

ExtendManagerServiceProxy::~ExtendManagerServiceProxy()
{
    RemoveExtProxy();
}

bool ExtendManagerServiceProxy::LoadExtProxy()
{
    return true;
}

bool ExtendManagerServiceProxy::RemoveExtProxy()
{
    return true;
}

bool ExtendManagerServiceProxy::SetGetDelayTimeCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetGetMagnificationStateCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetFindFocusedElementCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetExecuteActionOnAccessibilityFocusedCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetGetFocusedWindowIdCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetGetActiveWindowIdCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetGetAccessibilityWindowCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetSendPointerEventForHoverCallback()
{
    return true;
}

void *ExtendManagerServiceProxy::GetFunc(const std::string &funcName)
{
    (void)funcName;
    return nullptr;
}

bool ExtendManagerServiceProxy::SetSendAccessibilityEventToAACallback()
{
    return true;
}

bool ExtendManagerServiceProxy::SetDispatchKeyEventCallback()
{
    return true;
}

RetError ExtendManagerServiceProxy::InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    (void)gesturePath;
    return RET_OK;
}

void ExtendManagerServiceProxy::SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum)
{
    (void)connectionId;
    (void)isHandled;
    (void)sequenceNum;
    return;
}

RetError ExtendManagerServiceProxy::SetMouseAutoClick(int32_t time)
{
    (void)time;
    return RET_OK;
}

RetError ExtendManagerServiceProxy::SetClickConfig(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
        AccessibilityConfig::CLICK_RESPONSE_TIME responseTime)
{
    (void)clickTime;
    (void)state;
    (void)responseTime;
    return RET_OK;
}

void ExtendManagerServiceProxy::SetMagnificationMode(uint32_t mode)
{
    (void)mode;
    return;
}

void ExtendManagerServiceProxy::SetMagnificationType(uint32_t type)
{
    (void)type;
    return;
}

void ExtendManagerServiceProxy::SetMagnificationScale(float scale)
{
    (void)scale;
    return;
}

void ExtendManagerServiceProxy::UpdateInputFilter(uint32_t flag)
{
    (void)flag;
    return;
}

RetError ExtendManagerServiceProxy::SetCurtainScreenUsingStatus(bool isEnable)
{
    (void)isEnable;
    return RET_OK;
}

uint64_t ExtendManagerServiceProxy::GetDefaultDisplayId()
{
    return 0;
}

int32_t ExtendManagerServiceProxy::PublishIgnoreRepeatClickReminder()
{
    return 0;
}

void ExtendManagerServiceProxy::CancelNotification()
{
    return;
}

int32_t ExtendManagerServiceProxy::RegisterTimers(uint64_t beginTime)
{
    (void)beginTime;
    return 0;
}

void ExtendManagerServiceProxy::DestroyTimers()
{
    return;
}

int64_t ExtendManagerServiceProxy::GetWallTimeMs()
{
    return 0;
}

int32_t ExtendManagerServiceProxy::PublishTransitionAnimationsReminder()
{
    return 0;
}

void ExtendManagerServiceProxy::TransitionAnimationsCancelNotification()
{
    return;
}

int32_t ExtendManagerServiceProxy::TransitionAnimationsRegisterTimers(uint64_t beginTime)
{
    (void)beginTime;
    return 0;
}

void ExtendManagerServiceProxy::TransitionAnimationsDestroyTimers()
{
    return;
}

void ExtendManagerServiceProxy::OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType)
{
    (void)screenMagnificationType;
    return;
}

void ExtendManagerServiceProxy::OnScreenMagnificationStateChanged()
{
    return;
}

void ExtendManagerServiceProxy::UnregisterDisplayListener()
{
    return;
}

void ExtendManagerServiceProxy::GetClickPosition(int32_t &xPos, int32_t &yPos)
{
    (void)xPos;
    (void)yPos;
    return;
}

bool ExtendManagerServiceProxy::CheckExtProxyStatus()
{
    return true;
}

bool ExtendManagerServiceProxy::ExtendGetMagnificationModeCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::ExtendGetMagnificationScaleCallback()
{
    return true;
}

bool ExtendManagerServiceProxy::ExtendUpdateInputFilterCallback() {
    return true;
}

bool ExtendManagerServiceProxy::SetMagnificationModeCallback() {
    return true;
}

bool ExtendManagerServiceProxy::GetMagnificationTypeCallback() {
    return true;
}

bool ExtendManagerServiceProxy::ExtendAnnouncedForMagnificationCallback() {
    return true;
}

bool ExtendManagerServiceProxy::DiscountBrightness(const float discount)
{
    (void)discount;
    return true;
}

bool ExtendManagerServiceProxy::SetMagnificationScaleCallback() {
    return true;
}

bool ExtendManagerServiceProxy::ExtendGetAccessibilityWindowsCallback() {
    return true;
}

void ExtendManagerServiceProxy::FollowFocuseElement(const int32_t centerX, const int32_t centerY)
{
    (void)centerX;
    (void)centerY;
    return;
}

void ExtendManagerServiceProxy::OffZoomGesture()
{
    return;
}

bool ExtendManagerServiceProxy::ExtendSubscribeOsAccountCallback() {
    return true;
}

void ExtendManagerServiceProxy::SetMagnificationState(const bool state, const uint32_t type, const uint32_t mode)
{
    (void)state;
    (void)type;
    (void)mode;
    return;
}
bool ExtendManagerServiceProxy::IsMagnificationWindowActivate()
{
    return true;
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP