
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

#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_H

#include "condition_item.h"
#include <string>
#include "nlohmann/json.hpp"

namespace OHOS::Accessibility {

enum class CondType {
    UNKNOWN = -1,
    SINGLE,
    AND,
    OR,
};

class CustomProps;
class Condition {
public:
    bool Parse(const nlohmann::json& cond);
    bool Check(const std::shared_ptr<ReadableRulesNode>& node) const;
    size_t GetSize() const {
        size_t totalSize = sizeof(Condition);
        totalSize +=  items_.size() * sizeof(ConditionItem);

        for (auto & conditions: combinedConditions_) {
            totalSize += conditions.GetSize();
        }

        return totalSize;
    }

private:
    bool ParseCondType(const nlohmann::json& cond);
    bool ParseCombinedCond(const nlohmann::json& cond);
    bool ParseCondItems(const nlohmann::json& cond);

    bool CheckSingle(const std::shared_ptr<ReadableRulesNode>& node) const;

    CondType condType_ = CondType::UNKNOWN;
    bool isCombinedCond_ = false;
    std::vector<Condition> combinedConditions_;
    std::vector<ConditionItem> items_;
};

} // namespace OHOS::Accessibility
 
#endif // FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_H