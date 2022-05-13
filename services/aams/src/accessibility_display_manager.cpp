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
AccessibilityDisplayManager::AccessibilityDisplayManager()
{
}

AccessibilityDisplayManager::~AccessibilityDisplayManager()
{
    UnregisterDisplayListener();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDisplay(int32_t id)
{
    return Rosen::DisplayManager::GetInstance().GetDisplayById((Rosen::DisplayId)id);
}

std::vector<sptr<Rosen::Display>> AccessibilityDisplayManager::GetDisplays()
{
    return Rosen::DisplayManager::GetInstance().GetAllDisplays();
}

const sptr<Rosen::Display> AccessibilityDisplayManager::GetDefaultDisplay()
{
    return Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
}

void AccessibilityDisplayManager::RegisterDisplayListener(
    const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (listener_) {
        HILOG_DEBUG("Display listener is already registed!");
        return;
    }
    handler_ = handler;
    listener_ = new DisplayListener();
    Rosen::DisplayManager::GetInstance().RegisterDisplayListener(listener_);
}

void AccessibilityDisplayManager::UnregisterDisplayListener()
{
    if (listener_) {
        Rosen::DisplayManager::GetInstance().UnregisterDisplayListener(listener_);
        listener_ = nullptr;
        handler_ = nullptr;
    }
}
} // namespace Accessibility
} // namespace OHOS