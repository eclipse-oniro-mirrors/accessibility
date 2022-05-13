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
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetFocus(focusType, elementInfo);
}

bool AccessibilityExtensionContext::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
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

bool AccessibilityExtensionContext::GestureInject(const uint32_t sequence,
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

bool AccessibilityExtensionContext::GestureInject(const uint32_t sequence,
    const std::vector<std::shared_ptr<AccessibilityGestureInjectPath>>& gesturePaths,
    const std::shared_ptr<AccessibilityGestureResultListener> &listener)
{
    HILOG_DEBUG("This method is temporarily not implemented!");
    return false;
}

bool AccessibilityExtensionContext::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->GetRoot(elementInfo);
}

bool AccessibilityExtensionContext::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
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

std::vector<AccessibilityWindowInfo> AccessibilityExtensionContext::GetWindows()
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

std::vector<AccessibilityWindowInfo> AccessibilityExtensionContext::GetWindows(const uint64_t displayId)
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

bool AccessibilityExtensionContext::ExecuteCommonAction(const GlobalAction action)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->ExecuteCommonAction(action);
}

bool AccessibilityExtensionContext::GetNext(const AccessibilityElementInfo &elementInfo,
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

bool AccessibilityExtensionContext::GetChildWindowInfo(const int32_t index, const AccessibilityWindowInfo &parent,
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

bool AccessibilityExtensionContext::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
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

bool AccessibilityExtensionContext::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
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

bool AccessibilityExtensionContext::GetAnchor(const AccessibilityWindowInfo &windowInfo,
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

bool AccessibilityExtensionContext::GetSource(const AccessibilityEventInfo &eventInfo,
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

bool AccessibilityExtensionContext::GetParentWindowInfo(const AccessibilityWindowInfo &child,
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

bool AccessibilityExtensionContext::GetParentElementInfo(const AccessibilityElementInfo &child,
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

bool AccessibilityExtensionContext::GetLabeled(const AccessibilityElementInfo &elementInfo,
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

bool AccessibilityExtensionContext::ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->ExecuteAction(elementInfo, action, actionArguments);
}
bool AccessibilityExtensionContext::SetEventTypeFilter(const uint32_t eventTypes)
{
    HILOG_DEBUG("start.");
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return false;
    }
    return instance->SetEventTypeFilter(eventTypes);
}

bool AccessibilityExtensionContext::SetTargetBundleName(const std::vector<std::string> targetBundleNames)
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