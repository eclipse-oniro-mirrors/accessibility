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

#include "accessibility_blinking_reminder_proxy.h"
#include "hilog_wrapper.h"
#include <dlfcn.h>

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string_view blinkingReminderLibrary = "libaccessibleBlinking.z.so";
}

AccessibilityBlinkingReminderProxy::AccessibilityBlinkingReminderProxy()
{
}

AccessibilityBlinkingReminderProxy::~AccessibilityBlinkingReminderProxy()
{
}

void AccessibilityBlinkingReminderProxy::LoadLibrary()
{
    handle_ = dlopen(std::string(blinkingReminderLibrary).c_str(), RTLD_LAZY);
    if (!handle_) {
        HILOG_ERROR("load %{public}s failed, %{public}s", blinkingReminderLibrary.data(), dlerror());
        return;
    }

    setSwitchFunc_ = (SetFlashReminderSwitchFunc)dlsym(handle_, "SetFlashReminderSwitch");
    if (!setSwitchFunc_) {
        HILOG_ERROR("get SetFlashReminderSwitch failed, %{public}s", dlerror());
        return;
    }

    setModeFunc_ = (SetFlashReminderModeFunc)dlsym(handle_, "SetFlashReminderMode");
    if (!setModeFunc_) {
        HILOG_ERROR("get SetFlashReminderMode failed, %{public}s", dlerror());
        return;
    }

    setEnabledFunc_ = (SetFlashReminderFunctionEnabledFunc)dlsym(handle_, "SetFlashReminderFunctionEnabled");
    if (!setEnabledFunc_) {
        HILOG_ERROR("get SetFlashReminderFunctionEnabled failed, %{public}s", dlerror());
        return;
    }

    setUnlockFunc_ = (SetFlashReminderUnlockFunc)dlsym(handle_, "SetFlashReminderUnlock");
    if (!setUnlockFunc_) {
        HILOG_ERROR("get SetFlashReminderUnlock failed, %{public}s", dlerror());
        return;
    }

    startBlinkingFunc_ = (StartBlinkingFunc)dlsym(handle_, "StartBlinking");
    if (!startBlinkingFunc_) {
        HILOG_ERROR("get StartBlinking failed, %{public}s", dlerror());
        return;
    }

    stopBlinkingFunc_ = (StopBlinkingFunc)dlsym(handle_, "StopBlinking");
    if (!stopBlinkingFunc_) {
        HILOG_ERROR("get StopBlinking failed, %{public}s", dlerror());
        return;
    }

    loadFlag_ = true;
    HILOG_INFO("load %{public}s success", blinkingReminderLibrary.data());
}

void AccessibilityBlinkingReminderProxy::UnLoadLibrary()
{
    if (!loadFlag_) {
        return;
    }
    int ret = dlclose(handle_);
    char *error = dlerror();
    if (error) {
        HILOG_ERROR("dlclose error: %{public}s", error);
    }
    handle_ = nullptr;
    setSwitchFunc_ = nullptr;
    setModeFunc_ = nullptr;
    setEnabledFunc_ = nullptr;
    setUnlockFunc_ = nullptr;
    startBlinkingFunc_ = nullptr;
    stopBlinkingFunc_ = nullptr;
    loadFlag_ = false;
    HILOG_INFO("unload %{public}s done, ret=%{public}d", blinkingReminderLibrary.data(), ret);
}

void AccessibilityBlinkingReminderProxy::SetFlashReminderSwitch(bool state)
{
    HILOG_INFO("state=%{public}d", state);
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && setSwitchFunc_) {
        setSwitchFunc_(state);
    }
}

void AccessibilityBlinkingReminderProxy::SetFlashReminderMode(int32_t mode)
{
    HILOG_INFO("mode=%{public}d", mode);
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && setModeFunc_) {
        setModeFunc_(mode);
    }
}

void AccessibilityBlinkingReminderProxy::SetFlashReminderFunctionEnabled(const std::string &enabled)
{
    HILOG_INFO("enabled=%{public}s", enabled.c_str());
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && setEnabledFunc_) {
        setEnabledFunc_(enabled.c_str());
    }
}

void AccessibilityBlinkingReminderProxy::SetFlashReminderUnlock(bool state)
{
    HILOG_INFO("state=%{public}d", state);
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && setUnlockFunc_) {
        setUnlockFunc_(state);
    }
}

BlinkResultCode AccessibilityBlinkingReminderProxy::StartBlinking(int32_t mode, int32_t scenario)
{
    HILOG_INFO("mode=%{public}d, scenario=%{public}d", mode, scenario);
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && startBlinkingFunc_) {
        return startBlinkingFunc_(mode, scenario);
    }
    HILOG_ERROR("StartBlinking not available");
    return BlinkResultCode::FEATURE_DISABLE;
}

BlinkResultCode AccessibilityBlinkingReminderProxy::StopBlinking(int32_t mode, int32_t scenario)
{
    HILOG_INFO("mode=%{public}d, scenario=%{public}d", mode, scenario);
    std::lock_guard<ffrt::mutex> lock(lock_);
    if (!loadFlag_) {
        LoadLibrary();
    }
    if (loadFlag_ && stopBlinkingFunc_) {
        return stopBlinkingFunc_(mode, scenario);
    }
    HILOG_ERROR("StopBlinking not available");
    return BlinkResultCode::FEATURE_DISABLE;
}

} // namespace Accessibility
} // namespace OHOS
