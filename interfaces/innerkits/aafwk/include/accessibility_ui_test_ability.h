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
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_extension_context.h"
#include "accessibility_ui_test_ability_listener.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityUITestAbility final : public AccessibilityExtensionContext {
public:
    ~AccessibilityUITestAbility() = default;

    /**
     * @brief Gets an instance of AccessibleAbility.
     * @param
     * @return Return an instance of AccessibleAbility.
     */
    static AccessibilityUITestAbility& GetInstance();

    /**
     * @brief RegisterListener
     * @param
     * @return
     */
    bool RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener);

    /**
     * @brief Connect to AAMS.
     * @param
     * @return
     */
    bool Connect();

    /**
     * @brief disconnect to AAMS.
     * @param
     * @return
     */
    bool Disconnect();

private:
    AccessibilityUITestAbility();

    void GetService();

    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
    sptr<AccessibleAbilityManagerServiceClientProxy> serviceProxy_ = nullptr;
    static std::shared_ptr<AccessibilityUITestAbility> instance_;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_UI_TEST_ABILITY_H