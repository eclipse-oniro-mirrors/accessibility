/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "accessibility_circle_drawing_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
std::shared_ptr<MockAccessibilityCircleDrawingManager> MockAccessibilityCircleDrawingManager::pointDrawMgr_ = nullptr;

std::shared_ptr<MockAccessibilityCircleDrawingManager> MockAccessibilityCircleDrawingManager::GetInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        pointDrawMgr_ = std::make_shared<MockAccessibilityCircleDrawingManager>();
    }
    return pointDrawMgr_;
}

void MockAccessibilityCircleDrawingManager::DeleteInstance()
{
    HILOG_DEBUG();
    if (pointDrawMgr_ == nullptr) {
        HILOG_ERROR("surfaceNode_ is nullptr");
        return;
    }
    pointDrawMgr_ = nullptr;
}

MockAccessibilityCircleDrawingManager::AccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
}

MockAccessibilityCircleDrawingManager::~AccessibilityCircleDrawingManager()
{
    HILOG_DEBUG();
}

void MockAccessibilityCircleDrawingManager::UpdatePointerVisible(bool state)
{
    HILOG_DEBUG();
    (void)state;
}

void MockAccessibilityCircleDrawingManager::DrawPointer(int32_t physicalX, int32_t physicalY, int32_t angle,
    uint64_t screenId, int32_t startAngle)
{
    HILOG_DEBUG();
    (void)physicalX;
    (void)physicalY;
    (void)angle;
    (void)screenId;
    (void)startAngle;
}
} // namespace Accessibility
} // namespace OHOS