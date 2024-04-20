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

#ifndef ACCESSIBILITY_SETTING_PROVIDER_H
#define ACCESSIBILITY_SETTING_PROVIDER_H

#include "datashare_helper.h"
#include "errors.h"
#include "mutex"
#include "accessibility_setting_observer.h"
#include "accessibility_datashare_helper.h"
#include "accessibility_caption.h"

namespace OHOS {
namespace Accessibility {
class AccessibilitySettingProvider : public AccessibilityDatashareHelper {
public:
    static AccessibilitySettingProvider& GetInstance(int32_t systemAbilityId);
    AccessibilitySettingProvider();
    void DeleteInstance();
    RetError GetStringValue(const std::string& key, std::string& value);
    RetError GetIntValue(const std::string& key, int32_t& value);
    RetError GetLongValue(const std::string& key, int64_t& value);
    RetError GetBoolValue(const std::string& key, bool& value);
    RetError GetFloatValue(const std::string& key, float& value);
    RetError PutStringValue(const std::string& key, const std::string& value, bool needNotify = true);
    RetError PutIntValue(const std::string& key, int32_t value, bool needNotify = true);
    RetError PutLongValue(const std::string& key, int64_t value, bool needNotify = true);
    RetError PutBoolValue(const std::string& key, bool value, bool needNotify = true);
    bool IsValidKey(const std::string& key);
    sptr<AccessibilitySettingObserver> CreateObserver(const std::string& key,
        AccessibilitySettingObserver::UpdateFunc& func);
    RetError RegisterObserver(const sptr<AccessibilitySettingObserver>& observer);
    RetError UnregisterObserver(const sptr<AccessibilitySettingObserver>& observer);

    RetError RegisterObserver(const std::string& key, AccessibilitySettingObserver::UpdateFunc& func);
    RetError UnregisterObserver(const std::string& key);

private:
    std::string GetConfigKey(int32_t state);

protected:
    ~AccessibilitySettingProvider();

private:
    static AccessibilitySettingProvider* instance_;
    static std::mutex mutex_;
    static std::mutex observerMutex_;
    std::map<std::string, sptr<AccessibilitySettingObserver>> settingObserverMap_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SETTING_PROVIDER_H