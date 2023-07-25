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

#include <gtest/gtest.h>
#include "display.h"
#include "window/window_manager/utils/include/display_info.h"

namespace OHOS::Rosen {
namespace {
    const int32_t HEIGHT_VALUE = 1000;
    const int32_t WEIGHT_VALUE = 600;
} // namespace

class Display::Impl : public RefBase {
public:
    Impl(const std::string& name, sptr<DisplayInfo> displayInfo)
    {
        name_ = name;
        displayInfo_ = displayInfo;
    }
    ~Impl() = default;
    DEFINE_VAR_FUNC_GET_SET(std::string, Name, name);
    DEFINE_VAR_FUNC_GET_SET(sptr<DisplayInfo>, DisplayInfo, displayInfo);
};

Display::Display(const std::string& name, sptr<DisplayInfo> info) : pImpl_(new Impl(name, info))
{
    GTEST_LOG_(INFO) << "MOCK Display Display";
}

Display::~Display()
{
    GTEST_LOG_(INFO) << "MOCK Display ~Display";
}

DisplayId Display::GetId() const
{
    GTEST_LOG_(INFO) << "MOCK Display ~Display";
    DisplayId displayId {0};
    return displayId;
}

int32_t Display::GetWidth() const
{
    GTEST_LOG_(INFO) << "MOCK Display GetWidth";
    return WEIGHT_VALUE;
}

int32_t Display::GetHeight() const
{
    GTEST_LOG_(INFO) << "MOCK Display GetHeight";
    return HEIGHT_VALUE;
}

uint32_t Display::GetRefreshRate() const
{
    GTEST_LOG_(INFO) << "MOCK Display GetRefreshRate";
    return 0;
}

ScreenId Display::GetScreenId() const
{
    GTEST_LOG_(INFO) << "MOCK Display GetScreenId";
    ScreenId screenId {0};
    return screenId;
}

void Display::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    GTEST_LOG_(INFO) << "MOCK Display UpdateDisplayInfo";
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