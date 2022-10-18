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

#include "accessibility_ability_ut_helper.h"
#include "mock_accessible_ability_client.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    sptr<MockAccessibleAbilityClient> g_Instance = nullptr;
} // namespace

sptr<AccessibleAbilityClient> AccessibleAbilityClient::GetInstance()
{
    HILOG_DEBUG();

    bool isNull = AccessibilityAbilityUtHelper::GetInstance().GetAbilityClientNullFlag();
    if (isNull) {
        return nullptr;
    }
    if (!g_Instance) {
        g_Instance = new(std::nothrow) MockAccessibleAbilityClient();
    }
    return g_Instance;
}

MockAccessibleAbilityClient::MockAccessibleAbilityClient()
{
    HILOG_DEBUG();
}

sptr<IRemoteObject> MockAccessibleAbilityClient::GetRemoteObject()
{
    HILOG_DEBUG();
    return nullptr;
}

bool MockAccessibleAbilityClient::RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG();

    (void)listener;
    return true;
}

bool MockAccessibleAbilityClient::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();

    (void)focusType;
    (void)elementInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();

    (void)sourceInfo;
    (void)focusType;
    (void)elementInfo;
    return true;
}

bool MockAccessibleAbilityClient::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_INFO();

    (void)gesturePath;
    return true;
}

bool MockAccessibleAbilityClient::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();

    (void)elementInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();

    (void)windowInfo;
    (void)elementInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG();

    (void)windowId;
    (void)windowInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();

    (void)windows;
    return true;
}

bool MockAccessibleAbilityClient::GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();

    (void)displayId;
    (void)windows;
    return true;
}

bool MockAccessibleAbilityClient::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG();

    (void)elementInfo;
    (void)direction;
    (void)nextElementInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG();

    (void)index;
    (void)parent;
    (void)child;
    return true;
}

bool MockAccessibleAbilityClient::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG();

    (void)parent;
    (void)children;
    return true;
}

bool MockAccessibleAbilityClient::GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
    std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    
    (void)elementInfo;
    (void)text;
    (void)elementInfos;
    return true;
}

bool MockAccessibleAbilityClient::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();

    (void)eventInfo;
    (void)elementInfo;
    return true;
}

bool MockAccessibleAbilityClient::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();

    (void)child;
    (void)parent;
    return true;
}

bool MockAccessibleAbilityClient::ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
    const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();

    (void)elementInfo;
    (void)action;
    (void)actionArguments;
    return true;
}

bool MockAccessibleAbilityClient::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();

    (void)targetBundleNames;
    return true;
}

void MockAccessibleAbilityClient::SetCacheMode(const int32_t cacheMode)
{
    HILOG_DEBUG();
    AccessibilityAbilityUtHelper::GetInstance().SetCacheMode(cacheMode);
}
} // namespace Accessibility
} // namespace OHOS