/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_gesture_recognizer.h"

namespace OHOS{
namespace Accessibility{

GestureHandler::GestureHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, AccessibilityGestureRecognizer &server)
    : AppExecFwk::EventHandler(runner),server_(server) {

}

void GestureHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) {
    switch (event->GetInnerEventId()) {
        case AccessibilityGestureRecognizer::LONG_PRESS_MSG:
            RemoveEvent(AccessibilityGestureRecognizer::SINGLE_TAP_MSG);
            server_.SetIsLongpress(true);
            server_.MaybeRecognizeLongPress(*server_.GetCurDown());
            break;
        case AccessibilityGestureRecognizer::SINGLE_TAP_MSG:
            if (!server_.GetContinueDown()) {
                server_.SingleTapDetected();
            }
            break;
        default:
            break;
    }
}

AccessibilityGestureRecognizer::AccessibilityGestureRecognizer() {
    AccessibilityDisplayManager &displayMgr = AccessibilityDisplayManager::GetInstance();
    struct WMDisplayInfo displayInfo = displayMgr.GetDefaultDisplay();

    threshold_ = CALCULATION_DIMENSION(displayInfo.width);
    xMinPixels_= MIN_PIXELS(displayInfo.width);
    yMinPixels_ = MIN_PIXELS(displayInfo.height);

    float densityPixels = displayInfo.dpi;
    int slop = (int) (densityPixels * DOUBLE_TAP_SLOP + 0.5f);
    doubleTapScaledSlop_ = slop * slop;

    runner_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->GetMainRunner();
    if (!runner_) {
        HILOG_ERROR("get runner failed");
        return ;
    }
    handler_ = std::make_shared<GestureHandler>(runner_, *this);
    if (!handler_) {
        HILOG_ERROR("create event handler failed");
        return ;
    }
}

void AccessibilityGestureRecognizer::registerListener(AccessibilityGestureRecognizeListener *listener) {
    listener_ = listener;
}

void AccessibilityGestureRecognizer::UnregisterListener(AccessibilityGestureRecognizeListener *listener) {
    listener_ = nullptr;
}

bool AccessibilityGestureRecognizer::OnTouchEvent(TouchEvent &event) {
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
            HandleTouchDownEvent(event);
            break;
        case TouchEnum::POINT_MOVE:
            return HandleTouchMoveEvent(event);
        case TouchEnum::PRIMARY_POINT_UP:
            return HandleTouchUpEvent(event);
        case TouchEnum::OTHER_POINT_DOWN:
            isRecognizingGesture_ = false;
            isGestureStarted_ = false;
            pointerRoute_.clear();
            break;
        case TouchEnum::CANCEL:
            Clear();
            break;
        default:
            break;
    }
    if (!isRecognizingGesture_) {
        return false;
    }
    return StandardGestureRecognizer(event);
}

void AccessibilityGestureRecognizer::Clear() {
    isFirstTapUp_ = false;
    isDoubleTap_ = false;
    isGestureStarted_ = false;
    isRecognizingGesture_ = false;
    pointerRoute_.clear();
    continueDown_ = false;
    StandardGestureCancled();
}

void AccessibilityGestureRecognizer::HandleTouchDownEvent(TouchEvent &event) {
    Pointer mp;
    MmiPoint point = event.GetPointerPosition(event.GetIndex());
    unsigned int eventTime = event.GetOccurredTime();
    mp.px_ = point.GetX();
    mp.py_ = point.GetY();
    isDoubleTap_ = false;
    isRecognizingGesture_ = true;
    isGestureStarted_ = false;
    pointerRoute_.clear();
    pointerRoute_.push_back(mp);
    prePointer_ = point;
    startPointer_ = point;
    startTime_ = eventTime;
}

bool AccessibilityGestureRecognizer::HandleTouchMoveEvent(TouchEvent &event) {
    Pointer mp;
    MmiPoint point = event.GetPointerPosition(event.GetIndex());
    unsigned int eventTime = event.GetOccurredTime();
    float offsetX = startPointer_.GetX() - point.GetX();
    float offsetY = startPointer_.GetY() - point.GetY();
    double duration = hypot(offsetX, offsetY);
    if (isRecognizingGesture_) {
        if (duration > threshold_) {
            startPointer_ = point;
            startTime_ = eventTime;
            isFirstTapUp_ = false;
            isDoubleTap_ = false;
            if (!isGestureStarted_) {
                isGestureStarted_ = true;
                return listener_->OnStarted();
            }
        } else if (!isFirstTapUp_) {
            unsigned int durationTime = eventTime - startTime_;
            unsigned int thresholdTime = isGestureStarted_ ? 
                GESTURE_STARTED_TIME_THRESHOLD : GESTURE_NOT_STARTED_TIME_THRESHOLD;
            if (durationTime > thresholdTime) {
                isRecognizingGesture_ = false;
                isGestureStarted_ = false;
                pointerRoute_.clear();
                return listener_->OnCancelled(event);
            }
        }
        if ((abs(point.GetX() - prePointer_.GetX())) >= xMinPixels_ ||
                (abs(point.GetY() - prePointer_.GetY())) >= yMinPixels_) {
            prePointer_ = point;
            mp.px_ = point.GetX();
            mp.py_ = point.GetY();
            pointerRoute_.push_back(mp);
        }
    }
    if (!isRecognizingGesture_) {
        return false;
    }
    return StandardGestureRecognizer(event);
}

bool AccessibilityGestureRecognizer::HandleTouchUpEvent(TouchEvent &event) {
    Pointer mp;
    MmiPoint point = event.GetPointerPosition(event.GetIndex());

    if (isDoubleTap_) {
        return DoubleTapRecognized(event);
    }
    if (isGestureStarted_) {
        if ((abs(point.GetX() - prePointer_.GetX())) >= xMinPixels_ ||
            (abs(point.GetY() - prePointer_.GetY())) >= yMinPixels_) {
            mp.px_ = point.GetX();
            mp.py_ = point.GetY();
            pointerRoute_.push_back(mp);
        }
        return recognizeDirectionGesture(event);
    }
    if (!isRecognizingGesture_) {
        return false;
    }
    return StandardGestureRecognizer(event);
}

bool AccessibilityGestureRecognizer::StandardGestureRecognizer(TouchEvent &event) {
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_DOWN:
            if (pCurDown_ && pPreUp_ && isDoubleTap(event)) {
                isDoubleTapdetecting_ = true;
                isDoubleTap_ = true;
            } else {
                handler_->SendEvent(SINGLE_TAP_MSG, 0, DOUBLE_TAP_TIMEOUT);
            }
            pCurDown_ = std::make_shared<TouchEvent>(event);
            isTapDown_ = true;
            continueDown_ = true;
            isLongpress_ = false;
            handler_->RemoveEvent(LONG_PRESS_MSG);
            handler_->SendEvent(LONG_PRESS_MSG, 0, LONG_PRESS_TIMEOUT);
            break;
        case TouchEnum::PRIMARY_POINT_UP:
            continueDown_ = false;
            if (isLongpress_) {
                handler_->RemoveEvent(SINGLE_TAP_MSG);
                isLongpress_ = false;
            } else if (!isDoubleTapdetecting_ && isTapDown_) {
                isFirstTapUp_ = true;
            }
            pPreUp_ = std::make_unique<TouchEvent>(event);
            isDoubleTapdetecting_ = false;
            handler_->RemoveEvent(LONG_PRESS_MSG);
            break;
        case TouchEnum::OTHER_POINT_DOWN:
            StandardGestureCancled();
            break;
        default:
            break;
    }
    return false;
}

void AccessibilityGestureRecognizer::StandardGestureCancled() {
    handler_->RemoveEvent(LONG_PRESS_MSG);
    handler_->RemoveEvent(SINGLE_TAP_MSG);
    isLongpress_ = false;
    isDoubleTapdetecting_ = false;
    isTapDown_ = false;
}

void AccessibilityGestureRecognizer::SingleTapDetected() {
    Clear();
}

void AccessibilityGestureRecognizer::MaybeRecognizeLongPress(TouchEvent &event) {
    if (!isDoubleTap_) {
        return;
    }
    Clear();
    listener_->OnDoubleTapLongPress(event);
}

bool AccessibilityGestureRecognizer::DoubleTapRecognized(TouchEvent &event) {
    Clear();
    return listener_->OnDoubleTap(event);
}

bool AccessibilityGestureRecognizer::recognizeDirectionGesture(TouchEvent &event) {
    if (pointerRoute_.size() < 2) {
        return listener_->OnCancelled(event);
    }

    /* Check the angle of the most recent motion vector versus the preceding motion vector,
     * segment the line if the angle is about 90 degrees.
     */
    std::vector<Pointer> pointerPath = GetPointerPath(pointerRoute_);

    if (pointerPath.size() == 2) {
        int swipeDirection = GetSwipeDirection(pointerPath[0], pointerPath[1]);
        return listener_->OnCompleted(GESTURE_DIRECTION[swipeDirection]);
    } else if (pointerPath.size() == 3) {
        int swipeDirectionH = GetSwipeDirection(pointerPath[0], pointerPath[1]);
        int swipeDirectionHV = GetSwipeDirection(pointerPath[1], pointerPath[2]);
        return listener_->OnCompleted(GESTURE_DIRECTION_TO_ID[swipeDirectionH][swipeDirectionHV]);
    }
    return listener_->OnCancelled(event);
}

int AccessibilityGestureRecognizer::GetSwipeDirection(Pointer firstP, Pointer secondP) {
    float offsetX = secondP.px_ - firstP.px_;
    float offsetY = secondP.py_ - firstP.py_;
    if (abs(offsetX) > abs(offsetY)) {
        return offsetX > 0.0 ? SWIPE_RIGHT : SWIPE_LEFT;
    } else {
        return offsetY > 0.0 ? SWIPE_UP : SWIPE_DOWN;
    }
}

std::vector<Pointer> AccessibilityGestureRecognizer::GetPointerPath(std::vector<Pointer> &route) {
    std::vector<Pointer> pointerPath;
    Pointer firstSeparation = route[0];
    Pointer nextPoint;
    Pointer newSeparation;
    float xUnitVector = 0;
    float yUnitVector = 0;
    float xVector = 0;
    float yVector = 0;
    float vectorLength = 0;
    int numSinceFirstSep = 0;

    pointerPath.push_back(firstSeparation);
    for (unsigned int i = 1; i < route.size(); i++) {
        nextPoint = route[i];
        if (numSinceFirstSep > 0) {
            xVector = xUnitVector / numSinceFirstSep;
            yVector = yUnitVector / numSinceFirstSep;
            newSeparation.px_ = vectorLength * xVector + firstSeparation.px_;
            newSeparation.py_ = vectorLength * yVector + firstSeparation.py_;

            float xNextUnitVector = nextPoint.px_ - newSeparation.px_;
            float yNextUnitVector = nextPoint.py_ - newSeparation.py_;
            float nextVectorLength = hypot(xNextUnitVector, yNextUnitVector);
            xNextUnitVector /= nextVectorLength;
            yNextUnitVector /= nextVectorLength;

            if ((xVector * xNextUnitVector + yVector * yNextUnitVector) < DEGREES_THRESHOLD) {
                pointerPath.push_back(newSeparation);
                firstSeparation = newSeparation;
                xUnitVector = 0;
                yUnitVector = 0;
                numSinceFirstSep = 0;
            }
        }
        xVector = nextPoint.px_ - firstSeparation.px_;
        yVector = nextPoint.py_ - firstSeparation.py_;
        vectorLength = hypot(xVector, yVector);
        numSinceFirstSep += 1;
        xUnitVector += xVector / vectorLength;
        yUnitVector += yVector / vectorLength;
    }
    pointerPath.push_back(nextPoint);
    return pointerPath;
}

bool AccessibilityGestureRecognizer::isDoubleTap(TouchEvent &event) {
    int durationTime = event.GetOccurredTime() - pPreUp_->GetOccurredTime();
    if (!(durationTime <= DOUBLE_TAP_TIMEOUT && durationTime >= MIN_DOUBLE_TAP_TIME)) {
        return false;
    }

    MmiPoint firstMp = pCurDown_->GetPointerPosition(event.GetIndex());
    MmiPoint secondMp = event.GetPointerPosition(event.GetIndex());
    int durationX = (int)firstMp.GetX() - (int)secondMp.GetX();
    int durationY = (int)firstMp.GetY() - (int)secondMp.GetY();

    return (durationX * durationX + durationY * durationY < doubleTapScaledSlop_);
}

}  // namespace Accessibility
}  // namespace OHOS
