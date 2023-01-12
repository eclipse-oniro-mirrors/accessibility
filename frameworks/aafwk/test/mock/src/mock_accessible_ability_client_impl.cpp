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

#include "accessibility_ability_ut_helper.h"
#include "accessible_ability_client_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    std::mutex g_Mutex;
    sptr<AccessibleAbilityClientImpl> g_Instance = nullptr;
} // namespace

sptr<AccessibleAbilityClient> AccessibleAbilityClient::GetInstance()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(g_Mutex);
    bool isNull = AccessibilityAbilityUtHelper::GetInstance().GetAbilityClientNullFlag();
    if (isNull) {
        return nullptr;
    }

    if (!g_Instance) {
        g_Instance = new(std::nothrow) AccessibleAbilityClientImpl();
    }
    return g_Instance;
}

sptr<AccessibleAbilityClientImpl> AccessibleAbilityClientImpl::GetAbilityClientImplement()
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(g_Mutex);
    bool isNull = AccessibilityAbilityUtHelper::GetInstance().GetAbilityClientNullFlag();
    if (isNull) {
        return nullptr;
    }

    if (!g_Instance) {
        g_Instance = new(std::nothrow) AccessibleAbilityClientImpl();
    }
    return g_Instance;
}

AccessibleAbilityClientImpl::AccessibleAbilityClientImpl()
{
}

AccessibleAbilityClientImpl::~AccessibleAbilityClientImpl()
{
}

sptr<IRemoteObject> AccessibleAbilityClientImpl::GetRemoteObject()
{
    HILOG_DEBUG();
    return nullptr;
}

RetError AccessibleAbilityClientImpl::RegisterAbilityListener(
    const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG();
    (void)listener;
    return RET_OK;
}

void AccessibleAbilityClientImpl::Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId)
{
}

void AccessibleAbilityClientImpl::Disconnect(const int32_t channelId)
{
}

void AccessibleAbilityClientImpl::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
}

void AccessibleAbilityClientImpl::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
}

RetError AccessibleAbilityClientImpl::GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    (void)focusType;
    (void)elementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo,
    const int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    (void)sourceInfo;
    (void)focusType;
    (void)elementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_DEBUG();
    (void)gesturePath;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetRoot(AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    (void)elementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    (void)windowInfo;
    (void)elementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG();
    (void)windowId;
    (void)windowInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetWindows(std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    (void)windows;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetWindows(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows)
{
    HILOG_DEBUG();
    (void)displayId;
    (void)windows;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetNext(const AccessibilityElementInfo &elementInfo,
    const FocusMoveDirection direction, AccessibilityElementInfo &nextElementInfo)
{
    HILOG_DEBUG();
    (void)elementInfo;
    (void)direction;
    (void)nextElementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
    AccessibilityElementInfo &child)
{
    HILOG_DEBUG();
    (void)index;
    (void)parent;
    (void)child;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetChildren(const AccessibilityElementInfo &parent,
    std::vector<AccessibilityElementInfo> &children)
{
    HILOG_DEBUG();
    (void)parent;
    (void)children;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetByContent(const AccessibilityElementInfo &elementInfo,
    const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG();
    (void)elementInfo;
    (void)text;
    (void)elementInfos;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetSource(const AccessibilityEventInfo &eventInfo,
    AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG();
    (void)eventInfo;
    (void)elementInfo;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::GetParentElementInfo(const AccessibilityElementInfo &child,
    AccessibilityElementInfo &parent)
{
    HILOG_DEBUG();
    (void)child;
    (void)parent;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::ExecuteAction(const AccessibilityElementInfo &elementInfo,
    const ActionType action, const std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG();
    (void)elementInfo;
    (void)action;
    (void)actionArguments;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    (void)targetBundleNames;
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::SetCacheMode(const int32_t cacheMode)
{
    HILOG_DEBUG();
    AccessibilityAbilityUtHelper::GetInstance().SetCacheMode(cacheMode);
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::Connect()
{
    HILOG_DEBUG();
    return RET_OK;
}

RetError AccessibleAbilityClientImpl::Disconnect()
{
    HILOG_DEBUG();
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS