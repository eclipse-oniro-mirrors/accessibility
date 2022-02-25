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

#ifndef ACCESSIBILITY_UI_TEST_ABILITY_IMPL_H
#define ACCESSIBILITY_UI_TEST_ABILITY_IMPL_H

#include "accessibility_ui_test_ability.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityUITestAbilityImpl final : public AccessibilityUITestAbility {
public:
    /**
     * @brief Construct
     * @param
     * @return
     */
    AccessibilityUITestAbilityImpl();

    /**
     * @brief Destruct
     * @param
     * @return
     */
    ~AccessibilityUITestAbilityImpl() = default;

    /**
     * @brief RegisterListener
     * @param
     * @return
     */
    virtual bool RegisterListener(const std::shared_ptr<IAccessibleUITestAbilityListener> &listener) override;

    /**
     * @brief Connect to AAMS.
     * @param
     * @return
     */
    virtual bool Connect() override;

    /**
     * @brief disconnect to AAMS.
     * @param
     * @return
     */
    virtual bool Disconnect() override;
private:
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
    sptr<AccessibleAbilityManagerServiceClientProxy> serviceProxy_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_UI_TEST_ABILITY_IMPL_H