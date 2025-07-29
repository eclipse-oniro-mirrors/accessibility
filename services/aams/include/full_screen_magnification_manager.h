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

#ifndef FULL_SCREEN_MAGNIFICATION_MANAGER_H
#define FULL_SCREEN_MAGNIFICATION_MANAGER_H

#include "hilog_wrapper.h"
#include "magnification_def.h"
#include "ffrt.h"
#include "magnification_window_proxy.h"

namespace OHOS {
namespace Accessibility {

class FullScreenMagnificationManager {
public:
    FullScreenMagnificationManager();
    explicit FullScreenMagnificationManager(std::shared_ptr<MagnificationWindowProxy> proxy);
    ~FullScreenMagnificationManager() = default;

    void EnableMagnification(int32_t centerX, int32_t centerY);
    void ShowMagnification();
    void ShowMagnificationWithPosition(PointerPos pos);
    void DisableMagnification(bool needClear = false);
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void RefreshWindowParam(RotationType type);
    void SetScale(float scaleSpan);
    void MoveMagnification(int32_t deltaX, int32_t deltaY);
    void PersistScale();
    PointerPos ConvertGesture(uint32_t type, PointerPos coordinates);
    uint32_t CheckTapOnHotArea(int32_t posX, int32_t posY);
    void FollowFocuseElement(int32_t centerX, int32_t centerY);
    PointerPos GetSourceCenter();
    bool IsMagnificationWindowShow();
    float GetScale();
private:
    ffrt::mutex mutex_;
    std::shared_ptr<MagnificationWindowProxy> windowProxy_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // FULL_SCREEN_MAGNIFICATION_MANAGER_H