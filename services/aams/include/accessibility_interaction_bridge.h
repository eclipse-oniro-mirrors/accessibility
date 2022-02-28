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

#ifndef ACCESSIBILITY_INTERACTION_BRIDGE_H
#define ACCESSIBILITY_INTERACTION_BRIDGE_H

#include <string>
#include "accessible_ability_manager_service.h"
#include "accessibility_element_info.h"
#include "accessibility_element_operator_interface.h"
#include "accessibility_operator.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityInteractionBridge {
public:
    ~AccessibilityInteractionBridge() = default;

    /**
     * @brief Get the instance of Interaction Bridge.
     * @return Returns Static reference the instance of Interaction Bridge.
     */
    static AccessibilityInteractionBridge &GetInstance();

    /**
     * @brief Focused Element Info from ACE.
     * @return Returns Element Info.
     */
    AccessibilityElementInfo FindFocusedElementInfo();

    /**
     * @brief Perform action on Accessibility Focus.
     * @param action the action of Accessibility node.
     * @return Returns true if the action perform successfully; returns false code otherwise.
     */
    bool ExecuteActionOnAccessibilityFocused(const ActionType &action);
    bool GetPointerItermOfAccessibilityFocusClick(MMI::PointerEvent::PointerItem &point);

private:
    AccessibilityInteractionBridge();
    AccessibilityElementInfo FindFocusedElementInfo(const int &windowId);
    std::shared_ptr<AccessibleAbilityManagerService> aams_ = nullptr;
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    sptr<AccessibleAbilityConnection> connection_ = nullptr;
    sptr<AccessibleAbilityChannelStubImpl> channel_ = nullptr;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_INTERACTION_BRIDGE_H

