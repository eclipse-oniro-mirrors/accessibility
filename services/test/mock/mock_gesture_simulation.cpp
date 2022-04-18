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
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityGesturePath::AccessibilityGesturePath(AccessibilityGesturePathPosition& startPosition,
    AccessibilityGesturePathPosition& endPosition, int64_t durationTime)
{}

int64_t AccessibilityGesturePath::GetDurationTime() const
{
    return durationTime_;
}

AccessibilityGesturePathPosition& AccessibilityGesturePath::GetEndPosition()
{
    return endPosition_;
}

AccessibilityGesturePathPosition& AccessibilityGesturePath::GetStartPosition()
{
    return startPosition_;
}

void AccessibilityGesturePath::SetDurationTime(int64_t durationTime)
{
    durationTime_ = durationTime;
}

void AccessibilityGesturePath::SetEndPosition(AccessibilityGesturePathPosition& endPosition)
{
    endPosition_ = endPosition;
}

void AccessibilityGesturePath::SetStartPosition(AccessibilityGesturePathPosition& startPosition)
{
    startPosition_ = startPosition;
}
} // namespace Accessibility
} // namespace OHOS