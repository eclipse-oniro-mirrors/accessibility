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

#include "custom_props.h"

namespace OHOS::Accessibility {

bool CustomProps::Parse(const nlohmann::json& jsonRoot)
{
    nlohmann::json customProps = jsonRoot["custom_props"];
    customProps_.clear();
    for (auto& [key, value] : customProps.items()) {
        if (!customProps_[key].Parse(value)) {
            return false;
        }
    }
    return true;
}

const Condition* CustomProps::GetCondition(const std::string name) const
{
    auto findResult = customProps_.find(name);
    if (findResult == customProps_.end()) {
        return nullptr;
    }

    return &findResult->second;
}

} // namespace OHOS::Accessibility