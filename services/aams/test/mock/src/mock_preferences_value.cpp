/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "preferences_value.h"

namespace OHOS {
namespace NativePreferences {
PreferencesValue::PreferencesValue(PreferencesValue &&preferencesValue) noexcept
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    if (this == &preferencesValue) {
        return;
    }
    value_ = std::move(preferencesValue.value_);
}

PreferencesValue::PreferencesValue(const PreferencesValue &preferencesValue)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    if (this == &preferencesValue) {
        return;
    }
    value_ = preferencesValue.value_;
}

PreferencesValue::PreferencesValue(int64_t value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(int value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(double value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(float value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(const char *value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    PreferencesValue((std::string)value);
}

PreferencesValue::PreferencesValue(bool value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(std::vector<double> value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(std::string value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue::PreferencesValue(std::vector<bool> value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}


PreferencesValue::PreferencesValue(std::vector<std::string> value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue PreferencesValue";
    value_ = value;
}

PreferencesValue &PreferencesValue::operator=(const PreferencesValue &preferencesValue)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue operator=";
    if (this == &preferencesValue) {
        return *this;
    }
    value_ = preferencesValue.value_;
    return *this;
}

PreferencesValue &PreferencesValue::operator=(PreferencesValue &&preferencesValue) noexcept
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue operator=";
    if (this == &preferencesValue) {
        return *this;
    }
    value_ = std::move(preferencesValue.value_);
    return *this;
}

bool PreferencesValue::IsInt() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsInt";
    return std::holds_alternative<int>(value_);
}

bool PreferencesValue::IsLong() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsLong";
    return std::holds_alternative<int64_t>(value_);
}

bool PreferencesValue::IsDouble() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsDouble";
    return std::holds_alternative<double>(value_);
}

bool PreferencesValue::IsFloat() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsFloat";
    return std::holds_alternative<float>(value_);
}

bool PreferencesValue::IsString() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsString";
    return std::holds_alternative<std::string>(value_);
}

bool PreferencesValue::IsBool() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsBool";
    return std::holds_alternative<bool>(value_);
}

bool PreferencesValue::IsDoubleArray() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsDoubleArray";
    return std::holds_alternative<std::vector<double>>(value_);
}

bool PreferencesValue::IsBoolArray() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsBoolArray";
    return std::holds_alternative<std::vector<bool>>(value_);
}

bool PreferencesValue::IsStringArray() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue IsStringArray";
    return std::holds_alternative<std::vector<std::string>>(value_);
}

PreferencesValue::operator int() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue int";
    return std::get<int>(value_);
}

PreferencesValue::operator int64_t() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue int64_t";
    return std::get<int64_t>(value_);
}

PreferencesValue::operator double() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue double";
    return std::get<double>(value_);
}

PreferencesValue::operator float() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue float";
    return std::get<float>(value_);
}

PreferencesValue::operator std::string() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue string";
    return std::get<std::string>(value_);
}

PreferencesValue::operator bool() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue bool";
    return std::get<bool>(value_);
}

PreferencesValue::operator std::vector<double>() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue std::vector<double>";
    return std::get<std::vector<double>>(value_);
}

PreferencesValue::operator std::vector<std::string>() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue std::vector<std::string>";
    return std::get<std::vector<std::string>>(value_);
}

PreferencesValue::operator std::vector<bool>() const
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue std::vector<bool>";
    return std::get<std::vector<bool>>(value_);
}

bool PreferencesValue::operator==(const PreferencesValue &value)
{
    GTEST_LOG_(INFO) << "MOCK PreferencesValue operator==";
    return (this->value_ == value.value_);
}
} // End of namespace NativePreferences
} // End of namespace OHOS
