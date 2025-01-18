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
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManager)

WMError WindowManager::RegisterWindowUpdateListener(const sptr<IWindowUpdateListener> &listener)
{
    return WMError::WM_OK;
}

WMError WindowManager::UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener)
{
    return WMError::WM_OK;
}

WMError WindowManager::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    sptr<AccessibilityWindowInfo> winInfo = new(std::nothrow) AccessibilityWindowInfo();
    infos.emplace_back(winInfo);
    return WMError::WM_OK;
}

void WindowManager::GetFocusWindowInfo(FocusChangeInfo &info, DisplayId displayId)
{
    info.windowId_ = 1;
}
} // namespace Rosen
} // namespace OHOS