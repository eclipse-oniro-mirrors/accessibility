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
 
#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_RULES_READABLE_RULE_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_RULES_READABLE_RULE_H
 
#include "check_result.h"
#include "condition.h"
#include <string>
#include "nlohmann/json.hpp"

namespace OHOS::Accessibility {

class  ReadableRule {
public:
    bool Parse(const nlohmann::json& rule);
    CheckResult IsReadable(const std::shared_ptr<ReadableRulesNode>& node) const;
    const std::string& GetName() const
    {
        return name_;
    }

private:
    bool ParseName(const nlohmann::json& rule);
    bool ParseIsFocus(const nlohmann::json& rule);
    bool ParseCondition(const nlohmann::json& rule);

    std::string name_;
    bool isFocus_ = false;
    Condition cond_;
};

} // namespace OHOS::Accessibility

#endif // FRAMEWORKS_ASACFWK_SRC_RULES_RULES_READABLE_RULE_H