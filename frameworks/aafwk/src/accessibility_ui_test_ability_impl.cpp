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

#include "accessibility_ui_test_ability_impl.h"

#include <mutex>
#include "accessible_ability_client_impl.h"
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

RetError AccessibilityUITestAbilityImpl::RegisterAbilityListener(
    const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG();
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return RET_ERR_INVALID_PARAM;
    }

    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->RegisterAbilityListener(listener);
}

RetError AccessibilityUITestAbilityImpl::Connect()
{
    HILOG_INFO();
    sptr<AccessibleAbilityClientImpl> aaClient = AccessibleAbilityClientImpl::GetAbilityClientImplement();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->Connect();
}

RetError AccessibilityUITestAbilityImpl::Disconnect()
{
    HILOG_INFO();
    sptr<AccessibleAbilityClientImpl> aaClient = AccessibleAbilityClientImpl::GetAbilityClientImplement();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->Disconnect();
}

RetError AccessibilityUITestAbilityImpl::GetFocus(
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetFocus(focusType, elementInfo);
}

RetError AccessibilityUITestAbilityImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetFocusByElementInfo(sourceInfo, focusType, elementInfo);
}

RetError AccessibilityUITestAbilityImpl::InjectGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->InjectGesture(gesturePath);
}

RetError AccessibilityUITestAbilityImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetRoot(elementInfo);
}

RetError AccessibilityUITestAbilityImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetRootByWindow(windowInfo, elementInfo);
}

RetError AccessibilityUITestAbilityImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetWindow(windowId, windowInfo);
}

RetError AccessibilityUITestAbilityImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetWindows(windows);
}

RetError AccessibilityUITestAbilityImpl::GetWindows(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetWindows(displayId, windows);
}

RetError AccessibilityUITestAbilityImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetNext(elementInfo, direction, nextElementInfo);
}

RetError AccessibilityUITestAbilityImpl::GetChildElementInfo(const int32_t index,
    const AccessibilityElementInfo &parent, AccessibilityElementInfo &child)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetChildElementInfo(index, parent, child);
}

RetError AccessibilityUITestAbilityImpl::GetByContent(const AccessibilityElementInfo &elementInfo,
    const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetByContent(elementInfo, text, elementInfos);
}

RetError AccessibilityUITestAbilityImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetSource(eventInfo, elementInfo);
}

RetError AccessibilityUITestAbilityImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetParentElementInfo(child, parent);
}

RetError AccessibilityUITestAbilityImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo,
    const ActionType action, const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->ExecuteAction(elementInfo, action, actionArguments);
}

RetError AccessibilityUITestAbilityImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->SetTargetBundleName(targetBundleNames);
}

RetError AccessibilityUITestAbilityImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG();
    return AccessibleAbilityClient::GetInstance()->GetChildren(parent, children);
}

RetError AccessibilityUITestAbilityImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_DEBUG();
    return AccessibleAbilityClient::GetInstance()->SetCacheMode(cacheMode);
}
} // namespace Accessibility
} // namespace OHOS