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

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "app_event.h"
#include "app_event_processor_mgr.h"
#include "hilog_wrapper.h"
#include "api_event_reporter.h"
#include "uuid.h"

namespace OHOS {
namespace Accessibility {
// Global variables for caching file content and mutex
std::string ApiEventReporter::g_fileContent = "";
int64_t ApiEventReporter::g_processorId = -1;
std::mutex ApiEventReporter::g_apiOperationMutex;
constexpr size_t UUID_CHAR_ARRAY_LENGTH = 37;

ApiEventReporter::ApiEventReporter()
{
}

ApiEventReporter::~ApiEventReporter()
{
    m_thresholdData.clear();
}

bool ApiEventReporter::IsAppEventProccessorValid()
{
    if (g_processorId <= NULLPTR_PROCCESSORID) {
        g_processorId = AddProcessor();
    }
    if (g_processorId <= NULLPTR_PROCCESSORID) {
        return false;
    }
    return true;
}

bool ApiEventReporter::LoadConfigurationFile(const std::string &configFile)
{
    std::ifstream file(configFile);
    if (!file.is_open()) {
        perror("Unable to open api operation config file!");
        return false;
    }

    std::ostringstream oss;
    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (!line.empty() && line[0] != '#') {
            oss << line << "\n";
        }
    }
    g_fileContent = oss.str();
    file.close();
    return true;
}

void ApiEventReporter::GetConfigurationParams(ApiReportConfig &reportConfig, ApiEventConfig &eventConfig)
{
    std::istringstream stream(g_fileContent);
    ParseApiOperationManagement(stream, reportConfig, eventConfig);
}

std::string ApiEventReporter::Trim(const std::string &str)
{
    const char* whitespace = " \t\n\r";
    size_t first = str.find_first_not_of(whitespace);
    size_t last = str.find_last_not_of(whitespace);
    return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

std::pair<std::string, std::string> ApiEventReporter::ParseKeyValue(const std::string &line)
{
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string key = Trim(line.substr(0, colonPos));
        std::string value = Trim(line.substr(colonPos + 1));
        key.erase(std::remove(key.begin(), key.end(), '"'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
        if (!value.empty() && value.back() == ',') {
            value.pop_back(); // Remove trailing comma if present
        }
        return std::make_pair(key, value);
    }
    return std::make_pair("", "");
}

bool ApiEventReporter::TryParseInt(const std::string &str, int &out)
{
    char* end;
    long val = strtol(str.c_str(), &end, 10);
    if (*end == '\0' && end != str.c_str() && val >= INT_MIN && val <= INT_MAX) {
        out = static_cast<int>(val);
        return true;
    } else {
        HILOG_ERROR("Invalid integer: %{public}s", str.c_str());
        return false;
    }
}

void ApiEventReporter::ParseReportConfig(std::istringstream &stream, ApiReportConfig &reportConfig)
{
    std::unordered_map<std::string, std::function<void(const std::string&)>> configMap = {
        {"config_name", [&](const std::string &value) { reportConfig.config_name = value; }},
        {"config_appId", [&](const std::string &value) { reportConfig.config_appId = value; }},
        {"config_routeInfo", [&](const std::string &value) { reportConfig.config_routeInfo = value; }},
        {"config_TriggerCond.timeout", [&](const std::string &value) {
            int temp;
            if (TryParseInt(value, temp)) {
                reportConfig.config_timeout = temp;
            } else {
                HILOG_ERROR("Invalid integer for config_timeout: %{public}s", value.c_str());
            }
        }},
        {"config_TriggerCond.row", [&](const std::string &value) {
            int temp;
            if (TryParseInt(value, temp)) {
                reportConfig.config_row = temp;
            } else {
                HILOG_ERROR("Invalid integer for config_row: %{public}s", value.c_str());
            }
        }}
    };

    std::string line;
    while (std::getline(stream, line)) {
        line = Trim(line);
        if (line == "},") {
            break;
        }
        auto keyValue = ParseKeyValue(line);
        auto it = configMap.find(keyValue.first);
        if (it != configMap.end()) {
            it->second(keyValue.second);
        }
    }
}

void ApiEventReporter::ParseEvent(std::istringstream &stream, ApiEvent &event)
{
    std::unordered_map<std::string, std::function<void(const std::string&)>> eventMap = {
        {"domain", [&](const std::string &value) { event.domain = value; }},
        {"name", [&](const std::string &value) { event.name = value; }},
        {"isRealTime", [&](const std::string &value) { event.isRealTime = (value == "true"); }}
    };

    std::string line;
    while (std::getline(stream, line)) {
        line = Trim(line);
        if (line == "},") {
            break;
        }
        auto keyValue = ParseKeyValue(line);
        auto it = eventMap.find(keyValue.first);
        if (it != eventMap.end()) {
            it->second(keyValue.second);
        }
    }
}

void ApiEventReporter::ParseEventConfig(std::istringstream &stream, ApiEventConfig &eventConfig)
{
    std::unordered_map<std::string, std::function<void(std::istringstream&)>> eventConfigMap = {
        {"\"event1\": {", [&](std::istringstream &stream) { ParseEvent(stream, eventConfig.event1); }},
        {"\"event2\": {", [&](std::istringstream &stream) { ParseEvent(stream, eventConfig.event2); }},
        {"\"event3\": {", [&](std::istringstream &stream) { ParseEvent(stream, eventConfig.event3); }}
    };

    std::string line;
    while (std::getline(stream, line)) {
        line = Trim(line);
        if (line == "},") {
            break;
        }
        auto it = eventConfigMap.find(line);
        if (it != eventConfigMap.end()) {
            it->second(stream);
        }
    }
}

void ApiEventReporter::ParseApiOperationManagement(std::istringstream &stream, ApiReportConfig &reportConfig,
    ApiEventConfig &eventConfig)
{
    std::unordered_map<std::string, std::function<void(std::istringstream&)>> apiOpMgmtMap = {
        {"\"report_config\": {", [&](std::istringstream &stream) { ParseReportConfig(stream, reportConfig); }},
        {"\"event_config\": {", [&](std::istringstream &stream) { ParseEventConfig(stream, eventConfig); }}
    };

    std::string line;
    while (std::getline(stream, line)) {
        line = Trim(line);
        if (line == "}") {
            break;
        }
        auto it = apiOpMgmtMap.find(line);
        if (it != apiOpMgmtMap.end()) {
            it->second(stream);
        }
    }
}

int64_t ApiEventReporter::AddProcessor()
{
    HILOG_DEBUG("AddProcessor enter.");
    std::lock_guard<std::mutex> lock(g_apiOperationMutex);
    ApiReportConfig reportConfig;
    ApiEventConfig eventConfig;
    if (g_fileContent.empty()) {
        if (LoadConfigurationFile(ACCESSIBILITY_API_OPERATION_CONFIG_PATH) != true) {
            HILOG_ERROR("AddProcessor LoadConfigurationFile error!");
            return -1;
        }
    }
    GetConfigurationParams(reportConfig, eventConfig);
    HiviewDFX::HiAppEvent::ReportConfig config;
    config.name = reportConfig.config_name;
    config.appId = reportConfig.config_appId;
    config.routeInfo = reportConfig.config_routeInfo;
    config.triggerCond.timeout = reportConfig.config_timeout;
    config.triggerCond.row = reportConfig.config_row;
    config.eventConfigs.clear();
    HiviewDFX::HiAppEvent::EventConfig event1;
    event1.domain = eventConfig.event1.domain;
    event1.name = eventConfig.event1.name;
    event1.isRealTime = eventConfig.event1.isRealTime;
    config.eventConfigs.push_back(event1);
    HiviewDFX::HiAppEvent::EventConfig event2;
    event2.domain = eventConfig.event2.domain;
    event2.name = eventConfig.event2.name;
    event2.isRealTime = eventConfig.event2.isRealTime;
    config.eventConfigs.push_back(event2);
    HiviewDFX::HiAppEvent::EventConfig event3;
    event3.domain = eventConfig.event3.domain;
    event3.name = eventConfig.event3.name;
    event3.isRealTime = eventConfig.event3.isRealTime;
    config.eventConfigs.push_back(event3);
    g_processorId = HiviewDFX::HiAppEvent::AppEventProcessorMgr::AddProcessor(config);
    return g_processorId;
}

std::string RandomUuid()
{
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuidChars[UUID_CHAR_ARRAY_LENGTH] = {'\0'};
    uuid_unparse(uuid, uuidChars);
    return std::string(uuidChars);
}

void ApiEventReporter::WriteEndEvent(int result, int errCode, std::string apiName, int64_t beginTime)
{
    HILOG_DEBUG("WriteEndEvent enter.");
    std::string transId = std::string("traceId_") + RandomUuid();
    int64_t endTime = GetCurrentTime();
    
    HiviewDFX::HiAppEvent::Event event("api_diagnostic", "api_exec_end", OHOS::HiviewDFX::HiAppEvent::BEHAVIOR);
    event.AddParam("trans_id", transId);
    event.AddParam("api_name", apiName);
    event.AddParam("sdk_name", std::string("AccessibilityKit"));
    event.AddParam("begin_time", beginTime);
    event.AddParam("end_time", endTime);
    event.AddParam("result", result);
    event.AddParam("error_code", errCode);
    if (!IsAppEventProccessorValid()) {
        HILOG_ERROR("WriteEndEvent processorid invalid!");
        return;
    }
    HiviewDFX::HiAppEvent::Write(event);
}

void ApiEventReporter::ClearCacheData()
{
    for (auto it : m_thresholdData) {
        std::string apiName = it.first;
        auto expandableData = it.second;
        int32_t dataCount = static_cast<int32_t>(expandableData->runTime.size());
        if (dataCount < 1) {
            continue;
        }
        ExecuteThresholdWriteEndEvent(apiName, expandableData, dataCount);
    }
}

int64_t ApiEventReporter::GetCurrentTime()
{
    int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return time;
}

void ApiEventReporter::ThresholdWriteEndEvent(int result, std::string apiName, int64_t beginTime,
    int32_t thresholdValue)
{
    auto expandableData = CacheEventInfo(apiName, beginTime, result);
    if (expandableData == nullptr) {
        return;
    }
    int32_t dataCount = static_cast<int32_t>(expandableData->runTime.size());
    HILOG_DEBUG("ThresholdWriteEndEvent apiName: %{public}s, dataCount: %{public}d,
        thresholdValue: %{public}d", apiName.c_str(), dataCount, thresholdValue);
    if (dataCount % thresholdValue != 0) {
        return;
    }
    ExecuteThresholdWriteEndEvent(apiName, expandableData, dataCount);
}

std::shared_ptr<EventPeriodExpandableData> ApiEventReporter::CacheEventInfo(std::string apiName,
    int64_t beginTime, int result)
{
    std::lock_guard<std::mutex> lock(g_apiOperationMutex);
    if (m_thresholdData.find(apiName) == m_thresholdData.end()) {
        std::shared_ptr<EventPeriodExpandableData> expandableData = std::make_shared<EventPeriodExpandableData>();
        if (expandableData == nullptr) {
            HILOG_ERROR("ApiEventReporter CacheEventInfo expandableData make_shared failed");
            return nullptr;
        }
        m_thresholdData.insert(std::pair<std::string, std::shared_ptr<EventPeriodExpandableData>>(apiName,
            expandableData));
    }
    auto expandableData = m_thresholdData[apiName];
    int64_t costTime = GetCurrentTime() - beginTime;
    expandableData->runTime.push_back(costTime);
    expandableData->sumTime += costTime;
    if (result == 0) {
        expandableData->successCount++;
    }
    return expandableData;
}

void ApiEventReporter::ExecuteThresholdWriteEndEvent(std::string apiName,
    std::shared_ptr<EventPeriodExpandableData> expandableData, int32_t dataCount)
{
    HILOG_DEBUG("ExecuteThresholdWriteEndEvent enter.");
    HiviewDFX::HiAppEvent::Event event("api_diagnostic", "api_called_stat_cnt", OHOS::HiviewDFX::HiAppEvent::BEHAVIOR);
    event.AddParam("trans_id", std::string(""));
    event.AddParam("api_name", apiName);
    event.AddParam("sdk_name", std::string("AccessibilityKit"));
    event.AddParam("call_times", dataCount);
    event.AddParam("success_times", expandableData->successCount);
    event.AddParam("max_cost_time", *max_element(expandableData->runTime.begin(), expandableData->runTime.end()));
    event.AddParam("min_cost_time", *min_element(expandableData->runTime.begin(), expandableData->runTime.end()));
    event.AddParam("total_cost_time", expandableData->sumTime);
    if (!IsAppEventProccessorValid()) {
        HILOG_ERROR("ExecuteThresholdWriteEndEvent processorid invalid!");
        return;
    }
    HiviewDFX::HiAppEvent::Write(event);

    //重置数据
    expandableData->runTime.clear();
    expandableData->successCount = 0;
    expandableData->sumTime = 0;
}
}  // namespace Accessibility
}  // namespace OHOS