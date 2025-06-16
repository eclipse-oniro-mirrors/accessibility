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
 
#ifndef ACCESSIBILITY_SECURITY_COMPONENT_MANAGER_H
#define ACCESSIBILITY_SECURITY_COMPONENT_MANAGER_H
 
#include <string>
#include "accessibility_def.h"
#include "accessibility_sec_comp_rawdata.h"
 
namespace OHOS {
namespace Accessibility {

class AccessibilitySecurityComponentManager {
public:
    static int32_t SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData);
    static std::map<std::string, std::string> GenerateActionArgumentsWithHMAC(const ActionType &action,
        int64_t uniqueId, std::string bundleName, const std::map<std::string, std::string> &arguments);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SECURITY_COMPONENT_MANAGER_H