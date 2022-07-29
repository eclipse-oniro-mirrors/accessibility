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

#include "accessibility_ui_test_ability_impl.h"

#include <mutex>
#include "accessible_ability_client.h"
#include "display_manager.h"
#include "hilog_wrapper.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Accessibility {
static std::mutex g_Mutex;
static std::shared_ptr<AccessibilityUITestAbilityImpl> g_Instance = nullptr;

std::shared_ptr<AccessibilityUITestAbility> AccessibilityUITestAbility::GetInstance()
{
    std::lock_guard<std::mutex> lock(g_Mutex);
    if (!g_Instance) {
        g_Instance = std::make_shared<AccessibilityUITestAbilityImpl>();
    }
    return g_Instance;
}

AccessibilityUITestAbilityImpl::AccessibilityUITestAbilityImpl()
{
    HILOG_DEBUG();

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("Failed to get ISystemAbilityManager");
        return;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("Get IAccessibleAbilityManagerService object from samgr failed");
        return;
    }

    serviceProxy_ = iface_cast<AccessibleAbilityManagerServiceProxy>(object);
    if (!serviceProxy_) {
        HILOG_ERROR("Get aams proxy failed");
        return;
    }
}

bool AccessibilityUITestAbilityImpl::RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_INFO();
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return false;
    }

    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->RegisterAbilityListener(listener);
}

RetError AccessibilityUITestAbilityImpl::Connect()
{
    HILOG_INFO();
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }

    return serviceProxy_->EnableUITestAbility(aaClient->GetRemoteObject());
}

bool AccessibilityUITestAbilityImpl::Disconnect()
{
    HILOG_INFO();
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DisableUITestAbility();
}

bool AccessibilityUITestAbilityImpl::GetFocus(
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetFocus(focusType, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetFocusByElementInfo(sourceInfo, focusType, elementInfo);
}

bool AccessibilityUITestAbilityImpl::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->InjectGesture(gesturePath);
}

bool AccessibilityUITestAbilityImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetRoot(elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetRootByWindow(windowInfo, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetWindow(windowId, windowInfo);
}

bool AccessibilityUITestAbilityImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetWindows(windows);
}

bool AccessibilityUITestAbilityImpl::GetWindows(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetWindows(displayId, windows);
}

bool AccessibilityUITestAbilityImpl::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->ExecuteCommonAction(action);
}

bool AccessibilityUITestAbilityImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetNext(elementInfo, direction, nextElementInfo);
}

bool AccessibilityUITestAbilityImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetChildElementInfo(index, parent, child);
}

bool AccessibilityUITestAbilityImpl::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetByContent(elementInfo, text, elementInfos);
}

bool AccessibilityUITestAbilityImpl::GetAnchor(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetAnchor(windowInfo, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetSource(eventInfo, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetParentElementInfo(child, parent);
}

bool AccessibilityUITestAbilityImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo,
    const ActionType action, const std::map<std::string, std::string> &actionArguments)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->ExecuteAction(elementInfo, action, actionArguments);
}

bool AccessibilityUITestAbilityImpl::SetEventTypeFilter(const uint32_t filter)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->SetEventTypeFilter(filter);
}

bool AccessibilityUITestAbilityImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_INFO();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->SetTargetBundleName(targetBundleNames);
}

bool AccessibilityUITestAbilityImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_INFO();
    return AccessibleAbilityClient::GetInstance()->GetChildren(parent, children);
}

void AccessibilityUITestAbilityImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_INFO();
    return AccessibleAbilityClient::GetInstance()->SetCacheMode(cacheMode);
}
} // namespace Accessibility
} // namespace OHOS