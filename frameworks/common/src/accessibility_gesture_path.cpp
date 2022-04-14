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

#include "accessibility_gesture_path.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityGesturePath::AccessibilityGesturePath(AccessibilityGesturePathPosition &startPosition,
    AccessibilityGesturePathPosition &endPosition, int64_t durationTime)
{
    HILOG_DEBUG("start.");

    if (startPosition.positionX_ < 0 || startPosition.positionY_ < 0) {
        HILOG_ERROR("startPosition: (%{public}f , %{public}f) is not allowed.",
            startPosition.positionX_, startPosition.positionY_);
        return;
    }
    startPosition_ = startPosition;

    if (endPosition.positionX_ < 0 || endPosition.positionY_ < 0) {
        HILOG_ERROR("endPosition : (%{public}f , %{public}f) is not allowed.",
            endPosition.positionX_, endPosition.positionY_);
        return;
    }
    endPosition_ = endPosition;

    durationTime_ = durationTime;
}

int64_t AccessibilityGesturePath::GetDurationTime() const
{
    HILOG_DEBUG("start and max stroke duration is %{public}jd", durationTime_);
    return durationTime_;
}

AccessibilityGesturePathPosition &AccessibilityGesturePath::GetEndPosition()
{
    HILOG_DEBUG("start.");
    return endPosition_;
}

AccessibilityGesturePathPosition &AccessibilityGesturePath::GetStartPosition()
{
    HILOG_DEBUG("start.");
    return startPosition_;
}

void AccessibilityGesturePath::SetDurationTime(int64_t durationTime)
{
    HILOG_DEBUG("start.");
    durationTime_ = durationTime;
}

void AccessibilityGesturePath::SetEndPosition(AccessibilityGesturePathPosition &endPosition)
{
    HILOG_DEBUG("start.");
    endPosition_ = endPosition;
}

void AccessibilityGesturePath::SetStartPosition(AccessibilityGesturePathPosition &startPosition)
{
    HILOG_DEBUG("start.");
    startPosition_ = startPosition;
}
} // namespace Accessibility
} // namespace OHOS
