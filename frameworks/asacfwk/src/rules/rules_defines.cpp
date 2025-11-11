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
#include "rules_defines.h"
#include "hilog_wrapper.h"

namespace OHOS::Accessibility {

bool RulesDefines::Parse(const nlohmann::json& jsonRoot)
{
    nlohmann::json defines = jsonRoot["defines"];
    auto result = ParseRootTypes(defines);
    CHECK_NE_RETURN(result, true, false);
    result = ParseScrollIgnoreTypes(defines);
    CHECK_NE_RETURN(result, true, false);
    result = ParseScrollableTypes(defines);
    CHECK_NE_RETURN(result, true, false);
    return result;
}

bool RulesDefines::ParseRootTypes(const nlohmann::json& defines)
{
    if (!defines.is_object()) {
        HILOG_ERROR("json is not object.");
        return false;
    }

    std::vector<std::string> rootTypes;
    auto it = defines.find("root_types");
    if (it != defines.end() && it->is_array()) {
        const auto &array = *it;
        for (const auto &item : array) {
            if (!item.is_string()) {
                HILOG_ERROR("Array elements are not all strings");
                return false;
            }
        }
        HILOG_DEBUG("Find key root_types successful.");
        rootTypes = array.get<std::vector<std::string>>();
    }
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
    if (!defines.is_object()) {
        HILOG_ERROR("json is not object.");
        return false;
    }
    
    std::vector<std::string> scrollIgnoreTypes;
    auto it = defines.find("disabled_scroll_types");
    if (it != defines.end() && it->is_array()) {
        const auto &array = *it;
        for (const auto &item : array) {
            if (!item.is_string()) {
                HILOG_ERROR("Array elements are not all strings");
                return false;
            }
        }
        HILOG_DEBUG("Find key disabled_scroll_types successful.");
        scrollIgnoreTypes = array.get<std::vector<std::string>>();
    }
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

bool RulesDefines::ParseScrollableTypes(const nlohmann::json& defines)
{
    if (defines.find("scrollable_types") == defines.end()) {
        return true;
    }
    std::vector<std::string> scrollableTypes = defines["scrollable_types"];
    scrollableTypes_.clear();
    for (auto& type : scrollableTypes) {
        std::transform(type.begin(), type.end(), type.begin(),
            [](unsigned char c) { return std::tolower(c); });
        scrollableTypes_.insert(type);
    }
    return true;
}
 
bool RulesDefines::IsScrollableTypes(const std::string& type)
{
    return scrollableTypes_.find(type) != scrollableTypes_.end();
}
} // namespace OHOS::Accessibility
// LCOV_EXCL_STOP