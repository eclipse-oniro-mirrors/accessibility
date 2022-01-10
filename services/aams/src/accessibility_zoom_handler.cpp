/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessibility_zoom_handler.h"
#include <iostream>
#include <vector>
#include "hilog_wrapper.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

AccessibilityZoomHandler::AccessibilityZoomHandler(int displayId)
{
    displayId_ = displayId;
    // zoomProxy_ = proxy;

    readyState_.Register(*this);
    zoomInState_.Register(*this);
    slidingState_.Register(*this);
    currentState_ = readyState_;
}

AccessibilityZoomHandler::~AccessibilityZoomHandler()
{
    readyState_.UnRegisterAll();
    zoomInState_.UnRegisterAll();
    slidingState_.UnRegisterAll();
    EventTransmission::DestroyEvents();
}

void AccessibilityZoomHandler::Initialize()
{
    // currentState_ = readyState_;
}

/*
AccessibilityZoomProxy &AccessibilityZoomHandler::GetProxy()
{
    return zoomProxy_;
}
*/

void AccessibilityZoomHandler::OnTouchEvent(TouchEvent &event)
{
    currentState_.OnTouchEvent(event);
}

void AccessibilityZoomHandler::OnTransitionTo(const int state)
{
    currentState_.Exit();
    {
        // std::lock_guard<std::recursive_mutex> lock(stateMutex_);
        switch (state) {
            case READY_STATE:
                currentState_ = readyState_;
                break;
            case ZOOMIN_STATE:
                currentState_ = zoomInState_;
                break;
            case SLIDING_STATE:
                currentState_ = slidingState_;
                break;
            default:
                std::cout << "State input error" << std::endl;
                break;
        }
    }
    currentState_.Enter();
}

void AccessibilityZoomHandler::OnZoomIn()
{

}

void AccessibilityZoomHandler::OnZoomOut()
{

}

void AccessibilityZoomHandler::OnBack(TouchEvent &event)
{
    EventTransmission::OnTouchEvent(event);
}

void AccessibilityZoomHandler::ReadyState::Enter()
{
    HILOG_DEBUG("AccessibilityZoomHandler READY_STATE Enter.");
    gesture_.Clear();
}

void AccessibilityZoomHandler::ReadyState::Exit()
{
    HILOG_DEBUG("AccessibilityZoomHandler READY_STATE Exit.");
    gesture_.Clear();
}

void AccessibilityZoomHandler::ReadyState::OnTouchEvent(TouchEvent &event)
{
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_UP:
            gesture_.Up();
            observer_[0].OnBack(event);
            break;
        case TouchEnum::PRIMARY_POINT_DOWN:
            if (gesture_.Triple(event)) {
                observer_[0].OnTransitionTo(ZOOMIN_STATE);
            } else {
                observer_[0].OnBack(event);
            }
            break;
        default:
            observer_[0].OnBack(event);
            break;
    }
}

void AccessibilityZoomHandler::ZoomInState::Enter()
{
    HILOG_DEBUG("AccessibilityZoomHandler ZOOMIN_STATE Enter.");
    gesture_.Clear();
    observer_[0].OnZoomIn();
}

void AccessibilityZoomHandler::ZoomInState::Exit()
{
    HILOG_DEBUG("AccessibilityZoomHandler ZOOMIN_STATE Exit.");
    gesture_.Clear();
    observer_[0].OnZoomOut();
}

void AccessibilityZoomHandler::ZoomInState::OnTouchEvent(TouchEvent &event)
{
    switch (event.GetAction()) {
        case TouchEnum::PRIMARY_POINT_UP:
            gesture_.Up();
            observer_[0].OnBack(event);
            break;
        case TouchEnum::PRIMARY_POINT_DOWN:
            if (gesture_.Triple(event)) {
                observer_[0].OnTransitionTo(READY_STATE);
            } else {
                observer_[0].OnBack(event);
            }
            break;
        case TouchEnum::OTHER_POINT_DOWN:
            observer_[0].OnTransitionTo(SLIDING_STATE);
            break;
        case TouchEnum::CANCEL:
            observer_[0].OnTransitionTo(READY_STATE);
            break;
        default:
            observer_[0].OnBack(event);
            break;
    }

}

void AccessibilityZoomHandler::SlidingState::Enter()
{
    HILOG_DEBUG("AccessibilityZoomHandler SLIDING_STATE Enter.");
}

void AccessibilityZoomHandler::SlidingState::Exit()
{
    HILOG_DEBUG("AccessibilityZoomHandler SLIDING_STATE Exit.");
}

void AccessibilityZoomHandler::SlidingState::OnTouchEvent(TouchEvent &event)
{
    switch (event.GetAction()) {
        case TouchEnum::OTHER_POINT_UP:
            observer_[0].OnTransitionTo(ZOOMIN_STATE);
            break;
        case TouchEnum::POINT_MOVE:
            OnScroll(/*event*/);
            break;
        case TouchEnum::CANCEL:
            observer_[0].OnTransitionTo(READY_STATE);
            observer_[0].OnZoomOut();
            break;
        default:
            observer_[0].OnBack(event);
            break;
    }

}

bool AccessibilityZoomHandler::SlidingState::OnScroll( /*TouchEvent &event*/ )
{
    return true;
}

bool AccessibilityZoomHandler::SlidingState::OnScale( /*TouchEvent &event*/ )
{
    return true;
}

}  // namespace accessibility
}  // namespace OHOS
