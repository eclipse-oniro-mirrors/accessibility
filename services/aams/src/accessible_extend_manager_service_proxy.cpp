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
#include "accessible_extend_manager_service_proxy.h"
#define private public
#define protected public
#include "accessible_ability_manager_service.h"
#undef private
#undef protected
#include "accessibility_window_manager.h"
#include <dlfcn.h>
#include <shared_mutex>


namespace OHOS {
namespace Accessibility {
namespace {
    const std::string extendServiceName_ = "libaams_ext.z.so";
}

ExtendManagerServiceProxy::ExtendManagerServiceProxy()
{
}

ExtendManagerServiceProxy::~ExtendManagerServiceProxy()
{
    RemoveExtProxy();
}

static void SendAccessibilityEventToAA(EventType eventType, GestureType gestureId, uint64_t displayId)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendAccessibilityEventToAA(eventType, gestureId, displayId);
}

static std::vector<int32_t> DispatchKeyEvent(MMI::KeyEvent &event, uint32_t sequenceNum)
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return std::vector<int32_t>();
    }
    std::vector<int32_t> connctionIds {};
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();
    for (auto iter = connectionMaps.begin(); iter != connectionMaps.end(); iter++) {
        if (iter->second->OnKeyPressEvent(event, sequenceNum)) {
            connctionIds.emplace_back(iter->second->GetChannelId());
        }
    }
    return connctionIds;
}

bool ExtendManagerServiceProxy::LoadExtProxy()
{
    std::unique_lock<ffrt::shared_mutex> wLock(rwLock_);
    if (!handle_) {
        handle_ = dlopen(extendServiceName_.c_str(), RTLD_LAZY);
        SetSendAccessibilityEventToAACallback();
        SetFindFocusedElementCallback();
        SetExecuteActionOnAccessibilityFocusedCallback();
        SetGetFocusedWindowIdCallback();
        SetGetActiveWindowIdCallback();
        SetGetAccessibilityWindowCallback();
        SetSendPointerEventForHoverCallback();
        SetGetDelayTimeCallback();
        SetGetMagnificationStateCallback();
        ExtendGetMagnificationModeCallback();
        ExtendGetMagnificationScaleCallback();
        ExtendUpdateInputFilterCallback();
        SetMagnificationModeCallback();
        GetMagnificationTypeCallback();
        ExtendAnnouncedForMagnificationCallback();
        SetDispatchKeyEventCallback();
        SetMagnificationScaleCallback();
        ExtendGetAccessibilityWindowsCallback();
        ExtendSubscribeOsAccountCallback();
        SetCheckDisplayIdCallback();
    }
    if (!handle_) {
        HILOG_ERROR("dlopen error: %{public}s", dlerror());
        return false;
    }
    return true;
}

bool ExtendManagerServiceProxy::RemoveExtProxy()
{
    std::unique_lock<ffrt::shared_mutex> wLock(rwLock_);
    readyFunc_.clear();
    if (!handle_) {
        return false;
    }
 
    int ret = dlclose(handle_);
    HILOG_DEBUG("dlclose ret: %{public}d", ret);
    char* error = dlerror();
    if (error) {
        HILOG_ERROR("dlclose error: %{public}s", error);
    }
    handle_ = nullptr;
    return true;
}

static int64_t GetDelayTime()
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return 0;
    }
    int32_t delayTime = accountData->GetConfig()->GetMouseAutoClick();
    return delayTime;
}

static bool GetMagnificationState()
{
    return Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationState();
}

bool ExtendManagerServiceProxy::SetGetDelayTimeCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using GetDelayTimeCallback = int64_t(*)();
    using SetCallback = void (*)(GetDelayTimeCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetGetDelayTimeCallback");
    setCallback(GetDelayTime);
    return true;
}

bool ExtendManagerServiceProxy::SetGetMagnificationStateCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using GetMagnificationStateCallback = bool(*)();
    using SetCallback = void (*)(GetMagnificationStateCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetGetMagnificationStateCallback");
    setCallback(GetMagnificationState);
    return true;
}

static bool FindFocusedElement(AccessibilityElementInfo &elementInfo, uint32_t timeout, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    return Singleton<AccessibleAbilityManagerService>::GetInstance().FindFocusedElement(elementInfo, timeout, userId);
}

bool ExtendManagerServiceProxy::SetFindFocusedElementCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using FindFocusedElementCallback = bool (*)(AccessibilityElementInfo &elementInfo, uint32_t timeout, uint64_t displayId);
    using SetCallback = void (*)(FindFocusedElementCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetFindFocusedElementCallback");
    setCallback(FindFocusedElement);
    return true;
}

static bool ExecuteActionOnAccessibilityFocused(const ActionType &action, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    return Singleton<AccessibleAbilityManagerService>::GetInstance().ExecuteActionOnAccessibilityFocused(action, userId);
}

bool ExtendManagerServiceProxy::SetExecuteActionOnAccessibilityFocusedCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using ExecuteActionOnAccessibilityFocusedCallback = bool (*)(const ActionType &action, uint64_t displayId);
    using SetCallback = void (*)(ExecuteActionOnAccessibilityFocusedCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetExecuteActionOnAccessibilityFocusedCallback");
    setCallback(ExecuteActionOnAccessibilityFocused);
    return true;
}

static void GetFocusedWindowId(int32_t &focusedWindowId, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().InnerGetFocusedWindowId(focusedWindowId, userId);
}

bool ExtendManagerServiceProxy::SetGetFocusedWindowIdCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using GetFocusedWindowIdCallback = void (*)(int32_t &focusedWindowId, uint64_t displayId);
    using SetCallback = void (*)(GetFocusedWindowIdCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetGetFocusedWindowIdCallback");
    setCallback(GetFocusedWindowId);
    return true;
}

static void GetActiveWindowId(int32_t &focusedWindowId, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().InnerGetActiveWindow(focusedWindowId, userId);
}

bool ExtendManagerServiceProxy::SetGetActiveWindowIdCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using GetActiveWindowIdCallback = void (*)(int32_t &activeWindowId, uint64_t displayId);
    using SetCallback = void (*)(GetActiveWindowIdCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetGetActiveWindowIdCallback");
    setCallback(GetActiveWindowId);
    return true;
}

static bool GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    return Singleton<AccessibleAbilityManagerService>::GetInstance().InnerGetAccessibilityWindow(windowId, window, userId);
}

bool ExtendManagerServiceProxy::SetGetAccessibilityWindowCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using GetAccessibilityWindowCallback = bool (*)(int32_t windowId, AccessibilityWindowInfo &window, uint64_t displayId);
    using SetCallback = void (*)(GetAccessibilityWindowCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetGetAccessibilityWindowCallback");
    setCallback(GetAccessibilityWindow);
    return true;
}

static void SendPointerEventForHover(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(userId);
    if (accountData) {
        accountData->GetWindowManager().SendPointerEventForHover(pointerEvent);
    }
}

bool ExtendManagerServiceProxy::SetSendPointerEventForHoverCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using SendPointerEventForHoverCallback = void (*)(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint64_t displayId);
    using SetCallback = void (*)(SendPointerEventForHoverCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetSendPointerEventForHoverCallback");
    setCallback(SendPointerEventForHover);
    return true;
}

static bool CheckDisplayId(uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(userId);
    if (accountData) {
        return accountData != 0;
    }
    return false;
}
 
bool ExtendManagerServiceProxy::SetCheckDisplayIdCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using CheckDisplayIdCallback = bool (*)(uint64_t displayId);
    using SetCallback = void (*)(CheckDisplayIdCallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetCheckDisplayIdCallback");
    setCallback(CheckDisplayId);
    return true;
}

void *ExtendManagerServiceProxy::GetFunc(const std::string &funcName)
{
    HILOG_DEBUG("Get func start, %{public}s", funcName.c_str());
    void *func = dlsym(handle_, funcName.c_str());
    if (!func) {
        char *error = dlerror();
        if (error) {
            HILOG_ERROR(" %{public}s", error);
        } else {
            HILOG_ERROR("GetFunc %{public}s failed", funcName.c_str());
        }
    }
    HILOG_DEBUG("Get func end, %{public}s", funcName.c_str());
    return func;
}

bool ExtendManagerServiceProxy::SetSendAccessibilityEventToAACallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using SendAccessibilityEventToAACallback = void (*)(EventType eventType, GestureType gestureId, uint64_t displayId);
    using SetCallback = void (*)(SendAccessibilityEventToAACallback cb);
    SetCallback setCallback = (SetCallback)GetFunc("SetSendAccessibilityEventToAACallback");
    setCallback(SendAccessibilityEventToAA);
    return true;
}

bool ExtendManagerServiceProxy::SetDispatchKeyEventCallback()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    using DispatchKeyEventCallback =  std::vector<int32_t> (*) (MMI::KeyEvent &event, uint32_t sequenceNum);
    using SetDispatchKeyEventCallback = void (*)(DispatchKeyEventCallback cb);
    SetDispatchKeyEventCallback setDispatchKeyEventCallback = (
        SetDispatchKeyEventCallback)GetFunc("SetDispatchKeyEventCallback");
    setDispatchKeyEventCallback(DispatchKeyEvent);
    return true;
}

RetError ExtendManagerServiceProxy::InjectEvents(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using InjectEvents = RetError(*)(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath);
    static InjectEvents func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::INJECT_EVENTS) == readyFunc_.end()) {
        func = (InjectEvents)GetFunc("InjectEvents");
        if (func) {
            readyFunc_.insert(ExtMethod::INJECT_EVENTS);
            return func(gesturePath);
        } else {
            HILOG_ERROR("get InjectEvents func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func(gesturePath);
}

void ExtendManagerServiceProxy::SetServiceOnKeyEventResult(int32_t connectionId, bool isHandled, uint32_t sequenceNum)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetServiceOnKeyEventResult = void(*)(int32_t connectionId, bool isHandled, uint32_t sequenceNum);
    static SetServiceOnKeyEventResult func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_SERVICE_ON_KEY_EVENT_RESULT) == readyFunc_.end()) {
        func = (SetServiceOnKeyEventResult)GetFunc("SetServiceOnKeyEventResult");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_SERVICE_ON_KEY_EVENT_RESULT);
            return func(connectionId, isHandled, sequenceNum);
        } else {
            HILOG_ERROR("get SetServiceOnKeyEventResult func failed");
            return;
        }
    }
    return func(connectionId, isHandled, sequenceNum);
}

RetError ExtendManagerServiceProxy::SetMouseAutoClick(int32_t time)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetMouseAutoClick = RetError(*)(int32_t time);
    static SetMouseAutoClick func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_MOUSE_AUTO_CLICK) == readyFunc_.end()) {
        func = (SetMouseAutoClick)GetFunc("SetMouseAutoClick");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_MOUSE_AUTO_CLICK);
            return func(time);
        } else {
            HILOG_ERROR("get SetMouseAutoClick func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func(time);
}

RetError ExtendManagerServiceProxy::SetClickConfig(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
    AccessibilityConfig::CLICK_RESPONSE_TIME responseTime)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetClickConfig = RetError(*)(AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME clickTime, bool state,
        AccessibilityConfig::CLICK_RESPONSE_TIME responseTime);
    static SetClickConfig func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_CLICK_CONFIG) == readyFunc_.end()) {
        func = (SetClickConfig)GetFunc("SetClickConfig");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_CLICK_CONFIG);
            return func(clickTime, state, responseTime);
        } else {
            HILOG_ERROR("get SetClickConfig func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func(clickTime, state, responseTime);
}

void ExtendManagerServiceProxy::SetMagnificationMode(uint32_t mode)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetMagnificationMode = void(*)(uint32_t mode);
    static SetMagnificationMode func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_MAGNIFICATION_MODE) == readyFunc_.end()) {
        func = (SetMagnificationMode)GetFunc("SetMagnificationMode");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_MAGNIFICATION_MODE);
            return func(mode);
        } else {
            HILOG_ERROR("get SetMagnificationMode func failed");
            return;
        }
    }
    return func(mode);
}

void ExtendManagerServiceProxy::SetMagnificationType(uint32_t type)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetMagnificationType = void(*)(uint32_t type);
    static SetMagnificationType func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_MAGNIFICATION_TYPE) == readyFunc_.end()) {
        func = (SetMagnificationType)GetFunc("SetMagnificationType");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_MAGNIFICATION_TYPE);
            return func(type);
        } else {
            HILOG_ERROR("get SetMagnificationType func failed");
            return;
        }
    }
    return func(type);
}

void ExtendManagerServiceProxy::SetMagnificationScale(float scale)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetMagnificationScale = void(*)(float scale);
    static SetMagnificationScale func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_MAGNIFICATION_SCALE) == readyFunc_.end()) {
        func = (SetMagnificationScale)GetFunc("SetMagnificationScale");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_MAGNIFICATION_SCALE);
            return func(scale);
        } else {
            HILOG_ERROR("get SetMagnificationScale func failed");
            return;
        }
    }
    return func(scale);
}

void ExtendManagerServiceProxy::UpdateInputFilter(uint32_t flag)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using UpdateInputFilter = void(*)(uint32_t flag);
    static UpdateInputFilter func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::UPDATE_INPUT_FILTER) == readyFunc_.end()) {
        func = (UpdateInputFilter)GetFunc("UpdateInputFilter");
        if (func) {
            readyFunc_.insert(ExtMethod::UPDATE_INPUT_FILTER);
            return func(flag);
        } else {
            HILOG_ERROR("get UpdateInputFilter func failed");
            return;
        }
    }
    return func(flag);
}

RetError ExtendManagerServiceProxy::SetCurtainScreenUsingStatus(bool isEnable)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetCurtainScreenUsingStatus = RetError(*)(bool isEnable);
    static SetCurtainScreenUsingStatus func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_CURTAIN_SCREEN_USING_STATUS) == readyFunc_.end()) {
        func = (SetCurtainScreenUsingStatus)GetFunc("SetCurtainScreenUsingStatus");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_CURTAIN_SCREEN_USING_STATUS);
            return func(isEnable);
        } else {
            HILOG_ERROR("get SetCurtainScreenUsingStatus func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func(isEnable);
}

uint64_t ExtendManagerServiceProxy::GetDefaultDisplayId()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using GetDefaultDisplayId = uint64_t(*)();
    static GetDefaultDisplayId func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
    if (!func || readyFunc_.find(ExtMethod::GET_DEFAULT_DISPLAY_ID) == readyFunc_.end()) {
        func = (GetDefaultDisplayId)GetFunc("GetDefaultDisplayId");
        if (func) {
            readyFunc_.insert(ExtMethod::GET_DEFAULT_DISPLAY_ID);
            return func();
        } else {
            HILOG_ERROR("get GetDefaultDisplayId func failed");
            return 0;
        }
    }
    return func();
}

int32_t ExtendManagerServiceProxy::PublishIgnoreRepeatClickReminder()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using PublishIgnoreRepeatClickReminder = int32_t(*)();
    static PublishIgnoreRepeatClickReminder func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::PUBLISH_IGNORE_REPEAT_CLICK_REMINDER) == readyFunc_.end()) {
        func = (PublishIgnoreRepeatClickReminder)GetFunc("PublishIgnoreRepeatClickReminder");
        if (func) {
            readyFunc_.insert(ExtMethod::PUBLISH_IGNORE_REPEAT_CLICK_REMINDER);
            return func();
        } else {
            HILOG_ERROR("get PublishIgnoreRepeatClickReminder func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func();
}

void ExtendManagerServiceProxy::CancelNotification()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using CancelNotification = void(*)();
    static CancelNotification func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::CANCEL_NOTIFICATION) == readyFunc_.end()) {
        func = (CancelNotification)GetFunc("CancelNotification");
        if (func) {
            readyFunc_.insert(ExtMethod::CANCEL_NOTIFICATION);
            return func();
        } else {
            HILOG_ERROR("get CancelNotification func failed");
            return;
        }
    }
    return func();
}

int32_t ExtendManagerServiceProxy::RegisterTimers(uint64_t beginTime)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using RegisterTimers = int32_t(*)(uint64_t beginTime);
    static RegisterTimers func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return RET_ERR_NULLPTR;
    }
    if (!func || readyFunc_.find(ExtMethod::REGISTER_TIMERS) == readyFunc_.end()) {
        func = (RegisterTimers)GetFunc("RegisterTimers");
        if (func) {
            readyFunc_.insert(ExtMethod::REGISTER_TIMERS);
            return func(beginTime);
        } else {
            HILOG_ERROR("get RegisterTimers func failed");
            return RET_ERR_NULLPTR;
        }
    }
    return func(beginTime);
}

void ExtendManagerServiceProxy::DestroyTimers()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using DestroyTimers = void(*)();
    static DestroyTimers func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::DESTORY_TIMERS) == readyFunc_.end()) {
        func = (DestroyTimers)GetFunc("DestroyTimers");
        if (func) {
            readyFunc_.insert(ExtMethod::DESTORY_TIMERS);
            return func();
        } else {
            HILOG_ERROR("get DestroyTimers func failed");
            return;
        }
    }
    return func();
}

int64_t ExtendManagerServiceProxy::GetWallTimeMs()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using GetWallTimeMs = int64_t(*)();
    static GetWallTimeMs func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
    if (!func || readyFunc_.find(ExtMethod::GET_WALL_TIME_MS) == readyFunc_.end()) {
        func = (GetWallTimeMs)GetFunc("GetWallTimeMs");
        if (func) {
            readyFunc_.insert(ExtMethod::GET_WALL_TIME_MS);
            return func();
        } else {
            HILOG_ERROR("get GetWallTimeMs func failed");
            return 0;
        }
    }
    return func();
}

int32_t ExtendManagerServiceProxy::PublishTransitionAnimationsReminder()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using PublishTransitionAnimationsReminder = int32_t(*)();
    static PublishTransitionAnimationsReminder func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
    if (!func || readyFunc_.find(ExtMethod::PUBLISH_TRANSITION_ANIMATIONS_REMINDER) == readyFunc_.end()) {
        func = (PublishTransitionAnimationsReminder)GetFunc("PublishTransitionAnimationsReminder");
        if (func) {
            readyFunc_.insert(ExtMethod::PUBLISH_TRANSITION_ANIMATIONS_REMINDER);
            return func();
        } else {
            HILOG_ERROR("get PublishTransitionAnimationsReminder func failed");
            return 0;
        }
    }
    return func();
}

void ExtendManagerServiceProxy::TransitionAnimationsCancelNotification()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using TransitionAnimationsCancelNotification = void(*)();
    static TransitionAnimationsCancelNotification func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::TRANSITION_ANIMATIONS_CANCEL_NOTIFICATION) == readyFunc_.end()) {
        func = (TransitionAnimationsCancelNotification)GetFunc("TransitionAnimationsCancelNotification");
        if (func) {
            readyFunc_.insert(ExtMethod::TRANSITION_ANIMATIONS_CANCEL_NOTIFICATION);
            return func();
        } else {
            HILOG_ERROR("get TransitionAnimationsCancelNotification func failed");
            return;
        }
    }
    return func();
}

int32_t ExtendManagerServiceProxy::TransitionAnimationsRegisterTimers(uint64_t beginTime)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using TransitionAnimationsRegisterTimers = int32_t(*)(uint64_t beginTime);
    static TransitionAnimationsRegisterTimers func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
    if (!func || readyFunc_.find(ExtMethod::TRANSITION_ANIMATIONS_REGISTER_TIMERS) == readyFunc_.end()) {
        func = (TransitionAnimationsRegisterTimers)GetFunc("TransitionAnimationsRegisterTimers");
        if (func) {
            readyFunc_.insert(ExtMethod::TRANSITION_ANIMATIONS_REGISTER_TIMERS);
            return func(beginTime);
        } else {
            HILOG_ERROR("get TransitionAnimationsRegisterTimers func failed");
            return 0;
        }
    }
    return func(beginTime);
}

void ExtendManagerServiceProxy::TransitionAnimationsDestroyTimers()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using TransitionAnimationsDestroyTimers = void(*)();
    static TransitionAnimationsDestroyTimers func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::TRANSITION_ANIMATIONS_DESTROY_TIMERS) == readyFunc_.end()) {
        func = (TransitionAnimationsDestroyTimers)GetFunc("TransitionAnimationsDestroyTimers");
        if (func) {
            readyFunc_.insert(ExtMethod::TRANSITION_ANIMATIONS_DESTROY_TIMERS);
            return func();
        } else {
            HILOG_ERROR("get TransitionAnimationsDestroyTimers func failed");
            return;
        }
    }
    return func();
}

void ExtendManagerServiceProxy::OnScreenMagnificationTypeChanged(uint32_t screenMagnificationType)
{
    using OnScreenMagnificationTypeChanged = void(*)(uint32_t screenMagnificationType);
    static OnScreenMagnificationTypeChanged func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::ON_SCREEN_MAGNIFICATION_TYPE_CHANGED) == readyFunc_.end()) {
        func = (OnScreenMagnificationTypeChanged)GetFunc("OnScreenMagnificationTypeChanged");
        if (func) {
            readyFunc_.insert(ExtMethod::ON_SCREEN_MAGNIFICATION_TYPE_CHANGED);
            return func(screenMagnificationType);
        } else {
            HILOG_ERROR("get OnScreenMagnificationTypeChanged func failed");
            return;
        }
    }
    return func(screenMagnificationType);
}

void ExtendManagerServiceProxy::OnScreenMagnificationStateChanged()
{
    using OnScreenMagnificationStateChanged = void(*)();
    static OnScreenMagnificationStateChanged func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }

    if (!func || readyFunc_.find(ExtMethod::ON_SCREEN_MAGNIFICATION_STATE_CHANGED) == readyFunc_.end()) {
        func = (OnScreenMagnificationStateChanged)GetFunc("OnScreenMagnificationStateChanged");
        if (func) {
            readyFunc_.insert(ExtMethod::ON_SCREEN_MAGNIFICATION_STATE_CHANGED);
            return func();
        } else {
            HILOG_ERROR("get OnScreenMagnificationStateChanged func failed");
            return;
        }
    }
    return func();
}

void ExtendManagerServiceProxy::UnregisterDisplayListener()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using UnregisterDisplayListener = void(*)();
    static UnregisterDisplayListener func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::UNREGISTER_DISPLAY_LISTENER) == readyFunc_.end()) {
        func = (UnregisterDisplayListener)GetFunc("UnregisterDisplayListener");
        if (func) {
            readyFunc_.insert(ExtMethod::UNREGISTER_DISPLAY_LISTENER);
            func();
        } else {
            HILOG_ERROR("UnregisterDisplayListener func failed");
            return;
        }
    }
    func();
}

void ExtendManagerServiceProxy::GetClickPosition(int32_t &xPos, int32_t &yPos)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using GetClickPosition = void(*)(int32_t &xPos, int32_t &yPos);
    static GetClickPosition func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::GET_CLICK_POSITION) == readyFunc_.end()) {
        func = (GetClickPosition)GetFunc("GetClickPosition");
        if (func) {
            readyFunc_.insert(ExtMethod::GET_CLICK_POSITION);
            func(xPos, yPos);
        } else {
            HILOG_ERROR("GetClickPosition func failed");
            return;
        }
    }
    func(xPos, yPos);
}

bool ExtendManagerServiceProxy::CheckExtProxyStatus()
{
    if (!handle_) {
        HILOG_ERROR("Extension Proxy is not load");
        return false;
    }
    return true;
}

static uint32_t GetMagnificationModeCallback()
{
    return Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
}

bool ExtendManagerServiceProxy::ExtendGetMagnificationModeCallback()
{
    using GetMagnificationMode = uint32_t(*)();
    using SetCallback = void(*)(GetMagnificationMode cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendGetMagnificationModeCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(GetMagnificationModeCallback);
    return true;
}

static float GetMagnificationScaleCallback()
{
    return Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationScale();
}

bool ExtendManagerServiceProxy::ExtendGetMagnificationScaleCallback()
{
    using GetMagnificationScale = float(*)();
    using SetCallback = void(*)(GetMagnificationScale cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendGetMagnificationScaleCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(GetMagnificationScaleCallback);
    return true;
}

static void UpdateInputFilterCallback()
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateInputFilter();
}

bool ExtendManagerServiceProxy::ExtendUpdateInputFilterCallback()
{
    using UpdateInputFilter = void(*)();
    using SetCallback = void(*)(UpdateInputFilter);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendUpdateInputFilterCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(UpdateInputFilterCallback);
    return true;
}

static void MagnificationModeCallback(int32_t mode)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationMode(mode);
}

bool ExtendManagerServiceProxy::SetMagnificationModeCallback()
{
    using SetMagnificationMode = void(*)(int32_t mode);
    using SetCallback = void(*)(SetMagnificationMode cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("SetMagnificationModeCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(MagnificationModeCallback);
    return true;
}

static uint32_t MagnificationTypeCallback()
{
    return Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationType();
}

bool ExtendManagerServiceProxy::GetMagnificationTypeCallback()
{
    using GetMagnificationType = uint32_t(*)();
    using SetCallback = void(*)(GetMagnificationType cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("GetMagnificationTypeCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(MagnificationTypeCallback);
    return true;
}

static void AnnouncedForMagnificationCallback(AnnounceType announceType)
{
    HILOG_INFO("AnnouncedForMagnificationCallback start");
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(announceType);
}

bool ExtendManagerServiceProxy::ExtendAnnouncedForMagnificationCallback()
{
    using AnnouncedForMagnification = void(*)(AnnounceType announceType);
    using SetCallback = void(*)(AnnouncedForMagnification cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendAnnouncedForMagnificationCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(AnnouncedForMagnificationCallback);
    return true;
}

bool ExtendManagerServiceProxy::DiscountBrightness(const float discount)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using DiscountBrightness = bool(*)(const float discount);
    static DiscountBrightness func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
    if (!func || readyFunc_.find(ExtMethod::DISCOUNT_BRIGHTNESS) == readyFunc_.end()) {
        func = (DiscountBrightness)GetFunc("DiscountBrightness");
        if (func) {
            readyFunc_.insert(ExtMethod::DISCOUNT_BRIGHTNESS);
            return func(discount);
        } else {
            HILOG_ERROR("get DiscountBrightness func failed");
            return false;
        }
    }
    return func(discount);
}

static void MagnificationScaleCallback(float scale)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale);
}

bool ExtendManagerServiceProxy::SetMagnificationScaleCallback()
{
    using SetMagnificationScale = void(*)(float scale);
    using SetCallback = void(*)(SetMagnificationScale cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("SetMagnificationScaleCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(MagnificationScaleCallback);
    return true;
}

static std::vector<AccessibilityWindowInfo> GetAccessibilityWindowsCallback(uint64_t displayId)
{
    int userId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetUserIdByDisplayId(displayId);
    return Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccessibilityWindows(userId);
}

bool ExtendManagerServiceProxy::ExtendGetAccessibilityWindowsCallback()
{
    using GetAccessibilityWindows = std::vector<AccessibilityWindowInfo>(*)(uint64_t displayId);
    using SetCallback = void(*)(GetAccessibilityWindows cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendGetAccessibilityWindowsCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(GetAccessibilityWindowsCallback);
    return true;
}

void ExtendManagerServiceProxy::FollowFocuseElement(const int32_t centerX, const int32_t centerY)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using FollowFocuseElement = void(*)(const int32_t centerX, const int32_t centerY);
    static FollowFocuseElement func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::FOLLOW_FOCUSE_ELEMENT) == readyFunc_.end()) {
        func = (FollowFocuseElement)GetFunc("FollowFocuseElement");
        if (func) {
            readyFunc_.insert(ExtMethod::FOLLOW_FOCUSE_ELEMENT);
            return func(centerX, centerY);
        } else {
            HILOG_ERROR("get FollowFocuseElement func failed");
            return;
        }
    }
    return func(centerX, centerY);
}

void ExtendManagerServiceProxy::OffZoomGesture()
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using OffZoomGesture = void(*)();
    static OffZoomGesture func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::OFF_ZOOM_GESTURE) == readyFunc_.end()) {
        func = (OffZoomGesture)GetFunc("OffZoomGesture");
        if (func) {
            readyFunc_.insert(ExtMethod::OFF_ZOOM_GESTURE);
            return func();
        } else {
            HILOG_ERROR("get OffZoomGesture func failed");
            return;
        }
    }
    return func();
}

static void SubscribeOsAccountCallback()
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().SubscribeOsAccount();
}

bool ExtendManagerServiceProxy::ExtendSubscribeOsAccountCallback()
{
    using SubscribeOsAccount = void(*)();
    using SetCallback = void(*)(SubscribeOsAccount cb);
    SetCallback setCallbackFun = (SetCallback)GetFunc("ExtendSubscribeOsAccountCallback");
    if (!setCallbackFun) {
        HILOG_ERROR("setCallbackFun is null");
        return false;
    }
    setCallbackFun(SubscribeOsAccountCallback);
    return true;
}

void ExtendManagerServiceProxy::SetMagnificationState(const bool state, const uint32_t type, const uint32_t mode)
{
    std::shared_lock<ffrt::shared_mutex> rLock(rwLock_);
    using SetMagnificationState = void(*)(const bool state, const uint32_t type, const uint32_t mode);
    static SetMagnificationState func;
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
    if (!func || readyFunc_.find(ExtMethod::SET_MAGNIFICATION_STATE) == readyFunc_.end()) {
        func = (SetMagnificationState)GetFunc("SetMagnificationState");
        if (func) {
            readyFunc_.insert(ExtMethod::SET_MAGNIFICATION_STATE);
            return func(state, type, mode);
        } else {
            HILOG_ERROR("get SetMagnificationState func failed");
            return;
        }
    }
    return func(state, type, mode);
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP