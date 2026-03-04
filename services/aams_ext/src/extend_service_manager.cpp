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

// LCOV_EXCL_START
#include "extend_service_manager.h"
#include "ext_utils.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

namespace {
    const char* AAMS_SERVICE_NAME = "AccessibleAbilityManagerService";
    const char* AAMS_ACTION_RUNNER_NAME = "AamsActionRunner";
    const char* AAMS_SEND_EVENT_RUNNER_NAME = "AamsSendEventRunner";
    const char* AAMS_CHANNEL_RUNNER_NAME = "AamsChannelRunner";
    const char* AAMS_INPUT_MANAGER_RUNNER_NAME = "AamsInputManagerRunner";
    const char* AAMS_GESTURE_RUNNER_NAME = "AamsGestureRunner";
    const char* AAMS_HOVER_ENTER_RUNNER_NAME = "AamsHoverEnterRunner";
}

ExtendServiceManager::ExtendServiceManager()
{
    InitInputManagerHandler();
    InitGestureHandler();
}

void ExtendServiceManager::InitInputManagerHandler()
{
    if (!inputManagerRunner_) {
#ifdef ACCESSIBILITY_WATCH_FEATURE
        inputManagerRunner_ = AppExecFwk::EventRunner::Create(AAMS_INPUT_MANAGER_RUNNER_NAME);
#else
        inputManagerRunner_ = AppExecFwk::EventRunner::Create(AAMS_INPUT_MANAGER_RUNNER_NAME,
            AppExecFwk::ThreadMode::FFRT);
#endif
        if (!inputManagerRunner_) {
            HILOG_ERROR("ExtendServiceManager::InitInputManagerHandler failed:create AAMS input manager runner failed");
            return;
        }
    }
}

void ExtendServiceManager::InitGestureHandler()
{
    if (!gestureRunner_) {
        gestureRunner_ = AppExecFwk::EventRunner::Create(AAMS_GESTURE_RUNNER_NAME, AppExecFwk::ThreadMode::FFRT);
        if (!gestureRunner_) {
            HILOG_ERROR("ExtendServiceManager::InitGestureHandler failed:create AAMS gesture runner failed");
            return;
        }
    }
}

ExtendServiceManager::~ExtendServiceManager()
{
    HILOG_INFO("ExtendServiceManager::~ExtendServiceManager");
}

RetError ExtendServiceManager::InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG();
    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is nullptr.");
        return RET_ERR_NULLPTR;
    }
    return interceptor->InjectEvents(gesturePath);
}

void ExtendServiceManager::SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector)
{
    HILOG_DEBUG();
    touchEventInjector_ = touchEventInjector;
}

void ExtendServiceManager::SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum)
{
    HILOG_DEBUG();
    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (interceptor == nullptr) {
        HILOG_ERROR("interceptor is nullptr.");
        return;
    }
    interceptor->SetServiceOnKeyEventResult(connectionId, isHandled, sequenceNum);
}

RetError ExtendServiceManager::SetMouseAutoClick(int32_t time)
{
    HILOG_DEBUG();
    mouseAutoClick_ = time;
    return RET_OK;
}

RetError ExtendServiceManager::SetClickConfig(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
    AccessibilityConfig::CLICK_RESPONSE_TIME responseTime)
{
    HILOG_DEBUG();
    ignoreRepeatClickTime_ = clickTime;
    ignoreRepeatClickState_ = state;
    clickResponseTime_ = responseTime;
    HILOG_INFO(
        "ignoreRepeatClickTime_ = %{public}u ignoreRepeatClickState_ = %{public}d clickResponseTime_ = %{public}u",
        ignoreRepeatClickTime_,
        ignoreRepeatClickState_,
        clickResponseTime_);
    return RET_OK;
}

void ExtendServiceManager::SetMagnificationMode(uint32_t mode)
{
    HILOG_DEBUG();
    screenMagnificationMode_ = mode;
}

void ExtendServiceManager::SetMagnificationType(uint32_t type)
{
    HILOG_DEBUG();
    screenMagnificationType_ = type;
}

void ExtendServiceManager::SetMagnificationScale(float scale)
{
    HILOG_DEBUG();
    screenMagnificationScale_ = scale;
}

void ExtendServiceManager::UpdateInputFilter(uint32_t state)
{
    HILOG_DEBUG();
    InitMagnification();
    auto interceptor = AccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }
    interceptor->SetAvailableFunctions(state);
}

RetError ExtendServiceManager::SetCurtainScreenUsingStatus(bool isEnable)
{
    HILOG_DEBUG();
    auto rsInterfaces = &(Rosen::RSInterfaces::GetInstance());
    if (rsInterfaces == nullptr) {
        HILOG_ERROR("rsInterfaces is nullptr.");
        return RET_ERR_NULLPTR;
    }
    HILOG_INFO("SetCurtainScreenUsingStatus: status = %{public}d", isEnable);
    rsInterfaces->SetCurtainScreenUsingStatus(isEnable);
    return RET_OK;
}

void ExtendServiceManager::GetClickPosition(int32_t &xPos, int32_t &yPos)
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    int32_t displayWidth = displayMgr.GetWidth();
    int32_t displayHeight = displayMgr.GetHeight();
    xPos = displayWidth;
    yPos = displayHeight;
#endif
}

uint64_t ExtendServiceManager::GetDefaultDisplayId()
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    uint64_t displayId = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplayId();
    HILOG_DEBUG("default display id is %{public}" PRIu64 "", displayId);
    return displayId;
#else
    HILOG_DEBUG("not support display manager");
    return 0;
#endif
}

void ExtendServiceManager::OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType)
{
    if (magnificationManager_ != nullptr) {
        magnificationManager_->OnMagnificationTypeChanged(screenMagnificationType);
    }
}

void ExtendServiceManager::InitMagnification()
{
    HILOG_INFO();
    if (magnificationManager_ == nullptr) {
        magnificationManager_ = std::make_shared<MagnificationManager>();
    }

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    Singleton<AccessibilityDisplayManager>::GetInstance().RegisterDisplayListener(magnificationManager_);
    if (ExtUtils::IsSmallFold()) {
        Singleton<AccessibilityDisplayManager>::GetInstance().RegisterFoldStatusListener();
    }
#endif
    subscribeOsAccountCallback();
}

std::shared_ptr<MagnificationMenuManager> ExtendServiceManager::GetMenuManager()
{
    HILOG_DEBUG();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return nullptr;
    }
    return magnificationManager_->GetMenuManager();
}

std::shared_ptr<FullScreenMagnificationManager> ExtendServiceManager::GetFullScreenMagnificationManager()
{
    HILOG_DEBUG();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return nullptr;
    }
    return magnificationManager_->GetFullScreenMagnificationManager();
}

std::shared_ptr<WindowMagnificationManager> ExtendServiceManager::GetWindowMagnificationManager()
{
    HILOG_DEBUG();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return nullptr;
    }
    return magnificationManager_->GetWindowMagnificationManager();
}

void ExtendServiceManager::OnScreenMagnificationStateChanged()
{
    OffZoomGesture();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
    if (ExtUtils::IsSmallFold()) {
        Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterFoldStatusListener();
    }
#endif
}

void ExtendServiceManager::UnregisterDisplayListener()
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterDisplayListener();
    if (ExtUtils::IsSmallFold()) {
        Singleton<AccessibilityDisplayManager>::GetInstance().UnregisterFoldStatusListener();
    }
#endif
}

void ExtendServiceManager::OffZoomGesture()
{
    HILOG_INFO();
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return;
    }
    magnificationManager_->DisableMagnification();
    magnificationManager_->ResetCurrentMode();
}

void ExtendServiceManager::SetMagnificationState(bool state, uint32_t type, uint32_t mode)
{
    if (magnificationManager_ == nullptr) {
        HILOG_ERROR("magnificationManager_ is nullptr.");
        return;
    }

    if (state == magnificationManager_->GetMagnificationState()) {
        HILOG_ERROR("no need change state.");
        return;
    }

    if (state) {
        magnificationManager_->TriggerMagnification(type, mode);
    } else {
        magnificationManager_->DisableMagnification();
    }
}
}
}
// LCOV_EXCL_STOP