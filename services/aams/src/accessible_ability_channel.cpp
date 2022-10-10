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

#include "accessible_ability_channel.h"
#include "accessible_ability_manager_service.h"
#include "accessibility_window_connection.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_connection.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannel::AccessibleAbilityChannel(const int32_t accountId, const std::string &clientName)
    : clientName_(clientName), accountId_(accountId)
{
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

bool AccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator =
            GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name);
        if (!elementOperator) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            return;
        }
        elementOperator->SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
        HILOG_DEBUG("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }, accountId_, clientName_), "SearchElementInfoByAccessibilityId");
    return true;
}

bool AccessibleAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator =
            GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name);
        if (!elementOperator) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            return;
        }
        elementOperator->SearchElementInfosByText(elementId, text, requestId, callback);
        }, accountId_, clientName_), "SearchElementInfosByText");
    return true;
}

bool AccessibleAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator =
            GetElementOperator(accountId, accessibilityWindowId, focusType, name);
        if (!elementOperator) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            return;
        }
        elementOperator->FindFocusedElementInfo(elementId, focusType, requestId, callback);
        }, accountId_, clientName_), "FindFocusedElementInfo");
    return true;
}

bool AccessibleAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator =
            GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name);
        if (!elementOperator) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            return;
        }
        elementOperator->FocusMoveSearch(elementId, direction, requestId, callback);
        }, accountId_, clientName_), "FocusMoveSearch");
    return true;
}

bool AccessibleAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator =
            GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name);
        if (!elementOperator) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            return;
        }
        elementOperator->ExecuteAction(elementId, action, actionArguments, requestId, callback);
        }, accountId_, clientName_), "ExecuteAction");
    return true;
}

bool AccessibleAbilityChannel::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("windowId:%{public}d", windowId);

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }
 
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    eventHandler_->PostTask(std::bind([windowId, &windowInfo, &syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("windowId:%{public}d", windowId);
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise.set_value(false);
            return;
        }
        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("AccessibleAbilityChannel::GetWindow failed: no capability");
            syncPromise.set_value(false);
            return;
        }

        if (Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindow(windowId, windowInfo)) {
            syncPromise.set_value(true);
        } else {
            syncPromise.set_value(false);
        }
        }, accountId_, clientName_), "GetWindow");
    return syncFuture.get();
}

bool AccessibleAbilityChannel::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    uint64_t displayId = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplayId();
    HILOG_DEBUG("default display id is %{public}" PRIu64 "", displayId);
    return GetWindows(displayId, windows);
}

bool AccessibleAbilityChannel::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    return GetWindows(displayId, windows);
}

bool AccessibleAbilityChannel::GetWindows(uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows) const
{
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    eventHandler_->PostTask(std::bind([displayId, &windows, &syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise.set_value(false);
            return;
        }

        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("GetWindows failed: no capability");
            syncPromise.set_value(false);
            return;
        }

        std::vector<AccessibilityWindowInfo> windowInfos =
            Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
        for (auto &window : windowInfos) {
            if (window.GetDisplayId() == displayId) {
                windows.emplace_back(window);
            }
        }
        syncPromise.set_value(true);
        }, accountId_, clientName_), "GetWindows");
    return syncFuture.get();
}

void AccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_DEBUG();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }

    eventHandler_->PostTask(std::bind([=](int32_t accountId, const std::string &name) -> void {
        sptr<KeyEventFilter> keyEventFilter =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
        if (!keyEventFilter) {
            return;
        }

        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            return;
        }
        keyEventFilter->SetServiceOnKeyEventResult(*clientConnection, handled, sequence);
        }, accountId_, clientName_), "SetOnKeyPressEventResult");
}

bool AccessibleAbilityChannel::SendSimulateGesture(const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_INFO();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();

    eventHandler_->PostTask(std::bind([gesturePath, &syncPromise](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise.set_value(false);
            return;
        }

        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
            HILOG_ERROR("AccessibleAbilityChannel::SendSimulateGesture failed: no capability");
            syncPromise.set_value(false);
            return;
        }

        sptr<TouchEventInjector> touchEventInjector =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            syncPromise.set_value(false);
            return;
        }
        touchEventInjector->InjectEvents(gesturePath);
        syncPromise.set_value(true);
        }, accountId_, clientName_), "SendSimulateGesture");
    return syncFuture.get();
}

bool AccessibleAbilityChannel::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();

    eventHandler_->PostTask(std::bind([targetBundleNames, &syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise.set_value(false);
            return;
        }

        clientConnection->SetAbilityInfoTargetBundleName(targetBundleNames);
        syncPromise.set_value(true);
        }, accountId_, clientName_), "SetTargetBundleName");
    return syncFuture.get();
}

sptr<AccessibleAbilityConnection> AccessibleAbilityChannel::GetConnection(
    int32_t accountId, const std::string &clientName)
{
    HILOG_DEBUG();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }

    HILOG_DEBUG("accountId[%{public}d] clientName[%{public}s]", accountId, clientName.c_str());
    return accountData->GetAccessibleAbilityConnection(clientName);
}

sptr<IAccessibilityElementOperator> AccessibleAbilityChannel::GetElementOperator(
    int32_t accountId, int32_t windowId, int32_t focusType, const std::string &clientName)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
    if (!clientConnection) {
        HILOG_ERROR("There is no client connection");
        return nullptr;
    }
    if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("the client has no retieve capability");
        return nullptr;
    }

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection) {
        HILOG_ERROR("windowId[%{public}d] has no connection", realId);
        return nullptr;
    }
    return connection->GetProxy();
}
} // namespace Accessibility
} // namespace OHOS