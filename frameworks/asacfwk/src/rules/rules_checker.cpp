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

// LCOV_EXCL_START
#include "rules_checker.h"
#include "hilog_wrapper.h"
namespace OHOS::Accessibility {
ReadableRulesChecker& ReadableRulesChecker::GetInstance()
{
    static ReadableRulesChecker instance;
    return instance;
}

ReadableRulesNode::~ReadableRulesNode() = default;

bool ReadableRulesChecker::CheckInit(const std::string& readableRules)
{
    isChecked_ = true;
    if (!IsInited()) {
        Init(readableRules);
    }

    if (!IsInited()) {
        return false;
    }

    return true;
}

void ReadableRulesChecker::Init(const std::string& rules)
{
    isInited_ = false;

    nlohmann::json jsonRoot = nlohmann::json::parse(rules, nullptr, false);
    if (jsonRoot.is_null() || jsonRoot.is_discarded()) {
        return;
    }

    if (!rulesDefines_.Parse(jsonRoot)) {
        return;
    }

    if (!customProps_.Parse(jsonRoot)) {
        return;
    }

    if (!ParseRules(jsonRoot)) {
        return;
    }

    isInited_ = true;
}

bool ReadableRulesChecker::ParseRules(const nlohmann::json& jsonRoot)
{
    if (!jsonRoot.contains("rules")) {
        return false;
    }

    nlohmann::json jsonRules = jsonRoot["rules"];
    if (jsonRules.size() <= 0) {
        return false;
    }

    rules_.resize(jsonRules.size());
    for (size_t i = 0; i < jsonRules.size(); i++) {
        if (!rules_[i].Parse(jsonRules[i])) {
            return false;
        }
    }

    return true;
}

bool ReadableRulesChecker::IsReadable(const std::shared_ptr<ReadableRulesNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    for (const auto& rule : rules_) {
        CheckResult ret = rule.IsReadable(node);
        HILOG_INFO("rule:%{public}s, elementId:%{public}" PRId64 ", isReadable:%{public}d",
            rule.GetName().c_str(), node->GetAccessibilityId(), ret.isReadable);
        if (ret.handleResult == HandleResult::HANDLED) {
            return ret.isReadable;
        }
    }
    return false;
}

const CustomProps& ReadableRulesChecker::GetCustomProps()
{
    return ReadableRulesChecker::GetInstance().customProps_;
}

const RulesDefines& ReadableRulesChecker::GetRulesDefines()
{
    return ReadableRulesChecker::GetInstance().rulesDefines_;
}

bool ReadableRulesChecker::IsRootType(const std::shared_ptr<ReadableRulesNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    PropValue value;
    auto result = node->GetPropType(value);
    CHECK_NE_RETURN(result, true, false);
    std::string type = value.valueStr;
    std::transform(type.begin(), type.end(), type.begin(),
        [](unsigned char c) { return std::tolower(c); });

    bool isRootType = rulesDefines_.IsRootType(type) && node->IsModal();
    return isRootType;
}

bool ReadableRulesChecker::IsScrollIgnoreTypes(const std::shared_ptr<ReadableRulesNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    PropValue value;
    auto result = node->GetPropType(value);
    CHECK_NE_RETURN(result, true, false);
    std::string type = value.valueStr;
    std::transform(type.begin(), type.end(), type.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return rulesDefines_.IsScrollIgnoreTypes(type);
}

bool ReadableRulesChecker::IsAvailable(const std::shared_ptr<ReadableRulesNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    auto curNode = node;
    while (curNode) {
        PropValue value;
        auto result = curNode->GetPropType(value);
        if (IsRootType(curNode)) {
            return true;
        }
        auto children = curNode->GetChildren();
        auto childrenSize = children.size();
        if (childrenSize == 0) {
            curNode = curNode->GetParent();
            continue;
        }
        auto lastChildren = children[childrenSize - 1];
        if (IsRootType(lastChildren) && lastChildren->IsModal()) {
            return false;
        }
        curNode = curNode->GetParent();
    }
    return true;
}

bool ReadableRulesChecker::IsScrollableTypes(const std::shared_ptr<ReadableRulesNode>& node)
{
    CHECK_NULL_RETURN(node, false);
    PropValue value;
    auto result = node->GetPropType(value);
    CHECK_NE_RETURN(result, true, false);
    std::string type = value.valueStr;
    std::transform(type.begin(), type.end(), type.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return rulesDefines_.IsScrollableTypes(type);
}
} // namespace OHOS::Ace::Accessibility
// LCOV_EXCL_STOP