/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_EXT_UTILS_H
#define ACCESSIBILITY_EXT_UTILS_H

#include "wm/window.h"
#include <string>

namespace OHOS {
namespace Accessibility {

class ExtUtils {
public:
    static bool IsInRect(int32_t posX, int32_t posY, Rosen::Rect rect);
    static void RecordMagnificationUnavailableEvent(const std::string &name);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EXT_UTILS_H