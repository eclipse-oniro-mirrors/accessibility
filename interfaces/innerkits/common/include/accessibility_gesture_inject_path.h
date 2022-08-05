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

#ifndef ACCESSIBILITY_GESTURE_INJECT_PATH_H
#define ACCESSIBILITY_GESTURE_INJECT_PATH_H

#include <vector>

namespace OHOS {
namespace Accessibility {
struct AccessibilityGesturePosition {
    float positionX_ = .0f;
    float positionY_ = .0f;
};

class AccessibilityGestureInjectPath {
public:
    /**
     * @brief The constructor of AccessibilityGestureInjectPath
     */
    AccessibilityGestureInjectPath() = default;

    /**
     * @brief The deconstructor of AccessibilityGestureInjectPath
     */
    ~AccessibilityGestureInjectPath() = default;

    /**
     * @brief Obtains the duration in which this gesture path continues.
     * @return Return the duration in which this gesture path continues.
     */
    int64_t GetDurationTime() const;

    /**
     * @brief Obtains all positions of this gesture path.
     * @return Return all positions of this gesture path.
     */
    const std::vector<AccessibilityGesturePosition> &GetPositions() const;

    /**
     * @brief Sets the duration for this gesture path to continue.
     * @param durationTime The duration for this gesture path to continue.
     */
    void SetDurationTime(int64_t durationTime);

    /**
     * @brief Add a position of this gesture path.
     * @param position A position of this gesture path.
     */
    void AddPosition(AccessibilityGesturePosition &position);

protected:
    std::vector<AccessibilityGesturePosition> positions_;
    int64_t durationTime_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_GESTURE_INJECT_PATH_H