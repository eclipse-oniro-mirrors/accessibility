/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_UI_TEST_ABILITY_H
#define ACCESSIBILITY_UI_TEST_ABILITY_H

#include <memory>
#include "accessibility_extension_context.h"
#include "accessibility_ui_test_ability_listener.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityUITestAbility : public AccessibilityExtensionContext {
public:
    /**
     * @brief Destruct
     * @param
     * @return
     */
    virtual ~AccessibilityUITestAbility() = default;

    /**
     * @brief Gets an instance of AccessibleAbility.
     * @param
     * @return Return an instance of AccessibleAbility.
     */
    static std::shared_ptr<AccessibilityUITestAbility> GetInstance();

    /**
     * @brief RegisterListener
     * @param
     * @return
     */
    virtual bool RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener) = 0;

    /**
     * @brief Connect to AAMS.
     * @param
     * @return
     */
    virtual bool Connect() = 0;

    /**
        * @brief disconnect to AAMS.
        * @param
        * @return
        */
    virtual bool Disconnect() = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_UI_TEST_ABILITY_H