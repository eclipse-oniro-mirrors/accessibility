/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
 
#include "security_component_manager.h"
#include "hilog_wrapper.h"
#include "sec_comp_enhance_kit.h"
#include "sec_comp_enhance_adapter.h"
#include "accessibility_element_info.h"
 
namespace OHOS {
namespace Accessibility {
 
constexpr uint32_t MAX_HMAC_SIZE = 64;
const uint32_t TIMEOUT = 50;
 
int32_t SecurityComponentManager::SetEnhanceConfig(const char *cfg, uint32_t cfgLen)
{
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::SetEnhanceCfg((uint8_t *)cfg, cfgLen);
    HILOG_INFO("SetEnhanceCfg result: %{public}d", result);
    return result;
}
 
std::map<std::string, std::string> SecurityComponentManager::GenerateActionArgumentsWithHMAC(const ActionType &action,
    int64_t uniqueId, std::string bundleName, const std::map<std::string, std::string> &arguments)
{
    HILOG_INFO("actionType: %{public}d", action);
    std::map<std::string, std::string> actionArguments(arguments);
    if (action != ACCESSIBILITY_ACTION_CLICK) {
        return actionArguments;
    }
 
    auto secCompPoint = std::make_unique<SecCompPoint>();
    if (secCompPoint == nullptr) {
        HILOG_ERROR("create secCompPoint failed");
        return actionArguments;
    }
 
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::string timeStr = std::to_string(timeStamp);
 
    secCompPoint->uniqueId = uniqueId;
    secCompPoint->bundleName = bundleName;
    secCompPoint->timeStamp = timeStamp;
 
    uint32_t dataLen = sizeof(*secCompPoint);
    uint8_t outBuf[MAX_HMAC_SIZE] = { 0 };
    uint8_t *enHanceData = reinterpret_cast<uint8_t *>(&outBuf[0]);
    uint32_t enHanceDataLen = MAX_HMAC_SIZE;
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::GetPointerEventEnhanceData(
        secCompPoint.get(), dataLen, enHanceData, enHanceDataLen);
    HILOG_INFO("result: %{public}d", result);
    if (result != 0 || enHanceDataLen > MAX_HMAC_SIZE) {
        HILOG_ERROR("GetPointerEventEnhanceData failed!");
        return actionArguments;
    }
    std::vector<uint8_t> vecEnHanceData(enHanceData, enHanceData + enHanceDataLen);
    std::string strEnHanceData(vecEnHanceData.begin(), vecEnHanceData.end());
    actionArguments["HMAC"] = strEnHanceData;
    actionArguments["timeStamp"] = timeStamp;
    HILOG_INFO("result: %{public}d, strEnHanceData: %{public}s", result, strEnHanceData.c_str());
    return actionArguments;
}
} // namespace Accessibility
} // namespace OHOS