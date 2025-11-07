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

#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_RULES_CHECKER_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_RULES_CHECKER_H

#include <set>
#include <string>

#include "rules_defines.h"
#include "custom_props.h"
#include "readable_rule.h"
#include "rules_check_node.h"
#include "nlohmann/json.hpp"

namespace OHOS::Accessibility {

#define CHECK_NULL_VOID(ptr) \
    do {                     \
        if (!(ptr)) {        \
            return;          \
        }                    \
    } while (0)

#define CHECK_NULL_RETURN(ptr, ret) \
    do {                            \
        if (!(ptr)) {               \
            return ret;             \
        }                           \
    } while (0)

#define CHECK_NE_RETURN(var, value, ret) \
    do {                                    \
        if ((var) != (value)) {             \
            return ret;                     \
        }                                   \
    } while (0)

class ReadableRulesChecker {
public:
    ReadableRulesChecker() = default;
    virtual ~ReadableRulesChecker() = default;
    static ReadableRulesChecker& GetInstance();
    bool IsRootType(const std::shared_ptr<ReadableRulesNode>& node);
    bool IsScrollIgnoreTypes(const std::shared_ptr<ReadableRulesNode>& node);
    bool IsReadable(const std::shared_ptr<ReadableRulesNode>& node);
    bool IsScrollableTypes(const std::shared_ptr<ReadableRulesNode>& node);
    bool IsAvailable(const std::shared_ptr<ReadableRulesNode>& node);
    bool CheckInit(const std::string& readableRules);
    static const CustomProps& GetCustomProps();
    static const RulesDefines& GetRulesDefines();
    bool IsInited() { return isInited_; }
    bool IsCheckedRules() { return isChecked_; }
private:
    bool ParseRules(const nlohmann::json& jsonRoot);

    void Init(const std::string& rules);
    RulesDefines rulesDefines_;
    CustomProps customProps_;
    std::vector<ReadableRule> rules_;
    bool isInited_ = false;
    bool isChecked_ = false;
};

} // namespace OHOS::Accessibility

#endif // FRAMEWORKS_ASACFWK_SRC_RULES_RULES_CHECKER_H