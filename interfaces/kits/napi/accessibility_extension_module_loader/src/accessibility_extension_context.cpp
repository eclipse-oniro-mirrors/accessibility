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
#include "ability_manager_client.h"
#include "accessibility_permission.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
int AccessibilityExtensionContext::illegalRequestCode_(-1);
const std::string WANT_VERIFY = "";
RetError AccessibilityExtensionContext::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->InjectGesture(gesturePath);
}

RetError AccessibilityExtensionContext::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetRootByWindow(windowInfo, elementInfo);
}

RetError AccessibilityExtensionContext::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
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
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->ExecuteAction(elementInfo, action, actionArguments);
}

RetError AccessibilityExtensionContext::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->SetTargetBundleName(targetBundleNames);
}

RetError AccessibilityExtensionContext::StartAbility(const AAFwk::Want &want)
{
    HILOG_DEBUG();
    if (!Permission::IsSystemApp()) {
        HILOG_ERROR("Not system app");
        return RET_ERR_NOT_SYSTEM_APP;
    }

    if (want.GetBundle() == WANT_VERIFY || want.GetElement().GetAbilityName() == WANT_VERIFY) {
        HILOG_ERROR("Start ability failed, Want Parameter error.");
        return RET_ERR_INVALID_PARAM;
    }
    HILOG_DEBUG("bundleName: %{public}s, abilityName: %{public}s",
        want.GetBundle().c_str(), want.GetElement().GetAbilityName().c_str());

    auto ret = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, token_, illegalRequestCode_);
    if (ret != ERR_OK) {
        HILOG_ERROR("StartAbility is failed %{public}d", ret);
        return RET_ERR_FAILED;
    }
    return RET_OK;
}

RetError AccessibilityExtensionContext::EnableScreenCurtain(const bool isEnable)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->EnableScreenCurtain(isEnable);
}

RetError AccessibilityExtensionContext::GetElements(const int32_t windowId, const int64_t elementId,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetElements(windowId, elementId, elementInfos);
}

RetError AccessibilityExtensionContext::GetDefaultFocusedElementIds(const int32_t windowId,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (aaClient == nullptr) {
        HILOG_ERROR("aaClient is nullptr");
        return RET_ERR_NULLPTR;
    }
    return aaClient->GetDefaultFocusedElementIds(windowId, elementInfos);
}
} // namespace Accessibility
} // namespace OHOS