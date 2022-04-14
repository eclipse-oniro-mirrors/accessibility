/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mock_accessible_ability_connection.h"
#include <algorithm>
#include <map>
#include <vector>
#include "accessibility_account_data.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
MockAccessibleAbilityConnection::MockAccessibleAbilityConnection(const sptr<AccessibilityAccountData>& accountData,
    const int32_t connectionId, AccessibilityAbilityInfo& abilityInfo)
    : AccessibleAbilityConnection(accountData, connectionId, abilityInfo)
{
    (void)accountData;
    (void)connectionId;
    (void)abilityInfo;
}
MockAccessibleAbilityConnection::~MockAccessibleAbilityConnection()
{}

AccessibleAbilityChannel::AccessibleAbilityChannel(AccessibleAbilityConnection& connection)
    : connection_(connection)
{
    (void)connection;
}

bool AccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback,
    const int32_t mode)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)requestId;
    (void)callback;
    (void)mode;
    return true;
}

bool AccessibleAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string& text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)text;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)focusType;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)direction;
    (void)requestId;
    (void)callback;
    return true;
}

bool AccessibleAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, std::map<std::string, std::string>& actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback>& callback)
{
    (void)accessibilityWindowId;
    (void)elementId;
    (void)action;
    (void)actionArguments;
    (void)requestId;
    (void)callback;
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibleAbilityChannel::GetWindows()
{
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::GetWindows failed: no capability");
        std::vector<AccessibilityWindowInfo> windows;
        return windows;
    }

    std::vector<AccessibilityWindowInfo> windows;
    return windows;
}

bool AccessibleAbilityChannel::ExecuteCommonAction(int32_t action)
{
    (void)action;
    // temp deal: need external dependence
    return true;
}

void AccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    (void)handled;
    (void)sequence;
}

float AccessibleAbilityChannel::GetDisplayResizeScale(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannel::GetDisplayResizeCenterX(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

float AccessibleAbilityChannel::GetDisplayResizeCenterY(const int32_t displayId)
{
    (void)displayId;
    return 0;
}

Rect AccessibleAbilityChannel::GetDisplayResizeRect(const int32_t displayId)
{
    Rect temp;
    (void)displayId;
    return temp;
}

bool AccessibleAbilityChannel::ResetDisplayResize(const int32_t displayId, const bool animate)
{
    (void)displayId;
    (void)animate;
    return true;
}

bool AccessibleAbilityChannel::SetDisplayResizeScaleAndCenter(
    const int32_t displayId, const float scale, const float centerX, const float centerY, const bool animate)
{
    (void)displayId;
    (void)scale;
    (void)centerX;
    (void)centerY;
    (void)animate;
    return true;
}

void AccessibleAbilityChannel::SendSimulateGesture(
    const int32_t requestId, const std::vector<AccessibilityGesturePath>& gestureSteps)
{
    (void)requestId;
    (void)gestureSteps;
}

AccessibleAbilityConnection::AccessibleAbilityConnection(
    const sptr<AccessibilityAccountData>& accountData, int32_t connectionId, AccessibilityAbilityInfo& abilityInfo)
{
    connectionId_ = connectionId;
    abilityInfo_ = abilityInfo;
    accountData_ = accountData;
}

AccessibleAbilityConnection::~AccessibleAbilityConnection()
{
    HILOG_DEBUG("start");
}

void AccessibleAbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int32_t resultCode)
{
    (void)element;
    (void)remoteObject;
    (void)resultCode;
    sptr<AccessibleAbilityConnection> pointer = this;
    if (accountData_ != nullptr) {
        accountData_->AddConnectedAbility(pointer);
    }
}

void AccessibleAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int32_t resultCode)
{
    (void)element;
    (void)resultCode;
    return;
}

bool AccessibleAbilityConnection::IsAllowedListEvent(EventType eventType)
{
    (void)eventType;
    return true;
}

bool AccessibleAbilityConnection::OnKeyPressEvent(const MMI::KeyEvent& keyEvent, const int32_t sequence)
{
    (void)keyEvent;
    (void)sequence;
    return false;
}

void AccessibleAbilityConnection::OnDisplayResized(
    const int32_t displayId, const Rect& rect, const float scale, const float centerX, const float centerY)
{
    (void)displayId;
    (void)rect;
    (void)scale;
    (void)centerX;
    (void)centerY;
}

void AccessibleAbilityConnection::OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully)
{
    (void)sequence;
    (void)completedSuccessfully;
}

bool AccessibleAbilityConnection::IsWantedEvent(int32_t eventType)
{
    (void)eventType;
    return true;
}

AAFwk::Want CreateWant(AppExecFwk::ElementName& element)
{
    (void)element;
    AAFwk::Want want;
    return want;
}

void AccessibleAbilityConnection::Disconnect()
{
    HILOG_DEBUG("start");
}

void AccessibleAbilityConnection::Connect(const AppExecFwk::ElementName& element)
{
    HILOG_DEBUG("start");
    (void)element;
}

int32_t AccessibleAbilityConnection::GetChannelId()
{
    HILOG_DEBUG("connectionId_ is %{public}d", connectionId_);
    return connectionId_;
}

void AccessibleAbilityConnection::AccessibleAbilityConnectionDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject>& remote)
{
    (void)remote;
    // Temp deal: notify setting
}

void AccessibleAbilityConnection::OnAccessibilityEvent(AccessibilityEventInfo &eventInfo)
{
    (void)eventInfo;
}
} // namespace Accessibility
} // namespace OHOS