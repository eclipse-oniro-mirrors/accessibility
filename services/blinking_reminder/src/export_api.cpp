/*
 * Copyright (C) 2026-2026 Huawei Device Co., Ltd.
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

#include "accessible_blinking_reminder_manager.h"

#define API_EXPORT __attribute__((visibility("default")))

extern "C" {
API_EXPORT void SetFlashReminderSwitch(bool state)
{
    OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .SetFlashReminderSwitch(state);
}

API_EXPORT void SetFlashReminderMode(int32_t mode)
{
    OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .SetFlashReminderMode(mode);
}

API_EXPORT void SetFlashReminderFunctionEnabled(const char* enabled)
{
    std::string enabledStr(enabled ? enabled : "");
    OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .SetFlashReminderFunctionEnabled(enabledStr);
}

API_EXPORT void SetFlashReminderUnlock(bool state)
{
    OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .SetFlashReminderUnlock(state);
}

API_EXPORT OHOS::Accessibility::BlinkResultCode StartBlinking(int32_t mode, int32_t scenario)
{
    return OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .StartBlinking(mode, scenario);
}

API_EXPORT OHOS::Accessibility::BlinkResultCode StopBlinking(int32_t mode, int32_t scenario)
{
    return OHOS::Singleton<OHOS::Accessibility::AccessibleBlinkingReminderManager>::GetInstance()
        .StopBlinking(mode, scenario);
}
}
