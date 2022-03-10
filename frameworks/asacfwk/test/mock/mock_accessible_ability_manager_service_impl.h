/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef MOCK_ACCESSIBLE_ABILITY_MANAGER_SERVICE_IMPL_H
#define MOCK_ACCESSIBLE_ABILITY_MANAGER_SERVICE_IMPL_H

#include <cstdint>
#include <list>
#include <vector>
#include "accessible_ability_manager_service_interface.h"
#include "accessibility_ability_info.h"
#include "accessible_ability_manager_service_state_interface.h"
#include "accessibility_event_info.h"
#include "accessibility_element_operator_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface to call ABMS API.
*/
class MockAccessibleAbilityManagerServiceClientImpl : public IAccessibleAbilityManagerServiceClient {
public:
    void SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId) {}

    uint32_t RegisterStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback, const int accountId)
    {
        return 1;
    }

    std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType)
    {
        AccessibilityAbilityInfo info {};
        std::vector<AccessibilityAbilityInfo> infos;
        infos.push_back(info);
        return infos;
    }

    void RegisterElementOperator(const int windowId, const sptr<IAccessibilityElementOperator> &operation,
        const int accountId)
    {
    }

    void DeregisterElementOperator(const int windowId)
    {
    }
};
} // namespace Accessibility
} // namespace OHOS
#endif