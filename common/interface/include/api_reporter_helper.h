/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_API_REPORTER_HELPER_H
#define ACCESSIBILITY_API_REPORTER_HELPER_H

#include "api_event_reporter.h"

namespace OHOS {
namespace Accessibility {
class ApiReportHelper {
public:
    ApiReportHelper(const std::string &name)
    {
        beginTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        apiName_ = name;
    }

    ApiReportHelper(const std::string &name, int32_t thresholdValue)
    {
        beginTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        apiName_ = name;
        thresholdValue_ = thresholdValue;
    }

    ~ApiReportHelper()
    {
        Accessibility::ApiEventReporter::GetInstance().ThresholdWriteEndEvent(result_, apiName_,
            beginTime_, thresholdValue_);
    }

    void setResult(int result)
    {
        result_ = result;
    }

    void setApiName(const std::string &name)
    {
        apiName_ = name;
    }
private:
    int64_t beginTime_ = 0;
    int32_t thresholdValue_ = 1000;
    int result_ = 0;
    std::string apiName_;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_API_REPORTER_HELPER_H