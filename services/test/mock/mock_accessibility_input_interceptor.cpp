/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_mouse_autoclick.h"
#include "accessibility_short_key.h"
#include "accessibility_screen_touch.h"
#include "accessibility_touch_exploration.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_gesture.h"
#include "window_magnification_gesture.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "key_event.h"
#include "input_event.h"
#ifdef ACCESSIBILITY_WATCH_FEATURE
#include "res_type.h"
#include "res_sched_client.h"
#endif // ACCESSIBILITY_WATCH_FEATURE

namespace OHOS {
namespace Accessibility {
sptr<MockAccessibilityInputInterceptor> MockAccessibilityInputInterceptor::instance_ = nullptr;
sptr<MockAccessibilityInputInterceptor> MockAccessibilityInputInterceptor::GetInstance()
{
    HILOG_DEBUG();

    if (!instance_) {
        instance_ = new(std::nothrow) MockAccessibilityInputInterceptor();
        if (!instance_) {
            HILOG_ERROR("instance_ is null");
            return nullptr;
        }
    }
    return instance_;
}

MockAccessibilityInputInterceptor::MockAccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    inputManager_ = MMI::InputManager::GetInstance();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner());
}

MockAccessibilityInputInterceptor::~MockAccessibilityInputInterceptor()
{
    HILOG_DEBUG();

    availableFunctions_ = 0;
    DestroyInterceptor();
    DestroyTransmitters();
    inputManager_ = nullptr;
    inputEventConsumer_ = nullptr;
}

bool MockAccessibilityInputInterceptor::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(event);
    if (inputManager_) {
        inputManager_->SimulateInputEvent(keyEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
    return true;
}

bool MockAccessibilityInputInterceptor::OnPointerEvent(MMI::PointerEvent &event)
{
    if (event.GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE &&
        event.GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE) {
        HILOG_INFO("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
            event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());
    } else {
        HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d",
            event.GetPointerAction(), event.GetSourceType(), event.GetPointerId());
    }

    event.AddFlag(MMI::InputEvent::EVENT_FLAG_NO_INTERCEPT);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = std::make_shared<MMI::PointerEvent>(event);
    if (inputManager_) {
        inputManager_->SimulateInputEvent(pointerEvent);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
    return true;
}

void MockAccessibilityInputInterceptor::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG("offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);
    if (inputManager_) {
        inputManager_->MoveMouse(offsetX, offsetY);
    } else {
        HILOG_ERROR("inputManager_ is null.");
    }
}

void MockAccessibilityInputInterceptor::SetAvailableFunctions(uint32_t availableFunctions)
{
    HILOG_INFO("function[%{public}d].", availableFunctions);

    if (((availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) != FEATURE_SCREEN_MAGNIFICATION) &&
        (availableFunctions_ == availableFunctions) && ((availableFunctions & FEATURE_SCREEN_TOUCH) == 0)) {
        return;
    }
    availableFunctions_ = availableFunctions;

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty!");
        return;
    }
    eventHandler_->PostTask([this] {
        DestroyTransmitters();
        CreateTransmitters();
        UpdateInterceptor();
    });
}

void MockAccessibilityInputInterceptor::CreateTransmitters()
{
    HILOG_DEBUG("function[%{public}u].", availableFunctions_);

    if (!availableFunctions_) {
        return;
    }

    if ((availableFunctions_ & FEATURE_MOUSE_KEY) && (!mouseKey_)) {
        mouseKey_ = new(std::nothrow) MockAccessibilityMouseKey();
        if (mouseKey_) {
            mouseKey_->SetNext(instance_);
        }
    }

    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_INJECT_TOUCH_EVENTS) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) ||
        (availableFunctions_ & FEATURE_SCREEN_TOUCH)) {
        CreatePointerEventTransmitters();
    }
    
    if (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) {
        CreateKeyEventTransmitters();
    }
}

void MockAccessibilityInputInterceptor::CreatePointerEventTransmitters()
{
    HILOG_DEBUG();

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_MOUSE_AUTOCLICK) {
        sptr<MockAccessibilityMouseAutoclick> mouseAutoclick = new(std::nothrow) MockAccessibilityMouseAutoclick();
        if (!mouseAutoclick) {
            HILOG_ERROR("mouseAutoclick is null");
            return;
        }
        SetNextEventTransmitter(header, current, mouseAutoclick);
    }

    if (availableFunctions_& FEATURE_INJECT_TOUCH_EVENTS) {
        sptr<TouchEventInjector> touchEventInjector = new(std::nothrow) TouchEventInjector();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            return;
        }
        SetNextEventTransmitter(header, current, touchEventInjector);
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(touchEventInjector);
    }

    if (availableFunctions_& FEATURE_SCREEN_MAGNIFICATION) {
        CreateMagnificationGesture(header, current);
    } else {
        ClearMagnificationGesture();
    }

    if (availableFunctions_& FEATURE_TOUCH_EXPLORATION) {
        sptr<TouchExploration> touchExploration = new(std::nothrow) TouchExploration();
        if (!touchExploration) {
            HILOG_ERROR("touchExploration is null");
            return;
        }
        touchExploration->StartUp();
        SetNextEventTransmitter(header, current, touchExploration);
    }

    if ((availableFunctions_ & FEATURE_SCREEN_TOUCH) && ((availableFunctions_ & FEATURE_TOUCH_EXPLORATION) == 0)) {
        sptr<MockAccessibilityScreenTouch> screenTouch = new(std::nothrow) MockAccessibilityScreenTouch();
        if (!screenTouch) {
            HILOG_ERROR("screenTouch is null");
            return;
        }
        SetNextEventTransmitter(header, current, screenTouch);
    }

    SetNextEventTransmitter(header, current, instance_);
    pointerEventTransmitters_ = header;
}

void MockAccessibilityInputInterceptor::CreateMagnificationGesture(sptr<EventTransmission> &header,
    sptr<EventTransmission> &current)
{
    Singleton<AccessibleAbilityManagerService>::GetInstance().InitMagnification();
    uint32_t magnificationMode = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMagnificationMode();
    if (magnificationMode == FULL_SCREEN_MAGNIFICATION) {
        HILOG_INFO("create zoomGesture");
        CreatZoomGesture();
        if (zoomGesture_ == nullptr) {
            HILOG_ERROR("zoomGesture create error.");
            return;
        }
        SetNextEventTransmitter(header, current, zoomGesture_);
    } else if (magnificationMode == WINDOW_MAGNIFICATION) {
        HILOG_INFO("create windowMagnificationGesture");
        CreatWindowMagnificationGesture();
        if (windowMagnificationGesture_ == nullptr) {
            HILOG_ERROR("windowMagnificationGesture create error.");
            return;
        }
        SetNextEventTransmitter(header, current, windowMagnificationGesture_);
    } else {
        HILOG_WARN("invalid magnificationMode");
        ClearMagnificationGesture();
    }
}

void MockAccessibilityInputInterceptor::CreatZoomGesture()
{
    std::shared_ptr<FullScreenMagnificationManager> fullScreenMagnificationManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetFullScreenMagnificationManager();
    std::shared_ptr<MagnificationMenuManager> menuManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMenuManager();
    if (fullScreenMagnificationManager == nullptr || menuManager == nullptr) {
        HILOG_ERROR("get windowMagnification or menu manager failed.");
        return;
    }

    if (zoomGesture_ == nullptr) {
        sptr<AccessibilityZoomGesture> zoomGesture =
            new(std::nothrow) AccessibilityZoomGesture(fullScreenMagnificationManager, menuManager);
        if (zoomGesture == nullptr) {
            HILOG_ERROR("zoomGesture create error.");
            return;
        }
        zoomGesture_ = zoomGesture;
    }

    if (needInteractMagnification_) {
        zoomGesture_->StartMagnificationInteract();
        needInteractMagnification_ = false;
    }
}

void MockAccessibilityInputInterceptor::CreatWindowMagnificationGesture()
{
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetWindowMagnificationManager();
    std::shared_ptr<MagnificationMenuManager> menuManager =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMenuManager();
    if (windowMagnificationManager == nullptr || menuManager == nullptr) {
        HILOG_ERROR("get windowMagnification or menu manager failed.");
        return;
    }

    if (windowMagnificationGesture_ == nullptr) {
        sptr<WindowMagnificationGesture> windowMagnificationGesture =
            new(std::nothrow) WindowMagnificationGesture(windowMagnificationManager, menuManager);
        if (windowMagnificationGesture == nullptr) {
            HILOG_ERROR("windowMagnificationGesture create error.");
            return;
        }
        windowMagnificationGesture_ = windowMagnificationGesture;
    }
    if (needInteractMagnification_) {
        windowMagnificationGesture_->StartMagnificationInteract();
        needInteractMagnification_ = false;
    }
}

void MockAccessibilityInputInterceptor::ClearMagnificationGesture()
{
    zoomGesture_ = nullptr;
    windowMagnificationGesture_ = nullptr;
}

void MockAccessibilityInputInterceptor::CreateKeyEventTransmitters()
{
    HILOG_DEBUG();

    sptr<EventTransmission> header = nullptr;
    sptr<EventTransmission> current = nullptr;

    if (availableFunctions_& FEATURE_FILTER_KEY_EVENTS) {
        sptr<MockKeyEventFilter> MockKeyEventFilter = new(std::nothrow) MockKeyEventFilter();
        if (!MockKeyEventFilter) {
            HILOG_ERROR("MockKeyEventFilter is null");
            return;
        }
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetMockKeyEventFilter(MockKeyEventFilter);
        SetNextEventTransmitter(header, current, MockKeyEventFilter);
    }

    SetNextEventTransmitter(header, current, instance_);
    keyEventTransmitters_ = header;
}

void MockAccessibilityInputInterceptor::UpdateInterceptor()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();
    if (!inputManager_) {
        HILOG_ERROR("inputManger is null.");
        return;
    }

    if (interceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(interceptorId_);
        interceptorId_ = -1;
    }

    if ((availableFunctions_ & FEATURE_MOUSE_AUTOCLICK) ||
        (availableFunctions_ & FEATURE_TOUCH_EXPLORATION) ||
        (availableFunctions_ & FEATURE_SCREEN_MAGNIFICATION) ||
        (availableFunctions_ & FEATURE_MOUSE_KEY) ||
        (availableFunctions_ & FEATURE_SCREEN_TOUCH)) {
            inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
            interceptorId_ = inputManager_->AddInterceptor(inputEventConsumer_);
    } else if (availableFunctions_ & FEATURE_FILTER_KEY_EVENTS) {
            inputEventConsumer_ = std::make_shared<AccessibilityInputEventConsumer>();
            interceptorId_ = inputManager_->AddInterceptor(inputEventConsumer_, PRIORITY_EVENT,
                MMI::CapabilityToTags(MMI::INPUT_DEV_CAP_KEYBOARD));
    }
    HILOG_INFO("interceptorId:%{public}d", interceptorId_);
}

void MockAccessibilityInputInterceptor::DestroyInterceptor()
{
    HILOG_DEBUG("interceptorId:%{public}d.", interceptorId_);

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is null.");
        return;
    }
    if (interceptorId_ >= 0) {
        inputManager_->RemoveInterceptor(interceptorId_);
    }
    interceptorId_ = -1;
}

void MockAccessibilityInputInterceptor::DestroyTransmitters()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if ((availableFunctions_ & FEATURE_MOUSE_KEY) != FEATURE_MOUSE_KEY) {
        if (mouseKey_) {
            mouseKey_->DestroyEvents();
            mouseKey_ = nullptr;
        }
    }

    if (pointerEventTransmitters_ != nullptr) {
        pointerEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetTouchEventInjector(nullptr);
        pointerEventTransmitters_= nullptr;
        zoomGesture_ = nullptr;
        windowMagnificationGesture_ = nullptr;
    }
    if (keyEventTransmitters_ != nullptr) {
        keyEventTransmitters_->DestroyEvents();
        Singleton<AccessibleAbilityManagerService>::GetInstance().SetMockKeyEventFilter(nullptr);
        keyEventTransmitters_ = nullptr;
    }
}

void MockAccessibilityInputInterceptor::ProcessPointerEvent(std::shared_ptr<MMI::PointerEvent> event)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if (mouseKey_) {
        mouseKey_->OnPointerEvent(*event);
    }

    if (!pointerEventTransmitters_) {
        HILOG_DEBUG("pointerEventTransmitters_ is empty.");
        const_cast<MockAccessibilityInputInterceptor*>(this)->OnPointerEvent(*event);
        return;
    }

    pointerEventTransmitters_->OnPointerEvent(*event);
}

void MockAccessibilityInputInterceptor::ProcessKeyEvent(std::shared_ptr<MMI::KeyEvent> event)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    HILOG_DEBUG();

    if (mouseKey_) {
        bool result = mouseKey_->OnKeyEvent(*event);
        if (result) {
            HILOG_DEBUG("The event is mouse key event.");
            return;
        }
    }

    if (!keyEventTransmitters_) {
        HILOG_DEBUG("keyEventTransmitters_ is empty.");
        const_cast<MockAccessibilityInputInterceptor*>(this)->OnKeyEvent(*event);
        return;
    }

    keyEventTransmitters_->OnKeyEvent(*event);
}

void MockAccessibilityInputInterceptor::SetNextEventTransmitter(sptr<EventTransmission> &header,
    sptr<EventTransmission> &current, const sptr<EventTransmission> &next)
{
    HILOG_DEBUG();

    if (current != nullptr) {
        current->SetNext(next);
    } else {
        header = next;
    }
    current = next;
}

void MockAccessibilityInputInterceptor::ShieldZoomGesture(bool flag)
{
    HILOG_INFO("flag = %{public}d", flag);
    if (zoomGesture_) {
        zoomGesture_->ShieldZoomGesture(flag);
    }
    if (windowMagnificationGesture_) {
        windowMagnificationGesture_->ShieldZoomGesture(flag);
    }
}

void MockAccessibilityInputInterceptor::RefreshDisplayInfo()
{
    if (!zoomGesture_) {
        return;
    }
    zoomGesture_->GetWindowParam(true);
}

void MockAccessibilityInputInterceptor::StartMagnificationInteract(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    needInteractMagnification_ = true;
}

void MockAccessibilityInputInterceptor::DisableGesture(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    if (mode == FULL_SCREEN_MAGNIFICATION && zoomGesture_ != nullptr) {
        zoomGesture_->DisableGesture();
    } else if (mode == WINDOW_MAGNIFICATION && windowMagnificationGesture_ != nullptr) {
        windowMagnificationGesture_->DisableGesture();
    } else {
        HILOG_WARN("invalid mode.");
    }
}

void MockAccessibilityInputInterceptor::EnableGesture(uint32_t mode)
{
    HILOG_DEBUG("mode = %{public}d", mode);
    if (mode == FULL_SCREEN_MAGNIFICATION && zoomGesture_ != nullptr) {
        zoomGesture_->StartMagnificationInteract();
    } else if (mode == WINDOW_MAGNIFICATION && windowMagnificationGesture_ != nullptr) {
        windowMagnificationGesture_->StartMagnificationInteract();
    } else {
        HILOG_WARN("invalid mode.");
    }
}

AccessibilityInputEventConsumer::AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner());
#ifdef ACCESSIBILITY_WATCH_FEATURE
    eventHandler_->PostTask([] {
        auto pid = getpid();
        auto tid = gettid();
        uint32_t qosLevel = 7;
        std::string strBundleName = "accessibility";
        std::string strPid = std::to_string(pid);
        std::string strTid = std::to_string(tid);
        std::string strQos = std::to_string(qosLevel);
        std::unordered_map<std::string, std::string> mapPayLoad;
        mapPayLoad["pid"] = strPid;
        mapPayLoad[strTid] = strQos;
        mapPayLoad["bundleName"] = strBundleName;
        uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_THREAD_QOS_CHANGE;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, 0, mapPayLoad);
    });
#endif // ACCESSIBILITY_WATCH_FEATURE
}

AccessibilityInputEventConsumer::~AccessibilityInputEventConsumer()
{
    HILOG_DEBUG();

    eventHandler_ = nullptr;
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    HILOG_DEBUG();
    if (!keyEvent) {
        HILOG_WARN("keyEvent is null.");
        return;
    }

    auto interceptor = MockAccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }

    auto task = [keyEvent, interceptor] {interceptor->ProcessKeyEvent(keyEvent);};
    eventHandler_->PostTask(task, "InputKeyEvent");
}

void AccessibilityInputEventConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (!pointerEvent) {
        HILOG_WARN("pointerEvent is null.");
        return;
    }

    if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        HILOG_INFO("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
            pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());
    } else {
        HILOG_DEBUG("PointerAction:%{public}d, SourceType:%{public}d, PointerId:%{public}d.",
            pointerEvent->GetPointerAction(), pointerEvent->GetSourceType(), pointerEvent->GetPointerId());
    }

    auto interceptor = MockAccessibilityInputInterceptor::GetInstance();
    if (!interceptor) {
        HILOG_ERROR("interceptor is null.");
        return;
    }

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler is empty.");
        return;
    }
    auto task = [pointerEvent, interceptor] {interceptor->ProcessPointerEvent(pointerEvent);};
    eventHandler_->PostTask(task, "InputPointerEvent");
}

static bool IsWantedKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t keyCode = event.GetKeyCode();
    if (keyCode == MMI::KeyEvent::KEYCODE_VOLUME_UP || keyCode == MMI::KeyEvent::KEYCODE_VOLUME_DOWN) {
        return true;
    }
    return false;
}

MockKeyEventFilter::MockKeyEventFilter()
{
    HILOG_DEBUG();

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }

    timeoutHandler_ = std::make_shared<MockMockKeyEventFilterEventHandler>(runner_, *this);
    if (!timeoutHandler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

MockKeyEventFilter::~MockKeyEventFilter()
{
    HILOG_DEBUG();

    std::lock_guard<ffrt::mutex> lock(mutex_);
    eventMaps_.clear();
}

bool MockKeyEventFilter::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    bool whetherIntercept = IsWantedKeyEvent(event);
    if (whetherIntercept) {
        DispatchKeyEvent(event);
        return true;
    }
    EventTransmission::OnKeyEvent(event);
    return false;
}

void MockKeyEventFilter::SetServiceOnKeyEventResult(AccessibleAbilityConnection &connection, bool isHandled,
    uint32_t sequenceNum)
{
    HILOG_DEBUG("isHandled[%{public}d], sequenceNum[%{public}u].", isHandled, sequenceNum);

    std::shared_ptr<ProcessingEvent> processingEvent = FindProcessingEvent(connection, sequenceNum);
    if (!processingEvent) {
        HILOG_DEBUG("No event being processed.");
        return;
    }

    if (!isHandled) {
        if (!processingEvent->usedCount_) {
            timeoutHandler_->RemoveEvent(processingEvent->seqNum_);
            EventTransmission::OnKeyEvent(*processingEvent->event_);
        }
    } else {
        timeoutHandler_->RemoveEvent(processingEvent->seqNum_);
        RemoveProcessingEvent(processingEvent);
    }
}

void MockKeyEventFilter::ClearServiceKeyEvents(AccessibleAbilityConnection &connection)
{
    HILOG_DEBUG();

    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        if (iter->first.GetRefPtr() != &connection) {
            continue;
        }

        for (auto &val : iter->second) {
            val->usedCount_--;
            if (!val->usedCount_) {
                EventTransmission::OnKeyEvent(*val->event_);
            }
        }
        eventMaps_.erase(iter);
        break;
    }
}

void MockKeyEventFilter::DispatchKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMaps = accountData->GetConnectedA11yAbilities();

    std::shared_ptr<ProcessingEvent> processingEvent = nullptr;
    std::shared_ptr<MMI::KeyEvent> copyEvent = nullptr;
    sequenceNum_++;
    for (auto iter = connectionMaps.begin(); iter != connectionMaps.end(); iter++) {
        if (iter->second->OnKeyPressEvent(event, sequenceNum_)) {
            if (!processingEvent) {
                processingEvent = std::make_shared<ProcessingEvent>();
                copyEvent = std::make_shared<MMI::KeyEvent>(event);
                processingEvent->event_ = copyEvent;
                processingEvent->seqNum_ = sequenceNum_;
            }
            processingEvent->usedCount_++;

            std::lock_guard<ffrt::mutex> lock(mutex_);
            if (eventMaps_.find(iter->second) == eventMaps_.end()) {
                std::vector<std::shared_ptr<ProcessingEvent>> processingEvens;
                eventMaps_.insert(std::make_pair(iter->second, processingEvens));
            }
            eventMaps_.at(iter->second).emplace_back(processingEvent);
        }
    }

    if (!processingEvent) {
        HILOG_DEBUG("No service handles the event.");
        sequenceNum_--;
        EventTransmission::OnKeyEvent(event);
        return;
    }

    timeoutHandler_->SendEvent(sequenceNum_, processingEvent, g_taskTime);
}

bool MockKeyEventFilter::RemoveProcessingEvent(std::shared_ptr<ProcessingEvent> event)
{
    HILOG_DEBUG();

    std::lock_guard<ffrt::mutex> lock(mutex_);
    bool haveEvent = false;
    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        for (auto val = iter->second.begin(); val != iter->second.end(); val++) {
            if (*val != event) {
                continue;
            }
            (*val)->usedCount_--;
            iter->second.erase(val);
            haveEvent = true;
            break;
        }
    }

    return haveEvent;
}

std::shared_ptr<MockKeyEventFilter::ProcessingEvent> MockKeyEventFilter::FindProcessingEvent(
    AccessibleAbilityConnection &connection, uint32_t sequenceNum)
{
    HILOG_DEBUG();

    std::shared_ptr<ProcessingEvent> processingEvent = nullptr;

    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = eventMaps_.begin(); iter != eventMaps_.end(); iter++) {
        if (iter->first.GetRefPtr() != &connection) {
            continue;
        }

        for (auto val = iter->second.begin(); val != iter->second.end(); val++) {
            if ((*val) == nullptr) {
                HILOG_ERROR("connection is null");
                continue;
            }
            if ((*val)->seqNum_ != sequenceNum) {
                continue;
            }
            processingEvent = *val;
            iter->second.erase(val);
            processingEvent->usedCount_--;
            break;
        }
        break;
    }

    return processingEvent;
}

void MockKeyEventFilter::DestroyEvents()
{
    HILOG_DEBUG();

    std::lock_guard<ffrt::mutex> lock(mutex_);
    timeoutHandler_->RemoveAllEvents();
    eventMaps_.clear();
    EventTransmission::DestroyEvents();
}

void MockKeyEventFilter::SendEventToNext(MMI::KeyEvent &event)
{
    HILOG_DEBUG();
    EventTransmission::OnKeyEvent(event);
}

MockMockKeyEventFilterEventHandler::MockMockKeyEventFilterEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, MockKeyEventFilter &MockKeyEventFilter)
    : AppExecFwk::EventHandler(runner), MockKeyEventFilter_(MockKeyEventFilter)
{
    HILOG_DEBUG();
}

void MockMockKeyEventFilterEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    if (!event) {
        HILOG_ERROR("event is null.");
        return;
    }

    auto processingEvent = event->GetSharedObject<MockKeyEventFilter::ProcessingEvent>();
    if (processingEvent == nullptr) {
        HILOG_ERROR("processingEvent is nullptr");
        return;
    }
    if (processingEvent->seqNum_ != event->GetInnerEventId()) {
        HILOG_ERROR("event is wrong.");
        return;
    }

    bool haveEvent = MockKeyEventFilter_.RemoveProcessingEvent(processingEvent);
    if (haveEvent) {
        MockKeyEventFilter_.SendEventToNext(*processingEvent->event_);
    }
}

MockAccessibilityMouseAutoclick::MockAccessibilityMouseAutoclick()
{
    HILOG_DEBUG();

    std::shared_ptr<AppExecFwk::EventRunner> runner =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner) {
        HILOG_ERROR("get runner failed");
        return;
    }

    timeoutHandler_ = std::make_shared<MouseAutoclickEventHandler>(runner, *this);
    if (!timeoutHandler_) {
        HILOG_ERROR("create event handler failed");
    }
}

MockAccessibilityMouseAutoclick::~MockAccessibilityMouseAutoclick()
{
    HILOG_DEBUG();

    timeoutHandler_ = nullptr;
    lastMouseEvent_ = nullptr;
}

bool MockAccessibilityMouseAutoclick::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t source = event.GetSourceType();
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointers = event.GetPointerIds();
    size_t pointerCount = pointers.size();
    if ((source != MMI::PointerEvent::SOURCE_TYPE_MOUSE) ||
        (action != MMI::PointerEvent::POINTER_ACTION_MOVE) ||
        (pointerCount != POINTER_COUNT_1)) {
        CancelAutoclick();
    } else {
        RecognizeAutoclick(event);
    }

    EventTransmission::OnPointerEvent(event);
    return false;
}

void MockAccessibilityMouseAutoclick::SendMouseClickEvent()
{
    HILOG_DEBUG();

    if (!lastMouseEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }

    int64_t nowTime = GetSystemTime();
    // Update event information.
    lastMouseEvent_->SetActionTime(nowTime);
    lastMouseEvent_->SetActionStartTime(nowTime);
    lastMouseEvent_->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    lastMouseEvent_->SetButtonPressed(MMI::PointerEvent::MOUSE_BUTTON_LEFT);

    // Update pointer item information.
    int32_t pointerId = lastMouseEvent_->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    lastMouseEvent_->GetPointerItem(pointerId, item);
    item.SetDownTime(nowTime);
    item.SetPressed(true);
    lastMouseEvent_->UpdatePointerItem(pointerId, item);

    // Send mouse left button down event.
    lastMouseEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    EventTransmission::OnPointerEvent(*lastMouseEvent_);

    // Send mouse left button up event.
    lastMouseEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
    EventTransmission::OnPointerEvent(*lastMouseEvent_);
}

void MockAccessibilityMouseAutoclick::ResetAutoclickInfo()
{
    HILOG_DEBUG();

    lastMouseEvent_ = nullptr;
}

void MockAccessibilityMouseAutoclick::DestroyEvents()
{
    HILOG_DEBUG();

    CancelAutoclick();
    EventTransmission::DestroyEvents();
}

void MockAccessibilityMouseAutoclick::RecognizeAutoclick(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    bool isMove = IsMouseMovement(event);
    if (!isMove) {
        HILOG_DEBUG("Mouse is not moving.");
        return;
    }

    lastMouseEvent_ = std::make_shared<MMI::PointerEvent>(event);
    if (!timeoutHandler_) {
        HILOG_ERROR("handler is null.");
        return;
    }
    int64_t delayTime = GetDelayTime();
    timeoutHandler_->RemoveEvent(AUTOCLICK_TIMEOUT_MSG);
    timeoutHandler_->SendEvent(AUTOCLICK_TIMEOUT_MSG, 0, delayTime);
}

bool MockAccessibilityMouseAutoclick::IsMouseMovement(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (!lastMouseEvent_) {
        return true;
    }

    int32_t pointerId = event.GetPointerId();
    MMI::PointerEvent::PointerItem item;
    event.GetPointerItem(pointerId, item);
    int32_t newX = item.GetDisplayX();
    int32_t newY = item.GetDisplayY();

    pointerId = lastMouseEvent_->GetPointerId();
    lastMouseEvent_->GetPointerItem(pointerId, item);
    int32_t oldX = item.GetDisplayX();
    int32_t oldY = item.GetDisplayY();
    if ((newX != oldX) || (newY != oldY)) {
        HILOG_DEBUG("Mouse is moving.");
        return true;
    }
    return false;
}

void MockAccessibilityMouseAutoclick::CancelAutoclick()
{
    HILOG_DEBUG();

    ResetAutoclickInfo();
    timeoutHandler_->RemoveEvent(AUTOCLICK_TIMEOUT_MSG);
}

int64_t MockAccessibilityMouseAutoclick::GetSystemTime()
{
    HILOG_DEBUG();

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}

int64_t MockAccessibilityMouseAutoclick::GetDelayTime()
{
    HILOG_DEBUG();

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("GetCurrentAccountData failed");
        return 0;
    }

    int32_t delayTime = accountData->GetConfig()->GetMouseAutoClick();
    return delayTime;
}

MockAccessibilityMouseAutoclick::MouseAutoclickEventHandler::MouseAutoclickEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    MockAccessibilityMouseAutoclick &mouseAutoclick)
    : AppExecFwk::EventHandler(runner), mouseAutoclick_(mouseAutoclick)
{
    HILOG_DEBUG();
}

void MockAccessibilityMouseAutoclick::MouseAutoclickEventHandler::ProcessEvent(
    const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();

    switch (event->GetInnerEventId()) {
        case AUTOCLICK_TIMEOUT_MSG:
            mouseAutoclick_.SendMouseClickEvent();
            mouseAutoclick_.ResetAutoclickInfo();
            break;
        default:
            break;
    }
}

bool MockAccessibilityMouseKey::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int32_t sourceType = event.GetSourceType();
    int32_t action = event.GetPointerAction();
    std::vector<int32_t> pointers = event.GetPointerIds();
    size_t pointerCount = pointers.size();
    if ((sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (action == MMI::PointerEvent::POINTER_ACTION_MOVE) &&
        (pointerCount == ITEM_COUNT_1)) {
        UpdateLastMouseEvent(event);
    }
    return false;
}

bool MockAccessibilityMouseKey::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    int32_t actionKey = MMI::KeyEvent::KEYCODE_UNKNOWN;
    int32_t metaKey1 = MMI::KeyEvent::KEYCODE_UNKNOWN;
    int32_t metaKey2 = MMI::KeyEvent::KEYCODE_UNKNOWN;
    std::vector<int32_t> pressedKeys = event.GetPressedKeys();
    if (IsMouseKey(pressedKeys, actionKey, metaKey1, metaKey2)) {
        return ExecuteMouseKey(actionKey, metaKey1, metaKey2);
    }
    return false;
}

void MockAccessibilityMouseKey::UpdateLastMouseEvent(const MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    lastMouseMoveEvent_ = std::make_shared<MMI::PointerEvent>(event);
}

bool MockAccessibilityMouseKey::IsMouseKey(const std::vector<int32_t> &pressedKeys, int32_t &actionKey,
    int32_t &metaKey1, int32_t &metaKey2) const
{
    HILOG_DEBUG();

    size_t pressedKeyCount = pressedKeys.size();
    if (pressedKeyCount == ITEM_COUNT_1) {
        if (std::find(MOUSE_KEYCODE_V.begin(), MOUSE_KEYCODE_V.end(), pressedKeys[0]) != MOUSE_KEYCODE_V.end()) {
            actionKey = pressedKeys[0];
            return true;
        }
    } else if (pressedKeyCount == ITEM_COUNT_2) {
        for (size_t i = 0; i < ITEM_COUNT_2; i++) {
            if (std::find(MOUSE_MOVE_KEYCODE_V.begin(), MOUSE_MOVE_KEYCODE_V.end(), pressedKeys[i]) ==
                MOUSE_MOVE_KEYCODE_V.end()) {
                continue;
            }
            actionKey = pressedKeys[i];
            size_t Index = (i + 1) % ITEM_COUNT_2;
            if (std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index]) !=
                CTRL_SHIFT_KEYCODE_V.end()) {
                metaKey1 = pressedKeys[Index];
                return true;
            }
        }
    } else if (pressedKeyCount == ITEM_COUNT_3) {
        for (size_t i = 0; i < ITEM_COUNT_3; i++) {
            if (std::find(MOUSE_MOVE_KEYCODE_V.begin(), MOUSE_MOVE_KEYCODE_V.end(), pressedKeys[i]) ==
                MOUSE_MOVE_KEYCODE_V.end()) {
                continue;
            }
            actionKey = pressedKeys[i];
            size_t Index1 = (i + 1) % ITEM_COUNT_3;
            size_t Index2 = (i + 2) % ITEM_COUNT_3;
            if ((std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index1]) !=
                CTRL_SHIFT_KEYCODE_V.end()) &&
                (std::find(CTRL_SHIFT_KEYCODE_V.begin(), CTRL_SHIFT_KEYCODE_V.end(), pressedKeys[Index2]) !=
                CTRL_SHIFT_KEYCODE_V.end())) {
                metaKey1 = pressedKeys[Index1];
                metaKey2 = pressedKeys[Index2];
                return true;
            }
        }
    }
    return false;
}

int32_t MockAccessibilityMouseKey::ParseMetaKey(int32_t metaKey1, int32_t metaKey2) const
{
    HILOG_DEBUG();
    for (int32_t i = 0; i < ROW_COUNT; i++) {
        if ((metaKey1 == PRESSED_METAKEYS_TBL[i][0]) && (metaKey2 == PRESSED_METAKEYS_TBL[i][1])) {
            return PRESSED_METAKEYS_TBL[i][COLUMN_COUNT - 1];
        }
    }
    return INVALID_KEY;
}

bool MockAccessibilityMouseKey::ExecuteMouseKey(int32_t actionKey, int32_t metaKey1, int32_t metaKey2)
{
    HILOG_DEBUG("actionKey:%{public}d, metaKey1:%{public}d, metaKey2:%{public}d", actionKey, metaKey1, metaKey2);

    if ((actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_1) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_2) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_3) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_4) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_6) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_7) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_8) ||
        (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_9)) {
        auto iter = MOUSE_MOVE_OFFSET_M.find(actionKey);
        if (iter != MOUSE_MOVE_OFFSET_M.end()) {
            int32_t offsetX = iter->second.offsetX;
            int32_t offsetY = iter->second.offsetY;
            int32_t result = ParseMetaKey(metaKey1, metaKey2);
            if ((result == INVALID_KEY) || (result == CTRL_SHIFT_KEY)) {
                return false;
            }
            if (result == CTRL_KEY) {
                offsetX = static_cast<int32_t>(iter->second.offsetX * SPEED_UP_MULTIPLE);
                offsetY = static_cast<int32_t>(iter->second.offsetY * SPEED_UP_MULTIPLE);
            } else if (result == SHIFT_KEY) {
                offsetX = static_cast<int32_t>(iter->second.offsetX * SLOW_DOWN_MULTIPLE);
                offsetY = static_cast<int32_t>(iter->second.offsetY * SLOW_DOWN_MULTIPLE);
            }
            MoveMousePointer(offsetX, offsetY);
        }
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_5) {
        SendMouseClickEvent(SINGLE_CLICK);
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_DIVIDE) {
        selectedKeyType_ = LEFT_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_MULTIPLY) {
        selectedKeyType_ = BOOTH_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_SUBTRACT) {
        selectedKeyType_ = RIGHT_KEY;
    } else if (actionKey == MMI::KeyEvent::KEYCODE_NUMPAD_ADD) {
        SendMouseClickEvent(DOUBLE_CLICK);
    }
    return true;
}

void MockAccessibilityMouseKey::MoveMousePointer(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG("offsetX:%{public}d, offsetY:%{public}d", offsetX, offsetY);

    EventTransmission::OnMoveMouse(offsetX, offsetY);
}

void MockAccessibilityMouseKey::SendMouseClickEvent(CLICK_TYPE clickType)
{
    HILOG_DEBUG();

    if (!lastMouseMoveEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }

    int64_t nowTime = GetSystemTime();
    // Update event information.
    lastMouseMoveEvent_->SetActionTime(nowTime);
    lastMouseMoveEvent_->SetActionStartTime(nowTime);

    // Update pointer item information.
    int32_t pointerId = lastMouseMoveEvent_->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    lastMouseMoveEvent_->GetPointerItem(pointerId, item);
    item.SetDownTime(nowTime);
    item.SetPressed(true);
    lastMouseMoveEvent_->UpdatePointerItem(pointerId, item);

    for (uint32_t clickCount = 0; clickCount < clickType; clickCount ++) {
        HILOG_DEBUG("selectedKeyType:%{public}u", selectedKeyType_);
        if (selectedKeyType_ == LEFT_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        } else if (selectedKeyType_ == RIGHT_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        } else if (selectedKeyType_ == BOOTH_KEY) {
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);

            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_LEFT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
            PerformMouseAction(MMI::PointerEvent::MOUSE_BUTTON_RIGHT, MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
        }
    }
}

void MockAccessibilityMouseKey::PerformMouseAction(int32_t buttonId, int32_t actionType)
{
    HILOG_DEBUG();

    if (!lastMouseMoveEvent_) {
        HILOG_DEBUG("No mouse event to be sent.");
        return;
    }
    lastMouseMoveEvent_->SetButtonId(buttonId);
    lastMouseMoveEvent_->SetButtonPressed(buttonId);
    lastMouseMoveEvent_->SetPointerAction(actionType);
    EventTransmission::OnPointerEvent(*lastMouseMoveEvent_);
}

int64_t MockAccessibilityMouseKey::GetSystemTime() const
{
    HILOG_DEBUG();

    int64_t microsecond = Utils::GetSystemTime() * 1000;
    return microsecond;
}

bool MockAccessibilityPowerManager::DiscountBrightness(const float discount)
{
    auto& displayPowerMgrClient = DisplayPowerMgr::DisplayPowerMgrClient::GetInstance();
    if (!displayPowerMgrClient.DiscountBrightness(discount)) {
        HILOG_ERROR("Failed to set brightness discount");
        return false;
    }

    return true;
}

bool MockAccessibilityPowerManager::RefreshActivity()
{
    HILOG_DEBUG();
    return PowerMgr::PowerMgrClient::GetInstance().RefreshActivity(
        OHOS::PowerMgr::UserActivityType::USER_ACTIVITY_TYPE_TOUCH);
}

bool MockAccessibilityPowerManager::InitRunningLock()
{
    wakeLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(
        HOLD_LOCK_NAME, PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
    if (wakeLock_ == nullptr) {
        HILOG_INFO("CreateRunningLock InitRunningLock failed.");
        return false;
    }
    HILOG_INFO("CreateRunningLock InitRunningLock.");
    return true;
}

std::set<std::string> MockAccessibilityPowerManager::GetWakeLockAbilities()
{
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    return wakeLockAbilities_;
}

bool MockAccessibilityPowerManager::HoldRunningLock(std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    if (wakeLock_ == nullptr) {
        if (!InitRunningLock()) {
            return false;
        }
    }
    if (!wakeLockAbilities_.count(bundleName)) {
        wakeLockAbilities_.insert(bundleName);
    }
    wakeLock_->Lock();
    HILOG_DEBUG("wakeLock_ Lock success.");
    return true;
}

bool MockAccessibilityPowerManager::UnholdRunningLock(std::string &bundleName)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::mutex> lock(powerWakeLockMutex_);
    if (wakeLock_ == nullptr) {
        if (!InitRunningLock()) {
            return false;
        }
    }

    if (bundleName == "") {
        wakeLockAbilities_.clear();
        wakeLock_->UnLock();
        wakeLock_ = nullptr;
        HILOG_DEBUG("wakeLock_ unLock success.");
    } else {
        if (!wakeLockAbilities_.count(bundleName)) {
            HILOG_DEBUG("bundleName: %{public}s not in holdRunningLock status.", bundleName.c_str());
            return true;
        }
        wakeLockAbilities_.erase(bundleName);
        if (wakeLockAbilities_.empty()) {
            wakeLock_->UnLock();
            wakeLock_ = nullptr;
            HILOG_DEBUG("bundleName: %{public}s erased, wakeLock_ unLock success.", bundleName.c_str());
        }
    }
    return true;
}

sptr<AppExecFwk::IBundleMgr> MockAccessibilityResourceBundleManager::GetBundleMgrProxy()
{
    HILOG_DEBUG();
    if (bundleManager_) {
        return bundleManager_;
    }
 
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        HILOG_ERROR("failed:fail to get system ability mgr.");
        return nullptr;
    }
 
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        HILOG_ERROR("failed:fail to get bundle manager proxy.");
        return nullptr;
    }
 
    bundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (!bundleManager_) {
        HILOG_ERROR("fail to new bundle manager.");
        return nullptr;
    }
 
    if (!bundleManagerDeathRecipient_) {
        bundleManagerDeathRecipient_ = new(std::nothrow) BundleManagerDeathRecipient();
        if (!bundleManagerDeathRecipient_) {
            HILOG_ERROR("bundleManagerDeathRecipient_ is null");
            return nullptr;
        }
    }

    if (!bundleManager_) {
        HILOG_ERROR("bundleManager_ is nullptr");
        return nullptr;
    }
    bundleManager_->AsObject()->AddDeathRecipient(bundleManagerDeathRecipient_);
    return bundleManager_;
}
 
ErrCode MockAccessibilityResourceBundleManager::GetBundleInfoV9(const std::string& bundleName, int32_t flags,
    AppExecFwk::BundleInfo& bundleInfo, int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        HILOG_ERROR("get bundleMgr failed");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = bundleMgr->GetBundleInfoV9(bundleName, flags, bundleInfo, userId);
    if (ret != ERR_OK) {
        HILOG_ERROR("get bundleInfo failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

bool MockAccessibilityResourceBundleManager::GetBundleNameByUid(const int uid, std::string &bundleName)
{
    bool ret = true;
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    do {
        if (bundleMgr == nullptr) {
            ret = false;
            break;
        }
        ret = bundleMgr->GetBundleNameForUid(uid, bundleName);
    } while (0);
    if (ret == false) {
        HILOG_ERROR("GetBundleNameByUid failed");
        return false;
    }
    return ret;
}

int MockAccessibilityResourceBundleManager::GetUidByBundleName(const std::string &bundleName,
    const std::string &abilityName, const int userId)
{
    int result = -1;
    bool ret = true;
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    do {
        if (bundleMgr == nullptr) {
            ret = false;
            break;
        }
        result = bundleMgr->GetUidByBundleName(bundleName, userId);
        if (result == -1) {
            ret = false;
            break;
        }
    } while (0);
    if (ret == false) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("GetUidByBundleName failed");
        return ERR_INVALID_VALUE;
    }
    return result;
}
 
bool MockAccessibilityResourceBundleManager::QueryExtensionAbilityInfos(
    const AppExecFwk::ExtensionAbilityType &extensionType, const int32_t &userId,
    std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos)
{
    bool result = true;
    bool ret = true;
    std::lock_guard<ffrt::mutex> lock(bundleMutex_);
    sptr<AppExecFwk::IBundleMgr> bundleMgr = GetBundleMgrProxy();
    do {
        if (bundleMgr == nullptr) {
            ret = false;
            break;
        }
        result = bundleMgr->QueryExtensionAbilityInfos(extensionType, userId, extensionInfos);
        if (result == false) {
            ret = false;
            break;
        }
    } while (0);
    if (ret == false) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT,
            A11yError::ERROR_QUERY_PACKAGE_INFO_FAILED);
        HILOG_ERROR("QueryExtensionAbilityInfos failed");
        return false;
    }
    return result;
}
 
void MockAccessibilityResourceBundleManager::BundleManagerDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Singleton<MockAccessibilityResourceBundleManager>::GetInstance().OnBundleManagerDied(remote);
}
 
void MockAccessibilityResourceBundleManager::OnBundleManagerDied(const wptr<IRemoteObject> &remote)
{
    HILOG_INFO("OnBundleManagerDied ");
    if (bundleManager_->AsObject() == nullptr) {
        HILOG_ERROR("bundleManager_ is nullptr");
        return;
    }
    bundleManager_->AsObject()->RemoveDeathRecipient(bundleManagerDeathRecipient_);
    bundleManager_ = nullptr;
}

MockAccessibilityScreenTouch::MockAccessibilityScreenTouch()
{
    HILOG_DEBUG();
    // get from account data directly
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    clickResponseTime_ = accountData->GetConfig()->GetClickResponseTime();
    ignoreRepeatClickState_ = accountData->GetConfig()->GetIgnoreRepeatClickState();
    ignoreRepeatClickTime_ = accountData->GetConfig()->GetIgnoreRepeatClickTime();

    if (clickResponseTime_ > 0 && ignoreRepeatClickState_ == true) {
        currentState_ = BOTH_RESPONSE_DELAY_IGNORE_REPEAT_CLICK;
    } else if (clickResponseTime_ > 0) {
        currentState_ = CLICK_RESPONSE_DELAY_STATE;
    } else if (ignoreRepeatClickState_ == true) {
        currentState_ = IGNORE_REPEAT_CLICK_STATE;
    } else {
        currentState_ = DEFAULT_STATE;
    }

    lastUpTime_ = lastUpTime;

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetInputManagerRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return;
    }
    handler_ = std::make_shared<ScreenTouchHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

void ScreenTouchHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    HILOG_DEBUG();
    switch (event->GetInnerEventId()) {
        case MockAccessibilityScreenTouch::FINGER_DOWN_DELAY_MSG:
            server_.SendInterceptedEvent();
            break;
        default:
            break;
    }
}

MockAccessibilityScreenTouch::~MockAccessibilityScreenTouch()
{
    lastUpTime = lastUpTime_;
    if (drawCircleThread_ && drawCircleThread_->joinable()) {
        drawCircleThread_->join();
    }
    drawCircleThread_ = nullptr;
    AccessibilityCircleDrawingManager::DeleteInstance();
}

void MockAccessibilityScreenTouch::SendInterceptedEvent()
{
    HILOG_DEBUG();
    isStopDrawCircle_ = true;

    if (cachedDownPointerEvents_.empty()) {
        HILOG_ERROR("Cached down pointer event is empty!");
        return;
    }

    for (auto iter = cachedDownPointerEvents_.begin(); iter != cachedDownPointerEvents_.end(); ++iter) {
        iter->SetActionTime(Utils::GetSystemTime() * US_TO_MS);
        EventTransmission::OnPointerEvent(*iter);
    }
}

uint32_t MockAccessibilityScreenTouch::GetRealClickResponseTime()
{
    auto iter = CLICK_RESPONSE_TIME_MAP.find(clickResponseTime_);
    if (iter != CLICK_RESPONSE_TIME_MAP.end()) {
        return iter->second;
    }

    return CLICK_RESPONSE_TIME_MAP.at(CLICK_RESPONSE_DELAY_SHORT);
}

uint32_t MockAccessibilityScreenTouch::GetRealIgnoreRepeatClickTime()
{
    auto iter = IGNORE_REPEAT_CLICK_TIME_MAP.find(ignoreRepeatClickTime_);
    if (iter != IGNORE_REPEAT_CLICK_TIME_MAP.end()) {
        return iter->second;
    }

    return IGNORE_REPEAT_CLICK_TIME_MAP.at(IGNORE_REPEAT_CLICK_SHORTEST);
}

bool MockAccessibilityScreenTouch::GetRealIgnoreRepeatClickState()
{
    return ignoreRepeatClickState_;
}

void MockAccessibilityScreenTouch::ConversionCoordinates(int32_t originalX, int32_t originalY)
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    int32_t displayWidth = displayMgr.GetWidth();
    int32_t displayHeight = displayMgr.GetHeight();

    OHOS::Rosen::DisplayOrientation orientation = displayMgr.GetOrientation();
    switch (orientation) {
        case OHOS::Rosen::DisplayOrientation::PORTRAIT:
            circleCenterPhysicalX_ = originalX;
            circleCenterPhysicalY_ = originalY;
            startAngle_ = START_ANGLE_PORTRAIT;
            break;
        case OHOS::Rosen::DisplayOrientation::LANDSCAPE:
            circleCenterPhysicalX_ = originalY;
            circleCenterPhysicalY_ = displayWidth - originalX;
            startAngle_ = START_ANGLE_LANDSCAPE;
            break;
        case OHOS::Rosen::DisplayOrientation::PORTRAIT_INVERTED:
            circleCenterPhysicalX_ = displayWidth - originalX;
            circleCenterPhysicalY_ = displayHeight - originalY;
            startAngle_ = START_ANGLE_PORTRAIT_INVERTED;
            break;
        case OHOS::Rosen::DisplayOrientation::LANDSCAPE_INVERTED:
            circleCenterPhysicalX_ = displayHeight - originalY;
            circleCenterPhysicalY_ = originalX;
            startAngle_ = START_ANGLE_LANDSCAPE_INVERTED;
            break;
        default:
            break;
    }
#endif
}

void MockAccessibilityScreenTouch::HandleCoordinates(MMI::PointerEvent::PointerItem &pointerItem)
{
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    int32_t originalX = pointerItem.GetDisplayX();
    int32_t originalY = pointerItem.GetDisplayY();

    switch (ROTATE_POLICY) {
        case WINDOW_ROTATE:
            ConversionCoordinates(originalX, originalY);
            break;
        case SCREEN_ROTATE:
            circleCenterPhysicalX_ = originalX;
            circleCenterPhysicalY_ = originalY;
            startAngle_ = START_ANGLE_PORTRAIT;
            break;
        case FOLDABLE_DEVICE:
            if (FOLDABLE.length() < SUBSCRIPT_TWO) {
                HILOG_ERROR("get foldable parameter failed");
                ConversionCoordinates(originalX, originalY);
                break;
            }
            if ((displayMgr.GetFoldStatus() == Rosen::FoldStatus::EXPAND &&
                FOLDABLE[SUBSCRIPT_TWO] == FOLDABLE_SCREEN_ROTATE) ||
                (displayMgr.GetFoldStatus() == Rosen::FoldStatus::FOLDED &&
                FOLDABLE[SUBSCRIPT_ZERO] == FOLDABLE_SCREEN_ROTATE)) {
                circleCenterPhysicalX_ = originalX;
                circleCenterPhysicalY_ = originalY;
                startAngle_ = START_ANGLE_PORTRAIT;
            } else {
                ConversionCoordinates(originalX, originalY);
            }
            break;
        default:
            HILOG_WARN("unknown rotate policy");
            ConversionCoordinates(originalX, originalY);
            break;
    }
#endif
}

void MockAccessibilityScreenTouch::DrawCircleProgress()
{
    HILOG_DEBUG();

    AccessibilityCircleDrawingManager::GetInstance()->DrawPointer(circleCenterPhysicalX_,
        circleCenterPhysicalY_, 0, screenId_, startAngle_);
    AccessibilityCircleDrawingManager::GetInstance()->UpdatePointerVisible(true);
    uint32_t times = GetRealClickResponseTime() / NUMBER_10;
    uint32_t step = CIRCLE_ANGLE / times;
    uint32_t time = 0;
    while (time < times && isStopDrawCircle_ == false) {
        AccessibilityCircleDrawingManager::GetInstance()->DrawPointer(circleCenterPhysicalX_,
            circleCenterPhysicalY_, step * time, screenId_, startAngle_);
        time++;
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_10));
    }

    AccessibilityCircleDrawingManager::GetInstance()->UpdatePointerVisible(false);
}

void MockAccessibilityScreenTouch::HandleResponseDelayStateInnerDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    MMI::PointerEvent::PointerItem pointerItem;
    if (!event.GetPointerItem(event.GetPointerId(), pointerItem)) {
        HILOG_WARN("get GetPointerItem %{public}d failed", event.GetPointerId());
    }

    if (event.GetPointerIds().size() > POINTER_COUNT_1) {
        if (cachedDownPointerEvents_.empty()) {
            HILOG_ERROR("cached down pointer event is empty!");
            return;
        }
        if (isMoveBeyondThreshold_ == true) {
            cachedDownPointerEvents_.push_back(event);
            EventTransmission::OnPointerEvent(event);
            return;
        } else if (isStopDrawCircle_ == true) {
            return;
        } else {
            cachedDownPointerEvents_.push_back(event);
            return;
        }
    }

    screenId_ = event.GetTargetDisplayId();
    startTime_ = event.GetActionTime();
    startPointer_ = std::make_shared<MMI::PointerEvent::PointerItem>(pointerItem);
    isMoveBeyondThreshold_ = false;

    HILOG_INFO("ROTATE_POLICY = %{public}d, FOLDABLE = %{public}s", ROTATE_POLICY, FOLDABLE.c_str());
    HandleCoordinates(pointerItem);
    isStopDrawCircle_ = false;
    if (drawCircleThread_ && drawCircleThread_->joinable()) {
        drawCircleThread_->join();
    }

    drawCircleThread_ = nullptr;
    drawCircleThread_ = std::make_shared<std::thread>([this] {this->DrawCircleProgress();});
    if (drawCircleThread_ == nullptr) {
        HILOG_ERROR("create draw circle progress fail");
    }

    handler_->RemoveEvent(FINGER_DOWN_DELAY_MSG);
    cachedDownPointerEvents_.clear();
    cachedDownPointerEvents_.push_back(event);
    handler_->SendEvent(FINGER_DOWN_DELAY_MSG, 0, static_cast<int32_t>(GetRealClickResponseTime()));
}

void MockAccessibilityScreenTouch::HandleResponseDelayStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    if (cachedDownPointerEvents_.empty()) {
        HILOG_ERROR("cached down pointer event is empty!");
        return;
    }

    if (isMoveBeyondThreshold_ == true) {
        handler_->RemoveEvent(FINGER_DOWN_DELAY_MSG);
        EventTransmission::OnPointerEvent(event);
        return;
    }

    if (startPointer_ == nullptr) {
        return;
    }

    if (event.GetPointerId() != startPointer_->GetPointerId()) {
        if (isStopDrawCircle_ == true) {
            EventTransmission::OnPointerEvent(event);
        }
        return;
    }

    MMI::PointerEvent::PointerItem pointerItem;
    if (!event.GetPointerItem(event.GetPointerId(), pointerItem)) {
        HILOG_WARN("get GetPointerItem %{public}d failed", event.GetPointerId());
    }

    float offsetX = startPointer_->GetDisplayX() - pointerItem.GetDisplayX();
    float offsetY = startPointer_->GetDisplayY() - pointerItem.GetDisplayY();
    double duration = hypot(offsetX, offsetY);
    if (duration > TOUCH_SLOP) {
        handler_->RemoveEvent(FINGER_DOWN_DELAY_MSG);
        if (isStopDrawCircle_ != true && !cachedDownPointerEvents_.empty()) {
            for (auto iter = cachedDownPointerEvents_.begin(); iter != cachedDownPointerEvents_.end(); ++iter) {
                iter->SetActionTime(Utils::GetSystemTime() * US_TO_MS);
                EventTransmission::OnPointerEvent(*iter);
            }
        }
        EventTransmission::OnPointerEvent(event);
        isMoveBeyondThreshold_ = true;
        isStopDrawCircle_ = true;
        return;
    }

    if (isStopDrawCircle_ != true) {
        HandleCoordinates(pointerItem);
        return;
    }

    EventTransmission::OnPointerEvent(event);
}

void MockAccessibilityScreenTouch::HandleResponseDelayStateInnerUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (cachedDownPointerEvents_.empty()) {
        HILOG_ERROR("cached down pointer event is empty!");
        handler_->RemoveEvent(FINGER_DOWN_DELAY_MSG);
        isStopDrawCircle_ = true;
        return;
    }

    if (isStopDrawCircle_ == true) {
        for (auto iter = cachedDownPointerEvents_.begin(); iter != cachedDownPointerEvents_.end(); ++iter) {
            if (iter->GetPointerId() == event.GetPointerId()) {
                EventTransmission::OnPointerEvent(event);
            }
        }
        if (event.GetPointerIds().size() == POINTER_COUNT_1) {
            cachedDownPointerEvents_.clear();
        }
        return;
    }

    if (startPointer_ != nullptr && event.GetPointerId() == startPointer_->GetPointerId()) {
        handler_->RemoveEvent(FINGER_DOWN_DELAY_MSG);
        isStopDrawCircle_ = true;
        cachedDownPointerEvents_.clear();
    } else {
        auto iter = std::find_if(cachedDownPointerEvents_.begin(), cachedDownPointerEvents_.end(),
            [&](const MMI::PointerEvent &e) {
                return e.GetPointerId() == event.GetPointerId();
            });
        if (iter != cachedDownPointerEvents_.end()) {
            cachedDownPointerEvents_.erase(iter);
        }
    }
}

void MockAccessibilityScreenTouch::HandleResponseDelayState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            HandleResponseDelayStateInnerDown(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleResponseDelayStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            HandleResponseDelayStateInnerUp(event);
            break;
        default:
            EventTransmission::OnPointerEvent(event);
            break;
    }
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int64_t downTime = event.GetActionTime();
    if ((event.GetPointerIds().size() == POINTER_COUNT_1) &&
        ((downTime - lastUpTime_) / US_TO_MS < GetRealIgnoreRepeatClickTime())) {
        isInterceptClick_ = true;
        return;
    } else if ((event.GetPointerIds().size() > POINTER_COUNT_1) && (isInterceptClick_ == true)) {
        return;
    }

    EventTransmission::OnPointerEvent(event);
    isInterceptClick_ = false;
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        EventTransmission::OnPointerEvent(event);
    }
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        EventTransmission::OnPointerEvent(event);
        if (event.GetPointerIds().size() == POINTER_COUNT_1) {
            lastUpTime_ = event.GetActionTime();
        }
    }
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            HandleIgnoreRepeatClickStateInnerDown(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleIgnoreRepeatClickStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            HandleIgnoreRepeatClickStateInnerUp(event);
            break;
        default:
            EventTransmission::OnPointerEvent(event);
            break;
    }
}

void MockAccessibilityScreenTouch::HandleBothStateInnerDown(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    int64_t downTime = event.GetActionTime();
    if ((event.GetPointerIds().size() == POINTER_COUNT_1) &&
        ((downTime - lastUpTime_) / US_TO_MS < GetRealIgnoreRepeatClickTime())) {
        isInterceptClick_ = true;
        return;
    } else if ((event.GetPointerIds().size() > POINTER_COUNT_1) && (isInterceptClick_ == true)) {
        return;
    }

    isInterceptClick_ = false;

    HandleResponseDelayStateInnerDown(event);
}

void MockAccessibilityScreenTouch::HandleBothStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == true) {
        return;
    }

    HandleResponseDelayStateInnerMove(event);
}

void MockAccessibilityScreenTouch::HandleBothStateInnerUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == true) {
        return;
    }

    if (event.GetPointerIds().size() == POINTER_COUNT_1) {
        lastUpTime_ = event.GetActionTime();
    }

    HandleResponseDelayStateInnerUp(event);
}

void MockAccessibilityScreenTouch::HandleBothState(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    switch (event.GetPointerAction()) {
        case MMI::PointerEvent::POINTER_ACTION_DOWN:
            HandleBothStateInnerDown(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_MOVE:
            HandleBothStateInnerMove(event);
            break;
        case MMI::PointerEvent::POINTER_ACTION_UP:
            HandleBothStateInnerUp(event);
            break;
        default:
            EventTransmission::OnPointerEvent(event);
            break;
    }
}

void MockAccessibilityScreenTouch::Clear()
{
    isMoveBeyondThreshold_ = false;
    isInterceptClick_ = false;
    startPointer_ = nullptr;
}

bool MockAccessibilityScreenTouch::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();
    MMI::PointerEvent::PointerItem pointerItem;
    if (!event.GetPointerItem(event.GetPointerId(), pointerItem)) {
        HILOG_WARN("get GetPointerItem %{public}d failed", event.GetPointerId());
        return false;
    }
    if (pointerItem.GetToolType() == MMI::PointerEvent::TOOL_TYPE_KNUCKLE) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    if (event.GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        EventTransmission::OnPointerEvent(event);
        return false;
    }

    if (event.GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_CANCEL) {
        Clear();
        return true;
    }

    switch (currentState_) {
        case ScreenTouchState::CLICK_RESPONSE_DELAY_STATE:
            HandleResponseDelayState(event);
            break;
        case ScreenTouchState::IGNORE_REPEAT_CLICK_STATE:
            HandleIgnoreRepeatClickState(event);
            break;
        case ScreenTouchState::BOTH_RESPONSE_DELAY_IGNORE_REPEAT_CLICK:
            HandleBothState(event);
            break;
        case ScreenTouchState::DEFAULT_STATE:
        default:
            EventTransmission::OnPointerEvent(event);
    }

    r
} // namespace Accessibility
} // namespace OHOS