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

#include "readable_rule.h"

namespace OHOS::Accessibility {

bool ReadableRule::Parse(const nlohmann::json& rule)
{
    if (!ParseName(rule)) {
        return false;
    }

    if (!ParseCondition(rule)) {
        return false;
    }

    if (!ParseIsFocus(rule)) {
        return false;
    }

    return true;
}

CheckResult ReadableRule::IsReadable(const std::shared_ptr<ReadableRulesNode>& node) const
{
    if (cond_.Check(node)) {
        return {isFocus_, HandleResult::HANDLED};
    }

    return {false, HandleResult::TO_NEXT};
}

bool ReadableRule::ParseCondition(const nlohmann::json& rule)
{
    if (!rule.contains("check")) {
        return false;
    }

    return cond_.Parse(rule["check"]);
}

bool ReadableRule::ParseName(const nlohmann::json& rule)
{
    if (!rule.contains("name")) {
        return false;
    }

    name_ = rule["name"];
    return true;
}

bool ReadableRule::ParseIsFocus(const nlohmann::json& rule)
{
    if (!rule.contains("isfocus")) {
        return false;
    }

    isFocus_ = rule["isfocus"];
    return true;
}

} // namespace OHOS::Accessibility