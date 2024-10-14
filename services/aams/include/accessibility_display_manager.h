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
    void SetDisplayScale(const uint64_t screenId, float scaleX, float scaleY, float pivotX, float pivotY);

    void RegisterDisplayListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    void UnregisterDisplayListener();

private:
    class DisplayListener : public Rosen::DisplayManager::IDisplayListener {
    public:
        explicit DisplayListener() {}
        ~DisplayListener() = default;

        virtual void OnCreate(Rosen::DisplayId dId) override {}
        virtual void OnDestroy(Rosen::DisplayId dId) override {}
        virtual void OnChange(Rosen::DisplayId dId) override {}
    };

    sptr<DisplayListener> listener_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_DISPLAY_MANAGER_H