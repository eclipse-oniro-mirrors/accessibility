/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CJ_ACCESSIBILITY_CALLBACK_H
#define CJ_ACCESSIBILITY_CALLBACK_H

#include <vector>
#include <mutex>
#include <functional>
#include "accessibility_state_event.h"
#include "accessibility_ability_info.h"
#include "accessibility_utils.h"

namespace OHOS {
namespace Accessibility {
class cjAccessibilityStateObserver : public AccessibilityStateObserver {
public:
    cjAccessibilityStateObserver(){};
    void SubscribeObserver(std::function<void(bool)> cbFunc);
    void OnStateChanged(const bool state) override;

private:
    std::mutex cbMutex_;
    std::vector<std::function<void(bool)>> observers_;
};

static std::shared_ptr<cjAccessibilityStateObserver> cjAccessibilityStateListeners_ =
    std::make_shared<cjAccessibilityStateObserver>();
static std::shared_ptr<cjAccessibilityStateObserver> cjTouchGuideStateListeners_ =
    std::make_shared<cjAccessibilityStateObserver>();
} // namespace Accessibility
} // namespace OHOS
#endif // CJ_ACCESSIBILITY_CALLBACK_H