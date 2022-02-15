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

#ifndef OHOS_ACCESSIBLE_ABILITY_CONNECTION_H_
#define OHOS_ACCESSIBLE_ABILITY_CONNECTION_H_

#include <set>
#include <string>
#include <vector>
#include "ability_connect_callback_stub.h"
#include "accessible_ability_client_proxy.h"
#include "accessible_ability_channel_stub.h"
#include "accessibility_ability_info.h"
#include "accessible_ability_manager_service.h"
#include "common_event_manager.h"

namespace OHOS {
namespace Accessibility {

class AccessibilityAccountData;
class AccessibleAbilityManagerService;
class AccessibleAbilityConnection;

class AccessibleAbilityChannelStubImpl : public AccessibleAbilityChannelStub {
public:
    AccessibleAbilityChannelStubImpl(AccessibleAbilityConnection &connection);
    ~AccessibleAbilityChannelStubImpl() = default;
    bool SearchElementInfoByAccessibilityId(const int accessibilityWindowId, const long elementId,
        const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int mode) override;

    bool SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
        const std::string &text, const int requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    bool FindFocusedElementInfo(const int accessibilityWindowId, const long elementId,
        const int focusType, const int requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    bool FocusMoveSearch(const int accessibilityWindowId, const long elementId, const int direction,
        const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    bool ExecuteAction(const int accessibilityWindowId, const long elementId, const int action,
        std::map<std::string, std::string> &actionArguments, const int requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override;

    std::vector<AccessibilityWindowInfo> GetWindows() override;

    bool ExecuteCommonAction(const int action) override;

    void SetOnKeyPressEventResult(const bool handled, const int sequence) override;

    float GetDisplayResizeScale(const int displayId) override;

    float GetDisplayResizeCenterX(const int displayId) override;

    float GetDisplayResizeCenterY(const int displayId) override;

    Rect GetDisplayResizeRect(const int displayId) override;

    bool ResetDisplayResize(const int displayId, bool animate) override;

    bool SetDisplayResizeScaleAndCenter(const int displayId, const float scale, const float centerX,
        const float centerY, const bool animate) override;

    void SendSimulateGesture(const int requestId, const std::vector<GesturePathDefine> &gestureSteps) override;

private:
    AccessibleAbilityConnection& connection_;
    std::shared_ptr<AccessibleAbilityManagerService> aams_ = nullptr;
};

class AccessibleAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    AccessibleAbilityConnection(const sptr<AccessibilityAccountData> &accountData, const int connectionId,
        AccessibilityAbilityInfo &abilityInfo);

    virtual ~AccessibleAbilityConnection();

    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                      const sptr<IRemoteObject> &remoteObject,
                                      int resultCode) override;

    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    // For AccessibleAbilityClientProxy
    void OnAccessibilityEvent(AccessibilityEventInfo &eventInfo);

    bool OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence);

    void OnDisplayResized(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY);

    void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully);

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

    inline sptr<IAccessibleAbilityClient> GetProxy()
    {
        return proxy_;
    }

    void Disconnect();

    void Connect(const AppExecFwk::ElementName &element);

    int GetChannelId();

private:
    class AccessibleAbilityConnectionDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        AccessibleAbilityConnectionDeathRecipient(wptr<AccessibilityAccountData>& accountData,
            AppExecFwk::ElementName& elementName)
            : recipientAccountData_(accountData), recipientElementName_(elementName) {};
        ~AccessibleAbilityConnectionDeathRecipient() = default;
        DISALLOW_COPY_AND_MOVE(AccessibleAbilityConnectionDeathRecipient);

        void OnRemoteDied(const wptr<IRemoteObject>& remote);

        wptr<AccessibilityAccountData>& recipientAccountData_;
        AppExecFwk::ElementName& recipientElementName_;
    };

    bool IsWantedEvent(int eventType);
    bool IsWhiteListEvent(EventType eventType);

    sptr<IRemoteObject::DeathRecipient> deathRecipient_{};
    sptr<IAccessibleAbilityClient> proxy_ = nullptr;
    sptr<AccessibleAbilityChannelStubImpl> stub_ = nullptr;
    AccessibilityAbilityInfo abilityInfo_;
    AppExecFwk::ElementName elementName_;
    sptr<AccessibilityAccountData> accountData_;

    int connectionId_;
};

} // namespace Accessibility
} // namespace OHOS

#endif // OHOS_ACCESSIBLE_ABILITY_CONNECTION_H_