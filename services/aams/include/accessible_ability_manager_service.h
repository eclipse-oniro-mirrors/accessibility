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
#include "accessibility_display_manager.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_zoom_proxy.h"
#include "accessibility_window_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "input_manager.h"
#include "singleton.h"
#include "system_ability.h"
#include "window_manager.h"

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
    void SendEvent(const AccessibilityEventInfo& uiEvent, const int accountId) override;

    uint32_t RegisterStateCallback(
        const sptr<IAccessibleAbilityManagerServiceState>& callback, const int accountId) override;

    uint32_t RegisterCaptionPropertyCallback(
        const sptr<IAccessibleAbilityManagerServiceCaptionProperty>& callback, const int accountId) override;

    std::vector<AccessibilityAbilityInfo> GetAbilityList(const int abilityTypes, const int stateType) override;

    void RegisterElementOperator(
        const int windowId, const sptr<IAccessibilityElementOperator>& operation, const int accountId) override;

    void DeregisterElementOperator(const int windowId) override;

    CaptionProperty GetCaptionProperty() override;
    bool SetCaptionProperty(const CaptionProperty& caption) override;
    bool SetCaptionState(const bool state) override;
    bool SetEnabled(const bool state) override;

    void PersistElementNamesToSetting(
        const std::string& bundleName, std::map<std::string, AppExecFwk::ElementName>& componentNames, int accountId);

    /* For InputFilter */
    void SetTouchEventInjector(const sptr<TouchEventInjector>& touchEventInjector);

    inline sptr<TouchEventInjector> GetTouchEventInjector()
    {
        return touchEventInjector_;
    }

    /* For Key Event Filter */
    bool IsWantedKeyEvent(MMI::KeyEvent& event);

    inline sptr<KeyEventFilter> GetKeyEventFilter()
    {
        return keyEventFilter_;
    }

    void SetKeyEventFilter(const sptr<KeyEventFilter>& keyEventFilter);

    /* For DisplayResize */
    void NotifyDisplayResizeStateChanged(int displayId, Rect& rect, float scale, float centerX, float centerY);

    inline int32_t GetCurrentAccountId()
    {
        return currentAccountId_;
    }

    inline uint32_t GetConnectCounter()
    {
        return connectCounter_;
    }

    inline std::shared_ptr<AAMSEventHandler>& GetMainHandler()
    {
        return handler_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner>& GetMainRunner()
    {
        return runner_;
    }

    sptr<AccessibilityAccountData> GetCurrentAccountData();
    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();

    /* For common event */
    void RemovedUser(int32_t accountId);
    void PresentUser();
    void PackageChanged(std::string& bundleName);
    void PackageRemoved(std::string& bundleName);
    void PackageAdd(std::string& bundleName);
    void PackageUpdateFinished(std::string& bundleName);

    void UpdateAccessibilityManagerService();
    void UpdateAbilities();

    bool GetEnabledState() override;
    bool GetCaptionState() override;
    bool GetTouchGuideState() override;
    bool GetGestureState() override;
    bool GetKeyEventObserverState() override;

    bool SetTouchGuideState(const bool state) override;
    bool SetGestureState(const bool state) override;
    bool SetKeyEventObserverState(const bool state) override;

    bool SetEnabledObj(std::map<std::string, AppExecFwk::ElementName> it) override;
    std::map<std::string, AppExecFwk::ElementName> GetEnabledAbilities() override;
    std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() override;
    bool DisableAbilities(std::map<std::string, AppExecFwk::ElementName> it) override;
    bool RegisterUITestAbilityConnectionClient(const sptr<IRemoteObject>& obj) override;
    bool DeregisterUITestAbilityConnectionClient() override;
    int GetActiveWindow() override;

private:
    void AddUITestClient(const sptr<IRemoteObject>& obj);
    void RemoveUITestClient(sptr<AccessibleAbilityConnection>& connection);

    class StateCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        StateCallbackDeathRecipient() = default;
        ~StateCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(StateCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
    };

    class InteractionOperationDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        InteractionOperationDeathRecipient(int windowId) : windowId_(windowId){};
        ~InteractionOperationDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(InteractionOperationDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
        int windowId_ = INVALID_WINDOW_ID;
    };

    class CaptionPropertyCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        CaptionPropertyCallbackDeathRecipient() = default;
        ~CaptionPropertyCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(CaptionPropertyCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;
    };

    bool Init();
    void SendEventInner(AccessibilityEventInfo& uiEvent);
    sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(int windowId);
    void ClearFocus(int windowId);
    void OutsideTouch(int windowId);
    void OnChanging(bool selfChange, Uri &uri);
    void UpdateAccessibilityWindowStateByEvent(const AccessibilityEventInfo &event);

    void UpdateAccessibilityState();
    void UpdateInputFilter();
    void UpdateMagnification();
    void UpdateWindowChangeListener();
    void UpdateCaptionProperty();

    bool isRunning_ = false;
    int32_t currentAccountId_ = 0;
    uint32_t connectCounter_ = 1;
    AccessibilityCommonEventRegistry accessibilityCommonEventRegistry_;
    std::map<int, sptr<AccessibilityAccountData>> a11yAccountsData_;
    sptr<AppExecFwk::IBundleMgr> bundleManager_;

    sptr<AccessibilityInputInterceptor> inputInterceptor_;
    sptr<TouchEventInjector> touchEventInjector_;
    sptr<KeyEventFilter> keyEventFilter_;
    int32_t interceptorId_ = 0;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AAMSEventHandler> handler_;

    sptr<IRemoteObject::DeathRecipient> stateCallbackDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> interactionOperationDeathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> captionPropertyCallbackDeathRecipient_ = nullptr;

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityManagerService);
};

}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBLE_ABILITY_MANAGER_SERVICE_H