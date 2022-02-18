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

#ifndef ACCESSIBILITY_OPERATOR_ASYNC_LOCK_H
#define ACCESSIBILITY_OPERATOR_ASYNC_LOCK_H

#include <cstdint>
#include <mutex>

namespace OHOS {
namespace Accessibility {
class AccessibilityElementAsyncOperatorMng
{
public:
    /**
     * @brief construct function
     * @param -
     * @return -
     */
    AccessibilityElementAsyncOperatorMng() {}

    /**
     * @brief function: Set the timer related with the requestion from AA to ACE.
     * @param sequence The sequence value from AA, it is used to match with request and feedback.
     * @return -
     */
    bool SearchElementResultTimer(int sequence);

    /**
     * @brief function: Set the feedback searcg from ACE.
     * @param feedbackSequence The response sequence value from ACE
     * @return -
     */
    void UpdateSearchFeedback(int feedbackSequence);

    /**
     * @brief Inner function: Set the status requested from AA to ACE.
     * @param status true: The requestion is completed; otherwise is not.
     * @return -
     */
    void SetOperationStatus(bool status);

    /**
     * @brief Inner function: The status requested from AA to ACE.
     * @param
     * @return true: The requestion is completed; otherwise is not.
     */
    bool GetOperationStatus();

    /**
     * @brief Inner function: Count the unique value requested
     * @param
     * @return The sequence value from AA, it is used to match with request and response.
     */
    int RecordSearchSequence();
private:
    static const uint32_t MAX_REQUEST = 0x7FFFFFFF;
    static const uint32_t SECOND_TO_MILLIS = 1000000;
    static const long TIMEOUT_OPERATOR_MILLIS = 500000;
    static const long SLEEP_MILLIS = 5000;
    int feedbackSequence_ = 0;
    bool completed_ = false;
    static std::recursive_mutex mutex_;
    static int sequence_;    //matched request with callback result
};

} // namespace Accessibility
} // namespace OHOS
#endif