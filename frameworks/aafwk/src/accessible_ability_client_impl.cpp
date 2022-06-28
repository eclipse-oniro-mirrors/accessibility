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

#include <thread>
#include <chrono>

#include "accessible_ability_client.h"
#include "display_manager.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
// tmp: wait for window registing when client connect done
constexpr int WAIT_WINDOW_REGIST = 500;
namespace {
    constexpr int32_t NONE_ID = -1;
    std::mutex g_Mutex;
    sptr<AccessibleAbilityClientImpl> g_Instance = nullptr;
} // namespace

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

    serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(object);
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
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_WINDOW_REGIST));
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

bool AccessibleAbilityClientImpl::InjectGesture(const uint32_t sequence,
    const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath,
    const std::shared_ptr<AccessibilityGestureResultListener> &listener)
{
    HILOG_DEBUG("start.");

    if (!gesturePath) {
        HILOG_ERROR("The gesturePath is null.");
        return false;
    }

    std::vector<AccessibilityGesturePosition> positions = gesturePath->GetPositions();

    if (positions.size() <= 0) {
        HILOG_ERROR("The number of gesture path position is not allowed.");
        return false;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    if (listener) {
        gestureResultListenerInfos_.insert(make_pair(sequence, listener));
    }

    channelClient_->SendSimulateGesture(sequence, gesturePath);
    return true;
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

bool AccessibleAbilityClientImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindow(windowId, windowInfo);
}

bool AccessibleAbilityClientImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindows(windows);
}

bool AccessibleAbilityClientImpl::GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindows(displayId, windows);
}

bool AccessibleAbilityClientImpl::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    if (action >= GLOBAL_ACTION_MAX) {
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

bool AccessibleAbilityClientImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t windowId = parent.GetWindowId();
    std::vector<int32_t> childIds =  parent.GetChildIds();
    HILOG_DEBUG("The size of childIds is %{public}zu", childIds.size());
    for (auto &childId : childIds) {
        HILOG_INFO("windowId[%{public}d], childId[%{public}d]", windowId, childId);
        if (childId == -1) {
            HILOG_ERROR("childId is invalid");
            return false;
        }

        std::vector<AccessibilityElementInfo> elementInfos {};
        bool result = channelClient_->SearchElementInfosByAccessibilityId(windowId, childId, 0, elementInfos);
        if (!result) {
            HILOG_ERROR("Search elementInfos by AccessibilityId failed. windowId[%{public}d], childId[%{public}d]",
                windowId, childId);
            return false;
        } else {
            if (!elementInfos.empty()) {
                HILOG_DEBUG("Get child elementInfo OK. windowId[%{public}d], childId[%{public}d]", windowId, childId);
                children.emplace_back(elementInfos.front());
            }
        }
    }
    return true;
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

bool AccessibleAbilityClientImpl::SetEventTypeFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->SetEventTypeFilter(filter);
}

bool AccessibleAbilityClientImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG("start.");
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->SetTargetBundleName(targetBundleNames);
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
        gestureResultListenerInfos_.erase(it);
    }

    if (!gestureResultListener) {
        HILOG_ERROR("There is no gestureResultListenerInfo in gestureResultListenerInfos_[%{public}d", sequence);
        return;
    }

    HILOG_DEBUG("InjectGesture result is %{public}d", result);
    gestureResultListener->OnGestureInjectResult(sequence, result);
}
} // namespace Accessibility
} // namespace OHOS