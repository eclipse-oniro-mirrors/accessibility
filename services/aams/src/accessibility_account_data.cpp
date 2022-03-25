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

namespace OHOS {
namespace Accessibility {
const static std::string PREF_PATH =
    "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";

AccessibilityAccountData::AccessibilityAccountData(int accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int AccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("start.");
    return id_;
}

uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;
    if (!connectedA11yAbilities_.empty() || !connectingA11yAbilities_.empty()) {
        state |= AccessibilitySystemAbilityClient::STATE_ACCESSIBILITY_ENABLED;
        if (!isEnabled_) {
            SetEnabled(true);
        }
    } else {
        if (isEnabled_) {
            SetEnabled(false);
        }
    }

    if (isEventTouchGuideState_) {
        state |= AccessibilitySystemAbilityClient::STATE_EXPLORATION_ENABLED;
    }

    if (isCaptionState_) {
        state |= AccessibilitySystemAbilityClient::STATE_CAPTION_ENABLED;
    }

    if (isFilteringKeyEvents_) {
        state |= AccessibilitySystemAbilityClient::STATE_KEYEVENT_ENABLED;
    }

    if (isGesturesSimulation_) {
        state |= AccessibilitySystemAbilityClient::STATE_GESTURE_ENABLED;
    }

    return state;
}

void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_DEBUG("start.");
    // Reset AccessibleAbilityConnection
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

void AccessibilityAccountData::AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    if (!connectedA11yAbilities_.count(connection->GetElementName().GetURI())) {
        connectedA11yAbilities_.insert(make_pair(connection->GetElementName().GetURI(), connection));
    }

    HILOG_DEBUG("Add ConnectedAbility: %{public}d", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("URI is %{public}s", connection->GetElementName().GetURI().c_str());
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it =
        connectedA11yAbilities_.find(connection->GetElementName().GetURI());
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectedAbility: %{public}d", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerServiceState>& callback)
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
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback)
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

void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    std::map<int, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("start.");
    if (!connectingA11yAbilities_.count(elementName.GetURI())) {
        connectingA11yAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("start.");
    std::map<std::string, AppExecFwk::ElementName>::iterator it = connectingA11yAbilities_.find(elementName.GetURI());
    if (it != connectingA11yAbilities_.end()) {
        connectingA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::AddEnabledAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("start.");
    if (!enabledAbilities_.count(elementName.GetURI())) {
        enabledAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add EnabledAbility: %{public}d", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledFromPref(const std::string bundleName)
{
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }
    std::string strValue = pref_->GetString("BundleName", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());

    std::vector<std::string> vecvalue;
    StringToVector(strValue, vecvalue);
    for (std::vector<std::string>::iterator val = vecvalue.begin();val != vecvalue.end();) {
        if (!std::strcmp(val->c_str(), bundleName.c_str())) {
            val = vecvalue.erase(val);
            HILOG_DEBUG("remove val = %{public}s", val->c_str());
        } else {
            ++val;
        }
    }
    std::string stringOut = "";
    VectorToString(vecvalue, stringOut);
    int errCode = pref_->PutString("BundleName", stringOut);
    if (errCode) {
        HILOG_ERROR("pref_->PutString() error(%{public}d).", errCode);
    }

    errCode = pref_->FlushSync();
    if (errCode) {
        HILOG_ERROR("pref_->FlushSync() error(%{public}d).", errCode);
    }
}

void AccessibilityAccountData::RemoveEnabledAbility(const AppExecFwk::ElementName& elementName)
{
    std::map<std::string, AppExecFwk::ElementName>::iterator it = enabledAbilities_.find(elementName.GetURI());
    HILOG_DEBUG("EnabledAbility size(%{public}d)", enabledAbilities_.size());
    if (it != enabledAbilities_.end()) {
        HILOG_DEBUG("Removed %{public}s from EnabledAbility: ", elementName.GetBundleName().c_str());
        enabledAbilities_.erase(it);
    }
    RemoveEnabledFromPref(elementName.GetBundleName());
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
    HILOG_DEBUG("push back installed ability successfully and installedAbilities_'s size is %{public}d",
        installedAbilities_.size());
}

void AccessibilityAccountData::RemoveInstalledAbility(std::string bundleName)
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
    const std::string elementName)
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
    const int windowId)
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

const std::vector<sptr<IAccessibleAbilityManagerServiceState>> AccessibilityAccountData::GetStateCallbacks()
{
    HILOG_DEBUG("start.");
    return stateCallbacks_;
}

const std::map<int, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("start.");
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("start.");
    return captionPropertyCallbacks_;
}

const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("start.");
    return connectingA11yAbilities_;
}

const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("enabledAbilities_ size is (%{public}d).", enabledAbilities_.size());
    for (auto& abilitie : enabledAbilities_) {
        std::string bundleName = abilitie.second.GetBundleName();
        HILOG_DEBUG("bundleName = %{public}s ", bundleName.c_str());
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
    HILOG_DEBUG("state(%{public}d) start.", state);
    if (state == ABILITY_STATE_ENABLE) {
        std::vector<AccessibilityAbilityInfo> enabledAbilities;
        for (auto ability : connectedA11yAbilities_) {
            enabledAbilities.push_back(ability.second->GetAbilityInfo());
        }
        HILOG_DEBUG("connectedA11yAbilities_ is (%{public}d) enabledAbilities is (%{public}d).",
            connectedA11yAbilities_.size(), enabledAbilities.size());
        return enabledAbilities;
    } else if (state == ABILITY_STATE_DISABLE) {
        std::vector<AccessibilityAbilityInfo> disabledAbilities = GetDisableAbilities();
        HILOG_DEBUG("the size of disable abilities is %{public}d", disabledAbilities.size());
        return disabledAbilities;
    } else {
        return installedAbilities_;
    }
}

const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetDisableAbilities()
{
    HILOG_DEBUG("start.");
    std::vector<AccessibilityAbilityInfo> disabledAbilities = installedAbilities_;
    for (auto enabledAbility : connectedA11yAbilities_) {
        for (auto itr = disabledAbilities.begin(); itr != disabledAbilities.end();) {
            if (itr->GetId() == enabledAbility.second->GetAbilityInfo().GetId()) {
                disabledAbilities.erase(itr);
            } else {
                itr++;
            }
        }
    }
    return disabledAbilities;
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
    int FONTSCALE = captionProperty_.GetFontScale();
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

bool AccessibilityAccountData::SetCaptionProperty(const CaptionProperty& caption)
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

bool AccessibilityAccountData::SetStatePref(int type)
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
    std::string bundleName = "";
    std::vector<std::string> vecvalue;
    for (auto& abilitie : enabledAbilities_) {
        bundleName = abilitie.second.GetBundleName();
        vecvalue.push_back(bundleName);
        HILOG_DEBUG("bundleName = %{public}s ", bundleName.c_str());
    }
    std::string stringOut = "";
    VectorToString(vecvalue, stringOut);
    pref_->PutString("BundleName", stringOut);
    pref_->FlushSync();
}

bool AccessibilityAccountData::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start.");
    for (auto& ability : it) {
        enabledAbilities_.insert(std::pair<std::string, AppExecFwk::ElementName>(ability.first, ability.second));
    }
    UpdateEnabledFromPref();
    return true;
}

bool AccessibilityAccountData::ReadConfigurationForAccountData()
{
    HILOG_DEBUG("start.");
    // temp deal: read the user confige data.
    return true;
}

bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("start.");

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return false;
    }
    sptr<AppExecFwk::IBundleMgr> bms = nullptr;
    bms = aams->GetBundleMgrProxy();
    if (!bms) {
        HILOG_ERROR("GetBundleMgrProxy failed.");
        return false;
    }
    bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}d.", extensionInfos.size());
    for (auto& info : extensionInfos) {
        std::shared_ptr<AccessibilityAbilityInfo> accessibilityInfo =
            std::make_shared<AccessibilityAbilityInfo>(info);
        AddInstalledAbility(*accessibilityInfo);
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

    int FONTSCALE = pref->GetInt("fontScale", 0);
    HILOG_DEBUG(" pref->GetString() = %{public}d.", FONTSCALE);

    uint32_t FONTCOLOR = (uint32_t)pref->GetInt("fontColor", 0xff000000);
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", FONTCOLOR);

    std::string FONTEDGETYPE = pref->GetString("fontEdgeType", "none");
    HILOG_DEBUG(" pref->GetString() = 0x%{public}s.", FONTEDGETYPE.c_str());

    uint32_t BACKGROUNDCOLOR = (uint32_t)pref->GetInt("backgroundColor", 0xff000000);
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", BACKGROUNDCOLOR);

    uint32_t WINDOWCOLOR = (uint32_t)pref->GetInt("windowColor", 0xff000000);
    HILOG_DEBUG(" pref->GetString() = 0x%{public}x.", WINDOWCOLOR);

    captionProperty_.SetFontFamily(FONTFAMILY);
    captionProperty_.SetFontScale(FONTSCALE);
    captionProperty_.SetFontColor(FONTCOLOR);
    captionProperty_.SetFontEdgeType(FONTEDGETYPE);
    captionProperty_.SetBackgroundColor(BACKGROUNDCOLOR);
    captionProperty_.SetWindowColor(WINDOWCOLOR);
}

void AccessibilityAccountData::CapbilityInit(const std::shared_ptr<NativePreferences::Preferences> &pref)
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

    std::string BundleName = "";
    AccessibilityAbilityInfo abilityInfo;

    std::string strValue = pref->GetString("BundleName", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());

    std::vector<std::string> vecvalue;
    StringToVector(strValue, vecvalue);
    for (auto i = vecvalue.begin(); i != vecvalue.end(); i++) {
        BundleName = *i;
        HILOG_DEBUG("BundleName = %{public}s", BundleName.c_str());
        AppExecFwk::ElementName elementName("", BundleName, "");
        enabledAbilities_.insert(std::pair<std::string, AppExecFwk::ElementName>(elementName.GetURI(), elementName));
    }
}

void AccessibilityAccountData::StringToVector(std::string &stringIn, std::vector<std::string> &vectorResult)
{
    HILOG_DEBUG("start.");
    int strLength = (int)stringIn.size();
    std::vector<int> position;

    if (strLength == 0) {
        return;
    }

    for (int j = 0; j < strLength; j++) {
        if (stringIn[j] == ',') {
            position.push_back(j);
        }
    }

    int wrodCount = (int)position.size();
    if ((wrodCount == 0) && (strLength > 0)) {
        vectorResult.push_back(stringIn);
    } else {
        int startWrod = 0;
        int length = 0;
        for (int i = 0; i <= wrodCount; i++) {
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

void AccessibilityAccountData::init()
{
    int errCode = 0;
    pref_ = NativePreferences::PreferencesHelper::GetPreferences(PREF_PATH + "100.xml", errCode);
    if (errCode) {
        HILOG_ERROR("GetPreferences failed! errCode(%{public}d).", errCode);
        return;
    }

    CaptionInit(pref_);
    CapbilityInit(pref_);
    EnabledListInit(pref_);
}

bool AccessibilityAccountData::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("start.");
    for (auto &disableAbility : it) {
        HILOG_DEBUG("DisableAbilities URI(%{public}s)", disableAbility.first.c_str());
        enabledAbilities_.erase(disableAbility.first);
        RemoveEnabledFromPref(disableAbility.second.GetBundleName());
    }
    return true;
}

void AccessibilityAccountData::VectorToString(std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG("start.");
    int i = 0;
    for (auto& var : vectorVal) {
        if (i > 0) {
            stringOut = stringOut + ',';
        }
        stringOut = stringOut + var.c_str();
        i++;
    }
    HILOG_DEBUG("end stringOUT = %{public}s .", stringOut.c_str());
}
}  // namespace Accessibility
}  // namespace OHOS
