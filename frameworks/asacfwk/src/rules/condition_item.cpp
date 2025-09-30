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

#include "condition_item.h"
#include "custom_props.h"
#include "rules_checker.h"

namespace OHOS::Accessibility {

static const std::map<std::string, CondOperator> COND_OPER_MAP = {
    {"eq", CondOperator::EQ},
    {"ne", CondOperator::NE},
    {"not_oneof", CondOperator::NOT_ONEOF},
    {"is_oneof", CondOperator::IS_ONEOF},
    {"has_any", CondOperator::HAS_ANY},
};

static const std::map<std::string, TargetNode> TARGET_NODE_MAP = {
    {"default", TargetNode::DEFAULT},
    {"parent", TargetNode::PARENT},
    {"children", TargetNode::CHILDREN},
};

using ParseFunc = decltype(&ConditionItem::Parse);

bool ConditionItem::Parse(const nlohmann::json& condItem)
{
    static const std::vector<ParseFunc> PARSE_FUNC_ARRAY = {
        &ConditionItem::ParseProp,
        &ConditionItem::ParseOperator,
        &ConditionItem::ParseValue,
        &ConditionItem::ParseTarget,
        &ConditionItem::ParseIsCustom,
        &ConditionItem::ParseCascade,
    };

    for (auto func : PARSE_FUNC_ARRAY) {
        if (!(this->*func)(condItem)) {
            return false;
        }
    }

    return true;
}

static void GetChildrenRecursive(const std::shared_ptr<ReadableRulesNode>& node,
    std::vector<std::shared_ptr<ReadableRulesNode>>& children)
{
    auto directChildren = node->GetChildren();
    children.insert(children.end(), directChildren.begin(), directChildren.end());

    for (auto& child : directChildren) {
        GetChildrenRecursive(child, children);
    }
}

std::vector<std::shared_ptr<ReadableRulesNode>> GetChildren(const std::shared_ptr<ReadableRulesNode>& node,
    bool isCascade)
{
    std::vector<std::shared_ptr<ReadableRulesNode>> children;
    CHECK_NULL_RETURN(node, children);
    if (!isCascade) {
        return node->GetChildren();
    }
    GetChildrenRecursive(node, children);
    return children;
}

std::vector<std::shared_ptr<ReadableRulesNode>> ConditionItem::GetTargetNodes(
    const std::shared_ptr<ReadableRulesNode>& node) const
{
    if (target_ == TargetNode::DEFAULT) {
        return { node };
    }

    std::vector<std::shared_ptr<ReadableRulesNode>> ret;
    if (target_ == TargetNode::PARENT) {
        auto parentNode = node->GetParent();
        CHECK_NULL_RETURN(parentNode, ret);

        if (!isCascade_) {
            ret.emplace_back(parentNode);
            return ret;
        }

        while (parentNode) {
            ret.emplace_back(parentNode);
            parentNode = parentNode->GetParent();
        }

        return ret;
    }

    // TargetNode::CHILDREN
    return GetChildren(node, isCascade_);
}

bool ConditionItem::Check(const std::shared_ptr<ReadableRulesNode>& node) const
{
    std::vector<std::shared_ptr<ReadableRulesNode>> targetNodes = GetTargetNodes(node);
    if (isCustom_) {
        for (auto tmpNode : targetNodes) {
            if (CheckCustom(tmpNode)) {
                return true;
            }
        }
        return false;
    }

    for (auto tmpNode: targetNodes) {
        if (CheckItem(tmpNode)) {
            return true;
        }
    }

    return false;
}

static std::map<
    std::string,
    std::function<bool(const std::shared_ptr<ReadableRulesNode>& node, PropValue& value)>
> propFuncMap = {
    { "text", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropText(value);
    }},
    { "hintText", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropHintText(value);
    }},
    { "description", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropDesc(value);
    }},
    { "accessibilityText", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropAccessibilityText(value);
    }},
    { "type", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropType(value);
    }},
    { "accessibilityLevel", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropAccessibilityLevel(value);
    }},
    { "accessibilityGroup", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropAccessibilityGroup(value);
    }},
    { "isEnable", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropIsEnable(value);
    }},
    { "children_count", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropChildrenCount(value);
    }},
    { "actionNames", [](const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) {
        return node->GetPropActionNames(value);
    }}
};

bool ConditionItem::GetNodeProp(const std::shared_ptr<ReadableRulesNode>& node, PropValue& value) const
{
    CHECK_NULL_RETURN(node, false);
    auto findResult = propFuncMap.find(prop_);
    if (findResult == propFuncMap.end()) {
        return false;
    }

    return findResult->second(node, value);
}

bool ConditionItem::CheckItem(const std::shared_ptr<ReadableRulesNode>& node) const
{
    PropValue prop;
    if (!GetNodeProp(node, prop)) {
        return false;
    }

    return prop.Compare(operator_, compareValue_);
}

bool ConditionItem::CheckCustom(const std::shared_ptr<ReadableRulesNode>& node) const
{
    if (compareValue_.valueType != ValueType::BOOL) {
        return false;
    }

    if (operator_ != CondOperator::EQ && operator_ != CondOperator::NE) {
        return false;
    }

    const Condition* cond = ReadableRulesChecker::GetCustomProps().GetCondition(prop_);
    if (!cond) {
        return false;
    }

    bool checkResult = cond->Check(node);

    bool ret = (operator_ == CondOperator::EQ) ? (checkResult == compareValue_.valueBool) :
        (checkResult != compareValue_.valueBool);
    return ret;
}

bool ConditionItem::ParseTarget(const nlohmann::json& condItem)
{
    if (!condItem.contains("target")) {
        target_ = TargetNode::DEFAULT;
        return true;
    }

    std::string target = condItem["target"];

    auto findResult = TARGET_NODE_MAP.find(target);
    if (findResult == TARGET_NODE_MAP.end()) {
        return false;
    }

    target_ = findResult->second;
    return true;
}

bool ConditionItem::ParseIsCustom(const nlohmann::json& condItem)
{
    if (!condItem.contains("is_custom")) {
        isCustom_ = false;
        return true;
    }

    nlohmann::json value = condItem["is_custom"];
    if (!value.is_boolean()) {
        return false;
    }

    isCustom_ = value;
    return true;
}

bool ConditionItem::ParseCascade(const nlohmann::json& condItem)
{
    if (!condItem.contains("cascade")) {
        isCascade_ = false;
        return true;
    }

    nlohmann::json value = condItem["cascade"];
    if (!value.is_boolean()) {
        return false;
    }

    isCascade_ = value;
 
    return true;
}

bool ConditionItem::ParseValue(const nlohmann::json& condItem)
{
    if (!condItem.contains("value")) {
        return false;
    }

    nlohmann::json value = condItem["value"];
    if (value.is_boolean()) {
        compareValue_.valueType = ValueType::BOOL;
        compareValue_.valueBool = value;
        return true;
    }

    if (value.is_array()) {
        compareValue_.valueType = ValueType::ARRAY;
        compareValue_.valueArray = value;
        return true;
    }

    if (value.is_string()) {
        compareValue_.valueType = ValueType::STRING;
        compareValue_.valueStr = value;
        return true;
    }

    if (value.is_number()) {
        compareValue_.valueType = ValueType::NUMBER;
        compareValue_.valueNum = value;
        return true;
    }
    return false;
}

bool ConditionItem::ParseOperator(const nlohmann::json& condItem)
{
    if (!condItem.contains("operator")) {
        return false;
    }

    std::string oper = condItem["operator"];

    auto findResult = COND_OPER_MAP.find(oper);
    if (findResult == COND_OPER_MAP.end()) {
        return false;
    }

    operator_ = findResult->second;
    return true;
}

bool ConditionItem::ParseProp(const nlohmann::json& condItem)
{
    if (!condItem.contains("prop")) {
        return false;
    }

    prop_ = condItem["prop"];
    return true;
}

} // namespace OHOS::Accessibility