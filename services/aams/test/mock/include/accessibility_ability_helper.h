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

#ifndef ACCESSIBILITY_ABILILTY_HELPER_H
#define ACCESSIBILITY_ABILILTY_HELPER_H

#include <chrono>
#include <thread>
#include "accessibility_event_info.h"
#include "hilog/log.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
static const int32_t SLEEP_TIME = 10;

class AccessibilityAbilityHelper {
public:
    static AccessibilityAbilityHelper& GetInstance()
    {
        static AccessibilityAbilityHelper helper;
        return helper;
    }

    std::vector<int> GetTouchEventActionVector()
    {
        return touchAction_;
    }

    void ClearTouchEventActionVector()
    {
        touchAction_.clear();
    }

    void SetTouchEventActionVectors(int touchAction)
    {
        touchAction_.push_back(touchAction);
    }

    bool GetDestroyState()
    {
        return isDestroyEvents_;
    }

    void SetDestroyState()
    {
        isDestroyEvents_ = true;
    }

    bool GetClearState()
    {
        return isClearEvents_;
    }

    void SetClearState()
    {
        isClearEvents_ = true;
    }

    int GetTestElementId()
    {
        return testElementId_;
    }

    void SetTestElementId(int ElementId)
    {
        testElementId_ = ElementId;
    }

    int GetTestChannelElementId()
    {
        return testChannelElementId_;
    }

    void SetTestChannelElementId(int ElementId)
    {
        testChannelElementId_ = ElementId;
    }

    int GetTestChannelRequestId()
    {
        return testChannelrequestId_;
    }

    void SetTestChannelRequestId(int ElementId)
    {
        testChannelrequestId_ = ElementId;
    }

    int GetTestChannelMode()
    {
        return testChannelmode_;
    }

    void SetTestChannelMode(int ElementId)
    {
        testChannelmode_ = ElementId;
    }

    int GetTestClearFocus()
    {
        return testClearFocus_;
    }

    void SetTestClearFocus(int ElementId)
    {
        testClearFocus_ = ElementId;
    }

    int GetTestOutsideTouch()
    {
        return testOutsideTouch_;
    }

    void SetTestOutsideTouch(int ElementId)
    {
        testOutsideTouch_ = ElementId;
    }

    int GetGestureId()
    {
        return uTgestureId_;
    }

    void SetGestureId(int gestureId)
    {
        uTgestureId_ = gestureId;
    }

    std::vector<EventType> GetEventTypeVector()
    {
        return eventType_;
    }

    void ClearEventTypeActionVector()
    {
        eventType_.clear();
    }

    void SetEventTypeVector(EventType eventType)
    {
        eventType_.push_back(eventType);
    }

    int GetTestChannelId()
    {
        return testChannelId_;
    }

    void SetTestChannelId(int ChannalId)
    {
        testChannelId_ = ChannalId;
    }

    int GetTestEventType()
    {
        return testEventType_;
    }

    void SetTestEventType(int EventType)
    {
        testEventType_ = EventType;
    }

    int GetTestGesture()
    {
        return testGesture_;
    }

    void SetTestGesture(int Gesture)
    {
        testGesture_ = Gesture;
    }

    int GetTestKeyPressEvent()
    {
        return testKeyPressEvent_;
    }

    void SetTestKeyPressEvent(int KeyPressEvent)
    {
        testKeyPressEvent_ = KeyPressEvent;
    }

    int GetTestDisplayId()
    {
        return testDisplayId_;
    }

    void SetTestDisplayId(int DisplayId)
    {
        testDisplayId_ = DisplayId;
    }

    int GetTestGestureSimulateResult()
    {
        return testGestureSimulateResult_;
    }

    void SetTestGestureSimulateResult(int GestureSimulateResult)
    {
        testGestureSimulateResult_ = GestureSimulateResult;
    }

    int GetTestStateType()
    {
        return testStateType_;
    }

    void SetTestStateType(int StateType)
    {
        testStateType_ = StateType;
    }

    bool GetTestKeyEvent()
    {
        return testKeyEvent_;
    }

    void SetTestKeyEvent(bool KeyEvent)
    {
        testKeyEvent_ = true;
    }

    bool GetIsServicePublished()
    {
        return isServicePublished_;
    }

    void SetIsServicePublished(bool publish)
    {
        isServicePublished_ = publish;
    }

    void WaitForServicePublish()
    {
        while (1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
            if (isServicePublished_) {
                return;
            }
        }
    }

private:
    std::vector<int> touchAction_;
    bool isDestroyEvents_ = false;
    bool isClearEvents_ = false;
    int testElementId_ = -1;
    int testClearFocus_ = -1;
    int testOutsideTouch_ = -1;
    int testChannelElementId_ = -1;
    int testChannelrequestId_ = -1;
    int testChannelmode_ = -1;
    int uTgestureId_;
    std::vector<EventType> eventType_;
    int testChannelId_ = -1;
    int testEventType_ = -1;
    int testGesture_ = -1;
    int testKeyPressEvent_ = -1;
    int testDisplayId_ = -1;
    int testGestureSimulateResult_ = -1;
    int testStateType_ = -1;
    bool testKeyEvent_ = false;
    bool isServicePublished_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILILTY_HELPER_H