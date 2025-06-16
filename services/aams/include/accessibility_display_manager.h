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

#ifndef ACCESSIBILITY_DISPLAY_MANAGER_H
#define ACCESSIBILITY_DISPLAY_MANAGER_H

#include <memory>
#include "display_manager.h"
#include "display_info.h"
#include "dm_common.h"
#include "event_handler.h"
#include "singleton.h"
#include "magnification_manager.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityDisplayManager {
    DECLARE_SINGLETON(AccessibilityDisplayManager)
public:
    const sptr<Rosen::Display> GetDisplay(uint64_t id);
    std::vector<sptr<Rosen::Display>> GetDisplays();
    const sptr<Rosen::Display> GetDefaultDisplay();
    uint64_t GetDefaultDisplayId();
    int32_t GetDefaultDisplayDpi();
    sptr<Rosen::Display> GetDefaultDisplaySync();
    int32_t GetWidth();
    int32_t GetHeight();
    OHOS::Rosen::DisplayOrientation GetOrientation();
    bool IsFoldable();
    Rosen::FoldDisplayMode GetFoldDisplayMode();
    Rosen::FoldStatus GetFoldStatus();
    void SetDisplayScale(const uint64_t screenId, float scaleX, float scaleY, float pivotX, float pivotY);

    void RegisterDisplayListener(const std::shared_ptr<MagnificationManager> &manager);
    void UnregisterDisplayListener();

    void RegisterDisplayModeListener();
    void UnregisterDisplayModeListener();

private:
    class DisplayListener : public Rosen::DisplayManager::IDisplayListener {
    public:
        explicit DisplayListener(const std::shared_ptr<MagnificationManager> &manager)
            : manager_(manager) {}
        ~DisplayListener() = default;

        virtual void OnCreate(Rosen::DisplayId dId) override {}
        virtual void OnDestroy(Rosen::DisplayId dId) override {}
        virtual void OnChange(Rosen::DisplayId dId) override;

        void OnChangeForWideFold(OHOS::Rosen::DisplayOrientation currentOrientation,
            OHOS::Rosen::FoldDisplayMode currentMode);
        void OnChangeForBigFold(OHOS::Rosen::DisplayOrientation currentOrientation,
            OHOS::Rosen::FoldDisplayMode currentMode);
        void OnChangeDefault(OHOS::Rosen::DisplayOrientation currentOrientation);
        OHOS::Rosen::DisplayOrientation orientation_ = OHOS::Rosen::DisplayOrientation::UNKNOWN;
        OHOS::Rosen::FoldDisplayMode displayMode_ = Rosen::FoldDisplayMode::UNKNOWN;
    private:
        std::shared_ptr<MagnificationManager> manager_ = nullptr;
    };

    class DisplayModeListener : public Rosen::DisplayManager::IDisplayModeListener {
    public:
        explicit DisplayModeListener() {}
        ~DisplayModeListener() = default;

        void OnDisplayModeChanged(Rosen::FoldDisplayMode displayMode);
    };

    sptr<DisplayListener> listener_ = nullptr;
    sptr<DisplayModeListener> displayModeListener_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_DISPLAY_MANAGER_H