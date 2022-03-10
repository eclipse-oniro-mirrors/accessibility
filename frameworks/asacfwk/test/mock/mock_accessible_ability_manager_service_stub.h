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

    uint32_t RegisterStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback, const int accountId)
    {
        return 1;
    }

    std::vector<AccessibilityAbilityInfo> GetAbilityList(const uint32_t abilityTypes, const int32_t stateType)
    {
        std::vector<AccessibilityAbilityInfo> infos;
        return infos;
    }

    void RegisterElementOperator(const int windowId,
        const sptr<IAccessibilityElementOperator> &operation, const int accountId) {}

    void DeregisterElementOperator(const int windowId) {}

    uint32_t RegisterCaptionPropertyCallback(const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback,
        const int accountId)
    {
            return 0;
    }

    CaptionProperty GetCaptionProperty()
    {
        CaptionProperty cap;
        return cap;
    }

    bool GetEnabledState()
    {
        return true;
    }
    bool GetCaptionState()
    {
        return true;
    }
    bool GetTouchGuideState()
    {
        return true;
    }
    bool GetGestureState()
    {
        return true;
    }
    bool GetKeyEventObserverState()
    {
        return true;
    }
    bool SetCaptionProperty(const CaptionProperty& caption)
    {
        return true;
    }
    bool SetCaptionState(const bool state)
    {
        return true;
    }
    bool SetEnabled(const bool state)
    {
        return true;
    }
    bool SetTouchGuideState(const bool state)
    {
        return true;
    }
    bool SetGestureState(const bool state)
    {
        return true;
    }
    bool SetKeyEventObserverState(const bool state)
    {
        return true;
    }
    bool SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
    {
        return true;
    }
    std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities()
    {
        std::map<std::string, AppExecFwk::ElementName> test;
        return test;
    }
    std::vector<AccessibilityAbilityInfo> GetInstalledAbilities()
    {
        std::vector<AccessibilityAbilityInfo> test;
        return test;
    }
    bool DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
    {
        return true;
    }
    bool RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj)
    {
        return true;
    }
    bool DeregisterUITestAbilityConnectionClient()
    {
        return true;
    }
    int GetActiveWindow()
    {
        return 0;
    }
private:
    DISALLOW_COPY_AND_MOVE(MockAccessibleAbilityManagerServiceStub);
};
} // namespace Accessibility
} // namespace OHOS
#endif