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

#include "accessibility_gesture_inject_path.h"

#include <cinttypes>

#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
int64_t AccessibilityGestureInjectPath::GetDurationTime() const
{
    HILOG_DEBUG("durationTime:%{public}" PRId64 "", durationTime_);
    return durationTime_;
}

const std::vector<AccessibilityGesturePosition> &AccessibilityGestureInjectPath::GetPositions() const
{
    HILOG_DEBUG();

    return positions_;
}

void AccessibilityGestureInjectPath::SetDurationTime(int64_t durationTime)
{
    HILOG_DEBUG("durationTime:%{public}" PRId64 "", durationTime);
    durationTime_ = durationTime;
}

void AccessibilityGestureInjectPath::AddPosition(AccessibilityGesturePosition &position)
{
    HILOG_DEBUG();

    positions_.emplace_back(position);
}
} // namespace Accessibility
} // namespace OHOS
