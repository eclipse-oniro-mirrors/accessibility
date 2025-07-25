/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef WINDOW_MAGNIFICATION_MANAGER_H
#define WINDOW_MAGNIFICATION_MANAGER_H

#include "hilog_wrapper.h"
#include "magnification_def.h"
#include "ffrt.h"
#include "magnification_window_proxy.h"

namespace OHOS {
namespace Accessibility {

struct RectBound {
    uint32_t width;
    uint32_t height;
};

class WindowMagnificationManager {
public:
    WindowMagnificationManager();
    explicit WindowMagnificationManager(std::shared_ptr<MagnificationWindowProxy> proxy);
    ~WindowMagnificationManager() = default;

    bool IsMagnificationWindowShow();
    float GetScale();
    void EnableWindowMagnification(int32_t centerX, int32_t centerY);
    void ShowWindowMagnification();
    void ShowWindowMagnificationWithPosition(PointerPos pos);
    void DisableWindowMagnification(bool needClear = false);
    void SetScale(float deltaSpan);
    void MoveMagnificationWindow(int32_t deltaX, int32_t deltaY);
    bool IsTapOnHotArea(int32_t posX, int32_t posY);
    bool IsTapOnMagnificationWindow(int32_t posX, int32_t posY);
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void FixSourceCenter(bool needFix);
    void PersistScale();
    void RefreshWindowParam();
    void FollowFocuseElement(int32_t centerX, int32_t centerY);
    PointerPos GetSourceCenter();

private:
    ffrt::mutex mutex_;
    std::shared_ptr<MagnificationWindowProxy> windowProxy_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // WINDOW_MAGNIFICATION_MANAGER_H