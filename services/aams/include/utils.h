/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_UTILS_H
#define ACCESSIBILITY_UTILS_H

#include "accessibility_ability_info.h"
#include "element_name.h"
#include "extension_ability_info.h"

namespace OHOS {
namespace Accessibility {
enum class TraceTaskId : int32_t {
    ACCESSIBLE_ABILITY_CONNECT = 0,
};

enum class A11yUnavailableEvent : uint32_t {
    READ_EVENT,
    CONNECT_EVENT,
    QUERY_EVENT,
};

enum class A11yError : uint32_t {
    ERROR_NEED_REPORT_BASE = 1000, // error code > 1000 means need report
    ERROR_READ_FAILED,
    ERROR_CONNECT_A11Y_APPLICATION_FAILED,
    ERROR_A11Y_APPLICATION_DISCONNECT_ABNORMALLY,
    ERROR_CONNECT_TARGET_APPLICATION_FAILED,
    ERROR_TARGET_APPLICATION_DISCONNECT_ABNORMALLY,
    ERROR_QUERY_WINDOW_INFO_FAILED,
    ERROR_QUERY_PACKAGE_INFO_FAILED,
    ERROR_NEED_REPORT_END,
};

class Utils {
public:
    static void Parse(const AppExecFwk::ExtensionAbilityInfo &abilityInfo, AccessibilityAbilityInitParams &initParams);
    static int64_t GetSystemTime();
    static std::string GetUri(const OHOS::AppExecFwk::ElementName &elementName);
    static std::string GetUri(const std::string &bundleName, const std::string &abilityName);
    static void RecordUnavailableEvent(A11yUnavailableEvent event, A11yError errCode,
        const std::string &bundleName = "", const std::string &abilityName = "");
    static void RecordStartingA11yEvent(uint32_t flag);
    static void RecordStartingA11yEvent(const std::string &name);

private:
    static std::string TransferUnavailableEventToString(A11yUnavailableEvent type);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_UTILS_H