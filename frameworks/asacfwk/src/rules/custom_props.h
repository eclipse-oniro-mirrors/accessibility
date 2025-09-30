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

#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_CUSTOM_PROPS_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_CUSTOM_PROPS_H

#include "condition.h"
#include <string>
#include <map>
#include "nlohmann/json.hpp"

namespace OHOS::Accessibility {
constexpr int32_t PATCH_SIZE = 8;

class CustomProps {
public:
    bool Parse(const nlohmann::json& jsonRoot);

    const Condition* GetCondition(const std::string name) const;
    size_t GetTotalMemoryUsage() const
    {
        size_t totalSize = sizeof(CustomProps);

        for (auto &pair : customProps_) {
            totalSize += PATCH_SIZE;
            totalSize += pair.second.GetSize();
        }
        return totalSize;
    }

private:
    std::map<std::string, Condition> customProps_;
};

} // namespace OHOS::Accessibility

#endif // FRAMEWORKS_ASACFWK_SRC_RULES_CUSTOM_PROPS_H