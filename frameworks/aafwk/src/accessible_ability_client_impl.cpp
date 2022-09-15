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

#include <chrono>
#include <inttypes.h>
#include <thread>

#include "accessible_ability_client.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
// tmp: wait for window registing when client connect done
constexpr int WAIT_WINDOW_REGIST = 500;
namespace {
    constexpr int32_t ROOT_NONE_ID = -1;
    std::mutex g_Mutex;
    sptr<AccessibleAbilityClientImpl> g_Instance = nullptr;
} // namespace

sptr<AccessibleAbilityClient> AccessibleAbilityClient::GetInstance()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        g_Instance = new(std::nothrow) AccessibleAbilityClientImpl();
    }
    return g_Instance;
}

AccessibleAbilityClientImpl::AccessibleAbilityClientImpl()
{
    HILOG_DEBUG();

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
    HILOG_INFO();
    if (!g_Instance) {
        HILOG_ERROR("instance is nullptr");
        return nullptr;
    }
    return g_Instance->AsObject();
}

bool AccessibleAbilityClientImpl::RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_INFO();
    if (listener_) {
        HILOG_DEBUG("listener already exists.");
        return false;
    }

    listener_ = listener;
    return true;
}

void AccessibleAbilityClientImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId)
{
    HILOG_INFO("channelId[%{public}d]", channelId);
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
        deathRecipient_ = new(std::nothrow) AccessibleAbilityDeathRecipient(*this);
        if (!deathRecipient_) {
            HILOG_ERROR("Failed to create deathRecipient.");
            return;
        }
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
    HILOG_INFO("channelId[%{public}d]", channelId);

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
    HILOG_INFO();
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
    HILOG_INFO("sequence[%{public}d]", sequence);
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

bool AccessibleAbilityClientImpl::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO("focusType[%{public}d]", focusType);
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
    HILOG_INFO("focusType[%{public}d]", focusType);
    if ((focusType != FOCUS_TYPE_INPUT) && (focusType != FOCUS_TYPE_ACCESSIBILITY)) {
        HILOG_ERROR("focusType is not allowed.");
        return false;
    }

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

bool AccessibleAbilityClientImpl::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_INFO();

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

    return channelClient_->SendSimulateGesture(gesturePath);
}

bool AccessibleAbilityClientImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!channelClient_ || !serviceProxy_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t activeWindow = serviceProxy_->GetActiveWindow();
    HILOG_INFO("activeWindow[%{public}d]", activeWindow);
    if (GetCacheElementInfo(activeWindow, ROOT_NONE_ID, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(activeWindow, ROOT_NONE_ID, cacheMode_, elementInfo)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t windowId = windowInfo.GetWindowId();
    HILOG_INFO("windowId[%{public}d]", windowId);
    if (GetCacheElementInfo(windowId, ROOT_NONE_ID, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(windowId, ROOT_NONE_ID, cacheMode_, elementInfo)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_INFO("windowId[%{public}d]", windowId);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindow(windowId, windowInfo);
}

bool AccessibleAbilityClientImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_INFO();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindows(windows);
}

bool AccessibleAbilityClientImpl::GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_INFO("displayId[%{public}" PRIu64 "]", displayId);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->GetWindows(displayId, windows);
}

bool AccessibleAbilityClientImpl::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_INFO("action[%{public}d]", action);
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
    if (direction == DIRECTION_INVALID) {
        HILOG_ERROR("direction is invalid.");
        return false;
    }
    return channelClient_->FocusMoveSearch(elementInfo.GetWindowId(),
        elementInfo.GetAccessibilityId(), direction, nextElementInfo);
}

bool AccessibleAbilityClientImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t windowId = parent.GetWindowId();
    int32_t childId = parent.GetChildId(index);
    HILOG_INFO("windowId[%{public}d], childId[%{public}d]", windowId, childId);
    if (childId == -1) {
        HILOG_ERROR("childId[%{public}d] is invalid", childId);
        return false;
    }
    if (GetCacheElementInfo(windowId, childId, child)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(windowId, childId, cacheMode_, child)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    int32_t windowId = parent.GetWindowId();
    std::vector<int32_t> childIds =  parent.GetChildIds();
    HILOG_INFO("windowId[%{public}d], childIds.size[%{public}zu]", windowId, childIds.size());
    for (auto &childId : childIds) {
        HILOG_DEBUG("childId[%{public}d]", childId);
        if (childId == -1) {
            HILOG_ERROR("childId is invalid");
            return false;
        }

        AccessibilityElementInfo child;
        if (GetCacheElementInfo(windowId, childId, child)) {
            HILOG_DEBUG("get element info from cache");
            children.emplace_back(child);
            continue;
        }

        if (!SearchElementInfoFromAce(windowId, childId, cacheMode_, child)) {
            HILOG_ERROR("Get element info from ace failed");
            return false;
        }
        children.emplace_back(child);
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t windowId = windowInfo.GetWindowId();
    int32_t anchorId = windowInfo.GetAnchorId();
    HILOG_INFO("windowId[%{public}d], anchorId[%{public}d]", windowId, anchorId);
    if (GetCacheElementInfo(windowId, anchorId, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(windowId, anchorId, cacheMode_, elementInfo)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t windowId = eventInfo.GetWindowId();
    int32_t elementId = eventInfo.GetAccessibilityId();
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d]", windowId, elementId);
    if (GetCacheElementInfo(windowId, elementId, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(windowId, elementId, cacheMode_, elementInfo)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    int32_t windowId = child.GetWindowId();
    int32_t elementId = child.GetParentNodeId();
    HILOG_INFO("windowId[%{public}d], parentId[%{public}d]", windowId, elementId);
    if (GetCacheElementInfo(windowId, elementId, parent)) {
        HILOG_DEBUG("get element info from cache");
        return true;
    }

    if (!SearchElementInfoFromAce(windowId, elementId, cacheMode_, parent)) {
        HILOG_ERROR("Get element info from ace failed");
        return false;
    }
    return true;
}

bool AccessibleAbilityClientImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    if (action == ACCESSIBILITY_ACTION_INVALID) {
        HILOG_ERROR("action is invalid.");
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
    HILOG_INFO("filter[%{public}d]", filter);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->SetEventTypeFilter(filter);
}

bool AccessibleAbilityClientImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_INFO("targetBundleNames size[%{public}zu]", targetBundleNames.size());
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }
    return channelClient_->SetTargetBundleName(targetBundleNames);
}

void AccessibleAbilityClientImpl::AccessibleAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_ERROR();
    client_.ResetAAClient(remote);
}

void AccessibleAbilityClientImpl::ResetAAClient(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    if (channelClient_) {
        sptr<IRemoteObject> object = channelClient_->GetRemote();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            channelClient_ = nullptr;
            HILOG_DEBUG("ResetAAClient OK");
        }
    }
}

void AccessibleAbilityClientImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_INFO("set cache mode: [%{public}d]", cacheMode);
    cacheWindowId_ = -1;
    cacheElementInfos_.clear();
    if (cacheMode < 0) {
        cacheMode_ = 0;
    } else {
        uint32_t mode = static_cast<uint32_t>(cacheMode);
        cacheMode_ = mode & static_cast<uint32_t>(GET_SOURCE_PREFETCH_MODE);
    }
}

bool AccessibleAbilityClientImpl::GetCacheElementInfo(const int32_t windowId,
    const int32_t elementId, AccessibilityElementInfo &elementInfo) const
{
    HILOG_DEBUG();
    if (cacheWindowId_ == -1 || cacheWindowId_ != windowId) {
        HILOG_DEBUG("cacheWindowId[%{public}d], windowId[%{public}d]", cacheWindowId_, windowId);
        return false;
    }

    auto iter = cacheElementInfos_.find(elementId);
    if (iter == cacheElementInfos_.end()) {
        HILOG_DEBUG("the element id[%{public}d] is not in cache", elementId);
        return false;
    }

    elementInfo = iter->second;
    return true;
}

void AccessibleAbilityClientImpl::SetCacheElementInfo(const int32_t windowId,
    const std::vector<OHOS::Accessibility::AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("windowId[%{public}d], elementInfos size[%{public}zu]", windowId, elementInfos.size());
    cacheElementInfos_.clear();
    cacheWindowId_ = windowId;
    for (auto &elementInfo : elementInfos) {
        cacheElementInfos_.insert(std::make_pair(elementInfo.GetAccessibilityId(), elementInfo));
    }
}

bool AccessibleAbilityClientImpl::SearchElementInfoFromAce(const int32_t windowId, const int32_t elementId,
    const uint32_t mode, AccessibilityElementInfo &info)
{
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return false;
    }

    std::vector<AccessibilityElementInfo> elementInfos {};
    if (!channelClient_->SearchElementInfosByAccessibilityId(
        windowId, elementId, static_cast<int32_t>(mode), elementInfos)) {
        HILOG_ERROR("search element info failed. windowId[%{public}d] elementId[%{public}d] mode[%{public}d]",
            windowId, elementId, mode);
        return false;
    }
    if (elementInfos.empty()) {
        HILOG_ERROR("elementInfos from ace is empty");
        return false;
    }

    HILOG_DEBUG("element [elementSize:%{public}zu]", elementInfos.size());
    SetCacheElementInfo(windowId, elementInfos);
    info = elementInfos.front();
    return true;
}
} // namespace Accessibility
} // namespace OHOS