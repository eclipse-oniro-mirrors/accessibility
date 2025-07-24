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

#include "hilog_wrapper.h"
#include "magnification_manager.h"

namespace OHOS {
namespace Accessibility {
MagnificationManager::MagnificationManager()
{
}

std::shared_ptr<WindowMagnificationManager> MagnificationManager::GetWindowMagnificationManager()
{
    return nullptr;
}

std::shared_ptr<FullScreenMagnificationManager> MagnificationManager::GetFullScreenMagnificationManager()
{
    return nullptr;
}

std::shared_ptr<MagnificationMenuManager> MagnificationManager::GetMenuManager()
{
    return nullptr;
}

void MagnificationManager::OnMagnificationTypeChanged(uint32_t magnificationType)
{
    (void)magnificationType;
}

void MagnificationManager::OnModeChanged(uint32_t mode)
{
    (void)mode;
}

void MagnificationManager::DisableMagnification()
{
}

void MagnificationManager::TriggerMagnification(uint32_t type, uint32_t mode)
{
    (void)type;
    (void)mode;
}

bool MagnificationManager::GetMagnificationState()
{
    return false;
}

void MagnificationManager::RefreshWindowParam()
{
}

void MagnificationManager::FollowFocuseElement(int32_t centerX, int32_t centerY)
{
    (void)centerX;
    (void)centerY;
}
} // namespace Accessibility
} // namespace OHOS