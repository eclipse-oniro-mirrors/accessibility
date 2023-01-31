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
    constexpr int32_t CONFIG_PARAMETER_VALUE_SIZE = 10;
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
    HILOG_INFO();
    return this->AsObject();
}

RetError AccessibleAbilityClientImpl::RegisterAbilityListener(
    const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_INFO();
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
    HILOG_INFO("channelId[%{public}d]", channelId);
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

    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_WINDOW_REGIST));
    if (listener) {
        listener->OnAbilityConnected();
    }
}

void AccessibleAbilityClientImpl::Disconnect(const int32_t channelId)
{
    HILOG_INFO("channelId[%{public}d]", channelId);

    std::shared_ptr<AccessibleAbilityListener> listener = nullptr;
    {
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
    HILOG_INFO();
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
    HILOG_INFO("sequence[%{public}d]", sequence);
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
    HILOG_INFO("focusType[%{public}d]", focusType);
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
    HILOG_INFO("focusType[%{public}d]", focusType);
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
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d], focusType[%{public}d]", windowId, elementId, focusType);

    return channelClient_->FindFocusedElementInfo(windowId, elementId, focusType, elementInfo);
}

RetError AccessibleAbilityClientImpl::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_INFO();
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
    HILOG_INFO("activeWindow[%{public}d]", activeWindow);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = windowInfo.GetWindowId();
    HILOG_INFO("windowId[%{public}d]", windowId);
    if (GetCacheElementInfo(windowId, ROOT_NONE_ID, elementInfo)) {
        HILOG_DEBUG("get element info from cache");
        return RET_OK;
    }

    return SearchElementInfoFromAce(windowId, ROOT_NONE_ID, cacheMode_, elementInfo);
}

RetError AccessibleAbilityClientImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_INFO("windowId[%{public}d]", windowId);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    return channelClient_->GetWindow(windowId, windowInfo);
}

RetError AccessibleAbilityClientImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_INFO();
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
    HILOG_INFO("displayId[%{public}" PRIu64 "]", displayId);
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
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d], direction[%{public}d]",
        elementInfo.GetWindowId(), elementInfo.GetAccessibilityId(), direction);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = parent.GetWindowId();
    int32_t childId = parent.GetChildId(index);
    HILOG_INFO("windowId[%{public}d], childId[%{public}d]", windowId, childId);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }

    int32_t windowId = parent.GetWindowId();
    std::vector<int32_t> childIds =  parent.GetChildIds();
    HILOG_INFO("windowId[%{public}d], childIds.size[%{public}zu]", windowId, childIds.size());
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    int32_t windowId = elementInfo.GetWindowId();
    int32_t elementId = elementInfo.GetAccessibilityId();
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d], text[%{public}s]", windowId, elementId, text.c_str());
    return channelClient_->SearchElementInfosByText(windowId, elementId, text, elementInfos);
}

RetError AccessibleAbilityClientImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    int32_t windowId = eventInfo.GetWindowId();
    int32_t elementId = eventInfo.GetAccessibilityId();
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d]", windowId, elementId);
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
    std::lock_guard<std::mutex> lock(mutex_);
    if (!channelClient_) {
        HILOG_ERROR("The channel is invalid.");
        return RET_ERR_NO_CONNECTION;
    }
    int32_t windowId = child.GetWindowId();
    int32_t elementId = child.GetParentNodeId();
    HILOG_INFO("windowId[%{public}d], parentId[%{public}d]", windowId, elementId);
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
    HILOG_INFO("windowId[%{public}d], elementId[%{public}d], action[%{public}d", windowId, elementId, action);
    return channelClient_->ExecuteAction(windowId, elementId, action,
        const_cast<std::map<std::string, std::string> &>(actionArguments));
}

RetError AccessibleAbilityClientImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_INFO("targetBundleNames size[%{public}zu]", targetBundleNames.size());
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
}

RetError AccessibleAbilityClientImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_INFO("set cache mode: [%{public}d]", cacheMode);
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
} // namespace Accessibility
} // namespace OHOS