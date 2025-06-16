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
 
#include "accessibility_security_component_manager.h"
#include "hilog_wrapper.h"
#include "accessibility_element_info.h"
#ifdef ACCESSIBILITY_SECURITY_COMPONENT
#include "sec_comp_enhance_kit.h"
#include "sec_comp_enhance_adapter.h"
#endif // ACCESSIBILITY_SECURITY_COMPONENT
 
namespace OHOS {
namespace Accessibility {
 
constexpr uint32_t MAX_HMAC_SIZE = 64;
const uint32_t TIMEOUT = 50;

int32_t AccessibilitySecurityComponentManager::SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData)
{
    HILOG_INFO();
#ifdef ACCESSIBILITY_SECURITY_COMPONENT
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::SetEnhanceCfg(rawData.data, rawData.size);
    HILOG_INFO("SetEnhanceCfg result: %{public}d", result);
    return result;
#else
    return RET_OK;
#endif // ACCESSIBILITY_SECURITY_COMPONENT
}
 
std::map<std::string, std::string> AccessibilitySecurityComponentManager::GenerateActionArgumentsWithHMAC(const ActionType &action,
    int64_t uniqueId, std::string bundleName, const std::map<std::string, std::string> &arguments)
{
    HILOG_INFO("actionType: %{public}d", action);
#ifndef ACCESSIBILITY_SECURITY_COMPONENT
    return arguments;
#else
    std::map<std::string, std::string> actionArguments(arguments);
    if (action != ACCESSIBILITY_ACTION_CLICK) {
        return actionArguments;
    }
 
    std::unique_ptr<AccessibilitySecCompPoint> point = std::make_unique<AccessibilitySecCompPoint>();
    if (point == nullptr) {
        HILOG_ERROR("create point failed");
        return actionArguments;
    }
 
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    std::string timeStr = std::to_string(timeStamp);
 
    point->uniqueId = uniqueId;
    point->bundleName = bundleName;
    point->timeStamp = timeStamp;
 
    uint32_t dataLen = sizeof(*point);
    uint8_t outBuf[MAX_HMAC_SIZE + 1] = { 0 };
    uint8_t *enHanceData = reinterpret_cast<uint8_t *>(&outBuf[0]);
    uint32_t enHanceDataLen = MAX_HMAC_SIZE;
    int32_t result = Security::SecurityComponent::SecCompEnhanceKit::GetPointerEventEnhanceData(
        point.get(), dataLen, enHanceData, enHanceDataLen);
    HILOG_INFO("result: %{public}d", result);
    if (result != 0 || enHanceDataLen > MAX_HMAC_SIZE) {
        HILOG_ERROR("GetPointerEventEnhanceData failed!");
        return actionArguments;
    }
    std::vector<uint8_t> vecEnHanceData(enHanceData, enHanceData + enHanceDataLen);
    std::string strEnHanceData(vecEnHanceData.begin(), vecEnHanceData.end());
    actionArguments[ACTION_ARGU_CLICK_ENHANCE_DATA] = strEnHanceData;
    actionArguments[ACTION_ARGU_CLICK_TIMESTAMP] = timeStamp;
    HILOG_INFO("result: %{public}d, strEnHanceData: %{public}s", result, strEnHanceData.c_str());
    return actionArguments;
#endif // ACCESSIBILITY_SECURITY_COMPONENT
}
} // namespace Accessibility
} // namespace OHOS