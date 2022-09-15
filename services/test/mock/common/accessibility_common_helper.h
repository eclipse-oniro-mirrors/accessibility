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

#ifndef ACCESSIBILITY_COMMON_HELPER_H
#define ACCESSIBILITY_COMMON_HELPER_H

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonHelper {
public:
    static AccessibilityCommonHelper& GetInstance()
    {
        static AccessibilityCommonHelper helper;
        return helper;
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
            constexpr int32_t sleepTime = 10;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            if (isServicePublished_) {
                return;
            }
        }
    }

    bool WaitForLoop(const std::function<bool()> &compare, int32_t timeout)
    {
        constexpr int32_t sleepTime = 100;
        int32_t count = timeout * 1000 / sleepTime;
        while (count > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            if (compare() == true) {
                return true;
            } else {
                count--;
            }
        }
        return false;
    }

private:
    bool isServicePublished_ = false;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_COMMON_HELPER_H