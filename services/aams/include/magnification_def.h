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

#ifndef MAGNIFICATION_MANAGER_DEF_H
#define MAGNIFICATION_MANAGER_DEF_H

namespace OHOS {
namespace Accessibility {

struct PointerPos {
    int32_t posX;
    int32_t posY;
};

constexpr float EPS = 1e-6;
constexpr float DEFAULT_SCALE = 2.0f;
constexpr float MAX_SCALE = 8.0f;
constexpr uint8_t BG_ALPHA = 254;
constexpr float CORNER_RADIUS = 75.0f;
constexpr float MENU_CORNER_RADIUS = 25.0f;
constexpr float PEN_WIDTH = 10.0f;
constexpr int32_t DIVISOR_TWO = 2;
constexpr int32_t ROUND_RECT_MARGIN = 5;
constexpr int32_t ANCHOR_OFFSET = 20;
constexpr float HALF = 0.5f;
constexpr float MIN_SCALE_DISTANCE = 200.0f;

const std::string WINDOW_NAME = "magnification_window";
const std::string MENU_NAME = "magnification_menu";

constexpr uint32_t FULL_SCREEN_MAGNIFICATION = 1;
constexpr uint32_t WINDOW_MAGNIFICATION = 2;
constexpr uint32_t SWITCH_MAGNIFICATION = 3;
constexpr uint32_t INPUT_METHOD_WINDOW_TYPE = 2105;
constexpr uint32_t ORANGE_COLOR = 0xFFFFA500;

constexpr uint32_t INVALID_GESTURE_TYPE = 0;
constexpr uint32_t LEFT_BACK_GESTURE = 1; // Swipe from the left side of the screen inward
constexpr uint32_t RIGHT_BACK_GESTURE = 2; // Swipe from the right side of the screen inward
constexpr uint32_t BOTTOM_BACK_GESTURE = 3; // Swipe from the bottom side of the screen inward

} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_MANAGER_DEF_H