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

#include "accessibility_element_operator_async_mng.h"

#include <sys/time.h>
#include <unistd.h>
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
std::recursive_mutex AccessibilityElementAsyncOperatorMng::mutex_ = {};
int AccessibilityElementAsyncOperatorMng::sequence_ = 0;

bool AccessibilityElementAsyncOperatorMng::SearchElementResultTimer(const int sequence)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    struct timeval getTime {};
    gettimeofday(&getTime, NULL);
    uint64_t startTime = getTime.tv_sec * SECOND_TO_MILLIS + getTime.tv_usec;
    HILOG_DEBUG("element sequence[%{public}d]", sequence);

    do {
        if (feedbackSequence_ == sequence) {
            return true;
        }
        gettimeofday(&getTime, NULL);
        uint64_t endTime =  getTime.tv_sec * SECOND_TO_MILLIS + getTime.tv_usec;
        uint64_t waitTime = endTime - startTime;
        if (TIMEOUT_OPERATOR_MILLIS < waitTime) {
            completed_ = true;
            HILOG_ERROR("Failed to wait sequence[%{public}d], feedbackSequence_[%{public}d]",
                sequence, feedbackSequence_);
            return false;
        }
        usleep(SLEEP_MILLIS);
    } while (true);
    HILOG_DEBUG("Response [sequence:%{public}d] end", sequence);
    return true;
}

void AccessibilityElementAsyncOperatorMng::UpdateSearchFeedback(int feedbackSequence)
{
    feedbackSequence_ = feedbackSequence;
}

void AccessibilityElementAsyncOperatorMng::SetOperationStatus(bool status)
{
    completed_ = status;
}

bool AccessibilityElementAsyncOperatorMng::GetOperationStatus()
{
    HILOG_DEBUG("[completed_:%{public}d]", completed_);
    return completed_;
}

int AccessibilityElementAsyncOperatorMng::RecordSearchSequence()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    HILOG_INFO("[sequence_:%{public}d]", sequence_);
    sequence_++;
    sequence_ = sequence_ % MAX_REQUEST;
    HILOG_INFO("[sequence_:%{public}d] end", sequence_);
    return sequence_;
}
} // namespace Accessibility
} // namespace OHOS