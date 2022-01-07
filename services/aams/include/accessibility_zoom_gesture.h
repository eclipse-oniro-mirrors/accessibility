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

#ifndef ACCESSIBILITY_ZOOM_GESTURE_H
#define ACCESSIBILITY_ZOOM_GESTURE_H

// #include "accessibility_gesture_recognizer.h"
#include "touch_event.h"
// #include "accessibility_zoom_mock.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityZoomGesture {
public:
    AccessibilityZoomGesture();
    ~AccessibilityZoomGesture();

    /**
     * @brief Is it a triple tip.
     */
    bool Triple(TouchEvent &event);
    void Up();
    void Clear();

private:
    int distance_ = 0;
    int timeout_ = 0;
    int upCount_ = 0;
    int downCount_ = 0;
    TouchEvent lastDown_ {};

    void Initialize();
    void Reset(const TouchEvent &event);

    /**
     * @brief Is it a valid continuous DOWN event.
     */
    bool ValidDown(TouchEvent &event);


    /**
     * @brief Gets the effective distance between two touch events
     *        set by the system.
     */
    int GetSysDistance() const;

    /**
     * @brief Gets the effective time interval between two touch events
     *        set by the system.
     */
    int GetSysTimeout() const;
};
}  // namespace accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_ZOOM_GESTURE_H
