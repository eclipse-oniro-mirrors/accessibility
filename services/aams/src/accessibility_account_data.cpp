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

#include "accessibility_account_data.h"
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "json_utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string PREF_PATH = "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
} // namespace

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

uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;
    if (!connectedA11yAbilities_.empty() || !connectingA11yAbilities_.empty()) {
        state |= STATE_ACCESSIBILITY_ENABLED;
        if (!isEnabled_) {
            SetEnabled(true);
        }
    } else {
        if (isEnabled_) {
            SetEnabled(false);
        }
    }

    if (isEventTouchGuideState_) {
        state |= STATE_EXPLORATION_ENABLED;
    }

    if (isCaptionState_) {
        state |= STATE_CAPTION_ENABLED;
    }

    if (isFilteringKeyEvents_) {
        state |= STATE_KEYEVENT_ENABLED;
    }

    if (isGesturesSimulation_) {
        state |= STATE_GESTURE_ENABLED;
    }

    return state;
}

void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_DEBUG("start.");
    connectingA11yAbilities_.clear();
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        itr->second->Disconnect();
    }
}

void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    if (!connectedA11yAbilities_.count(connection->GetElementName().GetURI())) {
        connectedA11yAbilities_.insert(make_pair(connection->GetElementName().GetURI(), connection));
    }

    HILOG_DEBUG("Add ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it =
        connectedA11yAbilities_.find(connection->GetElementName().GetURI());
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectedAbility: %{public}zu", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerStateObserver>& callback)
{
    HILOG_DEBUG("start.");
    stateCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("start.");
    for (auto itr = stateCallbacks_.begin(); itr != stateCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            stateCallbacks_.erase(itr);
            break;
        }
    }
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerCaptionObserver>& callback)
{
    HILOG_DEBUG("start.");
    captionPropertyCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("start.");
    for (auto itr = captionPropertyCallbacks_.begin(); itr != captionPropertyCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            captionPropertyCallbacks_.erase(itr);
            break;
        }
    }
}

void AccessibilityAccountData::AddEnableAbilityListsObserver(
    const sptr<IAccessibilityEnableAbilityListsObserver>& observer)
{
    HILOG_DEBUG("start.");
    for (auto &enableAbilityListsObserver : enableAbilityListsObservers_) {
        if (enableAbilityListsObserver == observer) {
            HILOG_ERROR("observer is already exist");
            return;
        }
    }
    enableAbilityListsObservers_.push_back(observer);
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
}

void AccessibilityAccountData::RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer)
{
    HILOG_DEBUG("start.");
    for (auto itr = enableAbilityListsObservers_.begin(); itr != enableAbilityListsObservers_.end(); itr++) {
        if ((*itr)->AsObject() == observer) {
            HILOG_DEBUG("erase observer");
            enableAbilityListsObservers_.erase(itr);
            HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
            return;
        }
    }
}

void AccessibilityAccountData::UpdateEnableAbilityListsState()
{
    HILOG_DEBUG("observer's size is %{public}zu", enableAbilityListsObservers_.size());
    for (auto &observer : enableAbilityListsObservers_) {
        if (observer) {
            observer->OnAccessibilityEnableAbilityListsChanged();
        }
    }
}

void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    std::map<int32_t, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const std::string &name)
{
    HILOG_DEBUG("start.");
    for (auto &ability : connectingA11yAbilities_) {
        if (ability == name) {
            HILOG_ERROR("The ability is already connecting, and it's name is %{public}s", name.c_str());
            return;
        }
    }
    connectingA11yAbilities_.push_back(name);
    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = connectingA11yAbilities_.begin(); it != connectingA11yAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from ConnectingA11yAbility: ", name.c_str());
            connectingA11yAbilities_.erase(it);
            HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}zu", connectingA11yAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not connecting.", name.c_str());
}

void AccessibilityAccountData::AddEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start.");
    for (auto &ability : enabledAbilities_) {
        if (ability == name) {
            HILOG_ERROR("The ability is already enabled, and it's name is %{public}s", name.c_str());
            return;
        }
    }
    enabledAbilities_.push_back(name);
    UpdateEnableAbilityListsState();
    HILOG_DEBUG("Add EnabledAbility: %{public}zu", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledFromPref(const std::string &name)
{
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }
    std::string ability = name + "/" + std::to_string(GetAbilityCapabilities(name));
    std::string strValue = pref_->GetString("BundleName/AbilityName/Capabilities", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());

    std::vector<std::string> vecvalue;
    StringToVector(strValue, vecvalue);
    for (std::vector<std::string>::iterator val = vecvalue.begin();val != vecvalue.end();) {
        if (!std::strcmp(val->c_str(), ability.c_str())) {
            val = vecvalue.erase(val);
            HILOG_DEBUG("remove val = %{public}s", val->c_str());
        } else {
            ++val;
        }
    }
    std::string stringOut = "";
    VectorToString(vecvalue, stringOut);
    int errCode = pref_->PutString("BundleName/AbilityName/Capabilities", stringOut);
    if (errCode) {
        HILOG_ERROR("pref_->PutString() error(%{public}d).", errCode);
    }

    errCode = pref_->FlushSync();
    if (errCode) {
        HILOG_ERROR("pref_->FlushSync() error(%{public}d).", errCode);
    }
}

void AccessibilityAccountData::RemoveEnabledAbility(const std::string &name)
{
    HILOG_DEBUG("start");
    for (auto it = enabledAbilities_.begin(); it != enabledAbilities_.end(); it++) {
        if (*it == name) {
            HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", name.c_str());
            enabledAbilities_.erase(it);
            RemoveEnabledFromPref(name);
            UpdateEnableAbilityListsState();
            HILOG_DEBUG("EnabledAbility size(%{public}zu)", enabledAbilities_.size());
            return;
        }
    }
    HILOG_ERROR("The ability(%{public}s) is not enabled.", name.c_str());
}

void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    HILOG_DEBUG("abilityInfo's bundle name is %{public}s", abilityInfo.GetPackageName().c_str());
    for (size_t i = 0; i < installedAbilities_.size(); i++) {
        if (installedAbilities_[i].GetPackageName() == abilityInfo.GetPackageName()) {
            HILOG_DEBUG("the ability is already exist.");
            return;
        }
    }
    installedAbilities_.push_back(abilityInfo);
    HILOG_DEBUG("push back installed ability successfully and installedAbilities_'s size is %{public}zu",
        installedAbilities_.size());
}

void AccessibilityAccountData::RemoveInstalledAbility(const std::string &bundleName)
{
    HILOG_DEBUG("start.");
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == bundleName) {
            HILOG_DEBUG("Removed %{public}s from InstalledAbility: ", bundleName.c_str());
            installedAbilities_.erase(it);
        } else {
            ++it;
        }
    }
}

void AccessibilityAccountData::ClearInstalledAbility()
{
    HILOG_DEBUG("start.");
    installedAbilities_.clear();
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string &elementName)
{
    HILOG_DEBUG("URI is %{public}s", elementName.c_str());
    for (auto& connected : connectedA11yAbilities_) {
        std::string::size_type  idx = connected.first.find(elementName);
        if (idx == std::string::npos) {
            continue;
        } else {
            HILOG_DEBUG("founded URI = %{public}s ", connected.first.c_str());
            return connected.second;
        }
    }
    HILOG_DEBUG("URI %{public}s not found ", elementName.c_str());
    return nullptr;
}

const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int32_t windowId)
{
    HILOG_DEBUG("windowId(%{public}d).", windowId);
    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

    return nullptr;
}

const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectedA11yAbilities_;
}

const std::vector<sptr<IAccessibleAbilityManagerStateObserver>> &AccessibilityAccountData::GetStateCallbacks() const
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

const std::vector<std::string> &AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectingA11yAbilities_;
}

const std::vector<std::string> &AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_ size is (%{public}zu).", enabledAbilities_.size());
    for (auto& ability : enabledAbilities_) {
        HILOG_DEBUG("bundleName = %{public}s ", ability.c_str());
    }
    return enabledAbilities_;
}

const std::vector<AccessibilityAbilityInfo> &AccessibilityAccountData::GetInstalledAbilities() const
{
    HILOG_DEBUG("start.");
    return installedAbilities_;
}

void AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state,
    std::vector<AccessibilityAbilityInfo> &abilities) const
{
    HILOG_DEBUG("state(%{public}d) start.", state);
    if (state == ABILITY_STATE_ENABLE) {
        for (auto &ability : connectedA11yAbilities_) {
            if (ability.second) {
                abilities.push_back(ability.second->GetAbilityInfo());
            }
        }
        HILOG_DEBUG("connectedA11yAbilities_ is (%{public}zu) enabledAbilities is (%{public}zu).",
            connectedA11yAbilities_.size(), abilities.size());
    } else if (state == ABILITY_STATE_DISABLE) {
        GetDisableAbilities(abilities);
        HILOG_DEBUG("the size of disable abilities is %{public}zu", abilities.size());
    } else {
        abilities = installedAbilities_;
    }
}

void AccessibilityAccountData::GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities) const
{
    HILOG_DEBUG("start.");
    disabledAbilities = installedAbilities_;
    for (auto &enabledAbility : connectedA11yAbilities_) {
        for (auto itr = disabledAbilities.begin(); itr != disabledAbilities.end();) {
            if (enabledAbility.second && (itr->GetId() == enabledAbility.second->GetAbilityInfo().GetId())) {
                disabledAbilities.erase(itr);
            } else {
                itr++;
            }
        }
    }
}

void AccessibilityAccountData::UpdateAccountCapabilities()
{
    HILOG_DEBUG("start.");
    UpdateFilteringKeyEventsCapability();
    UpdateEventTouchGuideCapability();
    UpdateGesturesSimulationCapability();
    UpdateMagnificationCapability();

    SetTouchGuideState(isEventTouchGuideState_);
    SetGestureState(isGesturesSimulation_);
    SetKeyEventObserverState(isFilteringKeyEvents_);
}

void AccessibilityAccountData::UpdateEventTouchGuideCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_TOUCH_GUIDE) {
            isEventTouchGuideState_ = true;
            return;
        }
    }
    isEventTouchGuideState_ = false;
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE) {
            isGesturesSimulation_ = true;
            return;
        }
    }
    isGesturesSimulation_ = false;
}

void AccessibilityAccountData::UpdateFilteringKeyEventsCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
            isFilteringKeyEvents_ = true;
            return;
        }
    }
    isFilteringKeyEvents_ = false;
}

void AccessibilityAccountData::UpdateMagnificationCapability()
{
    HILOG_DEBUG("start.");
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM) {
            isScreenMagnification_ = true;
            return;
        }
    }
    isScreenMagnification_ = false;
}

bool AccessibilityAccountData::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start.");
    isCaptionState_ = state;
    SetStatePref(STATE::CAPTION);

    return true;
}

bool AccessibilityAccountData::SetCaptionPropertyPref()
{
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }
    std::string FONTFAMILY = captionProperty_.GetFontFamily();
    int32_t FONTSCALE = captionProperty_.GetFontScale();
    uint32_t FONTCOLOR = captionProperty_.GetFontColor();
    std::string FONTEDGETYPE = captionProperty_.GetFontEdgeType();
    uint32_t BACKGROUNDCOLOR = captionProperty_.GetBackgroundColor();
    uint32_t WINDOWCOLOR = captionProperty_.GetWindowColor();

    pref_->PutString("fontFamily", FONTFAMILY);
    pref_->PutInt("fontColor", (int)FONTCOLOR);
    pref_->PutString("fontEdgeType", FONTEDGETYPE);
    pref_->PutInt("backgroundColor", (int)BACKGROUNDCOLOR);
    pref_->PutInt("windowColor", (int)WINDOWCOLOR);
    pref_->PutInt("fontScale", FONTSCALE);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start.");
    captionProperty_ = caption;
    SetCaptionPropertyPref();
    return true;
}

std::string AccessibilityAccountData::StateChange(bool state)
{
    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    if (state) {
        return STATE_TRUE;
    } else {
        return STATE_FALSE;
    }
}

bool AccessibilityAccountData::SetStatePref(int32_t type)
{
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    std::string strValue = "";
    switch (type) {
        case STATE::ACCESSIBILITY:
            strValue = StateChange(isEnabled_);
            pref_->PutString("accessible", strValue);
            break;
        case STATE::TOUCHGUIDE:
            strValue = StateChange(isEventTouchGuideState_);
            pref_->PutString("touchGuide", strValue);
            break;
        case STATE::GESTURE:
            strValue = StateChange(isGesturesSimulation_);
            pref_->PutString("gesture", strValue);
            break;
        case STATE::KEYEVENT:
            strValue = StateChange(isFilteringKeyEvents_);
            pref_->PutString("keyEventObserver", strValue);
            break;
        case STATE::CAPTION:
            strValue = StateChange(isCaptionState_);
            pref_->PutString("CaptionState", strValue);
            break;
        case STATE::SCREENMAGNIFIER:
            strValue = StateChange(isScreenMagnificationState_);
            pref_->PutString("ScreenMagnification", strValue);
            break;
        case STATE::SHORTKEY:
            strValue = StateChange(isShortKeyState_);
            pref_->PutString("ShortKey", strValue);
            break;
        case STATE::MOUSEKEY:
            strValue = StateChange(isMouseKeyState_);
            pref_->PutString("MouseKey", strValue);
            break;
        case STATE::HIGHCONTRASTTEXT:
            strValue = StateChange(highContrastTextState_);
            pref_->PutString("highContrastText", strValue);
            break;
        case STATE::INVERTCOLORSTATE:
            strValue = StateChange(invertColorState_);
            pref_->PutString("invertColor", strValue);
            break;
        case STATE::ANIMATIONOFF:
            strValue = StateChange(animationOffState_);
            pref_->PutString("animationOff", strValue);
            break;
        case STATE::AUDIOMONO:
            strValue = StateChange(audioMonoState_);
            pref_->PutString("audioMono", strValue);
            break;
        default:
            break;
    }

    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetEnabled(const bool state)
{
    HILOG_DEBUG("start.");
    isEnabled_ = state;
    SetStatePref(STATE::ACCESSIBILITY);
    return true;
}

bool AccessibilityAccountData::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start.");
    isEventTouchGuideState_ = state;
    SetStatePref(STATE::TOUCHGUIDE);
    return true;
}

bool AccessibilityAccountData::SetGestureState(const bool state)
{
    HILOG_DEBUG("start.");
    isGesturesSimulation_ = state;
    SetStatePref(STATE::GESTURE);
    return true;
}

bool AccessibilityAccountData::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start.");
    isFilteringKeyEvents_ = state;
    SetStatePref(STATE::KEYEVENT);
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
{
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }
    std::vector<std::string> vecvalue;
    for (auto& enabledAbility : enabledAbilities_) {
        uint32_t capabilities = GetAbilityCapabilities(enabledAbility);
        if (capabilities == 0) {
            HILOG_ERROR("capabilities is wrong!");
            continue;
        }
        std::string ability = enabledAbility + "/" + std::to_string(capabilities);
        vecvalue.push_back(ability);
        HILOG_DEBUG("ability is %{public}s ", ability.c_str());
    }
    std::string stringOut = "";
    VectorToString(vecvalue, stringOut);
    pref_->PutString("BundleName/AbilityName/Capabilities", stringOut);
    pref_->FlushSync();
}

bool AccessibilityAccountData::EnableAbility(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start and name[%{public}s] capabilities[%{public}d]", name.c_str(), capabilities);

    // Parse name to bundle name and ability name
    std::string bundleName = name.substr(0, name.find("/"));
    std::string abilityName = name.substr(name.find("/") + 1);
    HILOG_DEBUG("bundleName[%{public}s], abilityName[%{public}s]", bundleName.c_str(), abilityName.c_str());

    // Parse config from bms according to bundle name and ability name
    uint32_t configCapabilities = GetConfigCapabilitiesFromBms(bundleName, abilityName);

    // Judge capabilities
    uint32_t resultCapabilities = configCapabilities & capabilities;
    HILOG_DEBUG("resultCapabilities is [%{public}d]", resultCapabilities);
    if (resultCapabilities == 0) {
        HILOG_ERROR("the result of capabilities is wrong");
        return false;
    }

    bool result = SetAbilityCapabilities(name, resultCapabilities);
    if (!result) {
        HILOG_ERROR("Reset capabilities failed");
        return false;
    }

    // Add enabled ability
    for (auto &enabledAbility : enabledAbilities_) {
        if (enabledAbility == name) {
            HILOG_ERROR("The ability[%{public}s] is already enabled", name.c_str());
            return false;
        }
    }
    enabledAbilities_.push_back(name);

    UpdateEnabledFromPref();
    UpdateEnableAbilityListsState();
    return true;
}

uint32_t AccessibilityAccountData::GetConfigCapabilitiesFromBms(const std::string &bundleName,
    const std::string &abilityName)
{
    HILOG_DEBUG("start");
    sptr<AppExecFwk::IBundleMgr> bmsMgr =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bmsMgr) {
        HILOG_ERROR("bmsMgr is nullptr.");
        return 0;
    }

    uint32_t configCapabilities = 0;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    bmsMgr->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto &info : extensionInfos) {
        if (info.bundleName == bundleName && info.name == abilityName) {
            AccessibilityAbilityInitParams initParams;
            AccessibilityConfigParse::Parse(info, initParams);
            configCapabilities = initParams.capabilities;
            HILOG_DEBUG("configCapabilities is [%{public}d]", configCapabilities);
            return configCapabilities;
        }
    }
    HILOG_ERROR("Query ability from bms failed. bundleName[%{public}s] abilityName[%{public}s]",
        bundleName.c_str(), abilityName.c_str());
    return 0;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    sptr<AppExecFwk::IBundleMgr> bms = Singleton<AccessibleAbilityManagerService>::GetInstance().GetBundleMgrProxy();
    if (!bms) {
        HILOG_ERROR("GetBundleMgrProxy failed.");
        return false;
    }
    bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}zu.", extensionInfos.size());
    for (auto& info : extensionInfos) {
        AccessibilityAbilityInitParams initParams;
        AccessibilityConfigParse::Parse(info, initParams);
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(initParams);
        AddInstalledAbility(*accessibilityInfo);
    }

    for (auto &ability : abilityCapabilities_) {
        bool result = SetAbilityCapabilities(ability.first, ability.second);
        if (!result) {
            HILOG_ERROR("set ability capabilities failed!");
            continue;
        }
        enabledAbilities_.push_back(ability.first);
        UpdateEnableAbilityListsState();
    }
    return true;
}

void AccessibilityAccountData::CaptionInit(const std::shared_ptr<NativePreferences::Preferences> &pref)
{
    HILOG_DEBUG("start.");
    if (!pref) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref->GetString("CaptionState", "");
    HILOG_DEBUG(" pref->GetString() = %{public}s.", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isCaptionState_ = true;
    } else {
        isCaptionState_ = false;
    }

    std::string FONTFAMILY = pref->GetString("fontFamily", "default");
    HILOG_DEBUG(" pref->GetString() = %{public}s.", FONTFAMILY.c_str());

    int32_t FONTSCALE = static_cast<int32_t>(pref->GetInt("fontScale", 0));
    HILOG_DEBUG(" pref->GetString() = %{public}d.", FONTSCALE);

    uint32_t FONTCOLOR = static_cast<uint32_t>(pref->GetInt("fontColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", FONTCOLOR);

    std::string FONTEDGETYPE = pref->GetString("fontEdgeType", "none");
    HILOG_DEBUG(" pref->GetString() = 0x%{public}s.", FONTEDGETYPE.c_str());

    uint32_t BACKGROUNDCOLOR = static_cast<uint32_t>(pref->GetInt("backgroundColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", BACKGROUNDCOLOR);

    uint32_t WINDOWCOLOR = static_cast<uint32_t>(pref->GetInt("windowColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", WINDOWCOLOR);

    captionProperty_.SetFontFamily(FONTFAMILY);
    captionProperty_.SetFontScale(FONTSCALE);
    captionProperty_.SetFontColor(FONTCOLOR);
    captionProperty_.SetFontEdgeType(FONTEDGETYPE);
    captionProperty_.SetBackgroundColor(BACKGROUNDCOLOR);
    captionProperty_.SetWindowColor(WINDOWCOLOR);
}

void AccessibilityAccountData::CapabilityInit(const std::shared_ptr<NativePreferences::Preferences> &pref)
{
    HILOG_DEBUG("start.");
    if (!pref) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref->GetString("accessible", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isEnabled_ = true;
    } else {
        isEnabled_ = false;
    }

    strValue = pref->GetString("touchGuide", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        HILOG_DEBUG("isEventTouchGuideState_ = true");
        isEventTouchGuideState_ = true;
    } else {
        HILOG_DEBUG("isEventTouchGuideState_ = false");
        isEventTouchGuideState_ = false;
    }

    strValue = pref->GetString("gesture", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isFilteringKeyEvents_ = true;
    } else {
        isFilteringKeyEvents_ = false;
    }

    strValue = pref->GetString("keyEventObserver", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isGesturesSimulation_ = true;
    } else {
        isGesturesSimulation_ = false;
    }
}

void AccessibilityAccountData::EnabledListInit(const std::shared_ptr<NativePreferences::Preferences> &pref)
{
    HILOG_DEBUG("start.");
    if (!pref) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    AccessibilityAbilityInfo abilityInfo;

    std::string strValue = pref->GetString("BundleName/AbilityName/Capabilities", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());

    std::vector<std::string> vecvalue;
    StringToVector(strValue, vecvalue);
    for (auto &value : vecvalue) {
        HILOG_DEBUG("BundleName/AbilityName/Capabilities = %{public}s", value.c_str());
        std::string name = value.substr(0, value.find_last_of("/"));
        std::string capabilities = value.substr(value.find_last_of("/") + 1);
        uint32_t abilityCapabilities = static_cast<uint32_t>(std::atoi(capabilities.c_str()));
        HILOG_DEBUG("name[%{public}s] abilityCapabilities[%{public}d]", name.c_str(), abilityCapabilities);
        if (abilityCapabilities == 0) {
            HILOG_ERROR("abilityCapabilities is wrong!");
            continue;
        }
        abilityCapabilities_.insert(std::make_pair(name, abilityCapabilities));
    }
}

void AccessibilityAccountData::StringToVector(const std::string &stringIn, std::vector<std::string> &vectorResult)
{
    HILOG_DEBUG("start.");
    int32_t strLength = static_cast<int32_t>(stringIn.size());
    std::vector<int32_t> position;

    if (strLength == 0) {
        return;
    }

    for (int32_t j = 0; j < strLength; j++) {
        if (stringIn[j] == ',') {
            position.push_back(j);
        }
    }

    int32_t wrodCount = static_cast<int32_t>(position.size());
    if ((wrodCount == 0) && (strLength > 0)) {
        vectorResult.push_back(stringIn);
    } else {
        int32_t startWrod = 0;
        int32_t length = 0;
        for (int32_t i = 0; i <= wrodCount; i++) {
            if (i == 0) {
                length = position[i];
                vectorResult.push_back(stringIn.substr(startWrod, length)); // First string
            } else if (i < wrodCount) {
                startWrod = position[i - 1] + 1;
                length = position[i] - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Second string to last-1 string
            } else {
                startWrod = position[i - 1] + 1;
                length = strLength - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Last string
            }
        }
    }
    HILOG_DEBUG("strLength = %{public}d, wrodCount = %{public}d, stringIn : %{public}s",
        strLength, wrodCount, stringIn.c_str());
    for (auto& var : vectorResult) {
        HILOG_DEBUG("vectorResult = %{public}s ", var.c_str());
    }
    HILOG_DEBUG("end.");
}

void AccessibilityAccountData::Init()
{
    int errCode = 0;
    pref_ = NativePreferences::PreferencesHelper::GetPreferences(PREF_PATH + std::to_string(id_) + ".xml", errCode);
    if (errCode) {
        HILOG_ERROR("GetPreferences failed! account id (%{public}d), errCode(%{public}d).", id_, errCode);
        return;
    }

    CaptionInit(pref_);
    CapabilityInit(pref_);
    EnabledListInit(pref_);
}

void AccessibilityAccountData::ClearData()
{
    int errCode = 0;
    errCode = NativePreferences::PreferencesHelper::DeletePreferences(PREF_PATH + std::to_string(id_) + ".xml");
    if (errCode) {
        HILOG_ERROR("DeletePreferences failed! account id (%{public}d)", id_);
    }
}

void AccessibilityAccountData::VectorToString(const std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG("start.");
    int32_t i = 0;
    for (auto& var : vectorVal) {
        if (i > 0) {
            stringOut = stringOut + ',';
        }
        stringOut = stringOut + var.c_str();
        i++;
    }
    HILOG_DEBUG("end stringOUT = %{public}s .", stringOut.c_str());
}

bool AccessibilityAccountData::SetAbilityCapabilities(const std::string &name, const uint32_t capabilities)
{
    HILOG_DEBUG("start. name[%{public}s] capabilities[%{public}d]", name.c_str(), capabilities);
    for (auto &installedAbility : installedAbilities_) {
        if (installedAbility.GetId() == name) {
            HILOG_DEBUG("reset ability capabilities");
            installedAbility.SetCapabilityValues(capabilities);
            return true;
        }
    }
    HILOG_ERROR("not found the ability %{public}s", name.c_str());
    return false;
}

uint32_t AccessibilityAccountData::GetAbilityCapabilities(const std::string &name)
{
    HILOG_DEBUG("start. name[%{public}s]", name.c_str());
    for (auto &installedAbility : installedAbilities_) {
        if (installedAbility.GetId() == name) {
            return installedAbility.GetCapabilityValues();
        }
    }
    HILOG_ERROR("not found the ability %{public}s", name.c_str());
    return 0;
}

bool AccessibilityAccountData::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start.");
    isScreenMagnificationState_ = state;
    SetStatePref(STATE::SCREENMAGNIFIER);

    return true;
}

bool AccessibilityAccountData::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyState_ = state;
    SetStatePref(STATE::SHORTKEY);

    return true;
}

bool AccessibilityAccountData::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isMouseKeyState_ = state;
    SetStatePref(STATE::MOUSEKEY);

    return true;
}

bool AccessibilityAccountData::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start.");
    mouseAutoClick_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutInt("MouseAutoClick", mouseAutoClick_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start.");
    shortkeyTarget_ = name;

    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutString("ShortkeyTarget", shortkeyTarget_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
    SetStatePref(STATE::HIGHCONTRASTTEXT);

    return true;
}

bool AccessibilityAccountData::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start.");
    invertColorState_ = state;
    SetStatePref(STATE::INVERTCOLORSTATE);

    return true;
}

bool AccessibilityAccountData::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start.");
    animationOffState_ = state;
    SetStatePref(STATE::ANIMATIONOFF);

    return true;
}

bool AccessibilityAccountData::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start.");
    audioMonoState_ = state;
    SetStatePref(STATE::AUDIOMONO);

    return true;
}

bool AccessibilityAccountData::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    daltonizationColorFilter_ = filter;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutInt("daltonizationColorFilter", (int)daltonizationColorFilter_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start.");
    contentTimeout_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutInt("contentTimeout", (int)contentTimeout_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start.");
    brightnessDiscount_ = discount;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutFloat("brightnessDiscount", brightnessDiscount_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start.");
    audioBalance_ = balance;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    pref_->PutFloat("audioBalance", audioBalance_);
    pref_->FlushSync();
    return true;
}

bool AccessibilityAccountData::GetScreenMagnificationState()
{
    return isScreenMagnificationState_;
}

bool AccessibilityAccountData::GetShortKeyState()
{
    return isShortKeyState_;
}
bool AccessibilityAccountData::GetMouseKeyState()
{
    return isMouseKeyState_;
}

uint32_t AccessibilityAccountData::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;
    if (isCaptionState_) {
        state |= STATE_CAPTION_ENABLED;
    }

    if (isScreenMagnificationState_) {
        state |= STATE_SCREENMAGNIFIER_ENABLED;
    }

    if (isMouseKeyState_) {
        state |= STATE_MOUSEKEY_ENABLED;
    }

    if (isShortKeyState_) {
        state |= STATE_SHORTKEY_ENABLED;
    }

    if (highContrastTextState_) {
        state |= STATE_HIGHCONTRAST_ENABLED;
    }

    if (invertColorState_) {
        state |= STATE_INVETRTCOLOR_ENABLED;
    }

    if (animationOffState_) {
        state |= STATE_ANIMATIONOFF_ENABLED;
    }

    if (audioMonoState_) {
        state |= STATE_AUDIOMONO_ENABLED;
    }
    return state;
}

int32_t AccessibilityAccountData::GetMouseAutoClick()
{
    return mouseAutoClick_;
}

std::string AccessibilityAccountData::GetShortkeyTarget()
{
    return shortkeyTarget_;
}

bool AccessibilityAccountData::GetHighContrastTextState()
{
    return highContrastTextState_;
}

bool AccessibilityAccountData::GetInvertColorState()
{
    return invertColorState_;
}

bool AccessibilityAccountData::GetAnimationOffState()
{
    return animationOffState_;
}

bool AccessibilityAccountData::GetAudioMonoState()
{
    return audioMonoState_;
}

uint32_t AccessibilityAccountData::GetDaltonizationColorFilter()
{
    return daltonizationColorFilter_;
}

uint32_t AccessibilityAccountData::GetContentTimeout()
{
    return contentTimeout_;
}

float AccessibilityAccountData::GetBrightnessDiscount()
{
    return brightnessDiscount_;
}

float AccessibilityAccountData::GetAudioBalance()
{
    return audioBalance_;
}

void AccessibilityAccountData::ConfigInit(const std::shared_ptr<NativePreferences::Preferences> &pref)
{
    HILOG_DEBUG("start.");
    if (!pref) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref->GetString("ScreenMagnification", "");
    HILOG_DEBUG(" pref->GetString() = %{public}s.", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isScreenMagnificationState_ = true;
    } else {
        isScreenMagnificationState_ = false;
    }

    strValue = pref->GetString("MouseKey", "");
    if (!std::strcmp(strValue.c_str(), "on")) {
        isMouseKeyState_ = true;
    } else {
        isMouseKeyState_ = false;
    }

    strValue = pref->GetString("ShortKey", "");
    if (!std::strcmp(strValue.c_str(), "on")) {
        isShortKeyState_ = true;
    } else {
        isShortKeyState_ = false;
    }

    shortkeyTarget_ = pref->GetString("ShortkeyTarget", "none");
    HILOG_DEBUG(" pref->GetString() = 0x%{public}s.", shortkeyTarget_.c_str());

    mouseAutoClick_ = static_cast<int32_t>(pref->GetInt("MouseAutoClick", -1));
    HILOG_DEBUG(" pref->GetInt() = 0x%{public}x.", mouseAutoClick_);

    daltonizationColorFilter_ = static_cast<uint32_t>(pref->GetInt("daltonizationColorFilter", 0));
    contentTimeout_ = static_cast<uint32_t>(pref->GetInt("contentTimeout", 0));
    brightnessDiscount_ = static_cast<float>(pref->GetFloat("brightnessDiscount", 0));
    audioBalance_ = static_cast<float>(pref->GetFloat("audioBalance", 0));
}

void AccessibilityAccountData::AddConfigCallback(
    const sptr<IAccessibleAbilityManagerConfigObserver>& callback)
{
    HILOG_DEBUG("start.");
    configCallbacks_.push_back(callback);
}

const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> &AccessibilityAccountData::GetConfigCallbacks() const
{
    HILOG_DEBUG("start.");
    return configCallbacks_;
}

void AccessibilityAccountData::RemoveConfigCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("start.");
    for (auto itr = configCallbacks_.begin(); itr != configCallbacks_.end(); itr++) {
        if ((*itr)->AsObject() == callback) {
            configCallbacks_.erase(itr);
            break;
        }
    }
}
} // namespace Accessibility
} // namespace OHOS
