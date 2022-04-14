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

#include "accessibility_account_data.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
const static std::string PREF_TEST_PATH = "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";

AccessibilityAccountData::AccessibilityAccountData(int32_t accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int32_t AccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("start.");
    return id_;
}

// get client state.
uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    return 0;
}

// switch the user causes state changed.
void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_DEBUG("start.");
    // reset AccessibleAbilityConnection
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        itr->second->Disconnect();
    }

    // Clear all abilities.
    connectedA11yAbilities_.clear();
    connectingA11yAbilities_.clear();
    enabledAbilities_.clear();
    installedAbilities_.clear();

    // Clear Setting State.
    isEventTouchGuideState_ = false;
    isScreenMagnification_ = false;
    isFilteringKeyEvents_ = false;
    isGesturesSimulation_ = false;
}

// add connect ability.
void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    if (!connectedA11yAbilities_.count(connection->GetElementName().GetURI())) {
        connectedA11yAbilities_.insert(std::make_pair(connection->GetElementName().GetURI(), connection));
    }
    HILOG_DEBUG("Add ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    HILOG_DEBUG("Remove ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    HILOG_DEBUG("start.");
    stateCallbacks_.push_back(callback);
}

// remove IAccessibleAbilityManagerServiceState
void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject>& callback)
{
    (void)callback;
    HILOG_DEBUG("start.");
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    (void)callback;
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    (void)callback;
    HILOG_DEBUG("start.");
}
// add AccessibilityInteractionConnection
void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    (void)interactionConnection;
    HILOG_DEBUG("windowId(%{public}d)", windowId);
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start.");
    for (auto& ability : connectingA11yAbilities_) {
        if (ability == bundleName) {
            HILOG_ERROR("The ability is already connecting, and it's bundle name is %{public}s", bundleName.c_str());
            return;
        }
    }
    connectingA11yAbilities_.push_back(bundleName);
    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start");
    for (auto it = connectingA11yAbilities_.begin(); it != connectingA11yAbilities_.end(); it++) {
        if (*it == bundleName) {
            HILOG_DEBUG("Removed %{public}s from ConnectingA11yAbility: ", bundleName.c_str());
            connectingA11yAbilities_.erase(it);
            HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not connecting.", bundleName.c_str());
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start.");
    for (auto& ability : enabledAbilities_) {
        if (ability == bundleName) {
            HILOG_ERROR("The ability is already enabled, and it's bundle name is %{public}s", bundleName.c_str());
            return;
        }
    }
    enabledAbilities_.push_back(bundleName);
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledFromPref(const std::string bundleName)
{
    (void)bundleName;
}

void AccessibilityAccountData::RemoveEnabledAbility(const std::string& bundleName)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == bundleName) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", bundleName.c_str());
            enabledAbilities_.erase(it);
            RemoveEnabledFromPref(bundleName);
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", bundleName.c_str());
}

// For UT
void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    (void)abilityInfo;
}

void AccessibilityAccountData::RemoveInstalledAbility(std::string bundleName)
{
    (void)bundleName;
}

void AccessibilityAccountData::ClearInstalledAbility()
{}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string elementName)
{
    (void)elementName;
    return nullptr;
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    (void)windowId;
    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectedA11yAbilities_;
}

const std::vector<sptr<IAccessibleAbilityManagerStateObserver>> AccessibilityAccountData::GetStateCallbacks()
{
    HILOG_DEBUG("start.");
    return stateCallbacks_;
}

const std::map<int32_t, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("start.");
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("start.");
    return captionPropertyCallbacks_;
}

const std::vector<std::string>& AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectingA11yAbilities_;
}

const std::vector<std::string>& AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_ size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName = %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetInstalledAbilities()
{
    HILOG_DEBUG("start.");
    return installedAbilities_;
}

const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state)
{
    (void)state;
    std::vector<AccessibilityAbilityInfo> enabledAbilities;
    return enabledAbilities;
}

void AccessibilityAccountData::UpdateAccountCapabilities()
{}

void AccessibilityAccountData::UpdateEventTouchGuideCapability()
{}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    isScreenMagnification_ = false;
}

bool AccessibilityAccountData::SetCaptionState(const bool state)
{
    (void)state;
    return true;
}

bool AccessibilityAccountData::SetCaptionPropertyPref()
{
    return true;
}

bool AccessibilityAccountData::SetCaptionProperty(const CaptionProperty& caption)
{
    (void)caption;
    return true;
}

std::string AccessibilityAccountData::StateChange(bool state)
{
    (void)state;
    std::string STATE_TRUE = "on";
    return STATE_TRUE;
}

bool AccessibilityAccountData::SetStatePref(int32_t type)
{
    (void)type;
    return true;
}

bool AccessibilityAccountData::SetEnabled(const bool state)
{
    (void)state;
    return true;
}

bool AccessibilityAccountData::SetTouchGuideState(const bool state)
{
    (void)state;
    return true;
}

bool AccessibilityAccountData::SetGestureState(const bool state)
{
    (void)state;
    return true;
}

bool AccessibilityAccountData::SetKeyEventObserverState(const bool state)
{
    (void)state;
    return true;
}

bool AccessibilityAccountData::GetEnabledState()
{
    return isEnabled_;
}

bool AccessibilityAccountData::GetTouchGuideState()
{
    HILOG_DEBUG("start. isEventTouchGuideState_ = %{public}s", isEventTouchGuideState_ ? "true" : "false");
    return isEventTouchGuideState_;
}

bool AccessibilityAccountData::GetGestureState()
{
    return isGesturesSimulation_;
}

bool AccessibilityAccountData::GetKeyEventObserverState()
{
    return isFilteringKeyEvents_;
}

bool AccessibilityAccountData::GetCaptionState()
{
    return isCaptionState_;
}

void AccessibilityAccountData::UpdateEnabledFromPref()
{}

bool AccessibilityAccountData::EnableAbilities(std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start.");
    for (auto& ability : abilities) {
        bool isEnabledAbility = false;
        for (auto& enabledAbility : enabledAbilities_) {
            if (ability == enabledAbility) {
                HILOG_ERROR("The ability[%{public}s] is already enabled", ability.c_str());
                isEnabledAbility = true;
                break;
            }
        }
        if (!isEnabledAbility) {
            enabledAbilities_.push_back(ability);
        }
    }
    UpdateEnabledFromPref();
    return true;
}

bool AccessibilityAccountData::ReadConfigurationForAccountData()
{
    HILOG_DEBUG("start.");
    // Temp deal: read the user confige data.
    return true;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");
    return true;
}

void AccessibilityAccountData::CaptionInit(const std::shared_ptr<NativePreferences::Preferences>& pref)
{
    (void)pref;
}

void AccessibilityAccountData::CapabilityInit(const std::shared_ptr<NativePreferences::Preferences>& pref)
{
    (void)pref;
}

void AccessibilityAccountData::EnabledListInit(const std::shared_ptr<NativePreferences::Preferences>& pref)
{
    (void)pref;
}

void AccessibilityAccountData::StringToVector(std::string& stringIn, std::vector<std::string>& vectorResult)
{
    (void)stringIn;
    (void)vectorResult;
    HILOG_DEBUG("end.");
}

void AccessibilityAccountData::init()
{}

bool AccessibilityAccountData::DisableAbilities(const std::vector<std::string>& abilities)
{
    HILOG_DEBUG("start.");
    for (auto& ability : abilities) {
        HILOG_ERROR("disable ability[%{public}s] start", ability.c_str());
        for (auto iter = enabledAbilities_.begin(); iter != enabledAbilities_.end();) {
            if (*iter == ability) {
                enabledAbilities_.erase(iter);
                RemoveEnabledFromPref(ability);
                HILOG_DEBUG("Removed %{public}s and EnabledAbility size(%{public}zu)",
                    ability.c_str(),
                    enabledAbilities_.size());
            } else {
                iter++;
            }
        }
    }
    return true;
}

void AccessibilityAccountData::VectorToString(std::vector<std::string>& vectorVal, std::string& stringOut)
{
    (void)vectorVal;
    HILOG_DEBUG("end stringOUT = %{public}s .", stringOut.c_str());
}
} // namespace Accessibility
} // namespace OHOS