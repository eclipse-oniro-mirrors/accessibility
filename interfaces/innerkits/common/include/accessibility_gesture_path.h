/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_GESTURE_PATH_H
#define ACCESSIBILITY_GESTURE_PATH_H

#include <cstdint>

namespace OHOS {
namespace Accessibility {
struct AccessibilityGesturePathPosition {
    float positionX_ = .0f;
    float positionY_ = .0f;
};

class AccessibilityGesturePath {
public:
    const static int64_t MAX_GESTURE_DURATION = 60 * 1000;
    const static uint32_t MAX_GESTURE_PATHS = 10;

    AccessibilityGesturePath() {}
    ~AccessibilityGesturePath() {}

    AccessibilityGesturePath(AccessibilityGesturePathPosition &startPosition,
        AccessibilityGesturePathPosition &endPosition, int64_t durationTime);

    /**
     * @brief Obtains the duration in which this gesture path continues.
     * @param
     * @return Return the duration in which this gesture path continues.
     */
    int64_t GetDurationTime() const;

    /**
     * @brief Obtains the end position of this gesture path.
     * @param
     * @return Return the end position of this gesture path.
     */
    AccessibilityGesturePathPosition &GetEndPosition();

    /**
     * @brief Obtains the start position of this gesture path.
     * @param
     * @return Return the start position of this gesture path.
     */
    AccessibilityGesturePathPosition &GetStartPosition();

    /**
     * @brief Sets the duration for this gesture path to continue.
     * @param durationTime The duration for this gesture path to continue.
     * @return
     */
    void SetDurationTime(int64_t durationTime);

    /**
     * @brief Sets the end position of this gesture path.
     * @param endPosition The end position of this gesture path.
     * @return
     */
    void SetEndPosition(AccessibilityGesturePathPosition &endPosition);

    /**
     * @brief Sets the start position of this gesture path.
     * @param startPosition The start position of this gesture path.
     * @return
     */
    void SetStartPosition(AccessibilityGesturePathPosition &startPosition);

protected:
    AccessibilityGesturePathPosition startPosition_;
    AccessibilityGesturePathPosition endPosition_;
    int64_t durationTime_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_GESTURE_PATH_H