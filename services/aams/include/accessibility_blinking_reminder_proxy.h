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

#ifndef ACCESSIBILITY_BLINKING_REMINDER_PROXY_H
#define ACCESSIBILITY_BLINKING_REMINDER_PROXY_H

#include "singleton.h"
#include "accessibility_def.h"
#include "ffrt.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityBlinkingReminderProxy {
    DECLARE_SINGLETON(AccessibilityBlinkingReminderProxy);

public:
    void SetFlashReminderSwitch(bool state);
    void SetFlashReminderMode(int32_t mode);
    void SetFlashReminderFunctionEnabled(const std::string &enabled);
    void SetFlashReminderUnlock(bool state);
    BlinkResultCode StartBlinking(int32_t mode, int32_t scenario);
    BlinkResultCode StopBlinking(int32_t mode, int32_t scenario);

private:
    void LoadLibrary();
    void UnLoadLibrary();

    using SetFlashReminderSwitchFunc = void (*)(bool);
    using SetFlashReminderModeFunc = void (*)(int32_t);
    using SetFlashReminderFunctionEnabledFunc = void (*)(const char *);
    using SetFlashReminderUnlockFunc = void (*)(bool);
    using StartBlinkingFunc = BlinkResultCode (*)(int32_t, int32_t);
    using StopBlinkingFunc = BlinkResultCode (*)(int32_t, int32_t);

    SetFlashReminderSwitchFunc setSwitchFunc_ = nullptr;
    SetFlashReminderModeFunc setModeFunc_ = nullptr;
    SetFlashReminderFunctionEnabledFunc setEnabledFunc_ = nullptr;
    SetFlashReminderUnlockFunc setUnlockFunc_ = nullptr;
    StartBlinkingFunc startBlinkingFunc_ = nullptr;
    StopBlinkingFunc stopBlinkingFunc_ = nullptr;
    void *handle_ = nullptr;
    bool loadFlag_ = false;
    ffrt::mutex lock_;
};

} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_BLINKING_REMINDER_PROXY_H
