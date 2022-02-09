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

#ifndef ACCESSIBLE_ABILITY_MANAGER_SERVICE_H
#define ACCESSIBLE_ABILITY_MANAGER_SERVICE_H

#define INT32_BIT_NUM 32

#include <map>
#include <memory>
#include <string>

#include "accessible_ability_manager_service_stub.h"
#include "accessible_ability_manager_service_event_handler.h"
#include "accessibility_account_data.h"
#include "accessibility_common_event_registry.h"
#include "accessibility_input_filter.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_proxy.h"
#include "accessibility_window_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "singleton.h"
#include "system_ability.h"
#include "dummy.h"
#include "multimoduleDummy.h"


namespace OHOS {
namespace Accessibility {

class AccessibilityAccountData;
class TouchEventInjector;

class AccessibleAbilityManagerService : public SystemAbility, public AccessibleAbilityManagerServiceClientStub {
    DECLARE_DELAYED_SINGLETON(AccessibleAbilityManagerService)
    DECLEAR_SYSTEM_ABILITY(AccessibleAbilityManagerService)
public:
    void OnStart() override;
    void OnStop() override;

    /* For AccessibleAbilityManagerServiceClientStub */
    void SendEvent(const AccessibilityEventInfo &uiEvent, const int accountId) override;

    uint32_t RegisterStateCallback(const sptr<IAccessibleAbilityManagerServiceState> &callback,
        const int accountId) override;

    std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType) override;

    void RegisterInteractionOperation(const int windowId,
        const sptr<IAccessibilityInteractionOperation> &operation,
        const int accountId) override;

    void DeregisterInteractionOperation(const int windowId) override;

    void Interrupt(const int accountId) override;

    uint64_t GetSuggestedInterval() override;

    void PersistElementNamesToSetting(std::string &bundleName, std::map<std::string, AppExecFwk::ElementName> &componentNames,
        int accountId);

    /* For InputFilter */
    void SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector);

    inline sptr<TouchEventInjector> GetTouchEventInjector() {return touchEventInjector_;}

     /* For Key Event Filter */
    bool IsWantedKeyEvent(MMI::KeyEvent &event);

    inline sptr<KeyEventFilter> GetKeyEventFilter() {return keyEventFilter_;}

    void SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter);

    /* For DisplayResize */
    void NotifyDisplayResizeStateChanged(int displayId, Rect &rect, float scale, float centerX, float centerY);

    /* For TouchExplore */
    inline void OnTouchInteractionStart() {isTouchInteraction_ = true;}

    inline void OnTouchInteractionEnd() {isTouchInteraction_ = false;}

    void OnGesture(int id);

    void RegisterAbilityConnectionClientTmp(const sptr<IRemoteObject>& obj) override;

    inline sptr<AccessibilityInputFilter> GetInputFilter() {return inputFilter_;}

    inline int32_t GetCurrentAccountId() {return currentAccountId_;}

    inline uint32_t GetConnectCounter() {return connectCounter_;}

    inline std::shared_ptr<AAMSEventHandler>& GetMainHandler() {return handler_;}

    inline std::shared_ptr<AppExecFwk::EventRunner>& GetMainRunner() {return runner_;}

    sptr<AccessibilityAccountData> GetCurrentAccountData();

    bool HasInputFilter() {return hasInputFilter_;}

    std::shared_ptr<MultimodalInputManager> GetInputManager() {return inputManager_;}

    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    sptr<IWindowManagerService> GetWindowMgrProxy();

    /* Interaction Bridge*/
    AccessibilityElementInfo FindFocusedElementInfo();

    bool PerformActionOnAccessibilityFocusedItem(const ActionType& action);

    bool GetAccessibilityFocusClickPointInScreen(MmiPoint &point);

    /* For common event */
    void SwitchedUser(int32_t accountId);
    void UnlockedUser(int32_t accountId);
    void RemovedUser(int32_t accountId);
    void PresentUser();
    void PackageChanged();
    void PackageRemoved(std::string &bundleName);
    void PackageUpdateFinished(std::string &bundleName);

    void UpdateAccessibilityManagerService();
    void UpdateAbilities();

private:
    class StateCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        StateCallbackDeathRecipient() = default;
        ~StateCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(StateCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
    };

    class InteractionOperationDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        InteractionOperationDeathRecipient(int windowId) : windowId_(windowId) {};
        ~InteractionOperationDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(InteractionOperationDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
        int windowId_ = INVALID_WINDOW_ID;
    };

    bool Init();
    void SendEventInner(AccessibilityEventInfo &uiEvent);
    void InterruptInner();
    sptr<AccessibilityInteractionConnection> GetAccessibilityInteractionConnection(int windowId);
    void ClearFocus(int windowId);
    void OutsideTouch(int windowId);
    void OnChanging(bool selfChange, Uri &uri);
    void UpdateAccessibilityWindowStateByAccessibilityEvent(const AccessibilityEventInfo &event);
    /* For common event */
    void PublicNoticeEventInner();

    void UpdateAccessibilityState();
    void UpdateInputFilter();
    void UpdateMagnification();
    void UpdateWindowChangeListener();

    bool isRunning_ = false;
    int32_t currentAccountId_ = 0;
    uint32_t connectCounter_ = 1;
    bool isTouchInteraction_ = false;
    AccessibilityCommonEventRegistry accessibilityCommonEventRegistry_;
    std::map<int, sptr<AccessibilityAccountData>> a11yAccountsData_;
    sptr<AppExecFwk::IBundleMgr> bundleManager_;
    sptr<IWindowManagerService> windowManager_;

    sptr<AccessibilityInputFilter> inputFilter_;
    sptr<TouchEventInjector> touchEventInjector_;
    sptr<KeyEventFilter> keyEventFilter_;
    std::shared_ptr<MultimodalInputManager> inputManager_;

    bool hasInputFilter_ = false;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AAMSEventHandler> handler_;

    sptr<IRemoteObject::DeathRecipient> stateCallbackDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> interactionOperationDeathRecipient_ = nullptr;

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityManagerService);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_MANAGER_SERVICE_H