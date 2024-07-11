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

#include "accessibility_short_key.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t SUBSCRIBE_ID_1 = 1;
    constexpr int32_t SUBSCRIBE_ID_2 = 2;
    constexpr int32_t KEYCODE_VOLUME_UP = 0;
    constexpr int32_t KEYCODE_VOLUME_DOWN = 1;
    constexpr int32_t SHORTCUT_TIMEOUT = 3000;
}

AccessibilityShortKey::~AccessibilityShortKey()
{
    HILOG_DEBUG("start.");
    subscribeIds_.clear();
}

void AccessibilityShortKey::SubscribeShortKey(std::set<int32_t> preKeys, int32_t finalKey, int32_t holdTime)
{
    HILOG_DEBUG("start.");
    (void)preKeys;
    (void)finalKey;
    (void)holdTime;
    subscribeIds_.emplace_back(SUBSCRIBE_ID_1);
    subscribeIds_.emplace_back(SUBSCRIBE_ID_2);
}

void AccessibilityShortKey::Register()
{
    HILOG_DEBUG("start.");
    std::set<int32_t> preDownKeysUp;
    preDownKeysUp.insert(KEYCODE_VOLUME_UP);
    SubscribeShortKey(preDownKeysUp, KEYCODE_VOLUME_DOWN, SHORTCUT_TIMEOUT);
}

void AccessibilityShortKey::OnShortKey()
{
    HILOG_DEBUG("start.");
}
} // namespace Accessibility
} // namespace OHOS