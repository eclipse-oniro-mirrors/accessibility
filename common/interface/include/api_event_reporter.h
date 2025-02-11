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

#ifndef ACCESSIBILITY_API_EVENT_REPORTER_H
#define ACCESSIBILITY_API_EVENT_REPORTER_H
#include <string>
#include "singleton.h"

namespace OHOS {
namespace Accessibility {
#define ACCESSIBILITY_API_OPERATION_CONFIG_PATH "/etc/accessibility/api_event_reporter.cfg"
struct ApiReportConfig {
    std::string config_name;
    std::string config_appId;
    std::string config_routeInfo;
    int config_timeout = 0;
    int config_row = 0;
};

struct ApiEvent {
    std::string domain;
    std::string name;
    bool isRealTime = false;
};

struct ApiEventConfig {
    ApiEvent event1;
    ApiEvent event2;
    ApiEvent event3;
};

struct EventPeriodExpandableData {
    std::vector<int64_t> runTime;
    int64_t sumTime;
    int64_t successCount;
};

class ApiEventReporter : public Singleton<ApiEventReporter> {
    DECLARE_SINGLETON(ApiEventReporter);
public:
    int64_t AddProcessor();
    void WriteEndEvent(int result, int errCode, std::string apiName, int64_t beginTime);
    void ThresholdWriteEndEvent(int result, std::string apiName, int64_t beginTime,
        int32_t thresholdValue);
    int64_t GetCurrentTime();
    void ClearCacheData();

private:
    static bool LoadConfigurationFile(const std::string &configFile);
    static void GetConfigurationParams(ApiReportConfig &reportConfig, ApiEventConfig &eventConfig);
    static std::string Trim(const std::string &str);
    static std::pair<std::string, std::string> ParseKeyValue(const std::string &line);
    static bool TryParseInt(const std::string &str, int &out);
    static void ParseReportConfig(std::istringstream &stream, ApiReportConfig &reportConfig);
    static void ParseEvent(std::istringstream &stream, ApiEvent &event);
    static void ParseEventConfig(std::istringstream &stream, ApiEventConfig &eventConfig);
    static void ParseApiOperationManagement(std::istringstream &stream, ApiReportConfig &reportConfig,
        ApiEventConfig &eventConfig);
    std::shared_ptr<EventPeriodExpandableData> CacheEventInfo(std::string apiName,
        int64_t beginTime, int result);
    bool IsAppEventProccessorValid();
    void ExecuteThresholdWriteEndEvent(std::string apiName,
        std::shared_ptr<EventPeriodExpandableData> expandableData, int32_t dataCount);
    static std::string g_fileContent;
    static std::mutex g_apiOperationMutex;
    static int64_t g_processorId = -1;
    static const int64_t NULLPTR_PROCCESSORID = 0;
    std::unordered_map<std::string, std::shared_ptr<EventPeriodExpandableData>> m_thresholdData;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_API_EVENT_REPORTER_H