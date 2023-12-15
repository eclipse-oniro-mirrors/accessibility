/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include <cinttypes>
#include <thread>

#include "accessible_ability_client.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
// tmp: wait for window registing when client connect done
constexpr int WAIT_WINDOW_REGIST = 500;
namespace {
    const std::string SYSTEM_PARAMETER_AAMS_NAME = "accessibility.config.ready";
    const std::string SHELL_NAME = "com.huawei.shell_assistant";
    constexpr int32_t CONFIG_PARAMETER_VALUE_SIZE = 10;
    constexpr int32_t ROOT_NONE_ID = -1;
    constexpr int32_t NODE_ID_MAX = 0x7FFFFFFE;
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

sptr<AccessibleAbilityClientImpl> AccessibleAbilityClientImpl::GetAbilityClientImplement()
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
    char value[CONFIG_PARAMETER_VALUE_SIZE] = "default";
    int retSysParam = GetParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(), "false", value, CONFIG_PARAMETER_VALUE_SIZE);
    if (retSysParam >= 0 && !std::strcmp(value, "true")) {
        // Accessibility service is ready
        if (!InitAccessibilityServiceProxy()) {
            HILOG_ERROR("Init accessibility service proxy failed");
        }
    }

    HILOG_DEBUG("Start watching accessibility service.");
    retSysParam = WatchParameter(SYSTEM_PARAMETER_AAMS_NAME.c_str(),
        &AccessibleAbilityClientImpl::OnParameterChanged, this);
    if (retSysParam) {
        HILOG_ERROR("Watch parameter failed, error = %{public}d", retSysParam);
    }
}

AccessibleAbilityClientImpl::~AccessibleAbilityClientImpl()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (serviceProxy_ && serviceProxy_->AsObject()) {
        HILOG_DEBUG("Remove service death recipient");
        serviceProxy_->AsObject()->RemoveDeathRecipient(accessibilityServiceDeathRecipient_);
    }
}

bool AccessibleAbilityClientImpl::InitAccessibilityServiceProxy()
{
    if (serviceProxy_) {
        HILOG_DEBUG("Accessibility Service is connected");
        return true;
    }

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return false;
    }
    HILOG_DEBUG("ISystemAbilityManager obtained");

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return false;
    }
    HILOG_DEBUG("Get remote object ok");

    serviceProxy_ = iface_cast<IAccessibleAbilityManagerService>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("Get aams proxy failed");
        return false;
    }

    // Add death recipient
    if (!accessibilityServiceDeathRecipient_) {
        accessibilityServiceDeathRecipient_ = new(std::nothrow) AccessibilityServiceDeathRecipient(*this);
        if (!accessibilityServiceDeathRecipient_) {
            HILOG_ERROR("Failed to create service deathRecipient.");
            return false;
        }
    }

    if (serviceProxy_->AsObject()) {
        HILOG_DEBUG("Add death recipient");
        serviceProxy_->AsObject()->AddDeathRecipient(accessibilityServiceDeathRecipient_);
    }
    return true;
}

void AccessibleAbilityClientImpl::OnParameterChanged(const char *key, const char *value, void *context)
{
    HILOG_DEBUG("Parameter key = [%{public}s] value = [%{public}s]", key, value);

    if (!key || std::strcmp(key, SYSTEM_PARAMETER_AAMS_NAME.c_str())) {
        HILOG_WARN("not accessibility.config.ready callback");
        return;
    }

    if (!value || std::strcmp(value, "true")) {
        HILOG_WARN("accessibility.config.ready value not true");
        return;
    }

    if (!context) {
        HILOG_ERROR("accessibility.config.ready context NULL");
        return;
    }

    AccessibleAbilityClientImpl* implPtr = static_cast<AccessibleAbilityClientImpl*>(context);
    {
        HILOG_DEBUG("ConnectToService start.");
        std::lock_guard<std::mutex> lock(implPtr->mutex_);
        if (implPtr->InitAccessibilityServiceProxy()) {
            HILOG_DEBUG("ConnectToService Success");
        }
    }
}

sptr<IRemoteObject> AccessibleAbilityClientImpl::GetRemoteObject()
{
    HILOG_DEBUG();
    return this->AsObject();
}

RetError AccessibleAbilityClientImpl::RegisterAbilityListener(
    const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (listener_) {
        HILOG_DEBUG("listener already exists.");
        return RET_ERR_REGISTER_EXIST;
    }

    listener_ = listener;
    return RET_OK;
}

void AccessibleAbilityClientImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId)
{
    HILOG_DEBUG("channelId[%{public}d]", channelId);
    if (!channel || channelId == INVALID_CHANNEL_ID) {
        HILOG_ERROR("channel is nullptr, or channelId is invalid");
        return;
    }

    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!listener_) {
            HILOG_ERROR("listener_ is nullptr.");
            return;
        }
        listener = listener_;
        channelClient_ = std::make_shared<AccessibleAbilityChannelClient>(channelId, channel);

        // Add death recipient
        if (!deathRecipient_) {
            deathRecipient_ = new(std::nothrow) AccessibleAbilityDeathRecipient(*this);
            if (!deathRecipient_) {
                HILOG_ERROR("Failed to create deathRecipient.");
                return;
            }
        }

        if (channel->AsObject()) {
            HILOG_DEBUG("Add death recipient");
            channel->AsObject()->AddDeathRecipient(deathRecipient_);
        }
    }

    isConnected_ = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_WINDOW_REGIST));
    if (listener) {
        listener->OnAbilityConnected();
    }
}

void AccessibleAbilityClientImpl::Disconnect(const int32_t channelId)
{
    HILOG_DEBUG("channelId[%{public}d]", channelId);

    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    {
        isConnected_ = false;
        std::lock_guard<std::mutex> lock(mutex_);
        // Delete death recipient
        if (channelClient_ && channelClient_->GetRemote()) {
            HILOG_ERROR("Remove death recipient");
            channelClient_->GetRemote()->RemoveDeathRecipient(deathRecipient_);
        }

        // Remove channel
        channelClient_ = nullptr;
        listener = listener_;
        listener_ = nullptr;
    }

    if (listener) {
        listener->OnAbilityDisconnected();
    }
}

void AccessibleAbilityClientImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG();
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!channelClient_) {
            HILOG_ERROR("The channel is invalid.");
            return;
        }
        listener = listener_;
    }
    if (listener) {
        listener->OnAccessibilityEvent(eventInfo);
    }
}

void AccessibleAbilityClientImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
    HILOG_DEBUG("sequence[%{public}d]", sequence);
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    std::shared_ptr<AccessibleAbilityChannelClient> channel = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listener = listener_;
        channel = channelClient_;
    }

    if (!channel) {
        HILOG_ERROR("The channel is invalid.");
        return;
    }
    bool handled = false;
    if (listener) {
        std::shared_ptr<MMI::KeyEvent> tmp = std::make_shared<MMI::KeyEvent>(keyEvent);
        handled = listener->OnKeyPressEvent(tmp);
    }
    channel->SetOnKeyPressEventResult(handled, sequence);
}

RetError AccessibleAbilityClientImpl::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("focusType[%{public}d]", focusType);
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if ((focusType != FOCUS_TYPE_INPUT) && (focusType != FOCUS_TYPE_ACCESSIBILITY)) {
        HILOG_ERROR("focusType is not allowed.");
        return RET_ERR_INVALID_PARAM;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    return channelClient_->FindFocusedElementInfo(ANY_WINDOW_ID, ROOT_NODE_ID, focusType, elementInfo);
}

RetError AccessibleAbilityClientImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("focusType[%{public}d]", focusType);
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if ((focusType != FOCUS_TYPE_INPUT) && (focusType != FOCUS_TYPE_ACCESSIBILITY)) {
        HILOG_ERROR("focusType is not allowed.");
        return RET_ERR_INVALID_PARAM;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = sourceInfo.GetWindowId();
    int32_t elementId = sourceInfo.GetAccessibilityId();
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}d], focusType[%{public}d]", windowId, elementId, focusType);

    return channelClient_->FindFocusedElementInfo(windowId, elementId, focusType, elementInfo);
}

RetError AccessibleAbilityClientImpl::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!gesturePath) {
        HILOG_ERROR("The gesturePath is null.");
        return RET_ERR_INVALID_PARAM;
    }

    std::vector<AccessibilityGesturePosition> positions = gesturePath->GetPositions();

    if (positions.size() == 0) {
        HILOG_ERROR("The number of gesture path position is not allowed.");
        return RET_ERR_INVALID_PARAM;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    return channelClient_->SendSimulateGesture(gesturePath);
}

RetError AccessibleAbilityClientImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to connect to aams");
        return RET_ERR_SAMGR;
    }

    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t activeWindow = serviceProxy_->GetActiveWindow();
    HILOG_DEBUG("activeWindow[%{public}d]", activeWindow);
    if (GetCacheElementInfo(activeWindow, ROOT_NONE_ID, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }

    return SearchElementInfoFromAce(activeWindow, ROOT_NONE_ID, cacheMode_, elementInfo);
}

RetError AccessibleAbilityClientImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = windowInfo.GetWindowId();
    HILOG_DEBUG("windowId[%{public}d]", windowId);
    if (GetCacheElementInfo(windowId, ROOT_NONE_ID, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }

    return SearchElementInfoFromAce(windowId, ROOT_NONE_ID, cacheMode_, elementInfo);
}

RetError AccessibleAbilityClientImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("windowId[%{public}d]", windowId);
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    return channelClient_->GetWindow(windowId, windowInfo);
}

RetError AccessibleAbilityClientImpl::GetRootBatch(std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    if (!serviceProxy_) {
        HILOG_ERROR("failed to connect to aams");
        return RET_ERR_SAMGR;
    }

    int32_t windowId = serviceProxy_->GetActiveWindow();
    int32_t elementId = ROOT_NONE_ID;
    RetError ret = SearchElementInfoRecursive(windowId, elementId, PREFETCH_RECURSIVE_CHILDREN, elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("get window element failed.");
        return ret;
    }
    SortElementInfosIfNecessary(elementInfos);
    return RET_OK;
}

void AccessibleAbilityClientImpl::SortElementInfosIfNecessary(std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    std::map<int32_t, std::shared_ptr<AccessibilityElementInfo>> elementInfosMap;
    std::vector<AccessibilityElementInfo> sortedElementInfos;
    int32_t nodeId = NODE_ID_MAX;
    int32_t count = 1;
    AccessibilityElementInfo virtualRoot = elementInfos.front();
    if (virtualRoot.GetAccessibilityId() != NODE_ID_MAX) {
        return;
    }
    elementInfos.erase(elementInfos.begin());
    for (auto &element : elementInfos) {
        if (element.GetAccessibilityId() == NODE_ID_MAX) {
            nodeId = NODE_ID_MAX - count;
            element.SetAccessibilityId(nodeId);
            element.SetParent(NODE_ID_MAX);
            virtualRoot.AddChild(nodeId);
            count += 1;
        }
        nodeId = element.GetAccessibilityId();
        elementInfosMap[nodeId] = std::make_shared<AccessibilityElementInfo>(element);
    }
    elementInfosMap[NODE_ID_MAX] = std::make_shared<AccessibilityElementInfo>(virtualRoot);

    std::vector<int32_t> elementList;
    elementList.push_back(NODE_ID_MAX);
    uint32_t index = 0;
    while (index < elementList.size()) {
        auto iter = elementInfosMap.find(elementList[index]);
        if (iter == elementInfosMap.end()) {
            sortedElementInfos.clear();
            elementInfos.insert(elementInfos.begin(), virtualRoot);
            return;
        }
        sortedElementInfos.push_back(*(iter->second));
        std::vector<int32_t> childNodeIds = iter->second->GetChildIds();
        for (auto &id : childNodeIds) {
            elementList.push_back(id);
        }
        index++;
    }

    if (static_cast<int32_t>(sortedElementInfos.size()) > 0) {
        elementInfos = sortedElementInfos;
    }
    elementInfosMap.clear();
}

RetError AccessibleAbilityClientImpl::GetRootByWindowBatch(const AccessibilityWindowInfo &windowInfo,
    std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("GetRootByWindowBatch %{public}d", windowInfo.GetWindowId());
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("failed to connect to aams");
        return RET_ERR_SAMGR;
    }

    if (!channelClient_) {
        HILOG_ERROR("channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = windowInfo.GetWindowId();
    int32_t elementId = ROOT_NONE_ID;
    RetError ret = SearchElementInfoRecursive(windowId, elementId, PREFETCH_RECURSIVE_CHILDREN, elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("get window element failed");
        return ret;
    }
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    return channelClient_->GetWindows(windows);
}

RetError AccessibleAbilityClientImpl::GetWindows(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG("displayId[%{public}" PRIu64 "]", displayId);
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    return channelClient_->GetWindows(displayId, windows);
}

RetError AccessibleAbilityClientImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}d], direction[%{public}d]",
        elementInfo.GetWindowId(), elementInfo.GetAccessibilityId(), direction);
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    if (direction == DIRECTION_INVALID) {
        HILOG_ERROR("direction is invalid.");
        return RET_ERR_INVALID_PARAM;
    }
    return channelClient_->FocusMoveSearch(elementInfo.GetWindowId(),
        elementInfo.GetAccessibilityId(), direction, nextElementInfo);
}

RetError AccessibleAbilityClientImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = parent.GetWindowId();
    int32_t childId = parent.GetChildId(index);
    HILOG_DEBUG("windowId[%{public}d], childId[%{public}d]", windowId, childId);
    if (childId == -1) {
        HILOG_ERROR("childId[%{public}d] is invalid", childId);
        return RET_ERR_INVALID_PARAM;
    }
    if (GetCacheElementInfo(windowId, childId, child)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }

    return SearchElementInfoFromAce(windowId, childId, cacheMode_, child);
}

RetError AccessibleAbilityClientImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = parent.GetWindowId();
    std::vector<int32_t> childIds =  parent.GetChildIds();
    HILOG_DEBUG("windowId[%{public}d], childIds.size[%{public}zu]", windowId, childIds.size());
    for (auto &childId : childIds) {
        HILOG_DEBUG("childId[%{public}d]", childId);
        if (childId == -1) {
            HILOG_ERROR("childId is invalid");
            return RET_ERR_INVALID_PARAM;
        }

        AccessibilityElementInfo child;
        if (GetCacheElementInfo(windowId, childId, child)) {
            HILOG_DEBUG("get element info from cache");
            children.emplace_back(child);
            continue;
        }

        RetError ret = SearchElementInfoFromAce(windowId, childId, cacheMode_, child);
        if (ret != RET_OK) {
            HILOG_ERROR("Get element info from ace failed");
            return ret;
        }
        children.emplace_back(child);
    }
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetByContent(const AccessibilityElementInfo &elementInfo,
    const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = elementInfo.GetWindowId();
    int32_t elementId = elementInfo.GetAccessibilityId();
    HILOG_DEBUG("windowId %{public}d, elementId %{public}d, text %{public}s", windowId, elementId, text.c_str());
    if (text != "") { // find element condition is null, so we will search all element info
        return channelClient_->SearchElementInfosByText(windowId, elementId, text, elementInfos);
    }

    RetError ret = SearchElementInfoRecursive(windowId, elementId, GET_SOURCE_MODE, elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("get window element info failed");
        return ret;
    }
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    int32_t windowId = eventInfo.GetWindowId();
    int32_t elementId = eventInfo.GetAccessibilityId();
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}d]", windowId, elementId);
    if (GetCacheElementInfo(windowId, elementId, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }
    return SearchElementInfoFromAce(windowId, elementId, cacheMode_, elementInfo);
}

RetError AccessibleAbilityClientImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    int32_t windowId = child.GetWindowId();
    int32_t elementId = child.GetParentNodeId();
    HILOG_DEBUG("windowId[%{public}d], parentId[%{public}d]", windowId, elementId);
    if (GetCacheElementInfo(windowId, elementId, parent)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }

    return SearchElementInfoFromAce(windowId, elementId, cacheMode_, parent);
}

RetError AccessibleAbilityClientImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo,
    const ActionType action, const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    if (action == ACCESSIBILITY_ACTION_INVALID) {
        HILOG_ERROR("action is invalid.");
        return RET_ERR_INVALID_PARAM;
    }
    int32_t windowId = elementInfo.GetWindowId();
    int32_t elementId = elementInfo.GetAccessibilityId();
    HILOG_DEBUG("windowId[%{public}d], elementId[%{public}d], action[%{public}d", windowId, elementId, action);
    return channelClient_->ExecuteAction(windowId, elementId, action,
        const_cast<std::map<std::string, std::string> &>(actionArguments));
}

RetError AccessibleAbilityClientImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG("targetBundleNames size[%{public}zu]", targetBundleNames.size());
    if (!isConnected_) {
        HILOG_ERROR("connection is broken");
        return RET_ERR_NO_CONNECTION;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    return channelClient_->SetTargetBundleName(targetBundleNames);
}

void AccessibleAbilityClientImpl::AccessibleAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_ERROR();
    client_.ResetAAClient(remote);
}

void AccessibleAbilityClientImpl::AccessibilityServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    HILOG_ERROR();
    client_.NotifyServiceDied(remote);
}

void AccessibleAbilityClientImpl::NotifyServiceDied(const wptr<IRemoteObject> &remote)
{
    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!serviceProxy_) {
            HILOG_ERROR("serviceProxy_ is nullptr");
            return;
        }
        sptr<IRemoteObject> object = serviceProxy_->AsObject();
        if (object && (remote == object)) {
            listener = listener_;
            listener_ = nullptr;

            object->RemoveDeathRecipient(accessibilityServiceDeathRecipient_);
            serviceProxy_ = nullptr;
            channelClient_ = nullptr;
            HILOG_DEBUG("ResetAAClient OK");
        }
    }

    isConnected_ = false;
    if (listener) {
        listener->OnAbilityDisconnected();
    }
}

void AccessibleAbilityClientImpl::ResetAAClient(const wptr<IRemoteObject> &remote)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (channelClient_) {
        sptr<IRemoteObject> object = channelClient_->GetRemote();
        if (object && (remote == object)) {
            object->RemoveDeathRecipient(deathRecipient_);
            channelClient_ = nullptr;
            HILOG_DEBUG("ResetAAClient OK");
        }
    }

    isConnected_ = false;
}

RetError AccessibleAbilityClientImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_DEBUG("set cache mode: [%{public}d]", cacheMode);
    std::lock_guard<std::mutex> lock(mutex_);
    cacheWindowId_ = -1;
    cacheElementInfos_.clear();
    if (cacheMode < 0) {
        cacheMode_ = 0;
    } else {
        uint32_t mode = static_cast<uint32_t>(cacheMode);
        cacheMode_ = mode & static_cast<uint32_t>(GET_SOURCE_PREFETCH_MODE);
    }
    return RET_OK;
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

RetError AccessibleAbilityClientImpl::SearchElementInfoFromAce(const int32_t windowId, const int32_t elementId,
    const uint32_t mode, AccessibilityElementInfo &info)
{
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    std::vector<AccessibilityElementInfo> elementInfos {};

    RetError ret = channelClient_->SearchElementInfosByAccessibilityId(
        windowId, elementId, static_cast<int32_t>(mode), elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("search element info failed. windowId[%{public}d] elementId[%{public}d] mode[%{public}d]",
            windowId, elementId, mode);
        return ret;
    }
    if (elementInfos.empty()) {
        HILOG_ERROR("elementInfos from ace is empty");
        return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
    }

    HILOG_DEBUG("element [elementSize:%{public}zu]", elementInfos.size());
    SetCacheElementInfo(windowId, elementInfos);
    info = elementInfos.front();
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::Connect()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    return serviceProxy_->EnableUITestAbility(this->AsObject());
}

RetError AccessibleAbilityClientImpl::Disconnect()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }
    return serviceProxy_->DisableUITestAbility();
}

void AccessibleAbilityClientImpl::SetConnectionState(bool state)
{
    isConnected_ = state;
}

void AccessibleAbilityClientImpl::AddWindowElementMapByWMS(int32_t windowId, int32_t elementId)
{
    // only used for scene board window, non scene board window not need window element map
    int32_t realWindowId = windowId;
    int32_t realElementId = elementId;
    serviceProxy_->GetRealWindowAndElementId(realWindowId, realElementId);
    HILOG_DEBUG("windowId %{public}d, real windowId %{public}d, real elementId %{public}d",
        windowId, realWindowId, realElementId);
    if (windowId != realElementId) {
        windowElementMap_.AddWindowElementIdPair(windowId, realElementId);
    }
}

void AccessibleAbilityClientImpl::AddWindowElementMapByAce(int32_t windowId, int32_t elementId)
{
    // only used for scene board window, non scene board window not need window element map
    if (windowId == SCENE_BOARD_WINDOW_ID) {
        int32_t innerWid = INVALID_SCENE_BOARD_INNER_WINDOW_ID;
        serviceProxy_->GetSceneBoardInnerWinId(windowId, elementId, innerWid);
        HILOG_DEBUG("windowId %{public}d, elementId %{public}d, innerWid %{public}d",
            windowId, elementId, innerWid);
        if (innerWid != INVALID_SCENE_BOARD_INNER_WINDOW_ID) {
            windowElementMap_.AddWindowElementIdPair(innerWid, elementId);
        }
    }
}

RetError AccessibleAbilityClientImpl::GetElementInfoFromCache(int32_t windowId, int32_t elementId,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    if (windowId == SCENE_BOARD_WINDOW_ID) { // sceneboard window id
        if (elementCacheInfo_.GetElementByWindowId(windowId, elementId, elementInfos)) {
            HILOG_DEBUG("get element info from cache");
            return RET_OK;
        }

        std::vector<int32_t> windowsList = windowElementMap_.GetWindowIdList();
        for (auto tmpWindowId : windowsList) {
            if (elementCacheInfo_.GetElementByWindowId(tmpWindowId, elementId, elementInfos)) {
                HILOG_DEBUG("get element info from cache");
                return RET_OK;
            }
        }
    } else {
        if (elementCacheInfo_.GetElementByWindowId(windowId, elementId, elementInfos)) {
            HILOG_DEBUG("get element info from cache");
            return RET_OK;
        }
    }

    return RET_ERR_FAILED;
}

RetError AccessibleAbilityClientImpl::SearchElementInfoRecursive(int32_t windowId, int32_t elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    RetError ret = channelClient_->SearchElementInfosByAccessibilityId(windowId, elementId,
        mode, elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("search element info failed. windowId %{public}d elementId %{public}d", windowId, elementId);
        return ret;
    }

    if (elementInfos.empty()) {
        HILOG_ERROR("elementInfos from ace is empty");
        return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
    }

    ret = CheckElementInfo(windowId, elementId, mode, elementInfos);
    if (ret != RET_OK) {
        HILOG_ERROR("check element info failed. windowId %{public}d", windowId);
        return ret;
    }
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::CheckElementInfo(int32_t windowId, int32_t elementId, int mode,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    AccessibilityElementInfo element = elementInfos.front();
    if (element.GetBundleName() == SHELL_NAME) {
        int32_t tempWid = windowId - 1;
        elementInfos.clear();
        RetError ret = channelClient_->SearchElementInfosByAccessibilityId(tempWid, elementId, mode, elementInfos);
        if (ret != RET_OK) {
            tempWid = tempWid - 1;
            elementInfos.clear();
            return channelClient_->SearchElementInfosByAccessibilityId(tempWid, elementId, mode, elementInfos);
        }
    }
    return RET_OK;
}

void AccessibleAbilityClientImpl::RemoveCacheData(const AccessibilityEventInfo& eventInfo)
{
    EventType type = eventInfo.GetEventType();
    if (type == TYPE_VIEW_TEXT_UPDATE_EVENT || type == TYPE_PAGE_STATE_UPDATE ||
        type == TYPE_NOTIFICATION_UPDATE_EVENT || type == TYPE_PAGE_CONTENT_UPDATE ||
        type == TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT || type == TYPE_WINDOW_UPDATE) {
        int32_t windowId = eventInfo.GetWindowId();
        HILOG_DEBUG("RemoveCacheData windowId %{public}d", windowId);
        if (windowId == SCENE_BOARD_WINDOW_ID) {
            elementCacheInfo_.RemoveElementByWindowId(windowId);
            windowElementMap_.RemovePairByWindowId(windowId);

            auto windowList = windowElementMap_.GetWindowIdList();
            windowElementMap_.RemovePairByWindowIdList(windowList);
            for (auto window: windowList) {
                elementCacheInfo_.RemoveElementByWindowId(window);
            }
        } else {
            elementCacheInfo_.RemoveElementByWindowId(windowId);
            windowElementMap_.RemovePairByWindowId(windowId);
        }
    }
}

void AccessibleAbilityClientImpl::AddCacheByWMS(int32_t windowId, int32_t elementId,
    std::vector<AccessibilityElementInfo>& elementInfos)
{
    HILOG_DEBUG("elementSize %{public}zu", elementInfos.size());
    AddWindowElementMapByWMS(windowId, elementId);
    elementCacheInfo_.AddElementCache(windowId, elementInfos);
}

void AccessibleAbilityClientImpl::AddCacheByAce(int32_t windowId, int32_t elementId,
    std::vector<AccessibilityElementInfo>& elementInfos)
{
    AddWindowElementMapByAce(windowId, elementId);
    HILOG_DEBUG("elementSize %{public}zu", elementInfos.size());
    if (windowId == SCENE_BOARD_WINDOW_ID) {
        windowId = windowElementMap_.GetWindowIdByElementId(elementId);
        HILOG_DEBUG("windowId convert to %{public}d", windowId);
        if (windowId == INVALID_SCENE_BOARD_INNER_WINDOW_ID) {
            elementCacheInfo_.AddElementCache(SCENE_BOARD_WINDOW_ID, elementInfos);
        } else {
            elementCacheInfo_.AddElementCache(windowId, elementInfos);
        }
    } else {
        elementCacheInfo_.AddElementCache(windowId, elementInfos);
    }
}

void AccessibleAbilityClientImpl::ElementCacheInfo::AddElementCache(const int32_t windowId,
    const std::vector<AccessibilityElementInfo>& elementInfos)
{
    std::lock_guard<std::mutex> lock(elementCacheMutex_);
    if (windowIdSet_.size() >= MAX_CACHE_WINDOW_SIZE) {
        auto winId = windowIdSet_.front();
        windowIdSet_.pop_front();
        elementCache_.erase(winId);
    }

    elementCache_.erase(windowId);

    std::map<int32_t, std::shared_ptr<AccessibilityElementInfo>> cache;
    for (auto& elementInfo : elementInfos) {
        int32_t elementId = elementInfo.GetAccessibilityId();
        cache[elementId] = std::make_shared<AccessibilityElementInfo>(elementInfo);
    }

    elementCache_[windowId] = std::move(cache);
    windowIdSet_.push_back(windowId);
}

bool AccessibleAbilityClientImpl::ElementCacheInfo::GetElementByWindowIdBFS(const int32_t realElementId,
    std::vector<AccessibilityElementInfo>& elementInfos,
    std::map<int32_t, std::shared_ptr<AccessibilityElementInfo>>& cache)
{
    std::vector<int32_t> elementList;
    elementList.push_back(realElementId);
    uint32_t index = 0;
    while (index < elementList.size()) {
        auto iter = cache.find(elementList[index]);
        if (iter == cache.end()) {
            elementInfos.clear();
            HILOG_DEBUG("element data abnormal, clear elementInfos");
            return false;
        }

        elementInfos.push_back(*(iter->second));
        std::vector<int32_t> childrenNode = iter->second->GetChildIds();
        for (auto& node : childrenNode) {
            elementList.push_back(node);
        }
        index++;
    }
    return true;
}

// window id is true, element id is true
bool AccessibleAbilityClientImpl::ElementCacheInfo::GetElementByWindowId(const int32_t windowId,
    const int32_t elementId, std::vector<AccessibilityElementInfo>& elementInfos)
{
    elementInfos.clear(); // clear
    std::lock_guard<std::mutex> lock(elementCacheMutex_);
    if (elementCache_.find(windowId) == elementCache_.end()) {
        HILOG_DEBUG("windowId %{public}d is not existed", windowId);
        return false;
    }

    auto& cache = elementCache_.find(windowId)->second;
    if (cache.size() == 0) {
        HILOG_DEBUG("windowId %{public}d element is null", windowId);
        return false;
    }

    if (cache.find(elementId) == cache.end() && elementId != ROOT_NONE_ID) {
        HILOG_DEBUG("elementId %{public}d is not existed", elementId);
        return false;
    }

    int32_t realElementId = elementId;
    if (realElementId == ROOT_NONE_ID) {
        for (auto iter = cache.begin(); iter != cache.end(); iter++) {
            if (iter->second->GetComponentType() == "root") {
                realElementId = iter->first;
                HILOG_DEBUG("find realElementId %{public}d", realElementId);
                break;
            }
        }
    }

    if (realElementId == ROOT_NONE_ID) {
        HILOG_ERROR("elementId %{public}d is not existed", realElementId);
        return false;
    }

    if (!GetElementByWindowIdBFS(realElementId, elementInfos, cache)) {
        HILOG_DEBUG("get window element fail");
        return false;
    }

    HILOG_DEBUG("get element info success, element size %{public}zu", elementInfos.size());
    return true;
}

void AccessibleAbilityClientImpl::ElementCacheInfo::RemoveElementByWindowId(const int32_t windowId)
{
    std::lock_guard<std::mutex> lock(elementCacheMutex_);
    HILOG_DEBUG("erase windowId %{public}d cache", windowId);
    for (auto iter = windowIdSet_.begin(); iter != windowIdSet_.end(); iter++) {
        if (*iter == windowId) {
            windowIdSet_.erase(iter);
            break;
        }
    }

    elementCache_.erase(windowId);
}

bool AccessibleAbilityClientImpl::ElementCacheInfo::IsExistWindowId(int32_t windowId)
{
    std::lock_guard<std::mutex> lock(elementCacheMutex_);
    for (auto iter = windowIdSet_.begin(); iter != windowIdSet_.end(); iter++) {
        if (*iter == windowId) {
            return true;
        }
    }

    return false;
}

bool AccessibleAbilityClientImpl::SceneBoardWindowElementMap::IsExistWindowId(int32_t windowId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    if (windowElementMap_.find(windowId) != windowElementMap_.end()) {
        return true;
    }

    return false;
}

void AccessibleAbilityClientImpl::SceneBoardWindowElementMap::AddWindowElementIdPair(int32_t windowId,
    int32_t elementId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    windowElementMap_[windowId] = elementId;
}

std::vector<int32_t> AccessibleAbilityClientImpl::SceneBoardWindowElementMap::GetWindowIdList()
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    std::vector<int32_t> windowList;
    for (auto iter = windowElementMap_.begin(); iter != windowElementMap_.end(); iter++) {
        windowList.push_back(iter->first);
    }

    return windowList;
}

int32_t AccessibleAbilityClientImpl::SceneBoardWindowElementMap::GetWindowIdByElementId(int32_t elementId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    for (auto iter = windowElementMap_.begin(); iter != windowElementMap_.end(); iter++) {
        if (iter->second == elementId) {
            return iter->first;
        }
    }

    return INVALID_SCENE_BOARD_INNER_WINDOW_ID;
}

void AccessibleAbilityClientImpl::SceneBoardWindowElementMap::RemovePairByWindowIdList(
    std::vector<int32_t>& windowIdList)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    for (auto windowId : windowIdList) {
        windowElementMap_.erase(windowId);
    }
}

void AccessibleAbilityClientImpl::SceneBoardWindowElementMap::RemovePairByWindowId(int32_t windowId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    windowElementMap_.erase(windowId);
}
} // namespace Accessibility
} // namespace OHOS