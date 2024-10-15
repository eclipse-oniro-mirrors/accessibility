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

#ifndef ACCESSIBLE_ABILITY_MANAGER_SERVICE_H
#define ACCESSIBLE_ABILITY_MANAGER_SERVICE_H

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "accessibility_dumper.h"
#include "accessibility_def.h"
#include "accessible_ability_manager_service_stub.h"
#include "accessible_ability_manager_service_event_handler.h"
#include "accessibility_account_data.h"
#include "accessibility_common_event.h"
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
#include "accessibility_display_manager.h"
#endif
#include "accessibility_element_operator_callback_stub.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_settings.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_window_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "input_manager.h"
#include "singleton.h"
#include "system_ability.h"
#include "window_manager.h"
#include "accessibility_short_key.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityAccountData;
class TouchEventInjector;
class AccessibilitySettings;

enum CallBackID {
    STATE_CALLBACK,
    CAPTION_PROPERTY_CALLBACK,
    ENABLE_ABILITY_LISTS_CALLBACK,
    CONFIG_CALLBACK
};

constexpr int REQUEST_ID_INIT = 65535;

const std::map<std::string, int32_t> AccessibilityConfigTable = {
    {"HIGH_CONTRAST_TEXT", HIGH_CONTRAST_TEXT},
    {"INVERT_COLOR", INVERT_COLOR},
    {"DALTONIZATION_COLOR_FILTER", DALTONIZATION_COLOR_FILTER},
    {"CONTENT_TIMEOUT", CONTENT_TIMEOUT},
    {"ANIMATION_OFF", ANIMATION_OFF},
    {"BRIGHTNESS_DISCOUNT", BRIGHTNESS_DISCOUNT},
    {"AUDIO_MONO", AUDIO_MONO},
    {"AUDIO_BALANCE", AUDIO_BALANCE},
    {"MOUSE_KEY", MOUSE_KEY},
    {"CAPTION_STATE", CAPTION_STATE},
    {"CAPTION_STYLE", CAPTION_STYLE},
    {"SCREEN_MAGNIFICATION", SCREEN_MAGNIFICATION},
    {"MOUSE_AUTOCLICK", MOUSE_AUTOCLICK}
};

class AccessibleAbilityManagerService : public SystemAbility, public AccessibleAbilityManagerServiceStub {
    DECLARE_SINGLETON(AccessibleAbilityManagerService)
    DECLEAR_SYSTEM_ABILITY(AccessibleAbilityManagerService)
public:
    /* For system ability */
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void PostDelayUnloadTask() override;

public:
    /* For AccessibleAbilityManagerServiceStub */
    RetError SendEvent(const AccessibilityEventInfo &uiEvent, const int32_t flag = 0) override;

    RetError VerifyingToKenId(const int32_t windowId, const int64_t elementId);

    uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) override;

    uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) override;

    void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) override;

    RetError GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) override;

    RetError RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation, bool isApp) override;

    RetError RegisterElementOperator(Registration parameter,
        const sptr<IAccessibilityElementOperator> &operation, bool isApp) override;

    RetError DeregisterElementOperator(const int32_t windowId) override;

    RetError DeregisterElementOperator(const int32_t windowId, const int32_t treeId) override;

    RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption) override;
    RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) override;
    RetError SetCaptionState(const bool state) override;

    bool GetEnabledState() override;
    RetError GetCaptionState(bool &state) override;
    bool GetTouchGuideState() override;
    bool GetGestureState() override;
    bool GetKeyEventObserverState() override;

    RetError EnableAbility(const std::string &name, const uint32_t capabilities) override;
    RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) override;
    RetError DisableAbility(const std::string &name) override;
    RetError EnableUITestAbility(const sptr<IRemoteObject>& obj) override;
    RetError DisableUITestAbility() override;
    int32_t GetActiveWindow() override;
    void GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId) override;
    void GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid) override;
    bool FindFocusedElement(AccessibilityElementInfo &elementInfo);
    bool ExecuteActionOnAccessibilityFocused(const ActionType &action);
    RetError GetFocusedWindowId(int32_t &focusedWindowId) override;
    void SetFocusWindowId(const int32_t focusWindowId);
    void SetFocusElementId(const int64_t focusElementId);
    int32_t GetFocusWindowId();
    int64_t GetFocusElementId();
    static int32_t GetTreeIdBySplitElementId(const int64_t elementId);
    int64_t GetRootParentId(int32_t windowId, int32_t treeId) override;
    RetError GetAllTreeId(int32_t windowId, std::vector<int32_t> &treeIds) override;
    void SetTokenIdMapAndRootParentId(const sptr<AccessibilityWindowConnection> connection,
        const int32_t treeId, const int64_t nodeId, const uint32_t tokenId);
    void RemoveTreeDeathRecipient(const int32_t windowId, const int32_t treeId,
        const sptr<AccessibilityWindowConnection> connection);
    int32_t GenerateRequestId();
    void GetElementOperatorConnection(sptr<AccessibilityWindowConnection> &connection,
        const int64_t elementId, sptr<IAccessibilityElementOperator> &elementOperator);
private:
    int32_t focusWindowId_ = -1;
    int64_t focusElementId_ = -1;
    std::atomic<int> requestId_ = REQUEST_ID_INIT;
public:
    /* For inner modules */
    bool EnableShortKeyTargetAbility(const std::string &name = "");
    bool DisableShortKeyTargetAbility();
    void OnShortKeyProcess();
    void UpdateShortKeyRegister();

    void SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector);

    inline sptr<TouchEventInjector> GetTouchEventInjector()
    {
        return touchEventInjector_;
    }

    inline sptr<KeyEventFilter> GetKeyEventFilter()
    {
        return keyEventFilter_;
    }

    void SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter);

    /* For DisplayResize */
    void NotifyDisplayResizeStateChanged(int32_t displayId, Rect& rect, float scale, float centerX, float centerY);

    inline bool IsServiceReady()
    {
        return isReady_;
    }

    inline int32_t GetCurrentAccountId()
    {
        return currentAccountId_;
    }

    inline std::shared_ptr<AAMSEventHandler> &GetMainHandler()
    {
        return handler_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner> &GetMainRunner()
    {
        return runner_;
    }

    sptr<AccessibilityAccountData> GetAccountData(int32_t accountId);
    sptr<AccessibilityAccountData> GetCurrentAccountData();
    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();

    /* For common event */
    void AddedUser(int32_t accountId);
    void RemovedUser(int32_t accountId);
    void SwitchedUser(int32_t accountId);
    void PackageChanged(const std::string &bundleName);
    void PackageRemoved(const std::string &bundleName);
    void PackageAdd(const std::string &bundleName);

    void UpdateAccessibilityManagerService();
    void InsertWindowIdEventPair(int32_t windowId, const AccessibilityEventInfo &event);
    bool CheckWindowIdEventExist(int32_t windowId);
    bool CheckWindowRegister(int32_t windowId);

    // used for arkui windowId 1 map to WMS windowId
    void FindInnerWindowId(const AccessibilityEventInfo &event, int32_t& windowId);
    bool GetParentElementRecursively(int32_t windowId, int64_t elementId,
        std::vector<AccessibilityElementInfo>& infos);

    // used for arkui windowId 1 map to WMS windowId
    class ElementOperatorCallbackImpl : public AccessibilityElementOperatorCallbackStub {
    public:
        ElementOperatorCallbackImpl() = default;
        ~ElementOperatorCallbackImpl() = default;

        virtual void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
            const int32_t requestId) override;
        virtual void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
            const int32_t requestId) override;
        virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
            const int32_t requestId) override;
        virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) override;
        virtual void SetExecuteActionResult(const bool succeeded, const int32_t requestId) override;
        virtual void SetCursorPositionResult(const int32_t cursorPosition, const int32_t requestId) override;

    private:
        std::promise<void> promise_;
        bool executeActionResult_ = false;
        AccessibilityElementInfo accessibilityInfoResult_ = {};
        std::vector<AccessibilityElementInfo> elementInfosResult_;
        int32_t callCursorPosition_ = 0;

        friend class AccessibleAbilityManagerService;
    };

    RetError SetScreenMagnificationState(const bool state) override;
    RetError SetShortKeyState(const bool state) override;
    RetError SetMouseKeyState(const bool state) override;
    RetError SetMouseAutoClick(const int32_t time) override;
    RetError SetShortkeyTarget(const std::string &name) override;
    RetError SetShortkeyMultiTarget(const std::vector<std::string> &name) override;
    RetError SetHighContrastTextState(const bool state) override;
    RetError SetInvertColorState(const bool state) override;
    RetError SetAnimationOffState(const bool state) override;
    RetError SetAudioMonoState(const bool state) override;
    RetError SetDaltonizationState(const bool state) override;
    RetError SetDaltonizationColorFilter(const uint32_t filter) override;
    RetError SetContentTimeout(const uint32_t time) override;
    RetError SetBrightnessDiscount(const float discount) override;
    RetError SetAudioBalance(const float balance) override;
    RetError SetClickResponseTime(const uint32_t time) override;
    RetError SetIgnoreRepeatClickState(const bool state) override;
    RetError SetIgnoreRepeatClickTime(const uint32_t time) override;

    RetError GetScreenMagnificationState(bool &state) override;
    RetError GetShortKeyState(bool &state) override;
    RetError GetMouseKeyState(bool &state) override;
    RetError GetMouseAutoClick(int32_t &time) override;
    RetError GetShortkeyTarget(std::string &name) override;
    RetError GetShortkeyMultiTarget(std::vector<std::string> &name) override;
    RetError GetHighContrastTextState(bool &state) override;
    RetError GetInvertColorState(bool &state) override;
    RetError GetAnimationOffState(bool &state) override;
    RetError GetAudioMonoState(bool &state) override;
    RetError GetDaltonizationState(bool &state) override;
    RetError GetDaltonizationColorFilter(uint32_t &type) override;
    RetError GetContentTimeout(uint32_t &timer) override;
    RetError GetBrightnessDiscount(float &brightness) override;
    RetError GetAudioBalance(float &balance) override;
    RetError GetClickResponseTime(uint32_t &time) override;
    RetError GetIgnoreRepeatClickState(bool &state) override;
    RetError GetIgnoreRepeatClickTime(uint32_t &time) override;
    void GetAllConfigs(AccessibilityConfigData &configData) override;

    uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) override;
    void UpdateConfigState();
    void UpdateAudioBalance();
    void UpdateBrightnessDiscount();
    void UpdateContentTimeout();
    void UpdateDaltonizationColorFilter();
    void UpdateMouseAutoClick();
    void UpdateShortkeyTarget();
    void UpdateShortkeyMultiTarget();
    void UpdateClickResponseTime();
    void UpdateIgnoreRepeatClickTime();

    void UpdateInputFilter();
    void AddRequestId(int32_t windowId, int32_t treeId, int32_t requestId,
        sptr<IAccessibilityElementOperatorCallback> callback);
    void RemoveRequestId(int32_t requestId) override;
    void OnDataClone();

private:
    void StopCallbackWait(int32_t windowId);
    void StopCallbackWait(int32_t windowId, int32_t treeId);
    RetError CheckCallingUid();
    sptr<AccessibilityWindowConnection> GetRealIdConnection();
    bool FindFocusedElementByConnection(sptr<AccessibilityWindowConnection> connection,
        AccessibilityElementInfo &elementInfo);
    bool SetTargetAbility(const int32_t targetAbilityValue);
    RetError RegisterElementOperatorChildWork(const Registration &parameter, const int32_t treeId,
        const int64_t nodeId, const sptr<IAccessibilityElementOperator> &operation,
        const uint32_t tokenId, bool isApp);
    void IsCheckWindowIdEventExist(const int32_t windowId);
    class StateCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        StateCallbackDeathRecipient() = default;
        ~StateCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(StateCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    class InteractionOperationDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        InteractionOperationDeathRecipient(int32_t windowId) : windowId_(windowId) {};
        InteractionOperationDeathRecipient(int32_t windowId, int32_t treeId) : windowId_(windowId), treeId_(treeId) {};
        ~InteractionOperationDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(InteractionOperationDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
        int32_t windowId_ = INVALID_WINDOW_ID;
        int32_t treeId_ = INVALID_TREE_ID;
    };

    class CaptionPropertyCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        CaptionPropertyCallbackDeathRecipient() = default;
        ~CaptionPropertyCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(CaptionPropertyCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    class EnableAbilityListsObserverDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        EnableAbilityListsObserverDeathRecipient() = default;
        ~EnableAbilityListsObserverDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(EnableAbilityListsObserverDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    bool Init();
    void InitInnerResource();

    class ConfigCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        ConfigCallbackDeathRecipient() = default;
        ~ConfigCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(ConfigCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    class BundleManagerDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        BundleManagerDeathRecipient() = default;
        ~BundleManagerDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(BundleManagerDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    class StateObservers {
    public:
        StateObservers() = default;
        ~StateObservers() = default;
        void AddStateObserver(const sptr<IAccessibleAbilityManagerStateObserver>& stateObserver);
        void OnStateObservers(uint32_t state);
        void RemoveStateObserver(const wptr<IRemoteObject>& remote);
        void Clear();
    private:
        std::vector<sptr<IAccessibleAbilityManagerStateObserver>> observersList_;
        std::mutex stateObserversMutex_;
    };

    RetError InnerEnableAbility(const std::string &name, const uint32_t capabilities);
    RetError InnerDisableAbility(const std::string &name);

    sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(int32_t windowId);
    void ClearFocus(int32_t windowId);
    void OutsideTouch(int32_t windowId);
    void UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event);

    void UpdateAccessibilityState();
    void UpdateCaptionProperty();
    void UpdateSettingsInAtoHosTask();
    void UpdateSettingsInAtoHos();
    void UpdateAutoStartAbilities();
    void UpdateAllSetting();

    void RemoveCallback(CallBackID callback, const sptr<DeathRecipient> &recipient, const wptr<IRemoteObject> &remote);
    void RemoveSavedConfigCallback(const wptr<IRemoteObject>& callback);
    void OnBundleManagerDied(const wptr<IRemoteObject> &remote);
    void DeleteConnectionAndDeathRecipient(
        const int32_t windowId, const sptr<AccessibilityWindowConnection> &connection);
    
    void OnDeviceProvisioned();
    void InitializeShortKeyState();
    void RegisterProvisionCallback();
    void RegisterShortKeyEvent();
    bool IsNeedUnload();
    void OffZoomGesture();
    void OnScreenMagnificationStateChanged();
    void RegisterScreenMagnificationState();
    void OnScreenMagnificationTypeChanged();
    void RegisterScreenMagnificationType();

    bool isReady_ = false;
    bool isPublished_ = false;
    std::map<int32_t, bool> dependentServicesStatus_;
    int32_t currentAccountId_ = -1;
    AccessibilityAccountDataMap  a11yAccountsData_;
    sptr<AppExecFwk::IBundleMgr> bundleManager_ = nullptr;

    sptr<AccessibilityInputInterceptor> inputInterceptor_ = nullptr;
    sptr<TouchEventInjector> touchEventInjector_ = nullptr;
    sptr<KeyEventFilter> keyEventFilter_ = nullptr;
    sptr<AccessibilityDumper> accessibilityDumper_ = nullptr;
    sptr<AccessibilityShortKey> accessibilityShortKey_ = nullptr;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AAMSEventHandler> handler_;

    std::shared_ptr<AppExecFwk::EventRunner> actionRunner_;
    std::shared_ptr<AAMSEventHandler> actionHandler_;

    int64_t ipcTimeoutNum_ = 0; // count ipc timeout number

    sptr<IRemoteObject::DeathRecipient> stateObserversDeathRecipient_ = nullptr;
    std::map<int32_t, sptr<IRemoteObject::DeathRecipient>> interactionOperationDeathRecipients_ {};
    std::map<int32_t, std::map<int32_t, sptr<IRemoteObject::DeathRecipient>>> interactionOperationDeathMap_ {};
    sptr<IRemoteObject::DeathRecipient> captionPropertyCallbackDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> enableAbilityListsObserverDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> configCallbackDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> bundleManagerDeathRecipient_ = nullptr;
    StateObservers stateObservers_;
    std::mutex mutex_; // current used for register state observer
    std::vector<sptr<IAccessibleAbilityManagerConfigObserver>> defaultConfigCallbacks_;
    std::shared_ptr<AccessibilitySettings> accessibilitySettings_ = nullptr;
    std::vector<std::string> removedAutoStartAbilities_ {};
    std::map<int32_t, AccessibilityEventInfo> windowFocusEventMap_ {};

    std::map<int32_t, std::map<int32_t, std::set<int32_t>>> windowRequestIdMap_ {}; // windowId->treeId->requestId
    std::map<int32_t, sptr<IAccessibilityElementOperatorCallback>> requestIdMap_ {}; // requestId->callback
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_MANAGER_SERVICE_H