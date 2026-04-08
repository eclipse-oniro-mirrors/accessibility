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

#ifndef EXTEND_SERVICE_MANAGER_H
#define EXTEND_SERVICE_MANAGER_H

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "transaction/rs_interfaces.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_def.h"
#include "accessible_ability_connection.h"
#include "accessibility_gesture_inject_path.h"
#include "accessible_ability_manager_service_event_handler.h"
#include "accessibility_touchEvent_injector.h"
#include "magnification_manager.h"

namespace OHOS {
namespace Accessibility {
class TouchEventInjector;
using SendAccessibilityEventToAACallback = void (*)(EventType eventType, GestureType gestureId, uint64_t displayId);
using FindFocusedElementCallback = bool (*)(
    AccessibilityElementInfo &elementInfo, uint32_t timeout, uint64_t displayId);
using ExecuteActionOnAccessibilityFocusedCallback = bool (*)(const ActionType &action, uint64_t displayId);
using GetFocusedWindowIdCallback = void (*)(int32_t &focusedWindowId, uint64_t displayId);
using GetActiveWindowIdCallback = void (*)(int32_t &activeWindowId, uint64_t displayId);
using GetAccessibilityWindowCallback = bool (*)(int32_t windowId, AccessibilityWindowInfo &window, uint64_t displayId);
using SendPointerEventForHoverCallback = void (*)(
    const std::shared_ptr<MMI::PointerEvent> &pointerEvent, uint64_t displayId);
using GetDelayTime = int64_t(*)();
using GetMagnificationState = bool(*)();
using GetMagnificationModeCallback = uint32_t(*)();  // get
using GetMagnificationScaleCallback = float(*)();   // get
using UpdateInputFilterCallback = void(*)();
using MagnificationModeCallback = void(*)(int32_t mode);   // set
using MagnificationTypeCallback = uint32_t(*)();
using AnnouncedForMagnificationCallback = void(*)(AnnounceType announceType);
using DispatchKeyEventCallback =  std::vector<int32_t> (*) (MMI::KeyEvent &event, uint32_t sequenceNum);
using MagnificationScaleCallback = void(*)(float scale);   // set
using GetAccessibilityWindowsCallback = std::vector<AccessibilityWindowInfo>(*)(uint64_t displayId);   // get
using SubscribeOsAccountCallback = void(*)();
using CheckDisplayIdCallback = bool(*)(uint64_t displayId);

class ExtendServiceManager {
    DECLARE_SINGLETON(ExtendServiceManager);
public:
    // callback to SA
    SendAccessibilityEventToAACallback sendAccessibilityEventToAACallback = nullptr;
    FindFocusedElementCallback findFocusedElementCallback = nullptr;
    ExecuteActionOnAccessibilityFocusedCallback executeActionOnAccessibilityFocusedCallback = nullptr;
    GetFocusedWindowIdCallback getFocusedWindowIdCallback = nullptr;
    GetActiveWindowIdCallback getActiveWindowIdCallback = nullptr;
    GetAccessibilityWindowCallback getAccessibilityWindowCallback = nullptr;
    SendPointerEventForHoverCallback sendPointerEventForHoverCallback = nullptr;
    GetDelayTime getDelayTime = nullptr;
    GetMagnificationState getMagnificationState = nullptr;
    DispatchKeyEventCallback dispatchKeyEventCallback = nullptr;
    
    void InitInputManagerHandler();
    void InitGestureHandler();
    void InitHoverEnterHandler();

    // common
    RetError InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);
    void UpdateInputFilter(uint32_t state);
    RetError SetCurtainScreenUsingStatus(bool isEnable);
    void GetClickPosition(int32_t &xPos, int32_t &yPos);
    uint64_t GetDefaultDisplayId();
    void SetMagnificationScale(float scale);
    void SetMagnificationMode(uint32_t mode);
    void SetMagnificationType(uint32_t type);
    RetError SetIgnoreRepeatClickTime(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME &time);
    RetError SetIgnoreRepeatClickState(bool &state);
    RetError SetClickResponseTime(AccessibilityConfig::CLICK_RESPONSE_TIME &time);

    RetError SetClickConfig(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
        AccessibilityConfig::CLICK_RESPONSE_TIME responseTime);

    RetError SetMouseAutoClick(int32_t time);
    void SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum);
    void OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType);
    void OnScreenMagnificationStateChanged();
    void UnregisterDisplayListener();
    void InitMagnification();
    void OffZoomGesture();
    std::shared_ptr<WindowMagnificationManager> GetWindowMagnificationManager();
    std::shared_ptr<FullScreenMagnificationManager> GetFullScreenMagnificationManager();
    std::shared_ptr<MagnificationMenuManager> GetMenuManager();
    void SetMagnificationState(bool state, uint32_t type, uint32_t mode);
    bool CheckDisplayId(uint64_t displayId);

    GetMagnificationModeCallback getMagnificationModeCallback = nullptr;
    GetMagnificationScaleCallback getMagnificationScaleCallback = nullptr;
    UpdateInputFilterCallback updateInputFilterCallback = nullptr;
    MagnificationModeCallback magnificationModeCallback = nullptr;
    MagnificationTypeCallback magnificationTypeCallback = nullptr;
    AnnouncedForMagnificationCallback announcedForMagnificationCallback = nullptr;
    MagnificationScaleCallback magnificationScaleCallback = nullptr;
    GetAccessibilityWindowsCallback getAccessibilityWindowsCallback = nullptr;
    SubscribeOsAccountCallback subscribeOsAccountCallback = nullptr;
    CheckDisplayIdCallback checkDisplayIdCallback = nullptr;

    void SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector);

    inline sptr<TouchEventInjector> GetTouchEventInjector()
    {
        return touchEventInjector_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner> &GetInputManagerRunner()
    {
        return inputManagerRunner_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner> &GetGestureRunner()
    {
        return gestureRunner_;
    }
    inline uint32_t GetClickResponseTime()
    {
        return clickResponseTime_;
    }
    inline bool GetIgnoreRepeatClickState()
    {
        return ignoreRepeatClickState_;
    }
    inline uint32_t GetIgnoreRepeatClickTime()
    {
        return ignoreRepeatClickTime_;
    }

private:
    int32_t mouseAutoClick_ = -1;
    uint32_t clickResponseTime_ = 0;
    bool ignoreRepeatClickState_ = false;
    uint32_t ignoreRepeatClickTime_ = 0;
    uint32_t screenMagnificationType_ = 0;
    uint32_t screenMagnificationMode_ = 0;
    float screenMagnificationScale_ = 2.0f;

    std::shared_ptr<AppExecFwk::EventRunner> actionRunner_;
    std::shared_ptr<AAMSEventHandler> actionHandler_;

    std::shared_ptr<AppExecFwk::EventRunner> sendEventRunner_;
    std::shared_ptr<AAMSEventHandler> sendEventHandler_;

    std::shared_ptr<AppExecFwk::EventRunner> channelRunner_;
    std::shared_ptr<AAMSEventHandler> channelHandler_;

    std::shared_ptr<AppExecFwk::EventRunner> inputManagerRunner_;
    std::shared_ptr<AppExecFwk::EventRunner> gestureRunner_;

    std::shared_ptr<AppExecFwk::EventRunner> hoverEnterRunner_;
    std::shared_ptr<AAMSEventHandler> hoverEnterHandler_;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AAMSEventHandler> handler_;

    std::shared_ptr<MagnificationManager> magnificationManager_ = nullptr;
    // std::shared_ptr<AccountSubscriber> accountSubscriber_ = nullptr;
    sptr<TouchEventInjector> touchEventInjector_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // EXTEND_SERVICE_MANAGER_H