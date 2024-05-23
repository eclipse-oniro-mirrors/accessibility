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
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t TIME_OUT_OPERATOR = 5000;
    MMI::InputManager* inputManager_ = MMI::InputManager::GetInstance();
    std::map<int32_t, std::pair<bool, std::pair<int32_t, int32_t>>> accessibleKeyCodeTable = {
        {ActionType::ACCESSIBILITY_ACTION_HOME,
            {false, {MMI::KeyEvent::KEYCODE_META_LEFT, MMI::KeyEvent::KEYCODE_D}}},
        {ActionType::ACCESSIBILITY_ACTION_RECENTTASK,
            {false, {MMI::KeyEvent::KEYCODE_META_LEFT, MMI::KeyEvent::KEYCODE_TAB}}},
        {ActionType::ACCESSIBILITY_ACTION_BACK,
            {true, {MMI::KeyEvent::KEYCODE_BACK, MMI::KeyEvent::KEYCODE_BACK}}},
        {ActionType::ACCESSIBILITY_ACTION_NOTIFICATIONCENTER,
            {true, {MMI::KeyEvent::KEYCODE_CALL_NOTIFICATION_CENTER, MMI::KeyEvent::KEYCODE_CALL_NOTIFICATION_CENTER}}},
        {ActionType::ACCESSIBILITY_ACTION_CONTROLCENTER,
            {true, {MMI::KeyEvent::KEYCODE_CALL_CONTROL_CENTER, MMI::KeyEvent::KEYCODE_CALL_CONTROL_CENTER}}}};
} // namespace

AccessibleAbilityChannel::AccessibleAbilityChannel(const int32_t accountId, const std::string &clientName)
    : clientName_(clientName), accountId_(accountId)
{
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

RetError AccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId, elementId, requestId, callback, mode,
        isFilter](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (awm.IsInnerWindowRootElement(elementId) && callback != nullptr) {
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            HILOG_DEBUG("IsInnerWindowRootElement elementId: %{public}" PRId64 "", elementId);
        } else {
            int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
            elementOperator->SearchElementInfoByAccessibilityId(realElementId, requestId, callback, mode, isFilter);
            HILOG_DEBUG("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "SearchElementInfoByAccessibilityId");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait SearchElementInfoByAccessibilityId result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId, elementId, text, requestId, callback](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        elementOperator->SearchElementInfosByText(realElementId, text, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "SearchElementInfosByText");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait SearchElementInfosByText result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId, elementId,
        focusType, requestId, callback](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, focusType, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        elementOperator->FindFocusedElementInfo(realElementId, focusType, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "FindFocusedElementInfo");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait FindFocusedElementInfo result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId,
        elementId, direction, requestId, callback](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        elementOperator->FocusMoveSearch(realElementId, direction, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "FocusMoveSearch");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait FocusMoveSearch result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void AccessibleAbilityChannel::SetKeyCodeSingle(std::shared_ptr<MMI::KeyEvent>& keyEvent, const int32_t keyCode)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!keyEvent) {
        HILOG_ERROR("KeyEvent is nullptr");
        return;
    }

    keyEvent->SetKeyCode(keyCode);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);

    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keyCode);
    item.SetPressed(true);

    keyEvent->AddKeyItem(item);
}

void AccessibleAbilityChannel::SetKeyCodeMulti(std::shared_ptr<MMI::KeyEvent>& keyEvent, const int32_t keyCodePre,
    const int32_t keyCodeNext)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!keyEvent) {
        HILOG_ERROR("KeyEvent is nullptr");
        return;
    }

    keyEvent->SetKeyCode(keyCodeNext);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);

    MMI::KeyEvent::KeyItem item1;
    item1.SetKeyCode(keyCodePre);
    item1.SetPressed(true);
    keyEvent->AddKeyItem(item1);

    MMI::KeyEvent::KeyItem item2;
    item2.SetKeyCode(keyCodeNext);
    item2.SetPressed(true);
    keyEvent->AddKeyItem(item2);
}

RetError AccessibleAbilityChannel::TransmitActionToMmi(const int32_t action)
{
    HILOG_DEBUG("The action is %{public}d", action);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    if (!keyEvent) {
        HILOG_ERROR("KeyEvent is nullptr");
        return RET_ERR_NULLPTR;
    }

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is nullptr");
        return RET_ERR_NULLPTR;
    }
    
    HILOG_DEBUG("Transmit keycode to MMI");

    if (accessibleKeyCodeTable.at(action).first) {
        SetKeyCodeSingle(keyEvent, accessibleKeyCodeTable.at(action).second.first);
    } else {
        SetKeyCodeMulti(keyEvent, accessibleKeyCodeTable.at(action).second.first,
            accessibleKeyCodeTable.at(action).second.second);
    }

    inputManager_->SimulateInputEvent(keyEvent);
    return RET_OK;
}

RetError AccessibleAbilityChannel::EnableScreenCurtain(bool isEnable)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    HILOG_INFO("enter AccessibleAbilityChannel::EnableScreenCurtain isEnable = %{public}d", isEnable);

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    HILOG_DEBUG("Run RSInterfaces -> SetCurtainScreenUsingStatus");
    auto rsInterfaces = &(Rosen::RSInterfaces::GetInstance());
    if (rsInterfaces == nullptr) {
        HILOG_ERROR("rsInterfaces is nullptr.");
        return RET_ERR_NULLPTR;
    }
    HILOG_INFO("EnableScreenCurtain : rsInterfaces->SetCurtainScreenUsingStatus(isEnable) %{public}d", isEnable);
    rsInterfaces->SetCurtainScreenUsingStatus(isEnable);
    return RET_OK;
}

RetError AccessibleAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    if (accessibleKeyCodeTable.find(action) != accessibleKeyCodeTable.end()) {
        RetError ret = TransmitActionToMmi(action);
        if (ret != RET_OK) {
            HILOG_ERROR("Transmit Action To Mmi failed!");
            return RET_ERR_FAILED;
        }
        return RET_OK;
    }
    
    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId, elementId, action,
        actionArguments, requestId, callback](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        elementOperator->ExecuteAction(realElementId, action, actionArguments, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "ExecuteAction");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait ExecuteAction result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("windowId:%{public}d", windowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
 
    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::shared_ptr<AccessibilityWindowInfo> tmpWindowInfo = std::make_shared<AccessibilityWindowInfo>(windowInfo);
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([windowId, tmpWindowInfo, syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("windowId:%{public}d", windowId);
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }
        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("AccessibleAbilityChannel::GetWindow failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }

        if (Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindow(windowId, *tmpWindowInfo)) {
            syncPromise->set_value(RET_OK);
        } else {
            syncPromise->set_value(RET_ERR_NO_WINDOW_CONNECTION);
        }
        }, accountId_, clientName_), "GetWindow");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait GetWindow result");
        return RET_ERR_TIME_OUT;
    }

    windowInfo = *tmpWindowInfo;
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
    uint64_t displayId = Singleton<AccessibilityDisplayManager>::GetInstance().GetDefaultDisplayId();
    HILOG_DEBUG("default display id is %{public}" PRIu64 "", displayId);
    return GetWindows(displayId, windows);
#else
    HILOG_DEBUG("not support display manager");
    return GetWindows(0, windows);
#endif
}

RetError AccessibleAbilityChannel::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    return GetWindows(displayId, windows);
}

RetError AccessibleAbilityChannel::GetWindows(uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows) const
{
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    auto tmpWindows = std::make_shared<std::vector<AccessibilityWindowInfo>>(windows);
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([displayId, tmpWindows, syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
            HILOG_ERROR("GetWindows failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }

        std::vector<AccessibilityWindowInfo> windowInfos =
            Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
#ifdef OHOS_BUILD_ENABLE_DISPLAY_MANAGER
        for (auto &window : windowInfos) {
            if (window.GetDisplayId() == displayId) {
                tmpWindows->emplace_back(window);
            }
        }
#else
        for (auto &window : windowInfos) {
            tmpWindows->emplace_back(window);
        }
#endif
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "GetWindows");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait GetWindows result");
        return RET_ERR_TIME_OUT;
    }

    windows = *tmpWindows;
    return syncFuture.get();
}

void AccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
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

RetError AccessibleAbilityChannel::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([syncPromise, accessibilityWindowId, elementId,
        requestId, callback](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, name.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, name, elementOperator);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        elementOperator->GetCursorPosition(realElementId, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "GetCursorPosition");
    
    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait GetCursorPosition result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_INFO();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([gesturePath, syncPromise](int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
            HILOG_ERROR("AccessibleAbilityChannel::SendSimulateGesture failed: no capability");
            syncPromise->set_value(RET_ERR_NO_CAPABILITY);
            return;
        }

        sptr<TouchEventInjector> touchEventInjector =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
        if (!touchEventInjector) {
            HILOG_ERROR("touchEventInjector is null");
            syncPromise->set_value(RET_ERR_NO_INJECTOR);
            return;
        }
        touchEventInjector->InjectEvents(gesturePath);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "SendSimulateGesture");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait SendSimulateGesture result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<std::promise<RetError>> syncPromise = std::make_shared<std::promise<RetError>>();
    std::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask(std::bind([targetBundleNames, syncPromise](
        int32_t accountId, const std::string &name) -> void {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, name);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        clientConnection->SetAbilityInfoTargetBundleName(targetBundleNames);
        syncPromise->set_value(RET_OK);
        }, accountId_, clientName_), "SetTargetBundleName");

    std::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait SetTargetBundleName result");
        return RET_ERR_TIME_OUT;
    }
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

RetError AccessibleAbilityChannel::GetElementOperator(
    int32_t accountId, int32_t windowId, int32_t focusType, const std::string &clientName,
    sptr<IAccessibilityElementOperator> &elementOperator)
{
    HILOG_DEBUG();
    elementOperator = nullptr;
    sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
    if (!clientConnection) {
        HILOG_ERROR("There is no client connection");
        return RET_ERR_NO_CONNECTION;
    }
    if (!(clientConnection->GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("the client has no retieve capability");
        return RET_ERR_NO_CAPABILITY;
    }

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(windowId, focusType);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection) {
        HILOG_ERROR("windowId[%{public}d] has no connection", realId);
        return RET_ERR_NO_WINDOW_CONNECTION;
    }
    elementOperator = connection->GetProxy();
    if (!elementOperator) {
        HILOG_ERROR("The proxy of window connection is nullptr");
        return RET_ERR_NULLPTR;
    }
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS