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

#ifndef MOCK_ACCESSIBLE_ABILITY_CLIENT_H
#define MOCK_ACCESSIBLE_ABILITY_CLIENT_H

#include "accessible_ability_client.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityClient : public AccessibleAbilityClient {
public:
    MockAccessibleAbilityClient();
    ~MockAccessibleAbilityClient() = default;
    sptr<IRemoteObject> GetRemoteObject() override;
    RetError RegisterAbilityListener(const std::shared_ptr<AccessibleAbilityListener> &listener) override;
    RetError GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo) override;
    RetError GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo, const int32_t focusType,
        AccessibilityElementInfo &elementInfo) override;
    RetError InjectGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath) override;
    RetError GetRoot(AccessibilityElementInfo &elementInfo) override;
    RetError GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
        AccessibilityElementInfo &elementInfo) override;
    RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo) override;
    RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows) override;
    RetError GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows) override;
    RetError GetNext(const AccessibilityElementInfo &elementInfo, const FocusMoveDirection direction,
        AccessibilityElementInfo &nextElementInfo) override;
    RetError GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
        AccessibilityElementInfo &child) override;
    RetError GetChildren(const AccessibilityElementInfo &parent,
        std::vector<AccessibilityElementInfo> &children) override;
    RetError GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
        std::vector<AccessibilityElementInfo> &elementInfos) override;
    RetError GetSource(const AccessibilityEventInfo &eventInfo, AccessibilityElementInfo &elementInfo) override;
    RetError GetParentElementInfo(const AccessibilityElementInfo &child, AccessibilityElementInfo &parent) override;
    RetError ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
        const std::map<std::string, std::string> &actionArguments) override;
    RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) override;
    RetError SetCacheMode(const int32_t cacheMode) override;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CLIENT_H