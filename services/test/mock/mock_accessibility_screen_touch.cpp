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
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
constexpr uint32_t START_ANGLE_PORTRAIT = -90;
constexpr uint32_t START_ANGLE_LANDSCAPE = 180;
constexpr uint32_t START_ANGLE_PORTRAIT_INVERTED = 90;
constexpr uint32_t START_ANGLE_LANDSCAPE_INVERTED = 0;
#endif
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

int64_t MockAccessibilityScreenTouch::lastUpTime = 0; // global last up time

MockScreenTouchHandler::MockScreenTouchHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
    MockAccessibilityScreenTouch &server) : AppExecFwk::EventHandler(runner), server_(server)
{
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
    handler_ = std::make_shared<MockScreenTouchHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return;
    }
}

void MockScreenTouchHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
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
    MockAccessibilityCircleDrawingManager::DeleteInstance();
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
        MockEventTransmission::OnPointerEvent(*iter);
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

    MockAccessibilityCircleDrawingManager::GetInstance()->DrawPointer(circleCenterPhysicalX_,
        circleCenterPhysicalY_, 0, screenId_, startAngle_);
    MockAccessibilityCircleDrawingManager::GetInstance()->UpdatePointerVisible(true);
    uint32_t times = GetRealClickResponseTime() / NUMBER_10;
    uint32_t step = CIRCLE_ANGLE / times;
    uint32_t time = 0;
    while (time < times && isStopDrawCircle_ == false) {
        MockAccessibilityCircleDrawingManager::GetInstance()->DrawPointer(circleCenterPhysicalX_,
            circleCenterPhysicalY_, step * time, screenId_, startAngle_);
        time++;
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_10));
    }

    MockAccessibilityCircleDrawingManager::GetInstance()->UpdatePointerVisible(false);
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
            MockEventTransmission::OnPointerEvent(event);
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
        MockEventTransmission::OnPointerEvent(event);
        return;
    }

    if (startPointer_ == nullptr) {
        return;
    }

    if (event.GetPointerId() != startPointer_->GetPointerId()) {
        if (isStopDrawCircle_ == true) {
            MockEventTransmission::OnPointerEvent(event);
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
                MockEventTransmission::OnPointerEvent(*iter);
            }
        }
        MockEventTransmission::OnPointerEvent(event);
        isMoveBeyondThreshold_ = true;
        isStopDrawCircle_ = true;
        return;
    }

    if (isStopDrawCircle_ != true) {
        HandleCoordinates(pointerItem);
        return;
    }

    MockEventTransmission::OnPointerEvent(event);
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
                MockEventTransmission::OnPointerEvent(event);
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
            MockEventTransmission::OnPointerEvent(event);
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

    MockEventTransmission::OnPointerEvent(event);
    isInterceptClick_ = false;
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerMove(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        MockEventTransmission::OnPointerEvent(event);
    }
}

void MockAccessibilityScreenTouch::HandleIgnoreRepeatClickStateInnerUp(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    if (isInterceptClick_ == false) {
        MockEventTransmission::OnPointerEvent(event);
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
            MockEventTransmission::OnPointerEvent(event);
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
            MockEventTransmission::OnPointerEvent(event);
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
        MockEventTransmission::OnPointerEvent(event);
        return false;
    }

    if (event.GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        MockEventTransmission::OnPointerEvent(event);
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
            MockEventTransmission::OnPointerEvent(event);
    }

    return true;
}

std::shared_ptr<MockAccessibilityCircleDrawingManager> MockAccessibilityCircleDrawingManager::GetInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        pointDrawMgr_ = std::make_shared<MockAccessibilityCircleDrawingManager>();
    }
    return pointDrawMgr_;
}

void MockAccessibilityCircleDrawingManager::DeleteInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }
    pointDrawMgr_ = nullptr;
}

MockAccessibilityCircleDrawingManager::MockAccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
    imageWidth_ = DEFAULT_WIDTH;
    imageHeight_ = DEFAULT_HEIGHT;
    half_ = DEFAULT_WIDTH / DEFAULT_HALF;

#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    AccessibilityDisplayManager &displayMgr = Singleton<AccessibilityDisplayManager>::GetInstance();
    screenId_ = displayMgr.GetDefaultDisplayId(); // default screenId 0
    auto dpi = displayMgr.GetDefaultDisplayDpi();
    dispalyDensity_ = static_cast<float>(dpi) / DEFAULT_PIXEL_DENSITY;
#else
    HILOG_DEBUG("not support display manager");
    screenId_ = 0;
    dispalyDensity_ = 1;
#endif
}

MockAccessibilityCircleDrawingManager::~MockAccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    surfaceNode_->ClearChildren();
    surfaceNode_->DetachToDisplay(screenId_);
    surfaceNode_ = nullptr;
    canvasNode_ = nullptr;
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void MockAccessibilityCircleDrawingManager::UpdatePointerVisible(bool state)
{
    HILOG_DEBUG("state %{public}s", state ? "true" : "false");
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    surfaceNode_->SetVisible(state);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void MockAccessibilityCircleDrawingManager::CreatePointerWindow(int32_t physicalX, int32_t physicalY, uint64_t screenId)
{
    HILOG_DEBUG();
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "screen touch progress";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    surfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("create surfaceNode_ fail");
        return;
    }

    surfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    surfaceNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    surfaceNode_->SetBounds(physicalX - half_, physicalY - half_, imageWidth_, imageHeight_);
    surfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT); // USE_ROSEN_DRAWING
    screenId_ = screenId;
    surfaceNode_->AttachToDisplay(screenId);
    surfaceNode_->SetRotation(0);

    canvasNode_ = Rosen::RSCanvasNode::Create();
    if (canvasNode_ == nullptr) {
        HILOG_ERROR("create canvasNode_ fail");
        return;
    }

    surfaceNode_->AddChild(canvasNode_, DEFAULT_VALUE);
    canvasNode_->SetBounds(0, 0, imageWidth_, imageHeight_);
    canvasNode_->SetFrame(0, 0, imageWidth_, imageHeight_);
    canvasNode_->SetBackgroundColor(SK_ColorTRANSPARENT); // USE_ROSEN_DRAWING
    canvasNode_->SetCornerRadius(1);
    canvasNode_->SetPositionZ(Rosen::RSSurfaceNode::POINTER_WINDOW_POSITION_Z);
    canvasNode_->SetRotation(0);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

#ifndef USE_ROSEN_DRAWING
void MockAccessibilityCircleDrawingManager::DrawingProgressByOpenSource(int32_t physicalX, int32_t physicalY, int32_t angle)
{
    HILOG_DEBUG();
    auto canvas = static_cast<Rosen::RSRecordingCanvas *>(canvasNode_->BeginRecording(imageWidth_, imageHeight_));

    // outer circle
    SkPaint outCirclePaint;
    outCirclePaint.setAntiAlias(true);
    outCirclePaint.setAlphaf(OUT_CIRCLE_BACKGROUND_ALPHA);

    std::vector<int32_t> outCircleColor = ParseColorString(OUT_CIRCLE_BACKGROUND_COLOR_TYPE);
    outCirclePaint.setARGB(OUT_CIRCLE_BACKGROUND_ALPHA, outCircleColor[NUMBER_0], outCircleColor[NUMBER_1],
        outCircleColor[NUMBER_2]);
    outCirclePaint.setStyle(SkPaint::kFill_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * OUT_CIRCLE_RADIUS, outCirclePaint);

    // center circle
    SkPaint centerCirclePaint;
    centerCirclePaint.setAntiAlias(true);
    centerCirclePaint.setAlphaf(CENTER_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> centerCircleColor = ParseColorString(PROGRESS_COLOR_TYPE);
    centerCirclePaint.setARGB(CENTER_CIRCLE_BACKGROUND_ALPHA, centerCircleColor[NUMBER_0],
        centerCircleColor[NUMBER_1], centerCircleColor[NUMBER_2]);
    centerCirclePaint.setStyle(SkPaint::kFill_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * CENTER_CIRCLE_RADIUS, centerCirclePaint);

    // progress circle
    SkPaint progressCirclePaint;
    progressCirclePaint.setAntiAlias(true);
    progressCirclePaint.setAlphaf(PROGRESS_BACKGROUND_ALPHA);
    std::vector<int32_t> progressCircleColor = ParseColorString(PROGRESS_BACKGROUND_COLOR_TYPE);
    progressCirclePaint.setARGB(PROGRESS_BACKGROUND_ALPHA, progressCircleColor[NUMBER_0],
        progressCircleColor[NUMBER_1], progressCircleColor[NUMBER_2]);
    progressCirclePaint.setStrokeWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);
    progressCirclePaint.setStyle(SkPaint::kStroke_Style);
    canvas->drawCircle(half_, half_, dispalyDensity_ * PROGRESS_RADIUS, progressCirclePaint);

    // progress arc
    SkPaint progressArcPaint;
    progressArcPaint.setAntiAlias(true);
    progressArcPaint.setAlphaf(PROGRESS_BAR_BACKGROUND_ALPHA);
    std::vector<int32_t> progressArcColor = ParseColorString(PROGRESS_COLOR_TYPE);
    progressArcPaint.setARGB(PROGRESS_BAR_BACKGROUND_ALPHA, progressArcColor[NUMBER_0],
        progressArcColor[NUMBER_1], progressArcColor[NUMBER_2]);
    progressArcPaint.setStrokeWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);
    progressArcPaint.setStyle(SkPaint::kStroke_Style);

    SkRect arcRect = SkRect::MakeLTRB(half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS);
    canvas->drawArc(arcRect, startAngle_, angle, false, progressArcPaint);

    canvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
}
#else
void MockAccessibilityCircleDrawingManager::DrawingProgressByRosenDrawing(int32_t physicalX, int32_t physicalY,
    int32_t angle)
{
    HILOG_DEBUG();
    auto canvas = canvasNode_->BeginRecording(imageWidth_, imageHeight_);

    // outer circle
    Rosen::Drawing::Brush outCircleBrush;
    outCircleBrush.SetAntiAlias(true);
    outCircleBrush.SetAlphaF(OUT_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> outCircleColor = ParseColorString(OUT_CIRCLE_BACKGROUND_COLOR_TYPE);
    outCircleBrush.SetARGB(OUT_CIRCLE_BACKGROUND_ALPHA, outCircleColor[NUMBER_0], outCircleColor[NUMBER_1],
        outCircleColor[NUMBER_2]);

    canvas->AttachBrush(outCircleBrush);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * OUT_CIRCLE_RADIUS);
    canvas->DetachBrush();

    // center circle
    Rosen::Drawing::Brush centerCircleBrush;
    centerCircleBrush.SetAntiAlias(true);
    centerCircleBrush.SetAlphaF(CENTER_CIRCLE_BACKGROUND_ALPHA);
    std::vector<int32_t> centerCircleColor = ParseColorString(PROGRESS_COLOR_TYPE);
    centerCircleBrush.SetARGB(CENTER_CIRCLE_BACKGROUND_ALPHA, centerCircleColor[NUMBER_0],
        centerCircleColor[NUMBER_1], centerCircleColor[NUMBER_2]);

    canvas->AttachBrush(centerCircleBrush);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * CENTER_CIRCLE_RADIUS);
    canvas->DetachBrush();

    // progress circle
    Rosen::Drawing::Pen progressPen;
    progressPen.SetAntiAlias(true);
    progressPen.SetAlphaF(PROGRESS_BACKGROUND_ALPHA);
    std::vector<int32_t> progressCircleColor = ParseColorString(PROGRESS_BACKGROUND_COLOR_TYPE);
    progressPen.SetARGB(PROGRESS_BACKGROUND_ALPHA, progressCircleColor[NUMBER_0],
        progressCircleColor[NUMBER_1], progressCircleColor[NUMBER_2]);
    progressPen.SetWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);

    canvas->AttachPen(progressPen);
    canvas->DrawCircle(Rosen::Drawing::Point(half_, half_), dispalyDensity_ * PROGRESS_RADIUS);
    canvas->DetachPen();

    // progress arc
    Rosen::Drawing::Pen progressArcPen;
    progressArcPen.SetAntiAlias(true);
    progressArcPen.SetAlphaF(PROGRESS_BAR_BACKGROUND_ALPHA);
    std::vector<int32_t> progressArcColor = ParseColorString(PROGRESS_COLOR_TYPE);
    progressArcPen.SetARGB(PROGRESS_BAR_BACKGROUND_ALPHA, progressArcColor[NUMBER_0],
        progressArcColor[NUMBER_1], progressArcColor[NUMBER_2]);
    progressArcPen.SetWidth(PROGRESS_STROKE_WIDTH * dispalyDensity_);

    Rosen::Drawing::Rect rect(half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ - dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS,
        half_ + dispalyDensity_ * PROGRESS_RADIUS);
    canvas->AttachPen(progressArcPen);
    canvas->DrawArc(rect, startAngle_, angle);
    canvas->DetachPen();

    canvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
}
#endif

void MockAccessibilityCircleDrawingManager::DrawingProgress(int32_t physicalX, int32_t physicalY, int32_t angle)
{
#ifndef USE_ROSEN_DRAWING
    DrawingProgressByOpenSource(physicalX, physicalY, angle);
#else
    DrawingProgressByRosenDrawing(physicalX, physicalY, angle);
#endif
}

void MockAccessibilityCircleDrawingManager::SetPointerLocation(int32_t physicalX, int32_t physicalY, uint64_t screenId)
{
    HILOG_DEBUG("Pointer window move, x:%{public}d, y:%{public}d", physicalX, physicalY);
    if (surfaceNode_ != nullptr) {
        surfaceNode_->SetBounds(physicalX - half_,
            physicalY - half_,
            surfaceNode_->GetStagingProperties().GetBounds().z_,
            surfaceNode_->GetStagingProperties().GetBounds().w_);
        screenId_ = screenId;
        surfaceNode_->AttachToDisplay(screenId);
        Rosen::RSTransaction::FlushImplicitTransaction();
    }

    if (canvasNode_ != nullptr) {
        canvasNode_->SetBounds(0,
            0,
            canvasNode_->GetStagingProperties().GetBounds().z_,
            canvasNode_->GetStagingProperties().GetBounds().w_);
        canvasNode_->SetFrame(0,
            0,
            canvasNode_->GetStagingProperties().GetBounds().z_,
            canvasNode_->GetStagingProperties().GetBounds().w_);
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
}

void MockAccessibilityCircleDrawingManager::DrawPointer(int32_t physicalX, int32_t physicalY, int32_t angle,
    uint64_t screenId, int32_t startAngle)
{
    HILOG_DEBUG();
    startAngle_ = startAngle;
    if (surfaceNode_ != nullptr) {
        SetPointerLocation(physicalX, physicalY, screenId);
        DrawingProgress(physicalX, physicalY, angle);
        UpdatePointerVisible(true);
        HILOG_DEBUG("surfaceNode_ is existed");
        return;
    }

    CreatePointerWindow(physicalX, physicalY, screenId);
    if (surfaceNode_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }

    DrawingProgress(physicalX, physicalY, angle);
    UpdatePointerVisible(true);
}

int32_t MockAccessibilityCircleDrawingManager::ParseStringToInteger(const std::string& color)
{
    int32_t val = 0;
    int32_t base = 1;
    if (color.size() < 1) {
        return val;
    }
    for (int32_t index = static_cast<int32_t>(color.size() - 1); index >= 0; index--) {
        if (color[index] >= CHAR_0 && color[index] <= CHAR_9) {
            val += base * static_cast<int32_t>(color[index] - CHAR_0);
        }

        if (color[index] == CHAR_A) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_B) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_C) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_D) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_E) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        if (color[index] == CHAR_F) {
            val += base * static_cast<int32_t>(color[index] - CHAR_A + DECIMAL);
        }

        base = base * HEXADECIMAL;
    }

    return val;
}

std::vector<int32_t> MockAccessibilityCircleDrawingManager::ParseColorString(const std::string& color)
{
    std::string r = color.substr(RED_START_INDEX, COLOR_STRING_LENGTH);
    std::string g = color.substr(GREEN_START_INDEX, COLOR_STRING_LENGTH);
    std::string b = color.substr(BLUE_START_INDEX, COLOR_STRING_LENGTH);

    int32_t rValue = ParseStringToInteger(r);
    int32_t gValue = ParseStringToInteger(g);
    int32_t bValue = ParseStringToInteger(b);

    std::vector<int32_t> rgbValue = {rValue, gValue, bValue};
    return rgbValue;
}

void MockAccessibilityCommonEvent::SubscriberEvent(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    HILOG_DEBUG();

    if (subscriber_) {
        HILOG_DEBUG("Common Event is already subscribered!");
        return;
    }

    EventFwk::MatchingSkills matchingSkills;
    for (auto &event : handleEventFunc_) {
        HILOG_DEBUG("Add event: %{public}s", event.first.c_str());
        matchingSkills.AddEvent(event.first);
    }

    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscriber_ = std::make_shared<MockAccessibilityCommonEventSubscriber>(subscribeInfo, *this);
    eventHandler_ = handler;

    int32_t retry = RETRY_SUBSCRIBER;
    do {
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        if (subscribeResult) {
            HILOG_INFO("SubscriberEvent success.");
            return;
        } else {
            HILOG_DEBUG("SubscriberEvent failed, retry %{public}d", retry);
            retry--;
            sleep(1);
        }
    } while (retry);

    HILOG_ERROR("SubscriberEvent failed.");
}

void MockAccessibilityCommonEvent::UnSubscriberEvent()
{
    HILOG_INFO();
    if (subscriber_) {
        bool unSubscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        HILOG_INFO("unSubscribeResult = %{public}d", unSubscribeResult);
        subscriber_ = nullptr;
        eventHandler_ = nullptr;
    }
}

void MockAccessibilityCommonEvent::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    HILOG_DEBUG();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    eventHandler_->PostTask([this, data]() {
        HILOG_DEBUG();
        std::string action = data.GetWant().GetAction();
        HILOG_INFO("Handle event:[%{public}s] eventHandles size[%{public}zu]", action.c_str(), eventHandles_.size());
        auto it = eventHandles_.find(action);
        if (it == eventHandles_.end()) {
            HILOG_ERROR("Ignore event: %{public}s", action.c_str());
            return;
        }
        it->second(data);
        }, "TASK_ON_RECEIVE_EVENT");
}

void MockAccessibilityCommonEvent::HandleUserAdded(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO();
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(accountId);
}

void MockAccessibilityCommonEvent::HandleUserRemoved(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO();
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(accountId);
}

void MockAccessibilityCommonEvent::HandleUserSwitched(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO();
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(accountId);
}

void MockAccessibilityCommonEvent::HandlePackageRemoved(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    int userId = data.GetWant().GetIntParam(KEY_USER_ID, 0);
    int32_t accountId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    if (userId != accountId) {
        HILOG_ERROR("not same user.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
}

void MockAccessibilityCommonEvent::HandlePackageAdd(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    int userId = data.GetWant().GetIntParam(KEY_USER_ID, 0);
    int32_t accountId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    if (userId != accountId) {
        HILOG_ERROR("not same user.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
}

void MockAccessibilityCommonEvent::HandlePackageChanged(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    int userId = data.GetWant().GetIntParam(KEY_USER_ID, 0);
    int32_t accountId = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    if (userId != accountId) {
        HILOG_ERROR("not same user.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
}

void MockAccessibilityCommonEvent::HandleDataShareReady(const EventFwk::CommonEventData &data) const
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        HILOG_ERROR("get accountData failed");
        return;
    }

    auto config = accountData->GetConfig();
    if (config == nullptr) {
        HILOG_WARN("config is nullptr");
        return;
    }

    if (config->GetInitializeState()) {
        HILOG_INFO("datashare no need reInit.");
        return;
    }

    HILOG_INFO("reInit datashare.");
    config->Init();
}

void MockAccessibilityCommonEvent::HandlePowerOnEvent(const EventFwk::CommonEventData &data) const
{
    IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
    HILOG_ERROR("HandlePowerOnEvent");
}

void MockAccessibilityCommonEvent::HandleLocalChangedEvent(const EventFwk::CommonEventData &data) const
{
    HILOG_DEBUG("reInit Resource.");
    Singleton<AccessibleAbilityManagerService>::GetInstance().InitResource(true);
}

MockAccessibilityDatashareHelper::~MockAccessibilityDatashareHelper()
{
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    if (dataShareHelper_ != nullptr) {
        DestoryDatashareHelper(dataShareHelper_);
        dataShareHelper_ = nullptr;
    }
#endif
}

std::string MockAccessibilityDatashareHelper::GetStringValue(const std::string& key, const std::string& defaultValue,
    const bool readOnlyFlag)
{
    std::string resultStr = defaultValue;
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    std::shared_ptr<DataShare::DataShareResultSet> resultSet = nullptr;
    do {
        std::vector<std::string> columns = { SETTING_COLUMN_VALUE };
        DataShare::DataSharePredicates predicates;
        Uri uri(AssembleUri(key));
        int32_t count = 0;
        predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
        if (dataShareHelper_ == nullptr) {
            break;
        }
        resultSet = dataShareHelper_->Query(uri, predicates, columns);
        if (resultSet == nullptr) {
            Utils::RecordDatashareInteraction(A11yDatashareValueType::GET);
            break;
        }
        resultSet->GetRowCount(count);
        if (count == 0) {
            if (!readOnlyFlag) {
                RetError ret = PutStringValue(key, defaultValue);
                HILOG_INFO("put default key %{public}s, ret = %{public}d", key.c_str(), static_cast<int32_t>(ret));
            }
            break;
        }
        resultSet->GoToRow(INDEX);
        int32_t rtn = resultSet->GetString(INDEX, resultStr);
        if (rtn  != DataShare::E_OK) {
            break;
        }
    } while (0);
    if (resultSet != nullptr) {
        resultSet->Close();
        resultSet = nullptr;
    }
    IPCSkeleton::SetCallingIdentity(callingIdentity);
#endif
    return resultStr;
}

int64_t MockAccessibilityDatashareHelper::GetLongValue(const std::string& key, const int64_t& defaultValue,
    const bool readOnlyFlag)
{
    int64_t result = defaultValue;
    std::string valueStr = GetStringValue(key, std::to_string(result), readOnlyFlag);
    if (valueStr != "") {
        result = static_cast<int64_t>(std::strtoll(valueStr.c_str(), nullptr, DECIMAL_NOTATION));
    }
    return result;
}

int32_t MockAccessibilityDatashareHelper::GetIntValue(const std::string& key, const int32_t& defaultValue,
    const bool readOnlyFlag)
{
    int64_t valueLong = GetLongValue(key, defaultValue, readOnlyFlag);
    return static_cast<int32_t>(valueLong);
}

bool MockAccessibilityDatashareHelper::GetBoolValue(const std::string& key, const bool& defaultValue,
    const bool readOnlyFlag)
{
    bool result = defaultValue;
    std::string valueStr = GetStringValue(key, result ? "1" : "0", readOnlyFlag);
    if (valueStr != "") {
        result = (valueStr == "1" || valueStr == "true");
    }
    return result;
}

float MockAccessibilityDatashareHelper::GetFloatValue(const std::string& key, const float& defaultValue,
    const bool readOnlyFlag)
{
    float result = defaultValue;
    std::string valueStr = GetStringValue(key, std::to_string(result), readOnlyFlag);
    if (valueStr != "") {
        result = std::stof(valueStr);
    }
    return result;
}

uint64_t MockAccessibilityDatashareHelper::GetUnsignedLongValue(const std::string& key, const uint64_t& defaultValue,
    const bool readOnlyFlag)
{
    int64_t result = defaultValue;
    std::string valueStr = GetStringValue(key, std::to_string(result), readOnlyFlag);
    if (valueStr != "") {
        result = static_cast<int64_t>(std::stoull(valueStr.c_str(), nullptr, DECIMAL_NOTATION));
    }
    return result;
}
 

RetError MockAccessibilityDatashareHelper::PutStringValue(const std::string& key, const std::string& value, bool needNotify)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    RetError rtn = RET_OK;
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    do {
        if (dataShareHelper_ == nullptr) {
            rtn = RET_ERR_NULLPTR;
            break;
        }
        DataShare::DataShareValueObject keyObj(key);
        DataShare::DataShareValueObject valueObj(value);
        DataShare::DataShareValuesBucket bucket;
        bucket.Put(SETTING_COLUMN_KEYWORD, keyObj);
        bucket.Put(SETTING_COLUMN_VALUE, valueObj);
        DataShare::DataSharePredicates predicates;
        predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
        Uri uri(AssembleUri(key));
        if (dataShareHelper_->Update(uri, predicates, bucket) <= 0) {
            HILOG_DEBUG("no data exist, insert one row");
            auto ret = dataShareHelper_->Insert(uri, bucket);
            if (ret <= 0) {
                rtn = RET_ERR_FAILED;
            }
            HILOG_INFO("helper insert %{public}s ret(%{public}d).", key.c_str(), static_cast<int>(ret));
        }
    } while (0);
    IPCSkeleton::SetCallingIdentity(callingIdentity);
#endif
    return rtn;
}

RetError MockAccessibilityDatashareHelper::PutUnsignedLongValue(const std::string& key, uint64_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError MockAccessibilityDatashareHelper::PutIntValue(const std::string& key, int32_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError MockAccessibilityDatashareHelper::PutLongValue(const std::string& key, int64_t value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError MockAccessibilityDatashareHelper::PutBoolValue(const std::string& key, bool value, bool needNotify)
{
    std::string valueStr = value ? "1" : "0";
    return PutStringValue(key, valueStr, needNotify);
}

RetError MockAccessibilityDatashareHelper::PutFloatValue(const std::string& key, float value, bool needNotify)
{
    return PutStringValue(key, std::to_string(value), needNotify);
}

RetError MockAccessibilityDatashareHelper::Initialize(int32_t systemAbilityId)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        HILOG_ERROR("get sam return nullptr");
        return RET_ERR_NULLPTR;
    }
    auto remoteObj = systemAbilityManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        HILOG_ERROR("Get remoteObj return nullptr, systemAbilityId=%{public}d", systemAbilityId);
        return RET_ERR_NULLPTR;
    }
    remoteObj_ = remoteObj;
    switch (type_) {
        case DATASHARE_TYPE::GLOBAL:
            uriProxyStr_ = SETTING_GLOBAL_URI + "?Proxy=true";
            break;
        case DATASHARE_TYPE::SYSTEM:
            uriProxyStr_ = SETTING_SYSTEM_URI + std::to_string(accountId_) + "?Proxy=true";
            break;
        case DATASHARE_TYPE::SECURE:
            uriProxyStr_ = SETTING_SECURE_URI + std::to_string(accountId_) + "?Proxy=true";
            break;
        default:
            uriProxyStr_ = SETTING_GLOBAL_URI + "?Proxy=true";
            HILOG_WARN("undefined DATASHARE_TYPE, use global table");
            break;
    }
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    dataShareHelper_ = CreateDatashareHelper();
    if (dataShareHelper_ == nullptr) {
        HILOG_ERROR("create dataShareHelper_ failed");
        return RET_ERR_NULLPTR;
    }
#endif
    return RET_OK;
}

sptr<AccessibilitySettingObserver> MockAccessibilityDatashareHelper::CreateObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    sptr<AccessibilitySettingObserver> observer = new AccessibilitySettingObserver();
    observer->SetKey(key);
    observer->SetUpdateFunc(func);
    return observer;
}

RetError MockAccessibilityDatashareHelper::RegisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    if (dataShareHelper_ == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return RET_ERR_NULLPTR;
    }
    dataShareHelper_->RegisterObserver(uri, observer);
#endif
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to register observer of uri=%{public}s", uri.ToString().c_str());
    return RET_OK;
}

RetError MockAccessibilityDatashareHelper::RegisterObserver(const std::string& key,
    AccessibilitySettingObserver::UpdateFunc& func)
{
    sptr<AccessibilitySettingObserver> observer = CreateObserver(key, func);
    if (observer == nullptr) {
        return RET_ERR_NULLPTR;
    }
    auto iter = settingObserverMap_.find(key);
    if (iter != settingObserverMap_.end() && iter->second != nullptr) {
        HILOG_INFO("observer of key = %{public}s already exist", key.c_str());
        return RET_OK;
    }
    if (RegisterObserver(observer) != ERR_OK) {
        return RET_ERR_NULLPTR;
    }
    std::lock_guard<ffrt::mutex> lock(observerMutex_);
    settingObserverMap_.insert(std::make_pair(key, observer));
    return RET_OK;
}

RetError MockAccessibilityDatashareHelper::UnregisterObserver(const sptr<AccessibilitySettingObserver>& observer)
{
    std::string callingIdentity = IPCSkeleton::ResetCallingIdentity();
    auto uri = AssembleUri(observer->GetKey());
#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
    if (dataShareHelper_ == nullptr) {
        IPCSkeleton::SetCallingIdentity(callingIdentity);
        return RET_ERR_NULLPTR;
    }
    dataShareHelper_->UnregisterObserver(uri, observer);
#endif
    IPCSkeleton::SetCallingIdentity(callingIdentity);
    HILOG_DEBUG("succeed to unregister observer of uri=%{public}s", uri.ToString().c_str());
    return RET_OK;
}

RetError MockAccessibilityDatashareHelper::UnregisterObserver(const std::string& key)
{
    std::lock_guard<ffrt::mutex> lock(observerMutex_);
    auto iter = settingObserverMap_.find(key);
    if (iter != settingObserverMap_.end() && iter->second != nullptr) {
        sptr<AccessibilitySettingObserver> observer = iter->second;
        if (UnregisterObserver(observer) == ERR_OK) {
            settingObserverMap_.erase(iter);
            HILOG_DEBUG("succeed to unregister observer of key %{public}s", key.c_str());
            return RET_OK;
        } else {
            settingObserverMap_.erase(iter);
            HILOG_WARN("failed to unregister observer of key %{public}s", key.c_str());
            return RET_ERR_FAILED;
        }
    }
    HILOG_WARN("failed to find the key %{public}s", key.c_str());
    return RET_ERR_FAILED;
}

#ifdef OHOS_BUILD_ENABLE_DATA_SHARE
std::shared_ptr<DataShare::DataShareHelper> MockAccessibilityDatashareHelper::CreateDatashareHelper()
{
    if (remoteObj_ == nullptr) {
        return nullptr;
    }
    std::pair<int, std::shared_ptr<DataShare::DataShareHelper>> ret = DataShare::DataShareHelper::Create(remoteObj_,
        uriProxyStr_, SETTINGS_DATA_EXT_URI);
    HILOG_INFO("create helper ret = %{public}d, uri=%{public}s", ret.first, uriProxyStr_.c_str());
    if (ret.first != DataShare::E_OK || ret.second == nullptr) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::READ_EVENT, A11yError::ERROR_READ_FAILED);
        return nullptr;
    }
    return ret.second;
}

bool MockAccessibilityDatashareHelper::DestoryDatashareHelper(std::shared_ptr<DataShare::DataShareHelper>& helper)
{
    if (helper && !helper->Release()) {
        HILOG_WARN("release helper fail.");
        return false;
    }
    return true;
}
#endif

int MockAccessibilityDumper::Dump(int fd, const std::vector<std::u16string>& args) const
{
    HILOG_DEBUG("Dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return -1;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    std::vector<std::string> params;
    std::transform(args.begin(), args.end(), std::back_inserter(params),
        [](const std::u16string &arg) { return Str16ToStr8(arg); });

    std::string dumpInfo;
    if (params.empty()) {
        ShowIllegalArgsInfo(dumpInfo);
    } else if (params[0] == ARG_DUMP_HELP) {
        ShowHelpInfo(dumpInfo);
    } else {
        DumpAccessibilityInfo(params, dumpInfo);
    }
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        HILOG_ERROR("dprintf error");
        return -1;
    }
    HILOG_INFO("Dump end");
    return 0;
}


int MockAccessibilityDumper::DumpAccessibilityWindowInfo(std::string& dumpInfo) const
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    std::ostringstream oss;
    size_t index = 0;
    std::map<int32_t, sptr<AccessibilityWindowConnection>> connectedWindowList = currentAccount->GetAsacConnections();
    oss << "connected window id: ";
    for (const auto &iter : connectedWindowList) {
        index++;
        oss << iter.first;

        if (index != connectedWindowList.size()) {
            oss << ", ";
        }
    }

    index = 0;
    const std::map<int32_t, AccessibilityWindowInfo> &windows = Singleton<AccessibilityWindowManager>::
        GetInstance().a11yWindows_;
    oss << std::endl << "a11yWindows_ id: ";
    for (const auto &iter : windows) {
        index++;
        oss << iter.first;

        if (index != connectedWindowList.size()) {
            oss << ", ";
        }
    }

    oss << std::endl << "active window id: " <<
        Singleton<AccessibilityWindowManager>::GetInstance().GetActiveWindowId() << std::endl;
    oss << "accessibility focused window id: " <<
        Singleton<AccessibilityWindowManager>::GetInstance().a11yFocusedWindowId_ << std::endl;

    dumpInfo.append(oss.str());
    return 0;
}

void ConvertCapabilities(const uint32_t value, std::string &capabilities)
{
    capabilities = "";
    if (value & Capability::CAPABILITY_RETRIEVE) {
        capabilities += "retrieve/";
    }
    if (value & Capability::CAPABILITY_TOUCH_GUIDE) {
        capabilities += "touchGuide/";
    }
    if (value & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
        capabilities += "keyEventObserver/";
    }
    if (value & Capability::CAPABILITY_ZOOM) {
        capabilities += "zoom/";
    }
    if (value & Capability::CAPABILITY_GESTURE) {
        capabilities += "gesture/";
    }
}

void ConvertAbilityTypes(const uint32_t value, std::string &abilityTypes)
{
    abilityTypes = "";
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN) {
        abilityTypes += "spoken/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC) {
        abilityTypes += "haptic/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE) {
        abilityTypes += "audible/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL) {
        abilityTypes += "visual/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC) {
        abilityTypes += "generic/";
    }
}

void ConvertEventTypes(const uint32_t value, std::string &eventTypes)
{
    eventTypes = "";
    std::map<EventType, std::string> accessibilityEventTable = {{EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_PAGE_STATE_UPDATE, "pageStateUpdate"},
        {EventType::TYPE_NOTIFICATION_UPDATE_EVENT, "notificationUpdate"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN, "touchGuideGestureBegin"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_END, "touchGuideGestureEnd"},
        {EventType::TYPE_PAGE_CONTENT_UPDATE, "pageContentUpdate"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_SCROLLED_START, "scrollStart"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_PUBLIC_NOTICE_EVENT, "publicNotice"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT, "textMoveUnit"},
        {EventType::TYPE_TOUCH_GUIDE_BEGIN, "touchGuideBegin"},
        {EventType::TYPE_TOUCH_GUIDE_END, "touchGuideEnd"},
        {EventType::TYPE_TOUCH_BEGIN, "touchBegin"},
        {EventType::TYPE_TOUCH_END, "touchEnd"},
        {EventType::TYPE_WINDOW_UPDATE, "windowUpdate"},
        {EventType::TYPE_INTERRUPT_EVENT, "interrupt"},
        {EventType::TYPE_GESTURE_EVENT, "gesture"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY, "requestFocusForAccessibility"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY, "announceForAccessibility"},
        {EventType::TYPE_PAGE_OPEN, "pageOpen"},
        {EventType::TYPE_PAGE_CLOSE, "pageClose"},
        {EventType::TYPE_ELEMENT_INFO_CHANGE, "elementInfoChange"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT, "announceForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT,
            "requestFocusForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_SCROLLING_EVENT, "scrolling"}};

    for (auto itr = accessibilityEventTable.begin(); itr != accessibilityEventTable.end(); ++itr) {
        if (value & itr->first) {
            eventTypes += itr->second;
            eventTypes += "/";
        }
    }
}

int MockAccessibilityDumper::DumpAccessibilityClientInfo(std::string& dumpInfo) const
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    auto connectedAbilities = currentAccount->GetConnectedA11yAbilities();
    std::ostringstream oss;
    oss << "client num:  " << connectedAbilities.size() << std::endl;

    // Dump client info details
    size_t index = 0;
    for (const auto &iter : connectedAbilities) {
        oss << std::endl << "client[" << index++ << "] info details: " << std::endl;
        if (!iter.second) {
            HILOG_ERROR("The connected ability[%{public}s] is null", iter.first.c_str());
            continue;
        }
        AccessibilityAbilityInfo accessibilityAbilityInfo = iter.second->GetAbilityInfo();
        oss << "    bundleName: " << accessibilityAbilityInfo.GetPackageName() << std::endl;
        oss << "    moduleName: " << accessibilityAbilityInfo.GetModuleName() << std::endl;
        oss << "    abilityName: " << accessibilityAbilityInfo.GetName() << std::endl;
        oss << "    description: " << accessibilityAbilityInfo.GetDescription() << std::endl;

        std::string capabilities;
        ConvertCapabilities(accessibilityAbilityInfo.GetCapabilityValues(), capabilities);
        oss << "    capabilities: " << capabilities << std::endl;

        std::string abilityTypes;
        ConvertAbilityTypes(accessibilityAbilityInfo.GetAccessibilityAbilityType(), abilityTypes);
        oss << "    abilityTypes: " << abilityTypes << std::endl;

        std::string eventTypes;
        ConvertEventTypes(accessibilityAbilityInfo.GetEventTypes(), eventTypes);
        oss << "    eventTypes: " << eventTypes << std::endl;

        std::vector<std::string> targetBundleNames = accessibilityAbilityInfo.GetFilterBundleNames();
        if (targetBundleNames.empty()) {
            oss << "    targetBundleNames: " << "all" << std::endl;
        } else {
            oss << "    targetBundleNames: " << std::endl;
            for (const auto &targetBundleName : targetBundleNames) {
                oss << "        " << targetBundleName << std::endl;
            }
        }

        if (index != connectedAbilities.size()) {
            oss << std::endl << "    -------------------------------" << std::endl << std::endl;
        }
    }
    dumpInfo.append(oss.str());
    return 0;
}

int MockAccessibilityDumper::DumpAccessibilityUserInfo(std::string& dumpInfo) const
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    std::ostringstream oss;
    oss << "id:  " << currentAccount->GetAccountId() << std::endl;

    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    if (!config) {
        HILOG_ERROR("config is null");
        return -1;
    }

    // Dump Capabilities
    oss << "accessible:  " << config->GetEnabledState() << std::endl;
    oss << "touchGuide:  " << config->GetTouchGuideState() << std::endl;
    oss << "gesture:  " << config->GetGestureState() << std::endl;
    oss << "keyEventObserver:  " << config->GetKeyEventObserverState() << std::endl;

    // Dump setting info
    oss << "screenMagnification:  " << config->GetScreenMagnificationState() << std::endl;
    oss << "mouseKey:  " << config->GetMouseKeyState() << std::endl;
    oss << "shortKey:  " << config->GetShortKeyState() << std::endl;
    oss << "shortKeyOnLockScreen:  " << config->GetShortKeyOnLockScreenState() << std::endl;
    oss << "animationOff:  " << config->GetAnimationOffState() << std::endl;
    oss << "invertColor:  " << config->GetInvertColorState() << std::endl;
    oss << "highContrastText:  " << config->GetHighContrastTextState() << std::endl;
    oss << "audioMono:  " << config->GetAudioMonoState() << std::endl;
    oss << "shortkeyTarget:  " << config->GetShortkeyTarget() << std::endl;
    // need to add
    oss << "mouseAutoClick:  " << config->GetMouseAutoClick() << std::endl;
    oss << "daltonizationState:  " << config->GetDaltonizationState() << std::endl;
    oss << "daltonizationColorFilter:  " << config->GetDaltonizationColorFilter() << std::endl;
    oss << "contentTimeout:  " << config->GetContentTimeout() << std::endl;
    oss << "brightnessDiscount:  " << config->GetBrightnessDiscount() << std::endl;
    oss << "audioBalance:  " << config->GetAudioBalance() << std::endl;
    oss << "clickResponseTime:  " << config->GetClickResponseTime() << std::endl;
    oss << "ignoreRepeatClickState:  " << config->GetIgnoreRepeatClickState() << std::endl;
    oss << "ignoreRepeatClickTime:  " << config->GetIgnoreRepeatClickTime() << std::endl;

    // Dump caption info
    oss << "captionState:  " << config->GetCaptionState() << std::endl;
    if (config->GetCaptionState()) {
        AccessibilityConfig::CaptionProperty captionProperty = config->GetCaptionProperty();
        oss << "    fontFamily:  " << captionProperty.GetFontFamily() << std::endl;
        oss << "    fontScale:  " << captionProperty.GetFontScale() << std::endl;
        oss << "    fontColor:  " << captionProperty.GetFontColor() << std::endl;
        oss << "    fontEdgeType:  " << captionProperty.GetFontEdgeType() << std::endl;
        oss << "    backgroundColor:  " << captionProperty.GetBackgroundColor() << std::endl;
        oss << "    windowColor:  " << captionProperty.GetWindowColor() << std::endl;
    }
    dumpInfo.append(oss.str());
    return 0;
}

int MockAccessibilityDumper::DumpAccessibilityInfo(const std::vector<std::string>& args, std::string& dumpInfo) const
{
    if (args.empty()) {
        return -1;
    }
    DumpType dumpType = DumpType::DUMP_NONE;
    if (args[0] == ARG_DUMP_USER) {
        dumpType = DumpType::DUMP_USER;
    } else if (args[0] == ARG_DUMP_CLIENT) {
        dumpType = DumpType::DUMP_CLIENT;
    } else if (args[0] == ARG_DUMP_ACCESSIBILITY_WINDOW) {
        dumpType = DumpType::DUMP_ACCESSIBILITY_WINDOW;
    }
    int ret = 0;
    switch (dumpType) {
        case DumpType::DUMP_USER:
            ret = DumpAccessibilityUserInfo(dumpInfo);
            break;
        case DumpType::DUMP_CLIENT:
            ret = DumpAccessibilityClientInfo(dumpInfo);
            break;
        case DumpType::DUMP_ACCESSIBILITY_WINDOW:
            ret = DumpAccessibilityWindowInfo(dumpInfo);
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}

void MockAccessibilityDumper::ShowIllegalArgsInfo(std::string& dumpInfo) const
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

void MockAccessibilityDumper::ShowHelpInfo(std::string& dumpInfo) const
{
    dumpInfo.append("Usage:\n")
        .append(" -h                    ")
        .append("|help text for the tool\n")
        .append(" -u                    ")
        .append("|dump accessibility current user in the system\n")
        .append(" -c                    ")
        .append("|dump accessibility client in the system\n")
        .append(" -w                    ")
        .append("|dump accessibility window info in the system\n");
}

bool MockEventTransmission::OnKeyEvent(MMI::KeyEvent &event)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        return next->OnKeyEvent(event);
    }
    return false;
}

bool MockEventTransmission::OnPointerEvent(MMI::PointerEvent &event)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        return next->OnPointerEvent(event);
    }
    return false;
}

void MockEventTransmission::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        next->OnMoveMouse(offsetX, offsetY);
    }
}

void MockEventTransmission::SetNext(const sptr<MockEventTransmission> &next)
{
    HILOG_DEBUG();

    next_ = next;
}

sptr<MockEventTransmission> MockEventTransmission::GetNext()
{
    HILOG_DEBUG();

    return next_;
}

void MockEventTransmission::DestroyEvents()
{
    HILOG_DEBUG();

    auto next = GetNext();
    if (next != nullptr) {
        next->DestroyEvents();
    }
}
} // namespace Accessibility
} // namespace OHOS