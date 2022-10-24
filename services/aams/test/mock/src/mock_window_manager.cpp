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

#include "window_manager.h"

namespace OHOS {
namespace Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManager)

class WindowManager::Impl {
};

WindowManager::WindowManager() : pImpl_(std::make_unique<Impl>()) {}

bool WindowManager::RegisterWindowUpdateListener(const sptr<IWindowUpdateListener> &listener)
{
    return true;
}

bool WindowManager::UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener)
{
    return true;
}

WMError WindowManager::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    sptr<AccessibilityWindowInfo> winInfo = new(std::nothrow) AccessibilityWindowInfo();
    infos.emplace_back(winInfo);
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS