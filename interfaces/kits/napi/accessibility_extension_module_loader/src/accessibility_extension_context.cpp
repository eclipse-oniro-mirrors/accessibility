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

#include "accessibility_extension_context.h"
#include "accessible_ability_client.h"
#include "hilog_wrapper.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
bool AccessibilityExtensionContext::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetFocus(focusType, elementInfo);
}

bool AccessibilityExtensionContext::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetFocusByElementInfo(sourceInfo, focusType, elementInfo);
}

bool AccessibilityExtensionContext::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->InjectGesture(gesturePath);
}

bool AccessibilityExtensionContext::InjectGesture(
    const std::vector<std::shared_ptr<AccessibilityGestureInjectPath>> &gesturePaths)
{
    HILOG_DEBUG("This method is temporarily not implemented!");
    return false;
}

bool AccessibilityExtensionContext::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetRoot(elementInfo);
}

bool AccessibilityExtensionContext::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetRootByWindow(windowInfo, elementInfo);
}

bool AccessibilityExtensionContext::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetWindows(windows);
}

bool AccessibilityExtensionContext::GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetWindows(displayId, windows);
}

bool AccessibilityExtensionContext::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->ExecuteCommonAction(action);
}

bool AccessibilityExtensionContext::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetNext(elementInfo, direction, nextElementInfo);
}

bool AccessibilityExtensionContext::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetChildElementInfo(index, parent, child);
}

bool AccessibilityExtensionContext::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetByContent(elementInfo, text, elementInfos);
}

bool AccessibilityExtensionContext::GetAnchor(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetAnchor(windowInfo, elementInfo);
}

bool AccessibilityExtensionContext::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->GetParentElementInfo(child, parent);
}

bool AccessibilityExtensionContext::ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->ExecuteAction(elementInfo, action, actionArguments);
}
bool AccessibilityExtensionContext::SetEventTypeFilter(const uint32_t filter)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->SetEventTypeFilter(filter);
}

bool AccessibilityExtensionContext::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return false;
    }
    return aaClient->SetTargetBundleName(targetBundleNames);
}
} // namespace Accessibility
} // namespace OHOS