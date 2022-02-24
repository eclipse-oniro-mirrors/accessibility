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

#include "hilog_wrapper.h"
#include "pointer_event.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityZoomGesture {
public:
    AccessibilityZoomGesture();
    ~AccessibilityZoomGesture();

    /**
     * @brief Is it a triple tip.
     */
    bool Triple(MMI::PointerEvent &event);
    void Up();
    void Clear();

private:
    int distance_ = 0;
    unsigned long timeout_ = 0;
    int upCount_ = 0;
    int downCount_ = 0;
    std::shared_ptr<MMI::PointerEvent> pLastDown_ = nullptr;

    void Initialize();
    void Reset(const MMI::PointerEvent &event);

    /**
     * @brief Is it a valid continuous DOWN event.
     */
    bool ValidDown(MMI::PointerEvent &event);

    /**
     * @brief Gets the effective distance between two pointer events
     *        set by the system.
     */
    int GetSysDistance() const;

    /**
     * @brief Gets the effective time interval between two pointer events
     *        set by the system.
     */
    int GetSysTimeout() const;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_ZOOM_GESTURE_H
