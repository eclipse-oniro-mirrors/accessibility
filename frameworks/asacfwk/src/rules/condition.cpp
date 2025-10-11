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

#include "condition.h"
#include "custom_props.h"

namespace OHOS::Accessibility {

static const std::map<std::string, CondType> COND_TYPE_MAP = {
    {"single", CondType::SINGLE},
    {"and", CondType::AND},
    {"or", CondType::OR},
};

bool Condition::Parse(const nlohmann::json& cond)
{
    if (!ParseCondType(cond)) {
        return false;
    }

    if (!ParseCombinedCond(cond)) {
        return false;
    }

    if (isCombinedCond_) {
        return true;
    }

    return ParseCondItems(cond);
}

template<class T> bool CheckCommon(const std::shared_ptr<ReadableRulesNode>& node, CondType condType,
    const T& condArray)
{
    for (const auto& cond : condArray) {
        bool checkResult = cond.Check(node);
        if (condType == CondType::AND && !checkResult) {
            return false;
        }

        if (condType == CondType::OR && checkResult) {
            return true;
        }
    }

    return condType == CondType::AND;
}

bool Condition::Check(const std::shared_ptr<ReadableRulesNode>& node) const
{
    if (condType_ == CondType::UNKNOWN) {
        return false;
    }

    if (condType_ == CondType::SINGLE) {
        return CheckSingle(node);
    }

    if (isCombinedCond_) {
        return CheckCommon(node, condType_, combinedConditions_);
    }

    return CheckCommon(node, condType_, items_);
}

bool Condition::CheckSingle(const std::shared_ptr<ReadableRulesNode>& node) const
{
    if (isCombinedCond_) {
        return false;
    }

    if (items_.size() != 1) {
        return false;
    }

    return items_[0].Check(node);
}

bool Condition::ParseCondItems(const nlohmann::json& cond)
{
    if (!cond.contains("condition")) {
        return false;
    }

    nlohmann::json item = cond["condition"];
    if (condType_ == CondType::SINGLE) {
        items_.resize(1);
        return items_[0].Parse(item);
    }

    if (item.size() <= 1) {
        return false;
    }

    items_.resize(item.size());
    for (uint32_t i = 0; i < item.size(); i++) {
        if (!items_[i].Parse(item[i])) {
            return false;
        }
    }
    return true;
}

bool Condition::ParseCombinedCond(const nlohmann::json& cond)
{
    if (!cond.contains("combined_condition")) {
        return true;
    }

    nlohmann::json condArray = cond["combined_condition"];
    if (condArray.size() <= 0) {
        return false;
    }

    isCombinedCond_ = true;
    combinedConditions_.resize(condArray.size());
    for (uint32_t i = 0; i < condArray.size(); i++) {
        combinedConditions_[i].Parse(condArray[i]);
    }

    return true;
}

bool Condition::ParseCondType(const nlohmann::json& cond)
{
    std::string condType = cond["condition_type"];
    if (condType.empty()) {
        return false;
    }

    auto findResult = COND_TYPE_MAP.find(condType);
    if (findResult == COND_TYPE_MAP.end()) {
        return false;
    }

    condType_ = findResult->second;
    return true;
}

} // namespace OHOS::Accessibility