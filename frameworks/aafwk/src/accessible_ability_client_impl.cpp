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

#include "accessible_ability_client_impl.h"
#include "accessible_ability_client.h"
#include "accessibility_display_resize_controller.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
static const int32_t NONE_ID = -1;

static std::mutex g_Mutex;
static sptr<AccessibleAbilityClientImpl> g_Instance = nullptr;
sptr<AccessibleAbilityClient> AccessibleAbilityClient::GetInstance()
{
    HILOG_DEBUG("start.");
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        g_Instance = new AccessibleAbilityClientImpl();
    }
    return g_Instance;
}

AccessibleAbilityClientImpl::AccessibleAbilityClientImpl()
{
    HILOG_DEBUG("start.");

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return;
    }
    HILOG_DEBUG("ISystemAbilityManager obtained");

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return;
    }
    HILOG_DEBUG("Get remote object ok");

    serviceProxy_ = iface_cast<AccessibleAbilityManagerServiceProxy>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("Get aams proxy failed");
        return;
    }
}

sptr<IRemoteObject> AccessibleAbilityClientImpl::GetRemoteObject()
{
    HILOG_DEBUG("start.");
    if (!g_Instance) {
        HILOG_ERROR("instance is nullptr");
        return nullptr;
    }
    return g_Instance->AsObject();
}

bool AccessibleAbilityClientImpl::RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG("start.");
    if (listener_) {
        HILOG_DEBUG("listener already exists.");
        return false;
    }

    listener_ = listener;
    return true;
}

void AccessibleAbilityClientImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId)
{
    HILOG_DEBUG("start.");
    if (!channel && channelId == INVALID_CHANNEL_ID) {
        HILOG_DEBUG("channel is nullptr, or channelId is invalid");
        return;
    }

    if (!listener_) {
        HILOG_ERROR("listener_ is nullptr.");
        return;
    }

    channelClient_ = std::make_shared<AccessibleAbilityChannelClient>(channelId, channel);

    // Add death recipient
    if (!deathRecipient_) {
        deathRecipient_ = new AccessibleAbilityDeathRecipient(*this);
    }

    auto object = channelClient_->GetRemote();
    if (object) {
        HILOG_DEBUG("Add death recipient");
        object->AddDeathRecipient(deathRecipient_);
    }
    listener_->OnAbilityConnected();
}

void AccessibleAbilityClientImpl::Disconnect(const int32_t channelId)
{
    HILOG_DEBUG("start.");

    // Delete death recipient
    if (channelClient_ && channelClient_->GetRemote()) {
        HILOG_ERROR("Remove death recipient");
        channelClient_->GetRemote()->RemoveDeathRecipient(deathRecipient_);
    }

    // Remove channel
    channelClient_ = nullptr;

    if (listener_) {
        listener_->OnAbilityDisconnected();
        listener_ = nullptr;
    }
}

void AccessibleAbilityClientImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return;
    }
    if (listener_) {
        listener_->OnAccessibilityEvent(eventInfo);
    }
}

void AccessibleAbilityClientImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return;
    }
    if (listener_) {
        std::shared_ptr<MMI::KeyEvent> tmp = std::make_shared<MMI::KeyEvent>(keyEvent);
        bool handled = listener_->OnKeyPressEvent(tmp);
        channelClient_->SetOnKeyPressEventResult(handled, sequence);
    }
}

void AccessibleAbilityClientImpl::OnDisplayResized(const int32_t displayId, const Rect &rect, const float scale,
    const float centerX, const float centerY)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return;
    }

    std::shared_ptr<DisplayResizeController> displayResizeController = GetDisplayResizeController(displayId);
    if (!displayResizeController) {
        HILOG_ERROR("There is no displayResizeController.");
        return;
    }
    displayResizeControllers_[displayId]->DispatchOnDisplayResized(rect, scale, centerX, centerY);
}

void AccessibleAbilityClientImpl::OnGestureInjectResult(const int32_t sequence, const bool completedSuccessfully)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return;
    }
    DispatchGestureInjectResult(sequence, completedSuccessfully);
}

bool AccessibleAbilityClientImpl::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if ((focusType != FOCUS_TYPE_INPUT) && (focusType != FOCUS_TYPE_ACCESSIBILITY)) {
        HILOG_ERROR("focusType is not allowed.");
        return false;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    return channelClient_->FindFocusedElementInfo(ANY_WINDOW_ID, ROOT_NODE_ID, focusType, elementInfo);
}

bool AccessibleAbilityClientImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t channelId = sourceInfo.GetChannelId();
    int32_t windowId = sourceInfo.GetWindowId();
    int32_t elementId = sourceInfo.GetAccessibilityId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], elementId[%{public}d], focusType[%{public}d]",
        channelId, windowId, elementId, focusType);

    return channelClient_->FindFocusedElementInfo(windowId, elementId, focusType, elementInfo);
}

bool AccessibleAbilityClientImpl::GestureInject(const uint32_t sequence,
    const std::vector<AccessibilityGesturePath> &gesturePathList,
    const std::shared_ptr<AccessibilityGestureResultListener> &listener)
{
    HILOG_DEBUG("start.");

    if (gesturePathList.size() > AccessibilityGesturePath::MAX_GESTURE_PATHS || gesturePathList.size() <= 0) {
        HILOG_ERROR("The number of gesturePathDefine : [%{public}zu] is not allowed.", gesturePathList.size());
        return false;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int64_t totalDurationTime = 0;
    for (auto &gesturePath : gesturePathList) {
        totalDurationTime += gesturePath.GetDurationTime();
    }
    HILOG_DEBUG("The total duration time is %{public}jd.", totalDurationTime);

    if (totalDurationTime > AccessibilityGesturePath::MAX_GESTURE_DURATION) {
        HILOG_ERROR("The total duration time : [%{public}jd] is not allowed.", totalDurationTime);
        return false;
    }

    if (listener) {
        gestureResultListenerInfos_.insert(make_pair(sequence, listener));
    }

    channelClient_->SendSimulateGesture(sequence, gesturePathList);
    return true;
}

std::shared_ptr<DisplayResizeController> AccessibleAbilityClientImpl::GetDisplayResizeController()
{
    HILOG_DEBUG("start.");

    // this is a temp deal: To make sure the id of DEFAULT_DISPALY.
    int32_t DEFAULT_DISPALY = 0;
    return GetDisplayResizeController(DEFAULT_DISPALY);
}

std::shared_ptr<DisplayResizeController> AccessibleAbilityClientImpl::GetDisplayResizeController(
    const int32_t displayId)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return nullptr;
    }

    auto iter = displayResizeControllers_.find(displayId);
    if (iter != displayResizeControllers_.end()) {
        return displayResizeControllers_[displayId];
    } else {
        HILOG_DEBUG("Have no DisplayResizeController and make a new one.");
        displayResizeControllers_[displayId] =
            std::make_shared<DisplayResizeControllerImpl>(displayId, channelClient_);
        return displayResizeControllers_[displayId];
    }
}

bool AccessibleAbilityClientImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_ || !serviceProxy_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t activeWindow = serviceProxy_->GetActiveWindow();
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = channelClient_->SearchElementInfosByAccessibilityId(activeWindow, NONE_ID, 0, elementInfos);
    if (!result) {
        return false;
    }

    HILOG_DEBUG("element [elementSize:%{public}zu]", elementInfos.size());
    if (!elementInfos.empty()) {
        elementInfo = elementInfos[0];
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t channelId = windowInfo.GetChannelId();
    int32_t windowId = windowInfo.GetWindowId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d]", channelId, windowId);
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, -1, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetRootByWindow OK.");
        elementInfo = elementInfos.front();
    }
    return result;
}

std::vector<AccessibilityWindowInfo> AccessibleAbilityClientImpl::GetWindows()
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        std::vector<AccessibilityWindowInfo> temp;
        return temp;
    }
    return channelClient_->GetWindows();
}

bool AccessibleAbilityClientImpl::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    if ((action != GLOBAL_ACTION_BACK) && (action != GLOBAL_ACTION_HOME) &&
        (action != GLOBAL_ACTION_RECENT) && (action != GLOBAL_ACTION_NOTIFICATION) &&
        (action != GLOBAL_ACTION_POP_UP_POWER_DIALOG) && (action != GLOBAL_ACTION_DIVIDE_SCREEN) &&
        (action != GLOBAL_ACTION_LOCK_SCREEN) && (action != GLOBAL_ACTION_CAPTURE_SCREEN)) {
        HILOG_ERROR("action is not allowed.");
        return false;
    }
    return channelClient_->ExecuteCommonAction(action);
}

bool AccessibleAbilityClientImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], elementId[%{public}d], direction[%{public}d]",
        elementInfo.GetChannelId(), elementInfo.GetWindowId(), elementInfo.GetAccessibilityId(), direction);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->FocusMoveSearch(elementInfo.GetWindowId(),
        elementInfo.GetAccessibilityId(), direction, nextElementInfo);
}

bool AccessibleAbilityClientImpl::GetChildWindowInfo(const int32_t index, const AccessibilityWindowInfo &parent,
    AccessibilityWindowInfo &child)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = parent.GetChannelId();
    int32_t childId = parent.GetChildIdByIndex(index);
    HILOG_INFO("channelId[%{public}d], childId[%{public}d]", channelId, childId);
    if (childId == -1) {
        HILOG_ERROR("index[%{public}d] is invalid", childId);
        return false;
    }
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> windows = channelClient_->GetWindows();
    for (auto &window : windows) {
        if (window.GetWindowId() == childId) {
            HILOG_DEBUG("GetWindows OK");
            child = window;
            break;
        }
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = parent.GetChannelId();
    int32_t windowId = parent.GetWindowId();
    int32_t childId = parent.GetChildId(index);
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], childId[%{public}d]", channelId, windowId, childId);
    if (childId == -1) {
        HILOG_ERROR("childId[%{public}d] is invalid", childId);
        return false;
    }
    bool result = false;
    std::vector<AccessibilityElementInfo> elementInfos {};
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, childId, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetChildElementInfo OK");
        child = elementInfos.front();
    }
    return result;
}

bool AccessibleAbilityClientImpl::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = elementInfo.GetChannelId();
    int32_t windowId = elementInfo.GetWindowId();
    int32_t elementId = elementInfo.GetAccessibilityId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], elementId[%{public}d], text[%{public}s]",
        channelId, windowId, elementId, text.c_str());
    return channelClient_->SearchElementInfosByText(windowId, elementId, text, elementInfos);
}

bool AccessibleAbilityClientImpl::GetAnchor(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = windowInfo.GetChannelId();
    int32_t windowId = windowInfo.GetWindowId();
    int32_t anchorId = windowInfo.GetAnchorId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], anchorId[%{public}d]",
        channelId, windowId, anchorId);
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, anchorId, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetAnchor OK");
        elementInfo = elementInfos.front();
    }
    return result;
}

bool AccessibleAbilityClientImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = eventInfo.GetChannelId();
    int32_t windowId = eventInfo.GetWindowId();
    int32_t elementId = eventInfo.GetAccessibilityId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], elementId[%{public}d]", channelId, windowId, elementId);
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, elementId, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetSource OK");
        elementInfo = elementInfos.front();
    }
    return result;
}

bool AccessibleAbilityClientImpl::GetParentWindowInfo(
    const AccessibilityWindowInfo &child, AccessibilityWindowInfo &parent)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = child.GetChannelId();
    int32_t parentId = child.GetParentId();
    HILOG_INFO("channelId[%{public}d], parentId[%{public}d]", channelId, parentId);
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> windows = channelClient_->GetWindows();
    for (auto &window : windows) {
        if (window.GetWindowId() == parentId) {
            HILOG_DEBUG("GetParentWindowInfo OK");
            parent = window;
            break;
        }
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = child.GetChannelId();
    int32_t windowId = child.GetWindowId();
    int32_t elementId = child.GetParentNodeId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], parentId[%{public}d]", channelId, windowId, elementId);
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, elementId, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetParentElementInfo OK");
        parent = elementInfos.front();
    }
    return result;
}

bool AccessibleAbilityClientImpl::GetLabeled(const AccessibilityElementInfo &elementInfo,
    AccessibilityElementInfo &labeledElementInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = elementInfo.GetChannelId();
    int32_t windowId = elementInfo.GetWindowId();
    int32_t labeled = elementInfo.GetLabeledAccessibilityId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], labeled[%{public}d]", channelId, windowId, labeled);
    std::vector<AccessibilityElementInfo> elementInfos {};
    bool result = false;
    result = channelClient_->SearchElementInfosByAccessibilityId(windowId, labeled, 0, elementInfos);
    if (!elementInfos.empty()) {
        HILOG_DEBUG("GetLabeled OK");
        labeledElementInfo = elementInfos.front();
    }
    return result;
}

bool AccessibleAbilityClientImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t channelId = elementInfo.GetChannelId();
    int32_t windowId = elementInfo.GetWindowId();
    int32_t elementId = elementInfo.GetAccessibilityId();
    HILOG_INFO("channelId[%{public}d], windowId[%{public}d], elementId[%{public}d], action[%{public}d",
        channelId, windowId, elementId, action);
    return channelClient_->ExecuteAction(windowId, elementId, action,
        const_cast<std::map<std::string, std::string> &>(actionArguments));
}

void AccessibleAbilityClientImpl::AccessibleAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_DEBUG("start.");
    client_.ResetAAClient(remote);
}

void AccessibleAbilityClientImpl::ResetAAClient(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG("start.");
    if (channelClient_) {
        sptr<IRemoteObject> object = channelClient_->GetRemote();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            channelClient_ = nullptr;
            HILOG_DEBUG("ResetAAClient OK");
        }
    }
}

void AccessibleAbilityClientImpl::DispatchGestureInjectResult(uint32_t sequence, bool result)
{
    HILOG_DEBUG("start.");

    if (gestureResultListenerInfos_.empty()) {
        HILOG_ERROR("There is no information of gestureResultListener");
        return;
    }

    std::shared_ptr<AccessibilityGestureResultListener> gestureResultListener = nullptr;
    auto it = gestureResultListenerInfos_.find(sequence);
    if (it != gestureResultListenerInfos_.end()) {
        HILOG_DEBUG("gestureResultListenerInfo has been found.");
        gestureResultListener = gestureResultListenerInfos_[sequence];
    }

    if (!gestureResultListener) {
        HILOG_ERROR("There is no gestureResultListenerInfo in gestureResultListenerInfos_[%{public}d", sequence);
        return;
    }

    HILOG_DEBUG("GestureInject result is %{public}d", result);
    gestureResultListener->OnGestureInjectResult(sequence, result);
}
} // namespace Accessibility
} // namespace OHOS