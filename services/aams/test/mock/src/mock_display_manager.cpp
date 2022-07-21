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

#include <cinttypes>
#include "display_manager.h"
#include "display.h"
#include "dm_common.h"
#include "singleton_delegator.h"
#include "window/window_manager/utils/include/display_info.h"

namespace OHOS::Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManager)
class DisplayManager::Impl : public RefBase {
    friend class DisplayManager;

private:
    bool CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const;
    bool CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const;
    void ClearDisplayStateCallback();
};

bool DisplayManager::Impl::CheckRectValid(const Media::Rect& rect, int32_t oriHeight, int32_t oriWidth) const
{
    return true;
}

bool DisplayManager::Impl::CheckSizeValid(const Media::Size& size, int32_t oriHeight, int32_t oriWidth) const
{
    return true;
}

void DisplayManager::Impl::ClearDisplayStateCallback()
{}

DisplayManager::DisplayManager()
{}

DisplayManager::~DisplayManager()
{}

DisplayId DisplayManager::GetDefaultDisplayId()
{
    DisplayId id {0};
    return id;
}

sptr<Display> DisplayManager::GetDisplayById(DisplayId displayId)
{
    sptr<Display> display = new Display("display_mock", nullptr);
    return display;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(DisplayId displayId)
{
    return nullptr;
}

std::shared_ptr<Media::PixelMap> DisplayManager::GetScreenshot(
    DisplayId displayId, const Media::Rect& rect, const Media::Size& size, int rotation)
{
    return nullptr;
}

sptr<Display> DisplayManager::GetDefaultDisplay()
{
    return GetDisplayById(GetDefaultDisplayId());
}

std::vector<DisplayId> DisplayManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res = {};
    return res;
}

std::vector<sptr<Display>> DisplayManager::GetAllDisplays()
{
    std::vector<sptr<Display>> res;
    return res;
}

bool DisplayManager::RegisterDisplayListener(sptr<IDisplayListener> listener)
{
    return true;
}

bool DisplayManager::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    return true;
}

bool DisplayManager::RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    return true;
}

bool DisplayManager::UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener)
{
    return true;
}

bool DisplayManager::WakeUpBegin(PowerStateChangeReason reason)
{
    return true;
}

bool DisplayManager::WakeUpEnd()
{
    return true;
}

bool DisplayManager::SuspendBegin(PowerStateChangeReason reason)
{
    return true;
}

bool DisplayManager::SuspendEnd()
{
    return true;
}

bool DisplayManager::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    return true;
}

DisplayState DisplayManager::GetDisplayState(DisplayId displayId)
{
    DisplayState state = DisplayState::UNKNOWN;
    return state;
}

bool DisplayManager::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    return true;
}

uint32_t DisplayManager::GetScreenBrightness(uint64_t screenId) const
{
    return 0;
}

void DisplayManager::NotifyDisplayEvent(DisplayEvent event)
{}
} // namespace OHOS::Rosen