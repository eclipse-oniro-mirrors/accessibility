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
static const std::string TASK_SEARCH_ELEMENTINFO_BY_ACCESSIBILITYID = "SendEvent";
static const std::string TASK_SEARCH_ELEMENTINFOS_BY_TEXT = "SearchElementInfosByText";
static const std::string TASK_FIND_FOCUSED_ELEMENTINFO = "FindFocusedElementInfo";
static const std::string TASK_FOCUS_MOVE_SEARCH = "FocusMoveSearch";
static const std::string TASK_EXECUTE_ACTION = "ExecuteAction";
static const std::string TASK_GET_WINDOWS = "GetWindows";
static const std::string TASK_EXECUTE_COMMON_ACTION = "ExecuteCommonAction";
static const std::string TASK_SET_ON_KEY_PRESS_EVENT_RESULT = "SetOnKeyPressEventResult";
static const std::string TASK_GET_DISPLAY_RESIZE_SCALE = "GetDisplayResizeScale";
static const std::string TASK_GET_DISPLAY_RESIZE_CENTER_X = "GetDisplayResizeCenterX";
static const std::string TASK_GET_DISPLAY_RESIZE_CENTER_Y = "GetDisplayResizeCenterY";
static const std::string TASK_GET_DISPLAY_RESIZE_RECT = "GetDisplayResizeRect";
static const std::string TASK_RESET_DISPLAY_RESIZE = "ResetDisplayResize";
static const std::string TASK_SET_DISPLAY_RESIZE_SCALE_AND_CENTER = "SetDisplayResizeScaleAndCenter";
static const std::string TASK_SEND_SIMULATE_GESTURE = "SendSimulateGesture";
AccessibleAbilityChannel::AccessibleAbilityChannel(AccessibleAbilityConnection& connection) : connection_(connection)
{
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr.");
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(aams->GetMainRunner());
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
    int32_t realWindowId = AccessibilityWindowManager::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);
    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realWindowId);
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
    int32_t realWindowId = AccessibilityWindowManager::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realWindowId);
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
    int32_t realWindowId = AccessibilityWindowManager::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, focusType);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realWindowId);
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
    int32_t realWindowId = AccessibilityWindowManager::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realWindowId);
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
    const int32_t action, std::map<std::string, std::string> &actionArguments, const int32_t requestId,
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
    const int32_t action, std::map<std::string, std::string> &actionArguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("ExecuteAction accessibilityWindowId = %{public}d", accessibilityWindowId);
    wptr<AccessibilityAccountData> accountData = connection_.GetAccountData();
    if (!accountData.GetRefPtr()) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    int32_t realWindowId = AccessibilityWindowManager::GetInstance().ConvertToRealWindowId(
        accessibilityWindowId, FOCUS_TYPE_INVALID);

    sptr<AccessibilityWindowConnection> connection = accountData->GetAccessibilityWindowConnection(realWindowId);
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

std::vector<AccessibilityWindowInfo> AccessibleAbilityChannel::GetWindows()
{
    HILOG_DEBUG("start");

    std::vector<AccessibilityWindowInfo> ret;
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return ret;
    }
 
    std::promise<std::vector<AccessibilityWindowInfo>> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetWindows,
        this, std::ref(syncPromise));

    eventHandler_->PostTask(task, TASK_GET_WINDOWS);
    ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetWindows(std::promise<std::vector<AccessibilityWindowInfo>> &syncPromise)
{
    HILOG_DEBUG("start");

    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_RETRIEVE)) {
        HILOG_ERROR("AccessibleAbilityChannel::GetWindows failed: no capability");
        std::vector<AccessibilityWindowInfo> windows;
        syncPromise.set_value(windows);
        return;
    }

    std::vector<AccessibilityWindowInfo> windowInfos =
        AccessibilityWindowManager::GetInstance().GetAccessibilityWindows();
    size_t size = windowInfos.size();
    int32_t currentChannelId = connection_.GetChannelId();
    for (size_t i = 0; i < size; i++) {
        windowInfos[i].SetChannelId(currentChannelId);
    }
    syncPromise.set_value(windowInfos);
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
        case GlobalAction::GLOBAL_ACTION_POP_UP_POWER_DIALOG:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_POP_UP_POWER_DIALOG");
            break;
        case GlobalAction::GLOBAL_ACTION_DIVIDE_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_DIVIDE_SCREEN");
            break;
        case GlobalAction::GLOBAL_ACTION_LOCK_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_LOCK_SCREEN");
            break;
        case GlobalAction::GLOBAL_ACTION_CAPTURE_SCREEN:
            HILOG_DEBUG("ExecuteCommonAction action = GLOBAL_ACTION_CAPTURE_SCREEN");
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
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    sptr<KeyEventFilter> keyEventFilter = aams->GetKeyEventFilter();
    if (!keyEventFilter) {
        return;
    }
    keyEventFilter->SetServiceOnKeyEventResult(connection_, handled, sequence);
}

float AccessibleAbilityChannel::GetDisplayResizeScale(const int32_t displayId)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return 0.0f;
    }
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetDisplayResizeScale,
        this, std::ref(syncPromise), displayId);

    eventHandler_->PostTask(task, TASK_GET_DISPLAY_RESIZE_SCALE);
    float ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetDisplayResizeScale(std::promise<float> &syncPromise, const int32_t displayId)
{
    HILOG_DEBUG("start");
    float result = AccessibilityZoomProxy::GetInstance().GetScale(displayId);
    syncPromise.set_value(result);
}

float AccessibleAbilityChannel::GetDisplayResizeCenterX(const int32_t displayId)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return 0.0f;
    }
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetDisplayResizeCenterX,
        this, std::ref(syncPromise), displayId);

    eventHandler_->PostTask(task, TASK_GET_DISPLAY_RESIZE_CENTER_X);
    float ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetDisplayResizeCenterX(std::promise<float> &syncPromise, const int32_t displayId)
{
    HILOG_DEBUG("start");
    float result = AccessibilityZoomProxy::GetInstance().GetCenterX(displayId);
    syncPromise.set_value(result);
}

float AccessibleAbilityChannel::GetDisplayResizeCenterY(const int32_t displayId)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return 0.0f;
    }
    std::promise<float> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetDisplayResizeCenterY,
        this, std::ref(syncPromise), displayId);

    eventHandler_->PostTask(task, TASK_GET_DISPLAY_RESIZE_CENTER_Y);
    float ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetDisplayResizeCenterY(std::promise<float> &syncPromise, const int32_t displayId)
{
    HILOG_DEBUG("start");
    float result = AccessibilityZoomProxy::GetInstance().GetCenterY(displayId);
    syncPromise.set_value(result);
}

Rect AccessibleAbilityChannel::GetDisplayResizeRect(const int32_t displayId)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        Rect temp;
        return temp;
    }
    std::promise<Rect> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerGetDisplayResizeRect,
        this, std::ref(syncPromise), displayId);

    eventHandler_->PostTask(task, TASK_GET_DISPLAY_RESIZE_RECT);
    Rect ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerGetDisplayResizeRect(std::promise<Rect> &syncPromise, const int32_t displayId)
{
    HILOG_DEBUG("start");
    Rect result = AccessibilityZoomProxy::GetInstance().GetDisplayResizeRect(displayId);
    syncPromise.set_value(result);
}

bool AccessibleAbilityChannel::ResetDisplayResize(const int32_t displayId, const bool animate)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerResetDisplayResize,
        this, std::ref(syncPromise), displayId, animate);

    eventHandler_->PostTask(task, TASK_RESET_DISPLAY_RESIZE);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerResetDisplayResize(std::promise<bool> &syncPromise,
    const int32_t displayId, const bool animate)
{
    HILOG_DEBUG("start");
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannel::ResetDisplayResize failed: no capability");
        syncPromise.set_value(false);
        return;
    }
    bool result = AccessibilityZoomProxy::GetInstance().Reset(displayId);
    syncPromise.set_value(result);
}

bool AccessibleAbilityChannel::SetDisplayResizeScaleAndCenter(const int32_t displayId, const float scale,
    const float centerX, const float centerY, const bool animate)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return false;
    }
    std::promise<bool> syncPromise;
    std::future syncFuture = syncPromise.get_future();
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSetDisplayResizeScaleAndCenter,
        this, std::ref(syncPromise), displayId, scale, centerX, centerY, animate);

    eventHandler_->PostTask(task, TASK_SET_DISPLAY_RESIZE_SCALE_AND_CENTER);
    bool ret = syncFuture.get();

    return ret;
}

void AccessibleAbilityChannel::InnerSetDisplayResizeScaleAndCenter(std::promise<bool> &syncPromise,
    const int32_t displayId, const float scale, const float centerX, const float centerY, const bool animate)
{
    HILOG_DEBUG("start");
    if (!(connection_.GetAbilityInfo().GetCapabilityValues() & Capability::CAPABILITY_ZOOM)) {
        HILOG_ERROR("AccessibleAbilityChannel::SetDisplayResizeScaleAndCenter failed: no capability");
        syncPromise.set_value(false);
        return;
    }
    bool result = AccessibilityZoomProxy::GetInstance().SetScaleAndCenter(displayId, scale, centerX, centerY);
    syncPromise.set_value(result);
}

void AccessibleAbilityChannel::SendSimulateGesture(const int32_t requestId,
    const std::vector<AccessibilityGesturePath> &gestureSteps)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr");
        return;
    }
    std::function<void()> task = std::bind(&AccessibleAbilityChannel::InnerSendSimulateGesture,
        this, requestId, gestureSteps);
    eventHandler_->PostTask(task, TASK_SEND_SIMULATE_GESTURE);
}

void AccessibleAbilityChannel::InnerSendSimulateGesture(const int32_t requestId,
    const std::vector<AccessibilityGesturePath> &gestureSteps)
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
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    sptr<TouchEventInjector> touchEventInjector = aams->GetTouchEventInjector();
    if (!touchEventInjector) {
        abilityClient->OnGestureInjectResult(requestId, false);
        return;
    }
    touchEventInjector->InjectEvents(gestureSteps, abilityClient, requestId);
}
} // namespace Accessibility
} // namespace OHOS