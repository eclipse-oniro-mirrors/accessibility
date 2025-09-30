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

#include "rules_defines.h"

namespace OHOS::Accessibility {

bool RulesDefines::Parse(const nlohmann::json& jsonRoot)
{
    nlohmann::json defines = jsonRoot["defines"];
    auto result = ParseRootTypes(defines);
    CHECK_NE_RETURN(result, true, false);
    result = ParseRootTypes(defines);
    return ParseScrollIgnoreTypes(defines);
}

bool RulesDefines::ParseRootTypes(const nlohmann::json& defines)
{
    std::vector<std::string> rootTypes = defines["root_types"];
    rootTypes_.clear();
    for (auto& type : rootTypes) {
        std::transform(type.begin(), type.end(), type.begin(),
            [](unsigned char c) { return std::tolower(c); });
        rootTypes_.insert(type);
    }

    PrintRootTypes();
    return true;
}

void RulesDefines::PrintRootTypes()
{
    std::string rootTypes;
    bool isFirst = true;
    for (const auto& rootType : rootTypes_) {
        if (!isFirst) {
            rootTypes.append(", ");
        }
        isFirst = false;
        rootTypes.append(rootType);
    }
}

bool RulesDefines::IsRootType(const std::string& type)
{
    return rootTypes_.find(type) != rootTypes_.end();
}

bool RulesDefines::ParseScrollIgnoreTypes(const nlohmann::json& defines)
{
    std::vector<std::string> scrollIgnoreTypes = defines["disabled_scroll_types"];
    scrollIgnoreTypes_.clear();
    for (auto& type : scrollIgnoreTypes) {
        std::transform(type.begin(), type.end(), type.begin(),
            [](unsigned char c) { return std::tolower(c); });
        scrollIgnoreTypes_.insert(type);
    }
    return true;
}

bool RulesDefines::IsScrollIgnoreTypes(const std::string& type)
{
    return scrollIgnoreTypes_.find(type) != scrollIgnoreTypes_.end();
}
} // namespace OHOS::Accessibility