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

#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_RULES_DEFINES_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_RULES_DEFINES_H

#include <set>
#include <string>
#include "nlohmann/json.hpp"

namespace OHOS::Accessibility {
#define CHECK_NE_RETURN(var, value, ret) \
    do {                                    \
        if ((var) != (value)) {             \
            return ret;                     \
        }                                   \
    } while (0)
class  RulesDefines {
public:
    bool IsRootType(const std::string& type);
    bool Parse(const nlohmann::json& jsonRoot);
    bool IsScrollIgnoreTypes(const std::string& type);
    size_t GetTotalMemoryUsage() const {
        return sizeof(RulesDefines) + rootTypes_.size() * 8;
    }

private:
    bool ParseRootTypes(const nlohmann::json& defines);
    void PrintRootTypes();
    bool ParseScrollIgnoreTypes(const nlohmann::json& defines);

    std::set<std::string> rootTypes_;
    std::set<std::string> scrollIgnoreTypes_;
};

} // namespace OHOS::Accessibility

#endif // FRAMEWORKS_ASACFWK_SRC_RULES_RULES_DEFINES_H