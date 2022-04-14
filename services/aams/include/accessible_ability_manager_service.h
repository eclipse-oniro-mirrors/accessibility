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

#include "accessible_ability_manager_service_stub.h"
#include "accessible_ability_manager_service_event_handler.h"
#include "accessibility_account_data.h"
#include "accessibility_common_event_registry.h"
#include "accessibility_display_manager.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_keyevent_filter.h"
#include "accessibility_touchEvent_injector.h"
#include "accessibility_window_info.h"
#include "accessibility_zoom_proxy.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "input_manager.h"
#include "singleton.h"
#include "system_ability.h"
#include "window_manager.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityAccountData;
class TouchEventInjector;

class AccessibleAbilityManagerService : public SystemAbility, public AccessibleAbilityManagerServiceStub {
    DECLARE_DELAYED_SINGLETON(AccessibleAbilityManagerService)
    DECLEAR_SYSTEM_ABILITY(AccessibleAbilityManagerService)
public:
    void OnStart() override;
    void OnStop() override;

    /* For AccessibleAbilityManagerServiceStub */
    void SendEvent(const AccessibilityEventInfo& uiEvent, const int32_t accountId) override;

    uint32_t RegisterStateObserver(
        const sptr<IAccessibleAbilityManagerStateObserver> &callback, const int32_t accountId) override;

    uint32_t RegisterCaptionObserver(
        const sptr<IAccessibleAbilityManagerCaptionObserver> &callback, const int32_t accountId) override;

    std::vector<AccessibilityAbilityInfo> GetAbilityList(const uint32_t abilityTypes,
        const int32_t stateType) override;

    void RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation, const int32_t accountId) override;

    void DeregisterElementOperator(const int32_t windowId) override;

    CaptionProperty GetCaptionProperty() override;
    bool SetCaptionProperty(const CaptionProperty &caption) override;
    bool SetCaptionState(const bool state) override;

    /* For InputFilter */
    void SetTouchEventInjector(const sptr<TouchEventInjector> &touchEventInjector);

    inline sptr<TouchEventInjector> GetTouchEventInjector()
    {
        return touchEventInjector_;
    }

    /* For Key Event Filter */
    bool IsWantedKeyEvent(MMI::KeyEvent &event);

    inline sptr<KeyEventFilter> GetKeyEventFilter()
    {
        return keyEventFilter_;
    }

    void SetKeyEventFilter(const sptr<KeyEventFilter> &keyEventFilter);

    /* For DisplayResize */
    void NotifyDisplayResizeStateChanged(int32_t displayId, Rect& rect, float scale, float centerX, float centerY);

    inline int32_t GetCurrentAccountId()
    {
        return currentAccountId_;
    }

    inline uint32_t GetConnectCounter()
    {
        return connectCounter_;
    }

    inline std::shared_ptr<AAMSEventHandler> &GetMainHandler()
    {
        return handler_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner> &GetMainRunner()
    {
        return runner_;
    }

    sptr<AccessibilityAccountData> GetCurrentAccountData();
    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();

    /* For common event */
    void RemovedUser(int32_t accountId);
    void PresentUser();
    void PackageChanged(std::string &bundleName);
    void PackageRemoved(std::string &bundleName);
    void PackageAdd(std::string &bundleName);
    void PackageUpdateFinished(std::string &bundleName);

    void UpdateAccessibilityManagerService();
    void UpdateAbilities();

    bool GetEnabledState() override;
    bool GetCaptionState() override;
    bool GetTouchGuideState() override;
    bool GetGestureState() override;
    bool GetKeyEventObserverState() override;

    bool EnableAbilities(std::vector<std::string> &abilities) override;
    std::vector<std::string> GetEnabledAbilities() override;
    std::vector<AccessibilityAbilityInfo> GetInstalledAbilities() override;
    bool DisableAbilities(std::vector<std::string> &abilities) override;
    bool EnableUITestAbility(const sptr<IRemoteObject>& obj) override;
    bool DisableUITestAbility() override;
    int32_t GetActiveWindow() override;
protected:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    void AddUITestClient(const sptr<IRemoteObject> &obj);
    void RemoveUITestClient(sptr<AccessibleAbilityConnection> &connection);

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
        ~InteractionOperationDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(InteractionOperationDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
        int32_t windowId_ = INVALID_WINDOW_ID;
    };

    class CaptionPropertyCallbackDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        CaptionPropertyCallbackDeathRecipient() = default;
        ~CaptionPropertyCallbackDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(CaptionPropertyCallbackDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
    };

    bool Init();
    void InnerSendEvent(AccessibilityEventInfo &uiEvent);
    void InnerRegisterStateObserver(std::promise<uint32_t> &syncPromise,
        const sptr<IAccessibleAbilityManagerStateObserver> &callback, const int32_t accountId);
    void InnerRegisterCaptionObserver(std::promise<uint32_t> &syncPromise,
        const sptr<IAccessibleAbilityManagerCaptionObserver> &callback, const int32_t accountId);
    void InnerGetAbilityList(std::promise<std::vector<AccessibilityAbilityInfo>> &syncPromise,
        const uint32_t abilityTypes, const int32_t stateType);
    void InnerRegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> operation, const int32_t accountId);
    void InnerDeregisterElementOperator(const int32_t windowId);
    void InnerGetCaptionProperty(std::promise<CaptionProperty> &syncPromise);
    void InnerSetCaptionProperty(std::promise<bool> &syncPromise, const CaptionProperty &caption);
    void InnerSetCaptionState(std::promise<bool> &syncPromise, const bool state);
    void InnerGetEnabledState(std::promise<bool> &syncPromise);
    void InnerGetCaptionState(std::promise<bool> &syncPromise);
    void InnerGetTouchGuideState(std::promise<bool> &syncPromise);
    void InnerGetGestureState(std::promise<bool> &syncPromise);
    void InnerGetKeyEventObserverState(std::promise<bool> &syncPromise);
    void InnerEnableAbilities(std::promise<bool> &syncPromise, std::vector<std::string> &abilities);
    void InnerGetEnabledAbilities(std::promise<std::vector<std::string>> &syncPromise);
    void InnerGetInstalledAbilities(std::promise<std::vector<AccessibilityAbilityInfo>> &syncPromise);
    void InnerDisableAbilities(std::promise<bool> &syncPromise, std::vector<std::string> &abilities);
    void InnerEnableUITestAbility(std::promise<bool> &syncPromise, const sptr<IRemoteObject>& obj);
    void InnerDisableUITestAbility(std::promise<bool> &syncPromise);
    void InnerGetActiveWindow(std::promise<int32_t> &syncPromise);

    sptr<AccessibilityWindowConnection> GetAccessibilityWindowConnection(int32_t windowId);
    void ClearFocus(int32_t windowId);
    void OutsideTouch(int32_t windowId);
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
    std::shared_ptr<AccessibilityCommonEventRegistry> commonEventRegistry_ = nullptr;
    std::map<int32_t, sptr<AccessibilityAccountData>> a11yAccountsData_;
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
    static std::mutex mutex_;

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityManagerService);
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBLE_ABILITY_MANAGER_SERVICE_H