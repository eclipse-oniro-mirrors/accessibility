/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mock_preferences.h"
#include "preferences_helper.h"

namespace OHOS {
namespace NativePreferences {
std::string PreferencesHelper::GetRealPath(const std::string& path, int& errorCode)
{
    (void)path;
    (void)errorCode;
    return "";
}

std::shared_ptr<Preferences> PreferencesHelper::GetPreferences(const std::string& path, int& errCode)
{
    std::shared_ptr<MockPreferences> pref = std::make_shared<MockPreferences>();
    errCode = 0;
    (void)path;
    return pref;
}

int PreferencesHelper::DeletePreferences(const std::string& path)
{
    (void)path;
    return 0;
}

int PreferencesHelper::RemovePreferencesFromCache(const std::string& path)
{
    (void)path;
    return 0;
}
} // namespace NativePreferences
} // namespace OHOS