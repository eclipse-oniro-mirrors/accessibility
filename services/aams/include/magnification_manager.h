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

#ifndef MAGNIFICATION_MANAGER_H
#define MAGNIFICATION_MANAGER_H

#include "window_magnification_manager.h"
#include "full_screen_magnification_manager.h"

namespace OHOS {
namespace Accessibility {
class MagnificationManager {
public:
    MagnificationManager();
    ~MagnificationManager() = default;

    std::shared_ptr<WindowMagnificationManager> GetWindowMagnificationManager();
    std::shared_ptr<FullScreenMagnificationManager> GetFullScreenMagnificationManager();
    void OnMagnificationTypeChanged(uint32_t magnificationType);
    void OnModeChanged(uint32_t mode);
    void DisableMagnification();
    void TriggerMagnification(uint32_t type, uint32_t mode);
    bool GetMagnificationState();
    void RefreshWindowParam();
    inline void ResetCurrentMode()
    {
        currentMode_ = 0;
    }
private:
    std::shared_ptr<WindowMagnificationManager> windowMagnificationManager_ = nullptr;
    std::shared_ptr<FullScreenMagnificationManager> fullScreenMagnificationManager_ = nullptr;
    uint32_t currentMode_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_MANAGER_H