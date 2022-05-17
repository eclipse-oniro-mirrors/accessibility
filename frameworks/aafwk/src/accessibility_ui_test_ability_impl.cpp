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

bool AccessibilityUITestAbilityImpl::RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG("start.");
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return false;
    }

    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->RegisterAbilityListener(listener);
}

RetError AccessibilityUITestAbilityImpl::Connect()
{
    HILOG_DEBUG("start.");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return RET_ERR_SAMGR;
    }

    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return RET_ERR_NULLPTR;
    }

    return serviceProxy_->EnableUITestAbility(instance->GetRemoteObject());
}

bool AccessibilityUITestAbilityImpl::Disconnect()
{
    HILOG_DEBUG("start.");
    if (!serviceProxy_) {
        HILOG_ERROR("Failed to get aams service");
        return false;
    }
    return serviceProxy_->DisableUITestAbility();
}

bool AccessibilityUITestAbilityImpl::GetFocus(
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetFocus(focusType, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetFocusByElementInfo(sourceInfo, focusType, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GestureInject(const uint32_t sequence,
    const std::shared_ptr<AccessibilityGestureInjectPath>& gesturePath,
    const std::shared_ptr<AccessibilityGestureResultListener>& listener)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GestureInject(sequence, gesturePath, listener);
}

bool AccessibilityUITestAbilityImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetRoot(elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetRootByWindow(windowInfo, elementInfo);
}

std::vector<AccessibilityWindowInfo> AccessibilityUITestAbilityImpl::GetWindows()
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        std::vector<AccessibilityWindowInfo> temp;
        return temp;
    }
    return instance->GetWindows();
}

std::vector<AccessibilityWindowInfo> AccessibilityUITestAbilityImpl::GetWindows(const uint64_t displayId)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        std::vector<AccessibilityWindowInfo> temp;
        return temp;
    }
    return instance->GetWindows(displayId);
}

bool AccessibilityUITestAbilityImpl::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->ExecuteCommonAction(action);
}

bool AccessibilityUITestAbilityImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetNext(elementInfo, direction, nextElementInfo);
}

bool AccessibilityUITestAbilityImpl::GetChildWindowInfo(const int32_t index, const AccessibilityWindowInfo &parent,
    AccessibilityWindowInfo &child)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetChildWindowInfo(index, parent, child);
}

bool AccessibilityUITestAbilityImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetChildElementInfo(index, parent, child);
}

bool AccessibilityUITestAbilityImpl::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetByContent(elementInfo, text, elementInfos);
}

bool AccessibilityUITestAbilityImpl::GetAnchor(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetAnchor(windowInfo, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetSource(eventInfo, elementInfo);
}

bool AccessibilityUITestAbilityImpl::GetParentWindowInfo(const AccessibilityWindowInfo &child,
    AccessibilityWindowInfo &parent)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetParentWindowInfo(child, parent);
}

bool AccessibilityUITestAbilityImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetParentElementInfo(child, parent);
}

bool AccessibilityUITestAbilityImpl::GetLabeled(const AccessibilityElementInfo &elementInfo,
    AccessibilityElementInfo &labeledElementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetLabeled(elementInfo, labeledElementInfo);
}

bool AccessibilityUITestAbilityImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo,
    const ActionType action, const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->ExecuteAction(elementInfo, action, actionArguments);
}

bool AccessibilityUITestAbilityImpl::SetEventTypeFilter(const uint32_t eventTypes)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->SetEventTypeFilter(eventTypes);
}

bool AccessibilityUITestAbilityImpl::SetTargetBundleName(const std::vector<std::string> targetBundleNames)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->SetTargetBundleName(targetBundleNames);
}
} // namespace Accessibility
} // namespace OHOS