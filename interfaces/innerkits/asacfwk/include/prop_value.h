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

#ifndef INTERFACES_INNERKITS_ASACFWK_INCLUDE_PROP_VALUE_H
#define INTERFACES_INNERKITS_ASACFWK_INCLUDE_PROP_VALUE_H

#include <set>
#include <string>

namespace OHOS::Accessibility {

enum class ValueType {
    UNKNOWN = -1,
    BOOL,
    STRING,
    ARRAY,
    NUMBER
};

enum class CondOperator {
    UNKNOWN = -1,
    EQ,
    NE,
    NOT_ONEOF,
    IS_ONEOF,
    HAS_ANY,
};

class PropValue {
public:
    bool Compare(CondOperator op, const PropValue& other);

    void Print() const;

    ValueType valueType = ValueType::UNKNOWN;
    std::set<std::string> valueArray;
    std::string valueStr;
    int32_t valueNum = 0;
    bool valueBool = false;

private:
    bool CompareBool(CondOperator op, const PropValue& other);
    bool CompareNumber(CondOperator op, const PropValue& other);
    bool CompareArray(CondOperator op, const PropValue& other);
    bool CompareString(CondOperator op, const PropValue& other);
};

} // namespace OHOS::Accessibility

#endif // INTERFACES_INNERKITS_ASACFWK_INCLUDE_PROP_VALUE_H