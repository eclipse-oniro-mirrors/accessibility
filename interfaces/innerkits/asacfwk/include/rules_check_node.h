
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
 
#ifndef INTERFACES_INNERKITS_ASACFWK_INCLUDE_RULES_CHECKE_NODE_H
#define INTERFACES_INNERKITS_ASACFWK_INCLUDE_RULES_CHECKE_NODE_H

#include <set>
#include <string>
#include "prop_value.h"

namespace OHOS::Accessibility {

class ReadableRulesNode {
public:

    ReadableRulesNode(int64_t accessibilityId) : accessibilityId_(accessibilityId) {}

    virtual ~ReadableRulesNode(); 

    int64_t GetAccessibilityId()
    {
        return  accessibilityId_;
    }

    virtual bool GetPropText(PropValue& value) = 0;

    virtual bool GetPropHintText(PropValue& value) = 0;

    virtual bool GetPropDesc(PropValue& value) = 0;

    virtual bool GetPropAccessibilityText(PropValue& value) = 0;

    virtual bool GetPropType(PropValue& value) = 0;

    virtual bool GetPropAccessibilityLevel(PropValue& value) = 0;

    virtual bool GetPropAccessibilityGroup(PropValue& value) = 0;

    virtual bool GetPropIsEnable(PropValue& value) = 0;

    virtual bool GetPropChildrenCount(PropValue& value) = 0;

    virtual bool GetPropActionNames(PropValue& value) = 0;

    virtual std::vector<std::shared_ptr<ReadableRulesNode>> GetChildren() = 0;

    virtual std::shared_ptr<ReadableRulesNode> GetParent() = 0;

    virtual bool IsModal()
    { 
        return true;
    }
private:
    int64_t accessibilityId_;

};

} // namespace OHOS::Accessibility

#endif // INTERFACES_INNERKITS_ASACFWK_INCLUDE_RULES_CHECKE_NODE_H