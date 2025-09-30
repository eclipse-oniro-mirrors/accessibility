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

#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_ITEM_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_ITEM_H

#include <string>
#include <set>
#include "nlohmann/json.hpp"
#include "prop_value.h"

namespace OHOS::Accessibility {

enum class TargetNode {
    DEFAULT,
    PARENT,
    CHILDREN,
};

class CustomProps;
class ReadableRulesNode;

class ConditionItem {
public:
    bool Parse(const nlohmann::json& condItem);
    bool Check(const std::shared_ptr<ReadableRulesNode>& node) const;
private:
    bool ParseProp(const nlohmann::json& condItem);
    bool ParseOperator(const nlohmann::json& condItem);
    bool ParseValue(const nlohmann::json& condItem);
    bool ParseTarget(const nlohmann::json& condItem);
    bool ParseIsCustom(const nlohmann::json& condItem);
    bool ParseCascade(const nlohmann::json& condItem);
    std::vector<std::shared_ptr<ReadableRulesNode>> GetTargetNodes(
        const std::shared_ptr<ReadableRulesNode>& node) const;

    bool CheckItem(const std::shared_ptr<ReadableRulesNode>& node) const;
    bool CheckCustom(const std::shared_ptr<ReadableRulesNode>& node) const;
    bool GetNodeProp(const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) const;
    std::string prop_;
    CondOperator operator_ = CondOperator::UNKNOWN;

    PropValue compareValue_;

    bool isCascade_ = false;
    bool isCustom_ = false;
    TargetNode target_ = TargetNode::DEFAULT;
};

} // namespace OHOS::Accessibility

#endif // FRAMEWORKS_ASACFWK_SRC_RULES_CONDITION_ITEM_H