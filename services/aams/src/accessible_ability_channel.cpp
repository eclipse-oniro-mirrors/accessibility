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
    constexpr int32_t WINDOW_ID_INVALID = -1;
    constexpr int64_t ELEMENT_ID_INVALID = -1;
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
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetChannelRunner());
}

RetError AccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter)
{
    int32_t treeId = elementBasicInfo.treeId;
    int32_t windowId = elementBasicInfo.windowId;
    int64_t elementId = elementBasicInfo.elementId;
    HILOG_DEBUG("elementId:%{public}" PRId64 " winId: %{public}d treeId: %{public}d", elementId, windowId, treeId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, windowId, elementId, treeId, requestId,
        callback, mode, isFilter]() {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, windowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK || !CheckWinFromAwm(windowId)) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", windowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (windowId == SCENE_BOARD_WINDOW_ID && awm.IsInnerWindowRootElement(elementId)) {
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            HILOG_DEBUG("IsInnerWindowRootElement elementId: %{public}" PRId64 "", elementId);
        } else {
            int64_t realElementId = awm.GetSceneBoardElementId(windowId, elementId);
            Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId,
                requestId, callback);
            elementOperator->SearchElementInfoByAccessibilityId(realElementId, requestId, callback, mode, isFilter);
            HILOG_DEBUG("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfoByAccessibilityId");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfoByAccessibilityId result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::SearchDefaultFocusedByWindowId(const ElementBasicInfo elementBasicInfo,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode, bool isFilter)
{
    int32_t windowId = elementBasicInfo.windowId;
    int64_t elementId = elementBasicInfo.elementId;
    int32_t treeId = elementBasicInfo.treeId;
    HILOG_DEBUG("elementId:%{public}" PRId64 " winId: %{public}d treeId: %{public}d", elementId, windowId, treeId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
 
    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }
 
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, windowId, elementId, treeId, requestId,
        callback, mode, isFilter]() {
        HILOG_DEBUG("search element accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, windowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK || !CheckWinFromAwm(windowId)) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", windowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchDefaultFocusByWindowIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }
 
        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        if (windowId == SCENE_BOARD_WINDOW_ID && awm.IsInnerWindowRootElement(elementId)) {
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchDefaultFocusByWindowIdResult(infos, requestId);
        } else {
            Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(windowId, treeId,
                requestId, callback);
            elementOperator->SearchDefaultFocusedByWindowId(windowId, requestId, callback, mode, isFilter);
            HILOG_DEBUG("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId successfully");
        }
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfoByAccessibilityId");
 
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfoByAccessibilityId result");
        return RET_ERR_TIME_OUT;
    }
    return RET_OK;
}

RetError AccessibleAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int64_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("SearchElementInfosByText :channel SearchElementInfo elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_ == nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }

    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("SearchElementInfosByText :channel SearchElementInfo treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId, text,
        requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->SearchElementInfosByText(realElementId, text, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "SearchElementInfosByText");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait SearchElementInfosByText result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("channel FindFocusedElementInfo elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("FindFocusedElementInfo :channel FindFocusedElementInfo treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId,
        focusType, requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, focusType,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->FindFocusedElementInfo(realElementId, focusType, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "FindFocusedElementInfo");
    
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait FindFocusedElementInfo result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("FocusMoveSearch :channel FocusMoveSearch elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();

    if (eventHandler_ == nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("FocusMoveSearch :channel FocusMoveSearch treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId,
        elementId, treeId, direction, requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID,
            clientName, elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            std::vector<AccessibilityElementInfo> infos = {};
            callback->SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->FocusMoveSearch(realElementId, direction, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "FocusMoveSearch");
    
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait FocusMoveSearch result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void AccessibleAbilityChannel::SetKeyCodeToMmi(std::shared_ptr<MMI::KeyEvent>& keyEvent, const bool isPress,
    const int32_t keyCode)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (keyEvent == nullptr) {
        HILOG_ERROR("KeyEvent is nullptr");
        return;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetPressed(isPress);
    item.SetKeyCode(keyCode);
    keyEvent->AddKeyItem(item);
    keyEvent->SetKeyCode(keyCode);
}

RetError AccessibleAbilityChannel::TransmitActionToMmi(const int32_t action)
{
    HILOG_DEBUG("The action is %{public}d", action);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    std::shared_ptr<MMI::KeyEvent> keyEventUp = MMI::KeyEvent::Create();
    std::shared_ptr<MMI::KeyEvent> keyEventDown = MMI::KeyEvent::Create();
    if (keyEventUp == nullptr || keyEventDown == nullptr) {
        HILOG_ERROR("KeyEvent is nullptr");
        return RET_ERR_NULLPTR;
    }

    if (!inputManager_) {
        HILOG_ERROR("inputManager_ is nullptr");
        return RET_ERR_NULLPTR;
    }
    
    HILOG_INFO("Transmit keycode to MMI");

    if (accessibleKeyCodeTable.at(action).first) {
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventUp, false, accessibleKeyCodeTable.at(action).second.first);
    } else {
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventUp, false, accessibleKeyCodeTable.at(action).second.first);
        SetKeyCodeToMmi(keyEventDown, true, accessibleKeyCodeTable.at(action).second.second);
    }
    keyEventDown->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    keyEventUp->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    inputManager_->SimulateInputEvent(keyEventDown);
    inputManager_->SimulateInputEvent(keyEventUp);

    return RET_OK;
}

RetError AccessibleAbilityChannel::EnableScreenCurtain(bool isEnable)
{
    HILOG_DEBUG();
    auto& aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.PostDelayUnloadTask();
    return aams.SetCurtainScreenUsingStatus(isEnable);
}

RetError AccessibleAbilityChannel::HoldRunningLock()
{
    HILOG_DEBUG();
    auto& aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.PostDelayUnloadTask();
    return aams.HoldRunningLock();
}

RetError AccessibleAbilityChannel::UnholdRunningLock()
{
    HILOG_DEBUG();
    auto& aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    aams.PostDelayUnloadTask();
    return aams.UnholdRunningLock();
}

RetError AccessibleAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("ExecuteAction elementId:%{public}" PRId64 " winId:%{public}d, action:%{public}d, requestId:%{public}d",
        elementId, accessibilityWindowId, action, requestId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_ == nullptr || callback == nullptr) {
        HILOG_ERROR("eventHandler_ exist: %{public}d, callback exist: %{public}d.", eventHandler_ != nullptr,
            callback != nullptr);
        return RET_ERR_NULLPTR;
    }

    if (accessibleKeyCodeTable.find(action) != accessibleKeyCodeTable.end()) {
        RetError ret = TransmitActionToMmi(action);
        if (ret != RET_OK) {
            HILOG_ERROR("Transmit Action To Mmi failed!");
            callback->SetExecuteActionResult(false, requestId);
            return RET_ERR_FAILED;
        }
        callback->SetExecuteActionResult(true, requestId);
        return RET_OK;
    }
    SetFocusWindowIdAndElementId(accessibilityWindowId, elementId, action);
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId, action,
        actionArguments, requestId, callback]() {
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->ExecuteAction(realElementId, action, actionArguments, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "ExecuteAction");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
        HILOG_ERROR("Failed to wait ExecuteAction result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

void AccessibleAbilityChannel::SetFocusWindowIdAndElementId(const int32_t accessibilityWindowId,
    const int64_t elementId, const int32_t action)
{
    auto& awm = Singleton<AccessibleAbilityManagerService>::GetInstance();
    if ((action == ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS)) {
        awm.SetFocusWindowId(accessibilityWindowId);
        awm.SetFocusElementId(elementId);
    } else if (action == ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS) {
        awm.SetFocusWindowId(WINDOW_ID_INVALID);
        awm.SetFocusElementId(ELEMENT_ID_INVALID);
    }
}

RetError AccessibleAbilityChannel::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("windowId:%{public}d", windowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
 
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    std::shared_ptr<AccessibilityWindowInfo> tmpWindowInfo = std::make_shared<AccessibilityWindowInfo>(windowInfo);
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, windowId, tmpWindowInfo, syncPromise]() {
        HILOG_DEBUG("windowId:%{public}d", windowId);
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
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
        }, "GetWindow");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
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
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }

    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    auto tmpWindows = std::make_shared<std::vector<AccessibilityWindowInfo>>(windows);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, displayId, tmpWindows, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
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
        }, "GetWindows");
    
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
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
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, handled, sequence]() {
        sptr<KeyEventFilter> keyEventFilter =
            Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
        if (!keyEventFilter) {
            return;
        }

        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            return;
        }
        keyEventFilter->SetServiceOnKeyEventResult(*clientConnection, handled, sequence);
        }, "SetOnKeyPressEventResult");
}

RetError AccessibleAbilityChannel::GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("GetCursorPosition :channel GetCursorPosition elementId: %{public}" PRId64 " winId: %{public}d",
        elementId, accessibilityWindowId);
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return RET_ERR_NULLPTR;
    }
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    int32_t treeId = AccessibleAbilityManagerService::GetTreeIdBySplitElementId(elementId);
    HILOG_DEBUG("GetCursorPosition :channel GetCursorPosition treeId: %{public}d", treeId);
    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    eventHandler_->PostTask([accountId, clientName, syncPromise, accessibilityWindowId, elementId, treeId,
        requestId, callback]() {
        HILOG_DEBUG("accountId[%{public}d], name[%{public}s]", accountId, clientName.c_str());
        sptr<IAccessibilityElementOperator> elementOperator = nullptr;
        RetError ret = GetElementOperator(accountId, accessibilityWindowId, FOCUS_TYPE_INVALID, clientName,
            elementOperator, treeId);
        if (ret != RET_OK) {
            HILOG_ERROR("Get elementOperator failed! accessibilityWindowId[%{public}d]", accessibilityWindowId);
            syncPromise->set_value(ret);
            return;
        }

        auto& awm = Singleton<AccessibilityWindowManager>::GetInstance();
        int64_t realElementId = awm.GetSceneBoardElementId(accessibilityWindowId, elementId);
        Singleton<AccessibleAbilityManagerService>::GetInstance().AddRequestId(accessibilityWindowId, treeId,
            requestId, callback);
        elementOperator->GetCursorPosition(realElementId, requestId, callback);
        syncPromise->set_value(RET_OK);
        }, "GetCursorPosition");
    
    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().RemoveRequestId(requestId);
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
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, gesturePath, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
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
        }, "SendSimulateGesture");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait SendSimulateGesture result");
        return RET_ERR_TIME_OUT;
    }
    return syncFuture.get();
}

RetError AccessibleAbilityChannel::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PostDelayUnloadTask();
    if (eventHandler_== nullptr) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return RET_ERR_NULLPTR;
    }

    int32_t accountId = accountId_;
    std::string clientName = clientName_;
    std::shared_ptr<ffrt::promise<RetError>> syncPromise = std::make_shared<ffrt::promise<RetError>>();
    ffrt::future syncFuture = syncPromise->get_future();
    eventHandler_->PostTask([accountId, clientName, targetBundleNames, syncPromise]() {
        HILOG_DEBUG();
        sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId, clientName);
        if (!clientConnection) {
            HILOG_ERROR("There is no client connection");
            syncPromise->set_value(RET_ERR_NO_CONNECTION);
            return;
        }

        clientConnection->SetAbilityInfoTargetBundleName(targetBundleNames);
        syncPromise->set_value(RET_OK);
        }, "SetTargetBundleName");

    ffrt::future_status wait = syncFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
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
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return nullptr;
    }

    HILOG_DEBUG("accountId[%{public}d] clientName[%{public}s]", accountId, clientName.c_str());
    return accountData->GetAccessibleAbilityConnection(clientName);
}

RetError AccessibleAbilityChannel::GetElementOperator(
    int32_t accountId, int32_t windowId, int32_t focusType, const std::string &clientName,
    sptr<IAccessibilityElementOperator> &elementOperator, const int32_t treeId)
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
    sptr<AccessibilityWindowConnection> connection =  nullptr;
    connection = accountData->GetAccessibilityWindowConnection(realId);
    if (connection == nullptr) {
        HILOG_ERROR("windowId[%{public}d] has no connection", realId);
        return RET_ERR_NO_WINDOW_CONNECTION;
    }
    if (treeId <= 0) {
        elementOperator = connection->GetProxy();
    } else {
        elementOperator = connection->GetCardProxy(treeId);
    }
    if (!elementOperator) {
        HILOG_ERROR("The proxy of window connection is nullptr");
        return RET_ERR_NULLPTR;
    }
    return RET_OK;
}

bool AccessibleAbilityChannel::CheckWinFromAwm(const int32_t windowId)
{
    std::vector<AccessibilityWindowInfo> windowsFromAwm =
        Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
    if (!windowsFromAwm.empty()) {
        for (const auto& window: windowsFromAwm) {
            if (windowId == window.GetWindowId()) {
                return true;
            }
        }
    }
    return false;
}

RetError AccessibleAbilityChannel::SetIsRegisterDisconnectCallback(bool isRegister)
{
    HILOG_INFO();
    sptr<AccessibleAbilityConnection> clientConnection = GetConnection(accountId_, clientName_);
    if (clientConnection == nullptr) {
        HILOG_ERROR("GetConnection failed, clientName: %{public}s", clientName_.c_str());
        return RET_ERR_NULLPTR;
    }
    clientConnection->SetIsRegisterDisconnectCallback(isRegister);
    return RET_OK;
}

RetError AccessibleAbilityChannel::NotifyDisconnect()
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
    if (accountData == nullptr) {
        HILOG_ERROR("accountData is nullptr");
        return RET_ERR_NULLPTR;
    }
    sptr<AccessibleAbilityConnection> clientConnection = accountData->GetWaitDisConnectAbility(clientName_);
    accountData->RemoveWaitDisconnectAbility(clientName_);
    if (clientConnection == nullptr) {
        HILOG_ERROR("GetConnection failed, clientName: %{public}s", clientName_.c_str());
        return RET_ERR_NULLPTR;
    }
    clientConnection->NotifyDisconnect();
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS