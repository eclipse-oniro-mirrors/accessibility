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

#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

constexpr int32_t DEFAULT_DPI = 540;

AccessibilityDisplayManager::AccessibilityDisplayManager()
{
}

AccessibilityDisplayManager::~AccessibilityDisplayManager()
{
    UnregisterDisplayListener();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDisplay(uint64_t id)
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDisplayById(id);
}

std::vector<sptr<Rosen::Display>> AccessibilityDisplayManager::GetDisplays()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetAllDisplays();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDefaultDisplay()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
}

uint64_t AccessibilityDisplayManager::GetDefaultDisplayId()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
}

int32_t AccessibilityDisplayManager::GetDefaultDisplayDpi()
{
    HILOG_DEBUG();
    if (GetDefaultDisplay() == nullptr) {
        HILOG_ERROR("default display is null");
        return DEFAULT_DPI;
    }

    return GetDefaultDisplay()->GetDpi();
}

bool AccessibilityDisplayManager::IsFoldable()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().IsFoldable();
}

Rosen::FoldDisplayMode AccessibilityDisplayManager::GetFoldDisplayMode()
{
    HILOG_DEBUG();
    return Rosen::DisplayManager::GetInstance().GetFoldDisplayMode();
}

void AccessibilityDisplayManager::RegisterDisplayListener(
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    HILOG_DEBUG();
    if (listener_) {
        HILOG_DEBUG("Display listener is already registed!");
        return;
    }
    handler_ = handler;
    listener_ = new(std::nothrow) DisplayListener();
    if (!listener_) {
        HILOG_ERROR("Create display listener fail!");
        return;
    }
    Rosen::DisplayManager::GetInstance().RegisterDisplayListener(listener_);
}

void AccessibilityDisplayManager::UnregisterDisplayListener()
{
    HILOG_DEBUG();
    if (listener_) {
        Rosen::DisplayManager::GetInstance().UnregisterDisplayListener(listener_);
        listener_ = nullptr;
        handler_ = nullptr;
    }
}
} // namespace Accessibility
} // namespace OHOS