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

#include <string>

namespace OHOS {
namespace Accessibility {

struct PointerPos {
    int32_t posX;
    int32_t posY;
};

enum class AnnounceType : uint32_t {
    ANNOUNCE_MAGNIFICATION_SCALE,
    ANNOUNCE_MAGNIFICATION_DISABLE,
    ANNOUNCE_SWITCH_FULL_SCREEN,
    ANNOUNCE_SWITCH_WINDOW
};

constexpr float EPS = 1e-6;
constexpr float DEFAULT_SCALE = 2.0f;
constexpr float MAX_SCALE = 8.0f;
constexpr uint8_t BG_ALPHA = 254;
constexpr float CORNER_RADIUS = 75.0f;
constexpr float MENU_CORNER_RADIUS = 25.0f;
constexpr float PEN_WIDTH = 15.0f;
constexpr int32_t DIVISOR_TWO = 2;
constexpr int32_t ROUND_RECT_MARGIN = 5;
constexpr int32_t ANCHOR_OFFSET = 20;
constexpr float HALF = 0.5f;
constexpr float MIN_SCALE_DISTANCE = 200.0f;
constexpr int32_t SCROLL_SHOT_POINTER_ID = 20001;
constexpr int32_t DEFAULT_POINTER_ID = 0;
constexpr uint32_t TAP_COUNT_TWO = 2;

constexpr uint32_t MAGNIFY_WINDOW_WIDTH = 940;
constexpr uint32_t MAGNIFY_WINDOW_HEIGHT = 550;
constexpr uint32_t BAR_MARGIN = 40;
constexpr uint32_t HOT_AREA_WIDTH = 60;
constexpr int32_t GESTURE_OFFSET = 100;
constexpr int32_t BAR_START = 400;
constexpr int32_t BAR_END = 550;

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