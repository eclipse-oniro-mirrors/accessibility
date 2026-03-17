/*
 * Copyright (C) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_EXTENSION_ABILITY_MANAGER_H
#define ACCESSIBILITY_EXTENSION_ABILITY_MANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <functional>

#include "accessibility_ability_info.h"
#include "accessible_ability_connection.h"
#include "accessibility_settings_config.h"
#include "iaccessibility_enable_ability_lists_observer.h"
#include "iaccessibility_enable_ability_callback_observer.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityAccountData;

class ExtensionAbilityManager {
public:
    ExtensionAbilityManager() = default;
    ~ExtensionAbilityManager();

    void AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection);
    void RemoveConnectedAbility(const AppExecFwk::ElementName &element);
    void RemoveConnectedAbilityByUri(const std::string &uri);
    void RemoveConnectedAbilityByName(const std::string &bundleName, bool& result);
    sptr<AccessibleAbilityConnection> GetConnectedAbilityByName(const std::string &elementName);
    void GetConnectedAbilities(std::vector<sptr<AccessibleAbilityConnection>>& connectionList);
    void GetConnectedAbilitiesInfo(std::vector<AccessibilityAbilityInfo>& abilities);
    void GetConnectedAbilitiesMap(std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap);
    bool HasConnectedCapability(Capability capability);
    void ClearConnectedAbilities();
    size_t GetConnectedAbilitiesSize();

    void AddConnectingAbility(const std::string &uri, const sptr<AccessibleAbilityConnection> &connection);
    void RemoveConnectingAbility(const std::string &uri);
    sptr<AccessibleAbilityConnection> GetConnectingAbility(const std::string &uri);
    size_t GetConnectingAbilitiesSize();

    void AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection>& connection);
    void RemoveWaitDisconnectAbility(const std::string &uri);
    const sptr<AccessibleAbilityConnection> GetWaitDisConnectAbility(const std::string &elementName);
    size_t GetWaitDisconnectAbilitiesSize();

    void AddAppStateObserverAbility(const std::string& uri, const sptr<AccessibleAbilityConnection>& connection);
    void RemoveAppStateObserverAbility(const std::string& uri);
    sptr<AccessibleAbilityConnection> GetAppStateObserverAbility(const std::string& uri);

    void AddEnabledAbility(const std::string &name);
    RetError RemoveEnabledAbility(const std::string &name);
    const std::vector<std::string> &GetEnabledAbilities() const;

    void AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo);
    void RemoveInstalledAbility(const std::string &bundleName);
    void ClearInstalledAbility();
    const std::vector<AccessibilityAbilityInfo> &GetInstalledAbilities() const;

    void GetAbilitiesByState(AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities);
    void GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities);

    void AddEnableAbilityListsObserver(const sptr<IAccessibilityEnableAbilityListsObserver>& observer);
    void RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer);
    void UpdateEnableAbilityListsState();
    void UpdateInstallAbilityListsState();

    void AddEnableAbilityCallbackObserver(const sptr<IAccessibilityEnableAbilityCallbackObserver>& observer);
    void RemoveEnableAbilityCallbackObserver(const wptr<IRemoteObject>& observer);
    void CallEnableAbilityCallback(const std::string &uri);

    void NotifyExtensionServiceDeath(const std::string& uri);

    void UpdateAbilities(
        const std::string& callerBundleName,
        int32_t accountId,
        uint32_t& connectCounter,
        std::function<sptr<AccessibleAbilityConnection>(
            int32_t, int32_t, AccessibilityAbilityInfo&)> connectionCreator);
    bool RemoveAbility(const std::string &bundleName);
    void AddAbility(const std::string &bundleName, 
        const std::vector<AccessibilityAbilityInfo>& abilityInfos,
        std::function<bool(const std::string&)> autoStartChecker);
    void ChangeAbility(const std::string &bundleName,
        std::function<bool(const std::string&)> autoStartChecker,
        std::function<void(const std::string&, bool)> autoStartSetter);

    void SetAbilityAutoStartState(const std::string &name, const bool state);
    bool GetAbilityAutoStartState(const std::string &name);
    void DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName);

    void AddUITestClient(const sptr<IRemoteObject> &obj,
        const std::string &bundleName, const std::string &abilityName);
    void RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection, const std::string &bundleName);

    void GetImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities) const;
    void UpdateImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities);
    void UpdateAutoStartEnabledAbilities();
    void Clear();

    bool IsExistCapability(Capability capability);
    int32_t GetSizeByUri(const std::string& uri);

    class AccessibilityAbility {
    public:
        AccessibilityAbility() = default;
        ~AccessibilityAbility() = default;
        void AddAccessibilityAbility(const std::string& uri, const sptr<AccessibleAbilityConnection>& connection);
        sptr<AccessibleAbilityConnection> GetAccessibilityAbilityByName(const std::string& elementName);
        sptr<AccessibleAbilityConnection> GetAccessibilityAbilityByUri(const std::string& uri);
        void GetAccessibilityAbilities(std::vector<sptr<AccessibleAbilityConnection>>& connectionList);
        void GetAbilitiesInfo(std::vector<AccessibilityAbilityInfo>& abilities);
        bool IsExistCapability(Capability capability);
        void GetAccessibilityAbilitiesMap(std::map<std::string, sptr<AccessibleAbilityConnection>>& connectionMap);
        void GetDisableAbilities(std::vector<AccessibilityAbilityInfo>& disabledAbilities);
        int32_t GetSizeByUri(const std::string& uri);
        void RemoveAccessibilityAbilityByName(const std::string& bundleName, bool& result);
        void RemoveAccessibilityAbilityByUri(const std::string& uri);
        void Clear();
        size_t GetSize();
    private:
        std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap_;
        ffrt::mutex mutex_;
    };

private:
    bool IsAbilityEnabled(const std::string& uri);
    bool ConnectAbility(
        sptr<AccessibleAbilityConnection>& connection,
        const AccessibilityAbilityInfo& ability,
        const std::string& callerBundleName);
    void DisconnectAbility(
        sptr<AccessibleAbilityConnection>& connection,
        const std::string& uri);

private:
    int32_t accountId_;
    AccessibilityAbility connectedA11yAbilities_;
    AccessibilityAbility connectingA11yAbilities_;
    AccessibilityAbility waitDisconnectA11yAbilities_;
    AccessibilityAbility appStateObserverAbilities_;
    std::vector<AccessibilityAbilityInfo> installedAbilities_;
    std::vector<std::string> enabledAbilities_;
    std::shared_ptr<AccessibilitySettingsConfig> config_;
    std::vector<sptr<IAccessibilityEnableAbilityListsObserver>> enableAbilityListsObservers_;
    ffrt::mutex enableAbilityListObserversMutex_; // mutex for enableAbilityListsObservers_
    std::vector<sptr<IAccessibilityEnableAbilityCallbackObserver>> enableAbilityCallbackObservers_;
    ffrt::mutex enableAbilityCallbackObserversMutex_; // mutex for enableAbilityCallbackObservers_
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBILITY_EXTENSION_ABILITY_MANAGER_H