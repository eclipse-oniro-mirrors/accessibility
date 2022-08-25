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

#include "mock_preferences.h"
#include "preferences_value.h"

namespace OHOS {
namespace NativePreferences {
MockPreferences::MockPreferences()
{}
MockPreferences::~MockPreferences()
{}
PreferencesValue MockPreferences::Get(const std::string &key, const PreferencesValue &defValue)
{
    (void)key;
    (void)defValue;
    return nullptr;
}
int MockPreferences::Put(const std::string &key, const PreferencesValue &value)
{
    (void)key;
    (void)value;
    return 0;
}
int MockPreferences::GetInt(const std::string &key, const int &defValue)
{
    (void)key;
    (void)defValue;
    return 0;
}

std::string MockPreferences::GetString(const std::string &key, const std::string &defValue)
{
    (void)key;
    (void)defValue;
    return "";
}

bool MockPreferences::GetBool(const std::string &key, const bool &defValue)
{
    (void)key;
    (void)defValue;
    return true;
}

float MockPreferences::GetFloat(const std::string &key, const float &defValue)
{
    (void)key;
    (void)defValue;
    return 0;
}

double MockPreferences::GetDouble(const std::string &key, const double &defValue)
{
    (void)key;
    (void)defValue;
    return 0;
}

int64_t MockPreferences::GetLong(const std::string &key, const int64_t &defValue)
{
    (void)key;
    (void)defValue;
    return 0;
}

bool MockPreferences::HasKey(const std::string &key)
{
    (void)key;
    return true;
}

int MockPreferences::PutInt(const std::string &key, int value)
{
    (void)key;
    (void)value;
    return 0;
}

int MockPreferences::PutString(const std::string &key, const std::string &value)
{
    (void)key;
    (void)value;
    return 0;
}

int MockPreferences::PutBool(const std::string &key, bool value)
{
    (void)key;
    (void)value;
    return 0;
}

int MockPreferences::PutLong(const std::string &key, int64_t value)
{
    (void)key;
    (void)value;
    return 0;
}

int MockPreferences::PutFloat(const std::string &key, float value)
{
    (void)key;
    (void)value;
    return 0;
}

int MockPreferences::PutDouble(const std::string &key, double value)
{
    (void)key;
    (void)value;
    return 0;
}

std::map<std::string, PreferencesValue> MockPreferences::GetAll()
{
    std::map<std::string, PreferencesValue> map_;
    return map_;
}

int MockPreferences::Delete(const std::string &key)
{
    (void)key;
    return 0;
}
int MockPreferences::Clear()
{
    return 0;
}

void MockPreferences::Flush()
{
}

int MockPreferences::FlushSync()
{
    return 0;
}

void MockPreferences::RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver)
{
    (void)preferencesObserver;
}
void MockPreferences::UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver)
{
    (void)preferencesObserver;
}
} // End of namespace NativePreferences
} // End of namespace OHOS
