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

#include "display.h"
#include "window/window_manager/utils/include/display_info.h"

namespace OHOS::Rosen {
class Display::Impl : public RefBase {
public:
    Impl(const std::string& name, sptr<DisplayInfo> info)
    {
        name_ = name;
        displayInfo_ = info;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET(std::string, Name, name);
    DEFINE_VAR_FUNC_GET_SET(sptr<DisplayInfo>, DisplayInfo, displayInfo);
};

Display::Display(const std::string& name, sptr<DisplayInfo> info) : pImpl_(new Impl(name, info))
{}

Display::~Display()
{}

DisplayId Display::GetId() const
{
    DisplayId id {0};
    return id;
}

int32_t Display::GetWidth() const
{
    return 0;
}

int32_t Display::GetHeight() const
{
    return 0;
}

uint32_t Display::GetRefreshRate() const
{
    return 0;
}

ScreenId Display::GetScreenId() const
{
    ScreenId id {0};
    return id;
}

void Display::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    if (!displayInfo) {
        return;
    }
    pImpl_->SetDisplayInfo(displayInfo);
}

float Display::GetVirtualPixelRatio() const
{
#ifdef PRODUCT_RK
    return 1.0f;
#else
    return 2.0f;
#endif
}
} // namespace OHOS::Rosen