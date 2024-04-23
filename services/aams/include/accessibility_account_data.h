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
#ifndef ACCESSIBILITY_ACCOUNT_DATA_H
#define ACCESSIBILITY_ACCOUNT_DATA_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "accessibility_caption.h"
#include "i_accessibility_enable_ability_lists_observer.h"
#include "accessibility_settings_config.h"
#include "accessibility_window_connection.h"
#include "accessible_ability_connection.h"
#include "i_accessible_ability_manager_caption_observer.h"
#include "i_accessible_ability_manager_config_observer.h"
#include "i_accessible_ability_manager_state_observer.h"
#include "element_name.h"
#include "accessibility_setting_provider.h"
#include "os_account_info.h"

namespace OHOS {
namespace Accessibility {
using CaptionPropertyCallbacks = std::vector<sptr<IAccessibleAbilityManagerCaptionObserver>>;
class AccessibleAbilityConnection;
class AccessibilityWindowConnection;

struct ConfigValueAtoHosUpdate {
    bool highContrastText = false;
    bool invertColor = false;
    bool audioMono = false;
    bool daltonizationState = false;
    float audioBalance = 0.0;
    bool isScreenReaderEnabled = false;
    int clickResponseTime = 0;
    bool ignoreRepeatClickState = false;
    int ignoreRepeatClickTime = 0;
    int displayDaltonizer = 0;
};

class AccessibilityAccountData final : public RefBase {
public:
    AccessibilityAccountData(int32_t accountId);
    ~AccessibilityAccountData();

    /**
     * @brief Get the ID of the account.
     * @return Returns accountId.
     */
    int32_t GetAccountId();

    /**
     * @brief Get Accessibility state.
     * @return returns whether accessibility is connected and whether can touch browse.
     */
    uint32_t GetAccessibilityState();

    /**
     * @brief Switch users and clear old user data.
     */
    void OnAccountSwitched();

    /**
     * @brief Add connected accessibility services.
     * @param connection Accessible ability connection.
     */
    void AddConnectedAbility(sptr<AccessibleAbilityConnection>& connection);

    /**
     * @brief Remove connected accessibility services.
     * @param element The element name of ability.
     */
    void RemoveConnectedAbility(const AppExecFwk::ElementName &element);

    /**
     * @brief Remove connecting accessibility services.
     * @param uri bundle name + / + ability name.
     */
    void RemoveConnectingA11yAbility(const std::string &uri);

    void AddCaptionPropertyCallback(const sptr<IAccessibleAbilityManagerCaptionObserver>& callback);
    void RemoveCaptionPropertyCallback(const wptr<IRemoteObject>& callback);

    void AddEnableAbilityListsObserver(const sptr<IAccessibilityEnableAbilityListsObserver>& observer);
    void RemoveEnableAbilityListsObserver(const wptr<IRemoteObject>& observer);

    /**
     * @brief Add interface operation interactive connection.
     * @param windowId Interface operation interactive connection the
     * corresponding window id.
     * @param interactionConnection Interface interface operation
     * interactive connection.
     */
    void AddAccessibilityWindowConnection(
        const int32_t windowId, const sptr<AccessibilityWindowConnection>& interactionConnection);

    /**
     * @brief Remove interface operation interactive connection.
     * @param windowId Interface operation interactive connection the corresponding window id.
     */
    void RemoveAccessibilityWindowConnection(const int32_t windowId);

    /**
     * @brief Add connecting accessibility.
     * @param uri The uri of ability
     * @param connection The connection of ability
     */
    void AddConnectingA11yAbility(const std::string &uri, const sptr<AccessibleAbilityConnection> &connection);

    void AddEnabledAbility(const std::string &name); // For UT

    /**
     * @brief Remove accessibility that have been opened.
     * @param name bundle name + / + ability name.
     * @return return true if removes enabled ability successfully, otherwise refer to the RetError for the failure.
     */
    RetError RemoveEnabledAbility(const std::string &name);

    void AddInstalledAbility(AccessibilityAbilityInfo& abilityInfo); // For UT

    /**
     * @brief Empty installed accessibility list.
     * @param abilityInfo Accessibility ability info.
     */
    void RemoveInstalledAbility(const std::string &bundleName);

    /**
     * @brief The real procedure for add connecting ability.
     */
    void ClearInstalledAbility();

    /**
     * @brief Get connected accessibility list.
     * @return Store map of connected accessibility abilities.
     */
    const std::map<std::string, sptr<AccessibleAbilityConnection>> GetConnectedA11yAbilities();

    const CaptionPropertyCallbacks GetCaptionPropertyCallbacks();

    /**
     * @brief Get interface operation interactive connection list.
     * @return Store map of interface operation interactive connection.
     */
    const std::map<int32_t, sptr<AccessibilityWindowConnection>> GetAsacConnections();

    /**
     * @brief Query accessible ability connection through elementName URI.
     * @param elementName Accessibility elementName URI
     * @return Accessible ability connection corresponding to elementName URI.
     */
    const sptr<AccessibleAbilityConnection> GetAccessibleAbilityConnection(const std::string &elementName);

    /**
     * @brief Query interface operation interactive connection through window id.
     * @param windowId Interface operation interactive connection the corresponding window id.
     * @return Interface operation interactive connection corresponding to window id.
     */
    const sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(const int32_t windowId);

    /**
     * @brief Get connecting ability of specified uri.
     * @param uri The name of ability.
     * @return Strong point of connecting ability.
     */
    sptr<AccessibleAbilityConnection> GetConnectingA11yAbility(const std::string &uri);

    /**
     * @brief Get the accessibility ability info of the corresponding state according to the
     *        ability state type.
     * @param state Ability state type.
     */
    void GetAbilitiesByState(AbilityStateType state, std::vector<AccessibilityAbilityInfo> &abilities);

    /**
     * @brief Get the accessibility ability info of the disabled ability.
     */
    void GetDisableAbilities(std::vector<AccessibilityAbilityInfo> &disabledAbilities);

    /**
     * @brief Get enable accessibility list.
     * @return Store map of enable accessibility abilities.
     */
    const std::vector<std::string> &GetEnabledAbilities();

    /**
     * @brief Get install accessibility list.
     * @return Store vector of install accessibility abilities.
     */
    const std::vector<AccessibilityAbilityInfo> &GetInstalledAbilities() const;

    /**
     * @brief Update user enabled accessibility capabilities.
     */
    void UpdateAccountCapabilities();

    /**
     * @brief Get flag with whether have zoom capability.
     * @return isScreenMagnification_.
     */
    bool GetScreenMagnificationFlag()
    {
        return isScreenMagnification_;
    }

    /**
     * @brief Get flag with whether have touch guide capability.
     * @return isEventTouchGuideState_.
     */
    bool GetEventTouchGuideStateFlag()
    {
        return isEventTouchGuideState_;
    }

    /**
     * @brief Get flag with whether have key event observer capability.
     * @return isFilteringKeyEvents_.
     */
    bool GetFilteringKeyEventsFlag()
    {
        return isFilteringKeyEvents_;
    }

    /**
     * @brief Get flag with whether have gesture capability.
     * @return isGesturesSimulation_.
     */
    bool GetGesturesSimulationFlag()
    {
        return isGesturesSimulation_;
    }
    bool GetInstalledAbilitiesFromBMS();

    std::shared_ptr<AccessibilitySettingsConfig> GetConfig();

    RetError EnableAbility(const std::string &name, const uint32_t capabilities);

    void SetScreenReaderState(const std::string &name, const std::string &state);
    bool GetScreenReaderState();

    void Init();

    void UpdateEnableAbilityListsState();
    void UpdateInstallAbilityListsState();

    void AddConfigCallback(const sptr<IAccessibleAbilityManagerConfigObserver>& callback);
    void RemoveConfigCallback(const wptr<IRemoteObject>& callback);
    const std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> &GetConfigCallbacks() const;
    void SetConfigCallbacks(std::vector<sptr<IAccessibleAbilityManagerConfigObserver>>& observer);

    void GetImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities) const;
    void UpdateImportantEnabledAbilities(std::map<std::string, uint32_t> &importantEnabledAbilities);
    void UpdateAutoStartEnabledAbilities();

    uint32_t GetInputFilterFlag() const;
    void UpdateAbilities();
    bool RemoveAbility(const std::string &bundleName);
    void AddAbility(const std::string &bundleName);
    void ChangeAbility(const std::string &bundleName);

    void AddUITestClient(const sptr<IRemoteObject> &obj,
        const std::string &bundleName, const std::string &abilityName);
    void RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection, const std::string &bundleName);
    void SetAbilityAutoStartState(const std::string &name, const bool state);
    void DelAutoStartPrefKeyInRemovePkg(const std::string &bundleName);
    bool GetAbilityAutoStartState(const std::string &name);
    void GetConfigValueAtoHos(ConfigValueAtoHosUpdate &value);

private:
    /**
     * @brief Update connected accessibility whether have touch guide
     * capability. Have touch guide capability isEventTouchGuideState_ is
     * true, otherwise isEventTouchGuideState_ is false.
     */
    void UpdateEventTouchGuideCapability();

    /**
     * @brief Update connected accessibility whether have gesture capability.
     *        Have gesture capability isGesturesSimulation_ is true,
     *        otherwise isGesturesSimulation_ is false.
     */
    void UpdateGesturesSimulationCapability();

    /**
     * @brief Update connected accessibility whether have key event observer capability.
     *        Have key event observer capability isFilteringKeyEvents_ is true,
     *        otherwise isFilteringKeyEvents_ is false.
     */
    void UpdateFilteringKeyEventsCapability();

    /**
     * @brief Update connected accessibility whether have zoom capability.
     *        Have zoom capability isScreenMagnification_ is true,
     *        otherwise isScreenMagnification_ is false.
     */
    void UpdateMagnificationCapability();

    AccountSA::OsAccountType GetAccountType();

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
        std::mutex mutex_;
    };

    int32_t id_;
    AccountSA::OsAccountType accountType_ = AccountSA::OsAccountType::END;
    bool isEventTouchGuideState_ = false;
    bool isScreenMagnification_ = false;
    bool isFilteringKeyEvents_ = false;
    bool isGesturesSimulation_ = false;
    uint32_t connectCounter_ = 1;
    AccessibilityAbility connectedA11yAbilities_;  // key: bundleName/abilityName
    AccessibilityAbility connectingA11yAbilities_;  // key: bundleName/abilityName
    std::vector<sptr<IAccessibilityEnableAbilityListsObserver>> enableAbilityListsObservers_;
    std::mutex asacConnectionsMutex_;
    std::map<int32_t, sptr<AccessibilityWindowConnection>> asacConnections_; // key: windowId
    CaptionPropertyCallbacks captionPropertyCallbacks_;
    std::vector<AccessibilityAbilityInfo> installedAbilities_;
    std::vector<std::string> enabledAbilities_; // bundleName/abilityName
    std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> configCallbacks_;
    std::shared_ptr<AccessibilitySettingsConfig> config_ = nullptr;
};

class AccessibilityAccountDataMap {
public:
    AccessibilityAccountDataMap() = default;
    ~AccessibilityAccountDataMap() = default;
    sptr<AccessibilityAccountData> AddAccountData(int32_t accountId);
    sptr<AccessibilityAccountData> GetCurrentAccountData(int32_t accountId);
    sptr<AccessibilityAccountData> GetAccountData(int32_t accountId);
    sptr<AccessibilityAccountData> RemoveAccountData(int32_t accountId);
    void Clear();
private:
    std::map<int32_t, sptr<AccessibilityAccountData>> accountDataMap_;
    std::mutex accountDataMutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ACCOUNT_DATA_H