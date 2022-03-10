/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "foundation/windowmanager/interfaces/innerkits/wm/window_manager.h"

#include "wm_single_instance.h"
#include "singleton_delegator.h"

namespace OHOS {
namespace Rosen {
bool WindowInfo::Marshalling(Parcel &parcel) const
{
    return true;
}

WindowInfo* WindowInfo::Unmarshalling(Parcel &parcel)
{
    WindowInfo* windowInfo = new WindowInfo();
    return windowInfo;
}

WM_IMPLEMENT_SINGLE_INSTANCE(WindowManager)

class WindowManager::Impl {
public:
    void NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId) const;
    void NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
        WindowType windowType, DisplayId displayId) const;
    void NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints) const;
    void NotifyWindowUpdate(const sptr<AccessibilityWindowInfo>& windowInfo, WindowUpdateType type) const;
    static inline SingletonDelegator<WindowManager> delegator_;

    std::vector<sptr<IWindowUpdateListener>> windowUpdateListeners_;
};

void WindowManager::Impl::NotifyFocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, DisplayId displayId) const
{
}

void WindowManager::Impl::NotifyUnfocused(uint32_t windowId, const sptr<IRemoteObject>& abilityToken,
    WindowType windowType, DisplayId displayId) const
{
}

void WindowManager::Impl::NotifySystemBarChanged(DisplayId displayId, const SystemBarRegionTints& tints) const
{
}

void WindowManager::Impl::NotifyWindowUpdate(const sptr<AccessibilityWindowInfo>& windowInfo,
    WindowUpdateType type) const
{
}

WindowManager::WindowManager() : pImpl_(std::make_unique<Impl>())
{
}

WindowManager::~WindowManager()
{
}

void WindowManager::RegisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
}

void WindowManager::UnregisterFocusChangedListener(const sptr<IFocusChangedListener>& listener)
{
}

void WindowManager::RegisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
}

void WindowManager::UnregisterSystemBarChangedListener(const sptr<ISystemBarChangedListener>& listener)
{
}

void WindowManager::MinimizeAllAppWindows(DisplayId displayId)
{
}
void WindowManager::RegisterWindowUpdateListener(const sptr<IWindowUpdateListener> &listener)
{
}

void WindowManager::UnregisterWindowUpdateListener(const sptr<IWindowUpdateListener>& listener)
{
}

void WindowManager::UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
    DisplayId displayId, bool focused) const
{
}

void WindowManager::UpdateSystemBarRegionTints(DisplayId displayId,
    const SystemBarRegionTints& tints) const
{
}

void WindowManager::UpdateWindowStatus(const sptr<AccessibilityWindowInfo>& windowInfo, WindowUpdateType type)
{
}
} // namespace Rosen
} // namespace OHOS