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

#ifndef ACCESSIBLE_ABILITY_CONNECTION_H
#define ACCESSIBLE_ABILITY_CONNECTION_H

#include "ability_connect_callback_stub.h"
#include "accessibility_ability_info.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_client_proxy.h"
#include "accessibility_resource_bundle_manager.h"
#include "ffrt.h"
#include "common_event_manager.h"
#include "app_mgr_interface.h"

namespace OHOS {
namespace Accessibility {
#define UID_MASK 200000

class AccessibilityAccountData;

class AccessibleAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    AccessibleAbilityConnection(int32_t accountId, int32_t connectionId, AccessibilityAbilityInfo &abilityInfo);

    virtual ~AccessibleAbilityConnection();

    void HandleNoEventHandler(const AppExecFwk::ElementName &element);

    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                      const sptr<IRemoteObject> &remoteObject,
                                      int32_t resultCode) override;

    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;

    // For AccessibleAbilityClientProxy
    void OnAccessibilityEvent(AccessibilityEventInfo &eventInfo);

    bool OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence);

    void SetAbilityInfoTargetBundleName(const std::vector<std::string> &targetBundleNames);

    // Get Attribution
    inline AccessibilityAbilityInfo& GetAbilityInfo()
    {
        return abilityInfo_;
    }

    inline AppExecFwk::ElementName& GetElementName()
    {
        return elementName_;
    }

    inline sptr<IAccessibleAbilityClient> GetAbilityClient()
    {
        return abilityClient_;
    }

    void Disconnect();

    bool Connect(const AppExecFwk::ElementName &element);

    int32_t GetChannelId();

    void OnAbilityConnectDoneSync(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject);
    void OnAbilityDisconnectDoneSync(const AppExecFwk::ElementName &element);
    bool GetIsRegisterDisconnectCallback() const;
    void SetIsRegisterDisconnectCallback(bool isRegister);
    void NotifyDisconnect();
    void DisconnectAbility();
    bool RegisterAppStateObserverToAMS(const std::string& bundleName, const std::string& abilityName,
                                 const sptr<AccessibleAbilityConnection>& connection,
                                 const sptr<AccessibilityAccountData>& accountData);

    static std::string GenerateConnectionKey(const std::string& bundleName, 
                                           const std::string& abilityName, 
                                           int32_t accountId);
    
    void SetConnectionKey(const std::string& key) { connectionKey_ = key; }
    const std::string& GetConnectionKey() const { return connectionKey_; }
    
    static void RegisterExtensionServiceMapping(const std::string& key, 
                                              const std::string& bundleName,
                                              const std::string& abilityName,
                                              const sptr<IRemoteObject>& remoteObject,
                                              int32_t accountId);
    static void UnregisterExtensionServiceMapping(const std::string& key);
    static std::tuple<std::string, std::string, int32_t> GetExtensionServiceInfoByKey(const std::string& key);
    static sptr<IRemoteObject> GetRemoteObjectByKey(const std::string& key);
    static void NotifyExtensionServiceDeath(const std::string& key);

    static void RegisterAppStateObserverMapping(const std::string& bundleName, 
                                              const sptr<AccessibleAbilityConnection>& connection);
    static void UnregisterAppStateObserverMapping(const std::string& bundleName);
    static sptr<AccessibleAbilityConnection> GetConnectionByBundleName(const std::string& bundleName);

private:
    class AccessibleAbilityConnectionDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityConnectionDeathRecipient(int32_t accountId,
            AppExecFwk::ElementName& elementName, std::shared_ptr<AppExecFwk::EventHandler> handler)
            : accountId_(accountId), recipientElementName_(elementName), handler_(handler) {};
        ~AccessibleAbilityConnectionDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityConnectionDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote);

        int32_t accountId_ = -1;
        AppExecFwk::ElementName recipientElementName_;
        std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    };

    bool IsWantedEvent(int32_t eventType);
    void InitAbilityClient(const sptr<IRemoteObject> &remoteObject);

    int32_t accountId_ = -1;
    int32_t connectionId_ = -1;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<IAccessibleAbilityClient> abilityClient_ = nullptr;
    sptr<AccessibleAbilityChannel> channel_ = nullptr;
    AccessibilityAbilityInfo abilityInfo_ {};
    AppExecFwk::ElementName elementName_ {};
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    bool isRegisterDisconnectCallback_ = false;
    std::string connectionKey_;
    sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    sptr<AppExecFwk::IAppMgr> GetAppMgrProxy();
    
    struct ExtensionServiceInfo {
        std::string bundleName;
        std::string abilityName;
        sptr<IRemoteObject> remoteObject;
        int32_t accountId;
        pid_t pid;
        uid_t uid;
    };
    
    static std::map<std::string, std::shared_ptr<ExtensionServiceInfo>> extensionServiceMappings_;
    static ffrt::mutex mappingMutex_;

    static std::map<std::string, sptr<AccessibleAbilityConnection>> appStateObserverMappings_;
    static ffrt::mutex appStateObserverMutex_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CONNECTION_H