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

#ifndef MAGNIFICATION_MENU_MANAGER_H
#define MAGNIFICATION_MENU_MANAGER_H

#include "hilog_wrapper.h"
#include "ffrt.h"
#include "magnification_window_proxy.h"

namespace OHOS {
namespace Accessibility {

class MagnificationMenuManager {
public:
    MagnificationMenuManager();
    explicit MagnificationMenuManager(std::shared_ptr<MagnificationWindowProxy> proxy);
    ~MagnificationMenuManager() = default;

    void ShowMenuWindow(uint32_t mode);
    void DisableMenuWindow();
    bool IsTapOnMenu(int32_t posX, int32_t posY);
    void SetCurrentType(uint32_t type);
    void OnMenuTap();
    void MoveMenuWindow(int32_t deltaX, int32_t deltaY);
    void AttachToEdge();
    void RefreshWindowParam();

private:
    ffrt::mutex mutex_;
    std::shared_ptr<MagnificationWindowProxy> windowProxy_ = nullptr;
};

} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_MENU_MANAGER_H