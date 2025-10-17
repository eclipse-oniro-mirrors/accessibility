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
#include "prop_value.h"

namespace OHOS::Accessibility {

bool PropValue::Compare(CondOperator op, const PropValue& other)
{
    bool result = false;
    if (valueType == ValueType::BOOL) {
        result = CompareBool(op, other);
    }

    if (valueType == ValueType::NUMBER) {
        result = CompareNumber(op, other);
    }

    if (valueType == ValueType::ARRAY) {
        result = CompareArray(op, other);
    }

    if (valueType == ValueType::STRING) {
        result = CompareString(op, other);
    }

    return result;
}

bool PropValue::CompareString(CondOperator op, const PropValue& other)
{
    if (other.valueType == ValueType::ARRAY) {
        if (op == CondOperator::IS_ONEOF) {
            return other.valueArray.find(valueStr) != other.valueArray.end();
        }

        if (op == CondOperator::NOT_ONEOF) {
            return other.valueArray.find(valueStr) == other.valueArray.end();
        }

        return false;
    }

    if (other.valueType == ValueType::STRING) {
        if (op == CondOperator::EQ) {
            return valueStr == other.valueStr;
        }

        if (op == CondOperator::NE) {
            return valueStr != other.valueStr;
        }

        return false;
    }

    return false;
}

bool PropValue::CompareArray(CondOperator op, const PropValue& other)
{
    if (other.valueType != ValueType::ARRAY) {
        return false;
    }

    if (op == CondOperator::HAS_ANY) {
        for (const auto& item : other.valueArray) {
            if (valueArray.find(item) != valueArray.end()) {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool PropValue::CompareNumber(CondOperator op, const PropValue& other)
{
    if (other.valueType != ValueType::NUMBER) {
        return false;
    }

    if (op == CondOperator::EQ) {
        return valueNum == other.valueNum;
    }

    if (op == CondOperator::NE) {
        return valueNum != other.valueNum;
    }

    return false;
}

bool PropValue::CompareBool(CondOperator op, const PropValue& other)
{
    if (other.valueType != ValueType::BOOL) {
        return false;
    }

    if (op == CondOperator::EQ) {
        return valueBool == other.valueBool;
    }

    if (op == CondOperator::NE) {
        return valueBool != other.valueBool;
    }

    return false;
}

void PropValue::Print() const
{
    if (valueType == ValueType::BOOL) {
        return;
    }

    if (valueType == ValueType::STRING) {
        return;
    }

    if (valueType == ValueType::NUMBER) {
        return;
    }

    if (valueType == ValueType::ARRAY) {
        std::string text;
        for (auto str : valueArray) {
            if (!text.empty()) {
                text.append(", ");
            }
            text.append(str);
        }

        return;
    }

    return;
}
} // namespace OHOS::Accessibility
// LCOV_EXCL_STOP