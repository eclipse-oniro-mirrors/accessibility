/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "singleton.h"

namespace OHOS {
namespace Accessibility {

void GetDisplayFromWms(std::vector<WMDisplayInfo> &displays) {
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<IWindowManagerService> wms = aams->GetWindowMgrProxy();
    WMError err = wms->GetDisplays(displays);
    if (err != WM_OK) {
        HILOG_ERROR("GetDisplayFromWms failed");
    }
}

AccessibilityDisplayManager::AccessibilityDisplayManager() {
    GetDisplayFromWms(displays_);
}

AccessibilityDisplayManager &AccessibilityDisplayManager::GetInstance() {
    static AccessibilityDisplayManager displayMgr;
    return displayMgr;
}

WMDisplayInfo AccessibilityDisplayManager::GetDisplay(int id) {
    for (auto display : displays_) {
        if (display.id == id) {
            return display;
        }
    }

    HILOG_ERROR("Invalid display id!! Return default display");
    return displays_.front();
}

std::vector<WMDisplayInfo> AccessibilityDisplayManager::GetDisplays() {
    return displays_;
}

WMDisplayInfo AccessibilityDisplayManager::GetDefaultDisplay() {
    return displays_.front();
}

void AccessibilityDisplayManager::RegisterDisplayChangeListener() {
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<IWindowManagerService> wms = aams->GetWindowMgrProxy();
    wms->AddDisplayChangeListener(this);
}

void AccessibilityDisplayManager::OnScreenPlugin(int32_t did) {
    // TODO: Do something for zoom
    displays_.clear();
    GetDisplayFromWms(displays_);
}

void AccessibilityDisplayManager::OnScreenPlugout(int32_t did) {
    // TODO: Do something for zoom
    displays_.clear();
    GetDisplayFromWms(displays_);
}

void AccessibilityDisplayManager::SetDisplay(WMDisplayInfo newWM) {
    displays_.clear();
    displays_.push_back(newWM);
}

}  // namespace Accessibility
}  // namespace OHOS