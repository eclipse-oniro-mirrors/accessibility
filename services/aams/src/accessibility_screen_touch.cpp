/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include <map>
#include "accessibility_screen_touch.h"
#include "accessibility_circle_drawing_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"
#include "parameters.h"

namespace OHOS {
namespace Accessibility {

constexpr int32_t POINTER_COUNT_1 = 1;

constexpr uint32_t CLICK_RESPONSE_DELAY_SHORT = 0;
constexpr uint32_t CLICK_RESPONSE_DELAY_MEDIUM = 1;
constexpr uint32_t CLICK_RESPONSE_DELAY_LONG = 2;

constexpr uint32_t CLICK_RESPONSE_TIME_SHORT = 0; // ms
constexpr uint32_t CLICK_RESPONSE_TIME_MEDIUM = 300; // ms
constexpr uint32_t CLICK_RESPONSE_TIME_LONG = 600; // ms

constexpr uint32_t IGNORE_REPEAT_CLICK_SHORTEST = 0;
constexpr uint32_t IGNORE_REPEAT_CLICK_SHORT = 1;
constexpr uint32_t IGNORE_REPEAT_CLICK_MEDIUM = 2;
constexpr uint32_t IGNORE_REPEAT_CLICK_LONG = 3;
constexpr uint32_t IGNORE_REPEAT_CLICK_LONGEST = 4;

constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_SHORTEST = 100; // ms
constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_SHORT = 400; // ms
constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_MEDIUM = 700; // ms
constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_LONG = 1000; // ms
constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_LONGEST = 1300; // ms

constexpr uint32_t CIRCLE_ANGLE = 360;
constexpr uint32_t START_ANGLE_PORTRAIT = -90;
constexpr uint32_t START_ANGLE_LANDSCAPE = 180;
constexpr uint32_t START_ANGLE_PORTRAIT_INVERTED = 90;
constexpr uint32_t START_ANGLE_LANDSCAPE_INVERTED = 0;

constexpr uint32_t NUMBER_10 = 10;

constexpr float TOUCH_SLOP = 8.0f;

const int32_t ROTATE_POLICY = system::GetIntParameter("const.window.device.rotate_policy", 0);
const std::string FOLDABLE = system::GetParameter("const.window.foldabledevice.rotate_policy", "");
constexpr int32_t WINDOW_ROTATE = 0;
constexpr int32_t SCREEN_ROTATE = 1;
constexpr int32_t FOLDABLE_DEVICE = 2;
constexpr int32_t SUBSCRIPT_TWO = 2;
constexpr int32_t SUBSCRIPT_ZERO = 0;
constexpr char FOLDABLE_SCREEN_ROTATE = '1';

const std::map<uint32_t, uint32_t> CLICK_RESPONSE_TIME_MAP = {
    {CLICK_RESPONSE_DELAY_SHORT, CLICK_RESPONSE_TIME_SHORT},
    {CLICK_RESPONSE_DELAY_MEDIUM, CLICK_RESPONSE_TIME_MEDIUM},
    {CLICK_RESPONSE_DELAY_LONG, CLICK_RESPONSE_TIME_LONG}
};

const std::map<uint32_t, uint32_t> IGNORE_REPEAT_CLICK_TIME_MAP = {
    {IGNORE_REPEAT_CLICK_SHORTEST, IGNORE_REPEAT_CLICK_TIME_SHORTEST},
    {IGNORE_REPEAT_CLICK_SHORT, IGNORE_REPEAT_CLICK_TIME_SHORT},
    {IGNORE_REPEAT_CLICK_MEDIUM, IGNORE_REPEAT_CLICK_TIME_MEDIUM},
    {IGNORE_REPEAT_CLICK_LONG, IGNORE_REPEAT_CLICK_TIME_LONG},
    {IGNORE_REPEAT_CLICK_LONGEST, IGNORE_REPEAT_CLICK_TIME_LONGEST}
};

int64_t AccessibilityScreenTouch::lastUpTime = 0; // global last up time

ScreenTouchHandler::ScreenTouchHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    AccessibilityScreenTouch &server) : AppExecFwk::EventHandler(runner), server_(server)
{
}

AccessibilityScreenTouch::AccessibilityScreenTouch()
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

    runner_ = Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner();
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
        case AccessibilityScreenTouch::FINGER_DOWN_DELAY_MSG:
            server_.SendInterceptedEvent();
            break;
        default:
            break;
    }
}

AccessibilityScreenTouch::~AccessibilityScreenTouch()
{
    lastUpTime = lastUpTime_;
    if (drawCircleThread_ && drawCircleThread_->joinable()) {
        drawCircleThread_->join();
    }
    drawCircleThread_ = nullptr;
    AccessibilityCircleDrawingManager::DeleteInstance();
}

void AccessibilityScreenTouch::SendInterceptedEvent()
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

uint32_t AccessibilityScreenTouch::GetRealClickResponseTime()
{
    auto iter = CLICK_RESPONSE_TIME_MAP.find(clickResponseTime_);
    if (iter != CLICK_RESPONSE_TIME_MAP.end()) {
        return iter->second;
    }

    return CLICK_RESPONSE_TIME_MAP.at(CLICK_RESPONSE_DELAY_SHORT);
}

uint32_t AccessibilityScreenTouch::GetRealIgnoreRepeatClickTime()
{
    auto iter = IGNORE_REPEAT_CLICK_TIME_MAP.find(ignoreRepeatClickTime_);
    if (iter != IGNORE_REPEAT_CLICK_TIME_MAP.end()) {
        return iter->second;
    }

    return IGNORE_REPEAT_CLICK_TIME_MAP.at(IGNORE_REPEAT_CLICK_SHORTEST);
}

bool AccessibilityScreenTouch::GetRealIgnoreRepeatClickState()
{
    return ignoreRepeatClickState_;
}

void AccessibilityScreenTouch::ConversionCoordinates(int32_t originalX, int32_t originalY)
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

void AccessibilityScreenTouch::HandleCoordinates(MMI::PointerEvent::PointerItem &pointerItem)
{
    int32_t originalX = pointerItem.GetDisplayX();
    int32_t originalY = pointerItem.GetDisplayY();

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
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
#else
    HILOG_WARN("display manager is not enable");
    circleCenterPhysicalX_ = originalX;
    circleCenterPhysicalY_ = originalY;
    startAngle_ = START_ANGLE_PORTRAIT;
#endif
}

void AccessibilityScreenTouch::DrawCircleProgress()
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

void AccessibilityScreenTouch::HandleResponseDelayStateInnerDown(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleResponseDelayStateInnerMove(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleResponseDelayStateInnerUp(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleResponseDelayState(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerDown(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        EventTransmission::OnPointerEvent(event);
    }
}

void AccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        EventTransmission::OnPointerEvent(event);
        if (event.GetPointerIds().size() == POINTER_COUNT_1) {
            lastUpTime_ = event.GetActionTime();
        }
    }
}

void AccessibilityScreenTouch::HandleIgnoreRepeatClickState(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleBothStateInnerDown(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleBothStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == true) {
        return;
    }

    HandleResponseDelayStateInnerMove(event);
}

void AccessibilityScreenTouch::HandleBothStateInnerUp(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::HandleBothState(MMI::PointerEvent &event)
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

void AccessibilityScreenTouch::Clear()
{
    isMoveBeyondThreshold_ = false;
    isInterceptClick_ = false;
    startPointer_ = nullptr;
}

bool AccessibilityScreenTouch::OnPointerEvent(MMI::PointerEvent &event)
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

    return true;
}
} // namespace Accessibility
} // namespace OHOS