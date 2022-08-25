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

#ifndef MOCK_PREFERENCES_H
#define MOCK_PREFERENCES_H

#include "preferences.h"

namespace OHOS {
namespace NativePreferences {
class MockPreferences : public Preferences {
public:
    explicit MockPreferences();
    ~MockPreferences() override;

    PreferencesValue Get(const std::string &key, const PreferencesValue &defValue) override;

    int Put(const std::string &key, const PreferencesValue &value) override;

    int GetInt(const std::string &key, const int &defValue = {}) override;

    std::string GetString(const std::string &key, const std::string &defValue = {}) override;

    bool GetBool(const std::string &key, const bool &defValue = {}) override;

    float GetFloat(const std::string &key, const float &defValue = {}) override;

    double GetDouble(const std::string &key, const double &defValue = {}) override;

    int64_t GetLong(const std::string &key, const int64_t &defValue = {}) override;

    std::map<std::string, PreferencesValue> GetAll() override;

    bool HasKey(const std::string &key) override;

    int PutInt(const std::string &key, int value) override;

    int PutString(const std::string &key, const std::string &value) override;

    int PutBool(const std::string &key, bool value) override;

    int PutLong(const std::string &key, int64_t value) override;

    int PutFloat(const std::string &key, float value) override;

    int PutDouble(const std::string &key, double value) override;

    int Delete(const std::string &key) override;

    int Clear() override;

    void Flush() override;

    int FlushSync() override;

    void RegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;

    void UnRegisterObserver(std::shared_ptr<PreferencesObserver> preferencesObserver) override;
};
} // End of namespace NativePreferences
} // End of namespace OHOS
#endif // End of #ifndef PREFERENCES_H
