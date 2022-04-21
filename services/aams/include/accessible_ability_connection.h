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
    AccessibleAbilityConnection(const sptr<AccessibilityAccountData> &accountData, const int32_t connectionId,
        AccessibilityAbilityInfo &abilityInfo);

    virtual ~AccessibleAbilityConnection();

    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                      const sptr<IRemoteObject> &remoteObject,
                                      int32_t resultCode) override;

    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;

    // For AccessibleAbilityClientProxy
    void OnAccessibilityEvent(AccessibilityEventInfo &eventInfo);

    bool OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence);

    void OnDisplayResized(const int32_t displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY);

    void OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully);

    // Get Attribution
    inline AccessibilityAbilityInfo& GetAbilityInfo()
    {
        return abilityInfo_;
    }

    inline AppExecFwk::ElementName& GetElementName()
    {
        return elementName_;
    }

    inline wptr<AccessibilityAccountData> GetAccountData()
    {
        return accountData_;
    }

    inline sptr<IAccessibleAbilityClient> GetAbilityClient()
    {
        return abilityClient_;
    }

    void Disconnect();

    void Connect(const AppExecFwk::ElementName &element);

    int32_t GetChannelId();

    void OnAbilityConnectDoneSync(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int32_t resultCode)
    {
        InnerOnAbilityConnectDone(element, remoteObject, resultCode);
    }

    void OnAbilityDisconnectDoneSync(const AppExecFwk::ElementName &element, int32_t resultCode)
    {
        InnerOnAbilityDisconnectDone(element, resultCode);
    }

private:
    class AccessibleAbilityConnectionDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityConnectionDeathRecipient(sptr<AccessibilityAccountData> accountData,
            AppExecFwk::ElementName& elementName)
            : recipientAccountData_(accountData), recipientElementName_(elementName) {};
        ~AccessibleAbilityConnectionDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityConnectionDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote);

        sptr<AccessibilityAccountData> recipientAccountData_;
        AppExecFwk::ElementName& recipientElementName_;
    };

    bool IsWantedEvent(int32_t eventType);
    bool IsAllowedListEvent(EventType eventType);

    void InnerOnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode);
    void InnerOnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode);

    int32_t connectionId_ = -1;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<IAccessibleAbilityClient> abilityClient_ = nullptr;
    sptr<AccessibleAbilityChannel> channel_ = nullptr;
    AccessibilityAbilityInfo abilityInfo_ {};
    AppExecFwk::ElementName elementName_ {};
    sptr<AccessibilityAccountData> accountData_ = nullptr;
    static std::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_CONNECTION_H