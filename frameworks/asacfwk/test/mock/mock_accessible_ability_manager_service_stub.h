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
 
#ifndef MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H
#define MOCK_ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H

#include "accessible_ability_manager_service_stub.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface to call ABMS API.
*/
class MockAccessibleAbilityManagerServiceStub : public AccessibleAbilityManagerServiceClientStub {
public:
    MockAccessibleAbilityManagerServiceStub() {}
    ~MockAccessibleAbilityManagerServiceStub() {}

    void SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId) {}

    uint32_t RegisterStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback, const int accountId) {
        return 1;
    }
    std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType) {
        std::vector<AccessibilityAbilityInfo> infos;
        return infos;
    }
    void RegisterInteractionOperation(const int windowId,
            const sptr<IAccessibilityInteractionOperation> &operation,
            const int accountId) {}
    void DeregisterInteractionOperation(const int windowId) {}
    void Interrupt(const int accountId) {}

    void RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj) {}
    uint64_t GetSuggestedInterval() {return 1;}
private:
    DISALLOW_COPY_AND_MOVE(MockAccessibleAbilityManagerServiceStub);
};

} //namespace Accessibility
} //namespace OHOS
#endif