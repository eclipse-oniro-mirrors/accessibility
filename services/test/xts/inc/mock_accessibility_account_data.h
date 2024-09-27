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

#ifndef MOCK_ACCESSIBILITY_ACCOUNT_DATA_H
#define MOCK_ACCESSIBILITY_ACCOUNT_DATA_H

#include <gmock/gmock.h>
#include "a11y_account_data.h"

namespace OHOS {
namespace A11y {
class MockA11yAccountData : public RefBase {
public:
    MockA11yAccountData(int32_t accountId);
    ~MockA11yAccountData();

    MOCK_METHOD0(GetAccountId, int32_t());
    MOCK_METHOD0(OnAccountSwitched, uint32_t());
    MOCK_METHOD1(AddConnectedAbility, void(sptr<AccessibleAbilityConnection>& connection));
    MOCK_METHOD1(RemoveConnectedAbility, void(const AppExecFwk::ElementName &element));
    MOCK_METHOD1(RemoveConnectingA11yAbility, void(const std::string &uri));
    MOCK_METHOD1(AddCaptionPropertyCallback, void(const sptr<IAccessibleAbilityManagerCaptionObserver>& callback));
    MOCK_METHOD1(RemoveCaptionPropertyCallback, void(const wptr<IRemoteObject>& callback));
    MOCK_METHOD2(AddA11yWindowConnection,
        void(const int32_t windowId, const sptr<A11yWindowConnection>& interactionConnection));
    MOCK_METHOD1(RemoveA11yWindowConnection, void(const int32_t windowId));
    MOCK_METHOD1(AddConnectingA11yAbility, void(const AppExecFwk::ElementName& elementName));
    MOCK_METHOD1(AddEnabledAbility, void(const AppExecFwk::ElementName& elementName));
    MOCK_METHOD1(RemoveEnabledAbility, RetError(const std::string& bundleName));
    MOCK_METHOD1(AddInstalledAbility, void(A11yAbilityInfo& abilityInfo));
    MOCK_METHOD1(RemoveInstalledAbility, void(const std::string &bundleName));
    MOCK_METHOD0(ClearInstalledAbility, void());
    MOCK_METHOD0(GetConnectedA11yAbilities, const std::map<std::string, sptr<AccessibleAbilityConnection>>());
    MOCK_METHOD0(GetCaptionPropertyCallbacks, const CaptionPropertyCallbacks());
    MOCK_METHOD0(GetAsacConnections, const std::map<int32_t, sptr<A11yWindowConnection>>());
    MOCK_METHOD1(
        GetAccessibleAbilityConnection, const sptr<AccessibleAbilityConnection>(const std::string &elementName));
    MOCK_METHOD1(GetA11yWindowConnection, const sptr<A11yWindowConnection>(const int32_t windowId));
    MOCK_METHOD1(GetConnectingA11yAbility, sptr<AccessibleAbilityConnection>(const std::string &uri));
    MOCK_METHOD2(GetAbilitiesByState, void(AbilityStateType state, std::vector<A11yAbilityInfo> &abilities));
    MOCK_METHOD0(GetEnabledAbilities, const std::map<std::string, AppExecFwk::ElementName> &());
    MOCK_METHOD0(GetInstalledAbilities, const std::vector<A11yAbilityInfo> &());
    MOCK_METHOD0(UpdateAccountCapabilities, void());
    MOCK_METHOD0(GetScreenMagnificationFlag, bool());
    MOCK_METHOD0(GetEventTouchGuideStateFlag, bool());
    MOCK_METHOD0(GetFilteringKeyEventsFlag, bool());
    MOCK_METHOD0(GetGesturesSimulationFlag, bool());
    MOCK_METHOD0(GetInstalledAbilitiesFromBMS, bool());
    MOCK_METHOD0(GetCaptionProperty, A11yConfig::CaptionProperty());
    MOCK_METHOD1(SetCaptionProperty, bool(const A11yConfig::CaptionProperty& caption));
    MOCK_METHOD1(SetCaptionState, bool(const bool state));
    MOCK_METHOD1(SetEnabled, bool(const bool state));
    MOCK_METHOD1(SetEnaSetTouchGuideStatebled, bool(const bool state));
    MOCK_METHOD1(SetGestureState, bool(const bool state));
    MOCK_METHOD1(SetKeyEventObserverState, bool(const bool state));
    MOCK_METHOD0(GetEnabledState, bool());
    MOCK_METHOD0(GetTouchGuideState, bool());
    MOCK_METHOD0(GetGestureState, bool());
    MOCK_METHOD0(GetKeyEventObserverState, bool());
    MOCK_METHOD0(GetCaptionState, bool());
    MOCK_METHOD1(SetEnabledObj, bool(std::map<std::string, AppExecFwk::ElementName> it));
    MOCK_METHOD0(Init, void());
    MOCK_METHOD1(DisableAbility, bool(const std::string &name));
    MOCK_METHOD1(AddUITestConnectedAbility, void(sptr<AccessibleAbilityConnection>& connection));
    MOCK_METHOD1(RemoveUITestConnectedAbility, void(sptr<AccessibleAbilityConnection>& connection));
    MOCK_METHOD0(GetUITestConnectedAbilityConnection, const sptr<AccessibleAbilityConnection>());
    MOCK_METHOD1(GetImportantEnabledAbilities, void(std::map<std::string, uint32_t> &importantEnabledAbilities));
    MOCK_METHOD1(UpdateImportantEnabledAbilities, void(std::map<std::string, uint32_t> &importantEnabledAbilities));
    MOCK_METHOD0(GetAccountType, AccountSA::OsAccountType());
};
} // namespace A11y
} // namespace OHOS
#endif // MOCK_ACCESSIBILITY_ACCOUNT_DATA_H