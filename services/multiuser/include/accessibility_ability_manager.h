/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef ACCESSIBILITY_ABILITY_MANAGER_H
#define ACCESSIBILITY_ABILITY_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include "accessibility_ability_info.h"
#include "accessibility_settings_config.h"
#include "accessible_ability_connection.h"
#include "element_name.h"
#include "ffrt.h"
#include "iaccessibility_enable_ability_lists_observer.h"
#include "iremote_object.h"
#include "os_account_info.h"


namespace OHOS {
namespace Accessibility {
constexpr const char *UI_TEST_BUNDLE_NAME = "ohos.uitest";
constexpr const char *SCREEN_READER_BUNDLE_NAME = "com.huawei.hmos.screenreader";
constexpr const char *SCREEN_READER_ABILITY = "com.huawei.hmos.screenreader/AccessibilityExtAbility";
constexpr const char *SCREEN_READER_KEY = "accessibility_screenreader_enabled";

class AccessibilityAbilityManager {
public:
    /**
     * Constructor for AccessibilityAbilityManager
     * @param accountId Account ID for the manager
     */
    AccessibilityAbilityManager(int32_t accountId);
    /**
     * Destructor for AccessibilityAbilityManager
     */
    ~AccessibilityAbilityManager();

    /**
     * Set the configuration for accessibility settings
     * @param config Shared pointer to AccessibilitySettingsConfig
     */
    void SetConfig(std::shared_ptr<AccessibilitySettingsConfig> config);
    /**
     * Add a connected accessibility ability (thread-safe)
     * @param connection Ability connection object
     */
    void AddConnectedAbility(sptr<AccessibleAbilityConnection> &connection);
    /**
     * Remove a connected accessibility ability (thread-safe)
     * @param element Ability element name
     */
    void RemoveConnectedAbility(const AppExecFwk::ElementName &element);
    /**
     * Remove a connecting accessibility ability (thread-safe)
     * @param uri Unique identifier of the ability
     */
    void RemoveConnectingA11yAbility(const std::string &uri);

    /**
     * Get the wait disconnect ability by URI (thread-safe)
     * @param uri Unique identifier of the ability
     * @return Ability connection object
     */
    const sptr<AccessibleAbilityConnection> GetWaitDisConnectAbility(const std::string &uri);
    /**
     * Add a wait disconnect ability (thread-safe)
     * @param connection Ability connection object
     */
    void AddWaitDisconnectAbility(sptr<AccessibleAbilityConnection> &connection);
    /**
     * Remove a wait disconnect ability (thread-safe)
     * @param uri Unique identifier of the ability
     */
    void RemoveWaitDisconnectAbility(const std::string &uri);
    /**
     * Add an observer for ability list changes (thread-safe)
     * @param observer Observer object
     */
    void AddEnableAbilityListsObserver(const sptr<IAccessibilityEnableAbilityListsObserver> &observer);
    /**
     * Remove an observer for ability list changes (thread-safe)
     * @param observer Observer object
     */
    void RemoveEnableAbilityListsObserver(const wptr<IRemoteObject> &observer);
    /**
     * Add a connecting accessibility ability (thread-safe)
     * @param uri Unique identifier of the ability
     * @param connection Ability connection object
     */
    void AddConnectingA11yAbility(const std::string &uri, const sptr<AccessibleAbilityConnection> &connection);
    /**
     * Add an enabled ability, for unit test only (thread-safe)
     * @param name Ability name
     */
    void AddEnabledAbility(const std::string &name);
    /**
     * Remove an enabled ability (thread-safe)
     * @param name Ability name
     * @return Operation result
     */
    RetError RemoveEnabledAbility(const std::string &name);
    /**
     * Add an installed ability, for unit test only (thread-safe)
     * @param abilityInfo Ability info
     */
    void AddInstalledAbility(AccessibilityAbilityInfo &abilityInfo);
    /**
     * Remove an installed ability (thread-safe)
     * @param bundleName Bundle name
     */
    void RemoveInstalledAbility(const std::string &bundleName);
    /**
     * Clear all installed abilities (thread-safe)
     */
    void ClearInstalledAbility();
    /**
     * Get all connected accessibility abilities (thread-safe)
     * @return Map of connected abilities
     */
    const std::map<std::string, sptr<AccessibleAbilityConnection>> GetConnectedA11yAbilities();
    /**
     * Get a connected ability by element name (thread-safe)
     * @param elementName Element name
     * @return Ability connection object
     */
    const sptr<AccessibleAbilityConnection> GetAccessibleAbilityConnection(const std::string &elementName);
    /**
     * Get a connecting ability by uri (thread-safe)
     * @param uri Unique identifier of the ability
     * @return Ability connection object
     */
    sptr<AccessibleAbilityConnection> GetConnectingA11yAbility(const std::string &uri);
    /**
     * Get the count of connecting abilities
     * @return Count of connecting abilities
     */
    size_t GetConnectingAbilitiesCount() const;
    /**
     * Filter ability list by state (thread-safe)
     * @param state Target state
     * @param abilities Output ability list
     */
    void GetAbilitiesByState(AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities);
    /**
     * Get all disabled abilities (thread-safe)
     * @param disabledAbilities Output disabled ability list
     */
    void GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities);
    /**
     * Get all enabled ability names (thread-safe)
     * @return List of enabled ability names
     */
    const std::vector<std::string> &GetEnabledAbilities();
    /**
     * Get all installed ability info (thread-safe)
     * @return List of installed ability info
     */
    const std::vector<AccessibilityAbilityInfo> &GetInstalledAbilities() const;
    /**
     * Sync installed abilities from BMS (thread-safe, mock implementation)
     * @return Success or not
     */
    bool GetInstalledAbilitiesFromBMS();
    /**
     * Enable a specific ability (thread-safe)
     * @param name Ability name
     * @param capabilities Ability flags
     * @return Operation result
     */
    RetError EnableAbility(const std::string &name, const uint32_t capabilities);
    /**
     * Notify all observers that the ability list has changed (thread-safe)
     */
    void UpdateEnableAbilityListsState();
    /**
     * Sync installed ability list state (thread-safe)
     */
    void UpdateInstallAbilityListsState();
    /**
     * Update ability list (thread-safe)
     */
    void UpdateAbilities();
    /**
     * Remove an ability by bundle name (thread-safe)
     * @param bundleName Bundle name
     * @return Whether removed successfully
     */
    bool RemoveAbility(const std::string &bundleName);
    /**
     * Add an ability by bundle name (thread-safe)
     * @param bundleName Bundle name
     */
    void AddAbility(const std::string &bundleName);
    /**
     * Listen for ability changes by bundle name (thread-safe)
     * @param bundleName Bundle name
     */
    void ChangeAbility(const std::string &bundleName);
    /**
     * Add UI test client ability (thread-safe)
     * @param obj Client object
     * @param bundleName Bundle name
     * @param abilityName Ability name
     */
    void AddUITestClient(const sptr<IRemoteObject> &obj, const std::string &bundleName, const std::string &abilityName);
    /**
     * Remove UI test client ability (thread-safe)
     * @param connection Ability connection object
     * @param bundleName Bundle name
     */
    void RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection, const std::string &bundleName);
    /**
     * Set the auto start state for an ability (thread-safe)
     * @param name Ability name
     * @param state Auto start state
     */
    void SetAbilityAutoStartState(const std::string &name, const bool state);
    /**
     * Get the auto start state for an ability (thread-safe)
     * @param name Ability name
     * @return Auto start state
     */
    bool GetAbilityAutoStartState(const std::string &name);
    /**
     * Delete the auto start state for an bundleName (thread-safe)
     * @param bundleName bundle name
     */
    void DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName);
    /**
     * Try to set the screen reader state for a specific ability (thread-safe)
     * @param ability Ability name
     * @param state Screen reader state
     */
    void TryToSetScreenReaderState(const std::string &ability, bool state);
    /**
     * Set the screen reader state (thread-safe)
     * @param state Screen reader state
     */
    void SetScreenReaderState(const bool state);
    /**
     * Get the screen reader state (thread-safe)
     * @return Screen reader state
     */
    bool GetScreenReaderState();
    /**
     * Get the default user screen reader state (thread-safe)
     * @return Default user screen reader state
     */
    bool GetDefaultUserScreenReaderState();

    /**
     * Add need event for an ability (thread-safe)
     * @param name Ability name
     * @param needEvents List of needed events
     */
    void AddNeedEvent(const std::string &name, const std::vector<uint32_t> &needEvents);
    /**
     * Remove need event for an ability (thread-safe)
     * @param name Ability name
     */
    void RemoveNeedEvent(const std::string &name);
    /**
     * Update the list of needed events (thread-safe)
     * @return Updated list of needed events
     */
    std::vector<uint32_t> UpdateNeedEvents();
    /**
     * Get the list of needed events (thread-safe)
     * @return List of needed events
     */
    std::vector<uint32_t> GetNeedEvents() const;

    /**
     * Handle account switching by clearing all connections and abilities (thread-safe)
     */
    void OnAccountSwitched();

    /**
     * Check if a capability exists among abilities (thread-safe)
     * @param capability Capability to check
     * @return True if exists, false otherwise
     */
    bool IsExistCapability(Capability capability);
    /**
     * Get important enabled abilities (thread-safe)
     * @param importantEnabledAbilities Output map of important enabled abilities
     */
    void GetImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities) const;
    /**
     * Update important enabled abilities (thread-safe)
     * @param importantEnabledAbilities Map of important enabled abilities
     */
    void UpdateImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities);
    /**
     * Update auto start enabled abilities (thread-safe)
     */
    void UpdateAutoStartEnabledAbilities();

private:
    class AccessibilityAbility {
    public:
        AccessibilityAbility() = default;
        ~AccessibilityAbility() = default;
        void AddAccessibilityAbility(const std::string &uri, const sptr<AccessibleAbilityConnection> &connection);
        sptr<AccessibleAbilityConnection> GetAccessibilityAbilityByName(const std::string &elementName);
        sptr<AccessibleAbilityConnection> GetAccessibilityAbilityByUri(const std::string &uri);
        void GetAccessibilityAbilities(std::vector<sptr<AccessibleAbilityConnection>> &connectionList);
        void GetAbilitiesInfo(std::vector<AccessibilityAbilityInfo> &abilities);
        bool IsExistCapability(Capability capability);
        void GetAccessibilityAbilitiesMap(std::map<std::string, sptr<AccessibleAbilityConnection>> &connectionMap);
        void GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities);
        int32_t GetSizeByUri(const std::string &uri);
        void RemoveAccessibilityAbilityByName(const std::string &bundleName, bool &result);
        void RemoveAccessibilityAbilityByUri(const std::string &uri);
        void Clear();
        size_t GetSize() const;

    private:
        std::map<std::string, sptr<AccessibleAbilityConnection>> connectionMap_;
        mutable ffrt::mutex mutex_;
    };

private:
    // Ability-related locks
    ffrt::mutex managerMutex_;
    ffrt::mutex autoStartStateMutex_;
    ffrt::mutex screenReaderMutex_;
    mutable ffrt::mutex abilitiesMutex_;
    mutable ffrt::mutex connectingAbilitiesMutex_;
    mutable ffrt::mutex connectedAbilitiesMutex_;
    mutable ffrt::mutex installedAbilitiesMutex_;
    // Event-related locks
    mutable ffrt::mutex eventsMutex_;
    // Observer-related locks
    ffrt::mutex observersMutex_;
    int32_t accountId_ = -1;
    AccountSA::OsAccountType accountType_ = AccountSA::OsAccountType::END;
    uint32_t connectCounter_ = 1;
    std::shared_ptr<AccessibilitySettingsConfig> config_ = nullptr;

    std::map<std::string, std::vector<uint32_t>> abilityNeedEvents_;
    std::vector<uint32_t> needEvents_;
    bool screenReaderState_ = false;

    AccessibilityAbility connectedA11yAbilities_;      // key: bundleName/abilityName
    AccessibilityAbility connectingA11yAbilities_;     // key: bundleName/abilityName
    AccessibilityAbility waitDisconnectA11yAbilities_; // key: bundleName/abilityName

    std::vector<AccessibilityAbilityInfo> installedAbilities_;
    std::vector<std::string> enabledAbilities_; // bundleName/abilityName

    std::vector<sptr<IAccessibilityEnableAbilityListsObserver>> enableAbilityListsObservers_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_MANAGER_H