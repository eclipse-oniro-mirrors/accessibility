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

#ifndef ACCESSIBLE_ABILITY_CONNECTION_H
#define ACCESSIBLE_ABILITY_CONNECTION_H

#include <mutex>
#include "ability_connect_callback_stub.h"
#include "accessibility_ability_info.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_client_proxy.h"
#include "common_event_manager.h"

namespace OHOS {
namespace Accessibility {
#define UID_MASK 200000

class AccessibilityAccountData;

class AccessibleAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    AccessibleAbilityConnection(int32_t accountId, int32_t connectionId, AccessibilityAbilityInfo &abilityInfo);

    virtual ~AccessibleAbilityConnection();

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

    void Connect(const AppExecFwk::ElementName &element);

    int32_t GetChannelId();

    void OnAbilityConnectDoneSync(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject);
    void OnAbilityDisconnectDoneSync(const AppExecFwk::ElementName &element);

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
        AppExecFwk::ElementName& recipientElementName_;
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
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CONNECTION_H