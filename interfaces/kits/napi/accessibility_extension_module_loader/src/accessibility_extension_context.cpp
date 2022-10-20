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
RetError AccessibilityExtensionContext::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetFocus(focusType, elementInfo);
}

RetError AccessibilityExtensionContext::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
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

RetError AccessibilityExtensionContext::InjectGesture(
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

RetError AccessibilityExtensionContext::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetRoot(elementInfo);
}

RetError AccessibilityExtensionContext::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
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

RetError AccessibilityExtensionContext::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetWindows(windows);
}

RetError AccessibilityExtensionContext::GetWindows(const uint64_t displayId,
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

RetError AccessibilityExtensionContext::GetNext(const AccessibilityElementInfo &elementInfo,
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

RetError AccessibilityExtensionContext::GetChildElementInfo(const int32_t index,
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

RetError AccessibilityExtensionContext::GetByContent(const AccessibilityElementInfo &elementInfo,
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

RetError AccessibilityExtensionContext::GetParentElementInfo(const AccessibilityElementInfo &child,
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

RetError AccessibilityExtensionContext::ExecuteAction(const AccessibilityElementInfo &elementInfo,
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

RetError AccessibilityExtensionContext::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->SetTargetBundleName(targetBundleNames);
}
} // namespace Accessibility
} // namespace OHOS