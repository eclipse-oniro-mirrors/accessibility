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
#include "accessibility_display_manager.h"
#include "accessible_ability_manager_service.h"
#include "extension_ability_info.h"
#include "hilog_wrapper.h"
#include "json_utils.h"

namespace OHOS {
namespace Accessibility {
AccessibilityAccountData::AccessibilityAccountData(int accountId)
{
    id_ = accountId;
}

AccessibilityAccountData::~AccessibilityAccountData()
{}

int AccessibilityAccountData::GetAccountId()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return id_;
}

// get client state.
uint32_t AccessibilityAccountData::GetAccessibilityState()
{
    HILOG_DEBUG("%{public}s start.", __func__);
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

// switch the user causes state changed.
void AccessibilityAccountData::OnAccountSwitched()
{
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start. URI is %{public}s", __func__, connection->GetElementName().GetURI().c_str());
    if (!connectedA11yAbilities_.count(connection->GetElementName().GetURI())) {
        connectedA11yAbilities_.insert(make_pair(connection->GetElementName().GetURI(), connection));
    }

    HILOG_DEBUG("Add ConnectedAbility: %{public}d", connectedA11yAbilities_.size());
}

// remove connect ability.
void AccessibilityAccountData::RemoveConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("%{public}s start. URI is %{public}s", __func__, connection->GetElementName().GetURI().c_str());
    std::map<std::string, sptr<AccessibleAbilityConnection>>::iterator it =
        connectedA11yAbilities_.find(connection->GetElementName().GetURI());
    if (it != connectedA11yAbilities_.end()) {
        connectedA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectedAbility: %{public}d", connectedA11yAbilities_.size());
}

void AccessibilityAccountData::AddStateCallback(const sptr<IAccessibleAbilityManagerServiceState>& callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    stateCallbacks_.push_back(callback);
}

// remove IAccessibleAbilityManagerServiceState
void AccessibilityAccountData::RemoveStateCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = stateCallbacks_.begin(); itr != stateCallbacks_.end(); itr++) {
        if (*itr == callback) {
            stateCallbacks_.erase(itr);
            break;
        }
    }
}

void AccessibilityAccountData::AddCaptionPropertyCallback(
    const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    captionPropertyCallbacks_.push_back(callback);
}

void AccessibilityAccountData::RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = captionPropertyCallbacks_.begin(); itr != captionPropertyCallbacks_.end(); itr++) {
        if (*itr == callback) {
            captionPropertyCallbacks_.erase(itr);
            break;
        }
    }
}
// add AccessibilityInteractionConnection
void AccessibilityAccountData::AddAccessibilityWindowConnection(
    const int windowId, const sptr<AccessibilityWindowConnection>& interactionConnection)
{
    HILOG_DEBUG("%{public}s start.windowId(%{public}d)", __func__, windowId);
    if (!asacConnections_.count(windowId)) {
        asacConnections_.insert(std::make_pair(windowId, interactionConnection));
    }
}

// remove AccessibilityWindowConnection
void AccessibilityAccountData::RemoveAccessibilityWindowConnection(const int windowId)
{
    HILOG_DEBUG("%{public}s start.windowId(%{public}d)", __func__, windowId);
    std::map<int, sptr<AccessibilityWindowConnection>>::iterator it = asacConnections_.find(windowId);
    if (it != asacConnections_.end()) {
        asacConnections_.erase(it);
    }
}

void AccessibilityAccountData::AddConnectingA11yAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!connectingA11yAbilities_.count(elementName.GetURI())) {
        connectingA11yAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

void AccessibilityAccountData::RemoveConnectingA11yAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<std::string, AppExecFwk::ElementName>::iterator it = connectingA11yAbilities_.find(elementName.GetURI());
    if (it != connectingA11yAbilities_.end()) {
        connectingA11yAbilities_.erase(it);
    }

    HILOG_DEBUG("Remove ConnectingA11yAbility: %{public}d", connectingA11yAbilities_.size());
}

// For UT
void AccessibilityAccountData::AddEnabledAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!enabledAbilities_.count(elementName.GetURI())) {
        enabledAbilities_.insert(make_pair(elementName.GetURI(), elementName));
    }

    HILOG_DEBUG("Add EnabledAbility: %{public}d", enabledAbilities_.size());
}

void AccessibilityAccountData::RemoveEnabledAbility(const AppExecFwk::ElementName& elementName)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::map<std::string, AppExecFwk::ElementName>::iterator it = enabledAbilities_.find(elementName.GetURI());
    HILOG_DEBUG("Remove EnabledAbility: %{public}d", enabledAbilities_.size());
    if (it != enabledAbilities_.end()) {
        enabledAbilities_.erase(it);
    }
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
    }

    std::string BundleName = elementName.GetBundleName();
    JsonUtils::RemoveVecValue(jsonObj, JSON_OBJECT_ENABLED, BundleName);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);

    HILOG_DEBUG("Remove EnabledAbility: %{public}d", enabledAbilities_.size());

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAbilities();
}

// For UT
void AccessibilityAccountData::AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    installedAbilities_.push_back(abilityInfo);
}

void AccessibilityAccountData::RemoveInstalledAbility(AccessibilityAbilityInfo& abilityInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto it = installedAbilities_.begin(); it != installedAbilities_.end();) {
        if (it->GetPackageName() == abilityInfo.GetPackageName()) {
            installedAbilities_.erase(it);
        } else {
            ++it;
        }
    }

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAbilities();
}

void AccessibilityAccountData::ClearInstalledAbility()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    installedAbilities_.clear();
}

// get AccessibleAbilityConnection
const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetAccessibleAbilityConnection(
    const std::string elementName)
{
    HILOG_DEBUG("%{public}s start. URI is %{public}s", __func__, elementName.c_str());
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

// get AccessibilityWindowConnection.
const sptr<AccessibilityWindowConnection> AccessibilityAccountData::GetAccessibilityWindowConnection(
    const int windowId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (asacConnections_.count(windowId) > 0) {
        return asacConnections_[windowId];
    }

    return nullptr;
}

// get map<std::string, sptr<AccessibleAbilityConnection>> connectedA11yAbilities_
const std::map<std::string, sptr<AccessibleAbilityConnection>> AccessibilityAccountData::GetConnectedA11yAbilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return connectedA11yAbilities_;
}

const std::vector<sptr<IAccessibleAbilityManagerServiceState>> AccessibilityAccountData::GetStateCallbacks()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return stateCallbacks_;
}

// get map<int, sptr<AccessibilityWindowConnection>> asacConnections_
const std::map<int, sptr<AccessibilityWindowConnection>> AccessibilityAccountData::GetAsacConnections()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return asacConnections_;
}

const CaptionPropertyCallbacks AccessibilityAccountData::GetCaptionPropertyCallbacks()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return captionPropertyCallbacks_;
}

// get connectingA11yAbilities_.
const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetConnectingA11yAbilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return connectingA11yAbilities_;
}

// get enabledAbilities_.
const std::map<std::string, AppExecFwk::ElementName> AccessibilityAccountData::GetEnabledAbilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto& abilitie : enabledAbilities_) {
        std::string bundleName = abilitie.second.GetBundleName();
        HILOG_DEBUG("bundleName = %{public}s ", bundleName.c_str());
    }
    return enabledAbilities_;
}

// get installedAbilities_.
const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetInstalledAbilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return installedAbilities_;
}

const std::vector<AccessibilityAbilityInfo> AccessibilityAccountData::GetAbilitiesByState(AbilityStateType state)
{
    HILOG_DEBUG("%{public}s by state(%{public}d) start.", __func__, state);
    if (state == ABILITY_STATE_ENABLE) {
        std::vector<AccessibilityAbilityInfo> enabledAbilities;
        for (auto ability : connectedA11yAbilities_) {
            enabledAbilities.push_back(ability.second->GetAbilityInfo());
        }
        return enabledAbilities;
    } else if (state == ABILITY_STATE_DISABLE) {
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
    } else {
        return installedAbilities_;
    }
}

void AccessibilityAccountData::UpdateAccountCapabilities()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    UpdateFilteringKeyEventsCapability();
    UpdateEventTouchGuideCapability();
    UpdateGesturesSimulationCapability();
    UpdateMagnificationCapability();
}

void AccessibilityAccountData::UpdateEventTouchGuideCapability()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto itr = connectedA11yAbilities_.begin(); itr != connectedA11yAbilities_.end(); itr++) {
        if (itr->second->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_TOUCH_GUIDE) {
            isEventTouchGuideState_ = true;
            return;
        }
    }
    isEventTouchGuideState_ = false; // temp deal
}

void AccessibilityAccountData::UpdateGesturesSimulationCapability()
{
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
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
    HILOG_DEBUG("%{public}s start.", __func__);
    isCaptionState_ = state;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    std::string strValue = "";
    if (isCaptionState_) {
        strValue = STATE_TRUE;
    } else {
        strValue = STATE_FALSE;
    }
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STATE, strValue);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::SetCaptionProperty(const CaptionProperty& caption)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    captionProperty_ = caption;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }
    std::string FONTFAMILY = captionProperty_.GetFontFamily();
    int FONTSCALE = captionProperty_.GetFontScale();
    std::string FONTCOLOR = captionProperty_.GetFontColor();
    std::string FONTEDGETYPE = captionProperty_.GetFontEdgeType();
    std::string BACKGROUNDCOLOR = captionProperty_.GetBackgroundColor();
    std::string WINDOWCOLOR = captionProperty_.GetWindowColor();

    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTFAMILY, FONTFAMILY);
    JsonUtils::SetIntValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTSCALE, FONTSCALE);
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTCOLOR, FONTCOLOR);
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTEDGETYPE, FONTEDGETYPE);
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_BACKGROUNDCOLOR, BACKGROUNDCOLOR);
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_WINDOWCOLOR, WINDOWCOLOR);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);

    return true;
}

bool AccessibilityAccountData::SetEnabled(const bool state)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    isEnabled_ = state;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    std::string strValue = "";
    HILOG_DEBUG("isEnabled_ = %{public}s", isEnabled_ ? "true" : "false");
    if (isEnabled_) {
        strValue = STATE_TRUE;
    } else {
        strValue = STATE_FALSE;
    }
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_ACCESSIBLE, strValue);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    isEventTouchGuideState_ = state;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    std::string strValue = "";
    if (isEventTouchGuideState_) {
        strValue = STATE_TRUE;
    } else {
        strValue = STATE_FALSE;
    }
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_TOUCH_GUIDE, strValue);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::SetGestureState(const bool state)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    isGesturesSimulation_ = state;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    std::string strValue = "";
    if (isGesturesSimulation_) {
        strValue = STATE_TRUE;
    } else {
        strValue = STATE_FALSE;
    }
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_GESTURE, strValue);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    isFilteringKeyEvents_ = state;
    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    std::string strValue = "";
    if (isFilteringKeyEvents_) {
        strValue = STATE_TRUE;
    } else {
        strValue = STATE_FALSE;
    }
    JsonUtils::SetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_KEY_EVENT_BOSERVER, strValue);
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::GetEnabledState()
{
    return isEnabled_;
}

bool AccessibilityAccountData::GetTouchGuideState()
{
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

bool AccessibilityAccountData::SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    // add ability to the last of enabledAbilities_
    for (auto& ability : it) {
        enabledAbilities_.insert(std::pair<std::string, AppExecFwk::ElementName>(ability.first, ability.second));
    }

    for (auto& abilitie : enabledAbilities_) {
        std::string bundleName = abilitie.second.GetBundleName();
        HILOG_DEBUG("bundleName = %{public}s ", bundleName.c_str());
    }

    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }

    std::string BundleName = "";
    for (auto& enableAbility : it) {
        BundleName = enableAbility.second.GetBundleName();
        JsonUtils::SetVecValue(jsonObj, JSON_OBJECT_ENABLED, BundleName);
    }

    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);
    return true;
}

bool AccessibilityAccountData::ReadConfigurationForAccountData()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    // temp deal: read the user confige data.
    return true;
}

// get installedAbilities_.
bool AccessibilityAccountData::GetInstalledAbilitiesFromBMS()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    sptr<AppExecFwk::IBundleMgr> bms = nullptr;
    bms = aams->GetBundleMgrProxy();
    if(bms == nullptr) {
        HILOG_ERROR("GetBundleMgrProxy failed.");
        return false;
    }
    bms->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::ACCESSIBILITY, id_, extensionInfos);
    HILOG_DEBUG("query extensionAbilityInfos' size is %{public}d.", extensionInfos.size());
    for (auto& info : extensionInfos) {
        AccessibilityAbilityInfo* accessibilityInfo = new AccessibilityAbilityInfo(info);
        AddInstalledAbility(*accessibilityInfo);
    }
    return true;
}

void AccessibilityAccountData::CaptionInit(nlohmann::json jsonObj)
{
    std::string FONTFAMILY = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTFAMILY);
    int FONTSCALE = JsonUtils::GetIntValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTSCALE);
    std::string FONTCOLOR = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTCOLOR);
    std::string FONTEDGETYPE = JsonUtils::GetStrValue(jsonObj,
        JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_FONTEDGETYPE);
    std::string BACKGROUNDCOLOR = JsonUtils::GetStrValue(jsonObj,
        JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_BACKGROUNDCOLOR);
    std::string WINDOWCOLOR = JsonUtils::GetStrValue(jsonObj,
        JSON_OBJECT_CAPTION_STYLE, CAPTION_JSON_VALUE_WINDOWCOLOR);

    captionProperty_.SetFontFamily(FONTFAMILY);
    captionProperty_.SetFontScale(FONTSCALE);
    captionProperty_.SetFontColor(FONTCOLOR);
    captionProperty_.SetFontEdgeType(FONTEDGETYPE);
    captionProperty_.SetBackgroundColor(BACKGROUNDCOLOR);
    captionProperty_.SetWindowColor(WINDOWCOLOR);

    std::string strValue = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPTION_STATE);
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (std::strcmp(strValue.c_str(), "on") == 0) {
        isCaptionState_ = true;
    } else {
        isCaptionState_ = false;
    }
}

void AccessibilityAccountData::CapbilityInit(nlohmann::json jsonObj)
{
    std::string strValue = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_ACCESSIBLE);
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (std::strcmp(strValue.c_str(), "on") == 0) {
        isEnabled_ = true;
    } else {
        isEnabled_ = false;
    }

    strValue = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_TOUCH_GUIDE);
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (std::strcmp(strValue.c_str(), "on") == 0) {
        isEventTouchGuideState_ = true;
    } else {
        isEventTouchGuideState_ = false;
    }

    strValue = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_KEY_EVENT_BOSERVER);
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (std::strcmp(strValue.c_str(), "on") == 0) {
        isFilteringKeyEvents_ = true;
    } else {
        isFilteringKeyEvents_ = false;
    }

    strValue = JsonUtils::GetStrValue(jsonObj, JSON_OBJECT_CAPABILITY, CAPABILITY_JSON_VALUE_GESTURE);
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (std::strcmp(strValue.c_str(), "on") == 0) {
        isGesturesSimulation_ = true;
    } else {
        isGesturesSimulation_ = false;
    }
}

void AccessibilityAccountData::EnabledListInit(nlohmann::json jsonObj)
{
    std::string BundleName = "";
    AccessibilityAbilityInfo abilityInfo;

    std::vector<std::string> vecvalue = JsonUtils::GetVecValue(jsonObj, JSON_OBJECT_ENABLED);
    for (auto i = vecvalue.begin(); i != vecvalue.end(); i++) {
        BundleName = *i;
        HILOG_DEBUG("BundleName = %{public}s", BundleName.c_str());
        AppExecFwk::ElementName elementName("", BundleName, "");
        enabledAbilities_.insert(std::pair<std::string, AppExecFwk::ElementName>(elementName.GetURI(), elementName));
    }
}

void AccessibilityAccountData::init()
{
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
    }
    CaptionInit(jsonObj);
    CapbilityInit(jsonObj);
    EnabledListInit(jsonObj);
}

bool AccessibilityAccountData::DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    for (auto &disAbilities : it) {
        enabledAbilities_.erase(disAbilities.first);
    }

    // temp deal: write json config
    nlohmann::json jsonObj;
    if (!JsonUtils::GetJsonObjFromJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH)) {
        HILOG_ERROR("Get JsonObj from json failed.");
        return false;
    }
    std::string BundleName = "";
    for (auto& disableAbility : it) {
        BundleName = disableAbility.second.GetBundleName();
        HILOG_DEBUG("BundleName = %{public}s", BundleName.c_str());
        JsonUtils::RemoveVecValue(jsonObj, JSON_OBJECT_ENABLED, BundleName);
    }
    JsonUtils::ToJson(jsonObj, AccessibleAbility_Config_JSON_FILE_PATH);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->UpdateAbilities();
    return true;
}

void AccessibilityAccountData::AddUITestConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!uiTestConnectedA11yAbility_) {
        HILOG_DEBUG("Add uiTestConnectedA11yAbility success");
        uiTestConnectedA11yAbility_ = connection;
    }
}

void AccessibilityAccountData::RemoveUITestConnectedAbility(sptr<AccessibleAbilityConnection>& connection)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    uiTestConnectedA11yAbility_ = nullptr;
}

const sptr<AccessibleAbilityConnection> AccessibilityAccountData::GetUITestConnectedAbilityConnection()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return uiTestConnectedA11yAbility_;
}
}  // namespace Accessibility
}  // namespace OHOS