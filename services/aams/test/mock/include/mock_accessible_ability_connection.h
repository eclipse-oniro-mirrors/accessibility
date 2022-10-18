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

#ifndef MOCK_ACCESSIBLE_ABILITY_CONNECTION_H
#define MOCK_ACCESSIBLE_ABILITY_CONNECTION_H

#include <gmock/gmock.h>
#include "accessible_ability_connection.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityChannel : public AccessibleAbilityChannel {
public:
    MockAccessibleAbilityChannel(const int32_t accountId, const std::string &clientName);
    ~MockAccessibleAbilityChannel();

    MOCK_METHOD5(SearchElementInfoByAccessibilityId,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t requestId,
            const sptr<IAccessibilityElementOperatorCallback>& callback, const int32_t mode));

    MOCK_METHOD5(SearchElementInfosByText,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const std::string& text,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD5(FindFocusedElementInfo,
        RetError(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t focusType,
            const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));

    MOCK_METHOD5(
        FocusMoveSearch, RetError(const int32_t accessibilityWindowId, const int32_t elementId, const int32_t direction,
                             const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD6(ExecuteAction, RetError(const int32_t accessibilityWindowId, const int32_t elementId,
        const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback>& callback));
    MOCK_METHOD1(GetWindows, RetError(std::vector<AccessibilityWindowInfo> &windows));
    MOCK_METHOD2(GetWindowsByDisplayId, RetError(const uint64_t displayId,
        std::vector<AccessibilityWindowInfo> &windows));
    MOCK_METHOD2(SetOnKeyPressEventResult, void(const bool handled, const int32_t sequence));
    MOCK_METHOD1(SendSimulateGesture, RetError(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath));
};

class MockAccessibleAbilityConnection : public AccessibleAbilityConnection {
public:
    MockAccessibleAbilityConnection(int32_t accountId, int32_t connectionId, AccessibilityAbilityInfo& abilityInfo);

    virtual ~MockAccessibleAbilityConnection();

    MOCK_METHOD3(OnAbilityConnectDone,
        void(const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int32_t resultCode));
    MOCK_METHOD2(OnAbilityDisconnectDone, void(const AppExecFwk::ElementName& element, int32_t resultCode));
    // For AccessibleAbilityClientProxy
    MOCK_METHOD1(OnAccessibilityEvent, void(AccessibilityEventInfo& eventInfo));
    MOCK_METHOD2(OnKeyPressEvent, bool(const MMI::KeyEvent& keyEvent, const int32_t sequence));
    MOCK_METHOD2(OnGestureSimulateResult, bool(const int32_t sequence, const bool completedSuccessfully));
    // Get Attribution
    AccessibilityAbilityInfo abilityInfo_;
    AppExecFwk::ElementName elementName_;
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
        return nullptr;
    }

    inline sptr<IAccessibleAbilityClient> GetProxy()
    {
        return nullptr;
    }

    MOCK_METHOD0(Disconnect, void());
    MOCK_METHOD1(Connect, void(const AppExecFwk::ElementName& element));
    MOCK_METHOD0(GetChannelId, int32_t());
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CONNECTION_H