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
namespace {
    const std::string TASK_SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID = "SendEvent";
    const std::string TASK_SEARCH_ELEMENTINFOS_BY_TEXT = "SearchElementInfosByText";
    const std::string TASK_FIND_FOCUSED_ELEMENTINFO = "FindFocusedElementInfo";
    const std::string TASK_FOCUS_MOVE_SEARCH = "FocusMoveSearch";
    const std::string TASK_EXECUTE_ACTION = "ExecuteAction";
    const std::string TASK_GET_WINDOWS_BY_DISPLAY_ID = "GetWindowsByDisplayId";
    const std::string TASK_EXECUTE_COMMON_ACTION = "ExecuteCommonAction";
    const std::string TASK_SET_ON_KEY_PRESS_EVENT_RESULT = "SetOnKeyPressEventResult";
    const std::string TASK_SEND_SIMULATE_GESTURE_PATH = "SendSimulateGesturePath";
    const std::string TASK_SET_EVENT_TYPE_FILTER = "SetEventTypeFilter";
    const std::string TASK_SET_TARGET_BUNDLE_NAME = "SetTargetBundleName";
} // namespace

AccessibleAbilityChannel::AccessibleAbilityChannel(AccessibleAbilityConnection& connection) : connection_(connection)
{
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

bool AccessibleAbilityChannel::SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSearchElementInfoByAccessibilityId,
        this, accessibilityWindowId, elementId, requestId, callback, mode);

    eventHandler_->PostTask(task, TASK_SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID);
    return true;
}

void AccessibleAbilityChannel::InnerSearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
    const int32_t mode)
{
    HILOG_DEBUG("start");

    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId failed: no connection");
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId failed: no capability");
        return;
    }
    connection->GetProxy()->SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode);
    HILOG_DEBUG("AccessibleAbilityChannel::SearchElementInfoByAccessibilityId successfully");
}

bool AccessibleAbilityChannel::SearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSearchElementInfosByText,
        this, accessibilityWindowId, elementId, text, requestId, callback);

    eventHandler_->PostTask(task, TASK_SEARCH_ELEMENTINFOS_BY_TEXT);
    return true;
}

void AccessibleAbilityChannel::InnerSearchElementInfosByText(const int32_t accessibilityWindowId,
    const int32_t elementId, const std::string &text, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("SearchElementInfosByText failed");
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::SearchElementInfosByText failed: no capability");
        return;
    }
    connection->GetProxy()->SearchElementInfosByText(elementId, text, requestId, callback);
}

bool AccessibleAbilityChannel::FindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerFindFocusedElementInfo,
        this, accessibilityWindowId, elementId, focusType, requestId, callback);

    eventHandler_->PostTask(task, TASK_FIND_FOCUSED_ELEMENTINFO);
    return true;
}

void AccessibleAbilityChannel::InnerFindFocusedElementInfo(const int32_t accessibilityWindowId,
    const int32_t elementId, const int32_t focusType, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, focusType);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("FindFocusedElementInfo failed");
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::FindFocusedElementInfo failed: no capability");
        return;
    }
    connection->GetProxy()->FindFocusedElementInfo(elementId, focusType, requestId, callback);
}

bool AccessibleAbilityChannel::FocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerFocusMoveSearch,
        this, accessibilityWindowId, elementId, direction, requestId, callback);

    eventHandler_->PostTask(task, TASK_FOCUS_MOVE_SEARCH);
    return true;
}

void AccessibleAbilityChannel::InnerFocusMoveSearch(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("FocusMoveSearch failed");
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::FocusMoveSearch failed: no capability");
        return;
    }
    connection->GetProxy()->FocusMoveSearch(elementId, direction, requestId, callback);
}

bool AccessibleAbilityChannel::ExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerExecuteAction,
        this, accessibilityWindowId, elementId, action, actionArguments, requestId, callback);

    eventHandler_->PostTask(task, TASK_EXECUTE_ACTION);
    return true;
}

void AccessibleAbilityChannel::InnerExecuteAction(const int32_t accessibilityWindowId, const int32_t elementId,
    const int32_t action, const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("ExecuteAction accessibilityWindowId = %{public}d", accessibilityWindowId);
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realId = Singleton<AccessibilityWindowManager>::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realId);
    if (!connection || !connection->GetProxy()) {
        HILOG_ERROR("ExecuteAction failed");
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::ExecuteAction failed: no capability");
        return;
    }
    connection->GetProxy()->ExecuteAction(elementId, action, actionArguments, requestId, callback);
}

bool AccessibleAbilityChannel::GetWindowsByDisplayId(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }
 
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetWindowsByDisplayId,
        this, std::ref(syncPromise), displayId, std::ref(windows));

    eventHandler_->PostTask(task, TASK_GET_WINDOWS_BY_DISPLAY_ID);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetWindowsByDisplayId(std::promise<bool> &syncPromise,
    const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG("start");

    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::GetWindows failed: no capability");
        syncPromise.set_value(false);
        return;
    }

    std::vector<AccessibilityWindowInfo> windowInfos =
        Singleton<AccessibilityWindowManager>::GetInstance().GetAccessibilityWindows();
    int32_t currentChannelId = connection_.GetChannelId();
    for (auto &window : windowInfos) {
        if (window.GetDisplayId() == displayId) {
            window.SetChannelId(currentChannelId);
            windows.push_back(window);
        }
    }
    syncPromise.set_value(true);
}

bool AccessibleAbilityChannel::ExecuteCommonAction(int32_t action)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return false;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerExecuteCommonAction, this, action);

    eventHandler_->PostTask(task, TASK_EXECUTE_COMMON_ACTION);
    return true;
}

void AccessibleAbilityChannel::InnerExecuteCommonAction(int32_t action)
{
    HILOG_DEBUG("ExecuteCommonAction action = %{public}d", action);
    switch (action) {
        case GlobalAction::GLOBAL_ACTION_BACK:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_BACK");
            break;
        case GlobalAction::GLOBAL_ACTION_HOME:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_HOME");
            break;
        case GlobalAction::GLOBAL_ACTION_RECENT:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_RECENT");
            break;
        case GlobalAction::GLOBAL_ACTION_NOTIFICATION:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_NOTIFICATION");
            break;
        case GlobalAction::GLOBAL_ACTION_LOCK_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_LOCK_SCREEN");
            break;
        default:
            HILOG_DEBUG("The action is not exist. %{public}d", action);
            break;
    }

    // Temp deal: need external dependence
    return;
}

void AccessibleAbilityChannel::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_DEBUG("start");

    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }

    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSetOnKeyPressEventResult,
        this, handled, sequence);

    eventHandler_->PostTask(task, TASK_SET_ON_KEY_PRESS_EVENT_RESULT);
}

void AccessibleAbilityChannel::InnerSetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    sptr<KeyEventFilter> keyEventFilter = Singleton<AccessibleAbilityManagerService>::GetInstance().GetKeyEventFilter();
    if (!keyEventFilter) {
        return;
    }
    keyEventFilter->SetServiceOnKeyEventResult(connection_, handled, sequence);
}

void AccessibleAbilityChannel::SendSimulateGesture(const int32_t requestId,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return;
    }
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSendSimulateGesturePath,
        this, requestId, gesturePath);
    eventHandler_->PostTask(task, TASK_SEND_SIMULATE_GESTURE_PATH);
}

void AccessibleAbilityChannel::InnerSendSimulateGesturePath(const int32_t requestId,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath)
{
    HILOG_DEBUG("start");
    sptr<IAccessibleAbilityClient> abilityClient = connection_.GetAbilityClient();
    if (!abilityClient) {
        return;
    }
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_GESTURE)) {
        HILOG_ERROR("AccessibleAbilityChannel::SendSimulateGesture failed: no capability");
        abilityClient->OnGestureInjectResult(requestId, false);
        return;
    }

    sptr<TouchEventInjector> touchEventInjector =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetTouchEventInjector();
    if (!touchEventInjector) {
        abilityClient->OnGestureInjectResult(requestId, false);
        return;
    }
    touchEventInjector->InjectEvents(gesturePath, abilityClient, requestId);
}

bool AccessibleAbilityChannel::SetEventTypeFilter(const uint32_t filter)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSetEventTypeFilter,
        this, std::ref(syncPromise), filter);

    eventHandler_->PostTask(task, TASK_SET_EVENT_TYPE_FILTER);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerSetEventTypeFilter(std::promise<bool> &syncPromise,
    const uint32_t filter)
{
    HILOG_DEBUG("start");
    connection_.SetAbilityInfoEventTypeFilter(filter);
    syncPromise.set_value(true);
}

bool AccessibleAbilityChannel::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSetTargetBundleName,
        this, std::ref(syncPromise), targetBundleNames);

    eventHandler_->PostTask(task, TASK_SET_TARGET_BUNDLE_NAME);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerSetTargetBundleName(std::promise<bool> &syncPromise,
    const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG("start");
    connection_.SetAbilityInfoTargetBundleName(targetBundleNames);
    syncPromise.set_value(true);
}
} // namespace Accessibility
} // namespace OHOS