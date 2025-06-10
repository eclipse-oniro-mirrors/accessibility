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
 
#ifndef SECURITY_COMPONENT_MANAGER_H
#define SECURITY_COMPONENT_MANAGER_H
 
#include <string>
#include "accessibility_def.h"
#include "sec_comp_enhance_kit.h"
#include "sec_comp_enhance_adapter.h"
 
namespace OHOS {
namespace Accessibility {
 
struct SecCompPoint {
    int64_t uniqueId;
    std::string bundleName;
    int64_t timeStamp;
};
 
class SecurityComponentManager {
public:
    static int32_t SetEnhanceConfig(const char *cfg, uint32_t cfgLen);
    static std::map<std::string, std::string> GenerateActionArgumentsWithHMAC(const ActionType &action,
        int64_t uniqueId, std::string bundleName, const std::map<std::string, std::string> &arguments);
};
} // namespace Accessibility
} // namespace OHOS
#endif // SECURITY_COMPONENT_MANAGER_H