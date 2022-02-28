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

#include "accessibility_extension_context.h"
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

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetFocusElementInfo(uint32_t focusType, std::optional<AccessibilityElementInfo> &elementInfo) override;

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    virtual bool GetRootElementInfo(std::optional<AccessibilityElementInfo> &elementInfo) override;

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param
     * @return The infomation of windows.
     */
    virtual std::vector<AccessibilityWindowInfo> GetWindows() override;

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::UiTestGlobalAction.
     * @return Return true if executes action successfully, else return false.
     */
    virtual bool ExecuteCommonAction(UiTestGlobalAction action) override;

    /**
     * @brief Dispatch the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param result The result of gesture completion.
     * @return
     */
    virtual void DispatchOnSimulationGestureResult(uint32_t sequence, bool result) override;

    /**
     * @brief Set channelId.
     * @param channelId The id of channel.
     * @return
     */
    virtual void SetChannelId(uint32_t channelId) override;

private:
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
    sptr<AccessibleAbilityManagerServiceClientProxy> serviceProxy_ = nullptr;
    std::shared_ptr<AccessibilityExtensionContext> extensionContext_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_UI_TEST_ABILITY_IMPL_H