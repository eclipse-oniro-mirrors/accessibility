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

#ifndef MOCK_ACCESSIBLE_EXTEND_MANAGER_SERVICE_PROXY_H
#define MOCK_ACCESSIBLE_EXTEND_MANAGER_SERVICE_PROXY_H

#include "hilog_wrapper.h"
#include "singleton.h"
#include "accessibility_def.h"
#include "key_event.h"
#include "ffrt.h"
#include <set>
#include "accessible_ability_connection.h"

namespace OHOS {
namespace Accessibility {

enum class ExtMethod : int32_t {
    INJECT_EVENTS = 1,
    SET_SERVICE_ON_KEY_EVENT_RESULT,
    SET_MOUSE_AUTO_CLICK,
    SET_CLICK_CONFIG,
    SET_MAGNIFICATION_MODE,
    SET_MAGNIFICATION_TYPE,
    SET_MAGNIFICATION_SCALE,
    UPDATE_INPUT_FILTER,
    SET_CURTAIN_SCREEN_USING_STATUS,
    GET_DEFAULT_DISPLAY_ID,
    PUBLISH_IGNORE_REPEAT_CLICK_REMINDER,
    CANCEl_NOTIFICATION,
    REGISTER_TIMERS,
    DESTORY_TIMERS,
    GET_WALL_TIME_MS,
    PUBLISH_TRANSITION_ANIMATIONS_REMINDER,
    TRANSITION_ANIMATIONS_CANCEL_NOTIFICATION,
    TRANSITION_ANIMATIONS_REGISTER_TIMERS,
    TRANSITION_ANIMATIONS_DESTROY_TIMERS,
    ADD_KEY_EVENT,
    DISCOUNT_BRIGHTNESS,
    UNREGISTER_DISPLAY_LISTENER,
    GET_CLICK_POSITION,
    FOLLOW_FOCUSE_ELEMENT,
    ON_SCREEN_MAGNIFICATION_TYPE_CHANGED,
    ON_SCREEN_MAGNIFICATION_STATE_CHANGED,
    OFF_ZOOM_GESTURE,
    SET_MAGNIFICATION_STATE,
    IS_MAGNIFICATION_WINDOW_ACTIVATE
};

class ExtendManagerServiceProxy {
    DECLARE_SINGLETON(ExtendManagerServiceProxy);
public:
    bool RemoveExtProxy();
    bool LoadExtProxy();
    // common
    void UpdateInputFilter(uint32_t flag);

    RetError InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);
    void SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum);
    RetError SetMouseAutoClick(int32_t time);
    RetError SetClickConfig(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
        AccessibilityConfig::CLICK_RESPONSE_TIME responseTime);
    void SetMagnificationMode(uint32_t mode);
    void SetMagnificationType(uint32_t type);
    void SetMagnificationScale(float scale);
    RetError SetCurtainScreenUsingStatus(bool isEnable);
    uint64_t GetDefaultDisplayId();
    int32_t PublishIgnoreRepeatClickReminder();
    void CancelNotification();
    int32_t RegisterTimers(uint64_t beginTime);
    void DestroyTimers();
    int64_t GetWallTimeMs();
    int32_t PublishTransitionAnimationsReminder();
    void TransitionAnimationsCancelNotification();
    int32_t TransitionAnimationsRegisterTimers(uint64_t beginTime);
    void TransitionAnimationsDestroyTimers();
    void OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType);
    void OnScreenMagnificationStateChanged();
    void UnregisterDisplayListener();
    bool CheckExtProxyStatus();
    bool DiscountBrightness(const float discount);
    void FollowFocuseElement(const int32_t centerX, const int32_t centerY);
    void GetClickPosition(int32_t &xPos, int32_t &yPos);
    void OffZoomGesture();
    void SetMagnificationState(const bool state, const uint32_t type, const uint32_t mode);
    bool IsMagnificationWindowActivate();

    // callback
    bool SetSendAccessibilityEventToAACallback();
    bool SetFindFocusedElementCallback();
    bool SetExecuteActionOnAccessibilityFocusedCallback();
    bool SetGetFocusedWindowIdCallback();
    bool SetGetActiveWindowIdCallback();
    bool SetGetAccessibilityWindowCallback();
    bool SetSendPointerEventForHoverCallback();
    bool SetGetDelayTimeCallback();
    bool SetGetMagnificationStateCallback();
    bool ExtendGetMagnificationModeCallback();
    bool ExtendGetMagnificationScaleCallback();
    bool ExtendUpdateInputFilterCallback();
    bool SetMagnificationModeCallback();
    bool GetMagnificationTypeCallback();
    bool SetDispatchKeyEventCallback();
    bool ExtendAnnouncedForMagnificationCallback();
    bool SetMagnificationScaleCallback();
    bool ExtendGetAccessibilityWindowsCallback();
    bool ExtendSubscribeOsAccountCallback();

private:
    void* GetFunc(const std::string& funcName);
    void* handle_ = nullptr;
    ffrt::shared_mutex rwLock_;
    std::set<ExtMethod> readyFunc_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_EXTEND_MANAGER_SERVICE_PROXY_H