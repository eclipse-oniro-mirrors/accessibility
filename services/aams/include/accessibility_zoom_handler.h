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

#ifndef ACCESSIBILITY_ZOOM_HANDLER_H
#define ACCESSIBILITY_ZOOM_HANDLER_H

#include <vector>
#include <mutex>

#include "accessibility_display_manager.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityZoomHandler {
public:
    /**
     * @brief A constructor used to create a zoom handler.
     *
     * @param displayId  The Logical display id.
     * @since 1.0
     * @version 1.0
     */
    explicit AccessibilityZoomHandler(Rosen::DisplayId displayId);

    /**
     * @brief A destructor used to delete the zoom handler.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual ~AccessibilityZoomHandler() = default;

    void OnZoomIn(int32_t centerX, int32_t centerY);
    void OnZoomOut();
    void OnScroll(float offsetX, float offsetY);
    void OnScale(float scaleRatio, float focusX, float focusY);

private:
    float displayCenterX_ = 0.0f;
    float displayCenterY_ = 0.0f;
    float currentCenterX_ = 0.0f;
    float currentCenterY_ = 0.0f;
    float currentScale_ = 1.0f;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ZOOM_HANDLER_H
