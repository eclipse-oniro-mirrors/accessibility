/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include "accessibility_common_event_registry.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

using namespace std;
using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;

namespace OHOS{
namespace Accessibility{
AccessibilityCommonEventRegistry::AccessibilityCommonEventRegistry()
{
    HILOG_DEBUG("AccessibilityCommonEventRegistry AccessibilityCommonEventRegistry");
        handleEventFunc_[CommonEventSupport::COMMON_EVENT_USER_REMOVED] =
    &AccessibilityCommonEventRegistry::HandleRemovedUser;
    handleEventFunc_[CommonEventSupport::COMMON_EVENT_USER_PRESENT] =
        &AccessibilityCommonEventRegistry::HandlePresentUser;

    handleEventFunc_[CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED] =
        &AccessibilityCommonEventRegistry::HandlePackageAdd;
    handleEventFunc_[CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED] =
        &AccessibilityCommonEventRegistry::HandlePackageRemoved;
    handleEventFunc_[CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED] =
        &AccessibilityCommonEventRegistry::HandlePackageChanged;
}

bool AccessibilityCommonEventRegistry::StartRegister()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!eventHandles_.empty()) {
        HILOG_DEBUG("eventHandles_ is not empty");
        eventHandles_.clear();
    }

    for (auto it = handleEventFunc_.begin(); it != handleEventFunc_.end(); ++it) {
        HILOG_DEBUG("Add event: %{public}s", it->first.c_str());
        eventHandles_.emplace(it->first, bind(it->second, this, placeholders::_1));
    }

    if (!RegisterSubscriber()) {
        HILOG_ERROR("RegisterSubscriber failed");
        return false;
    }

    return true;
}

bool AccessibilityCommonEventRegistry::RegisterSubscriber()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    bool subscribeResult = false;
    MatchingSkills matchingSkills;
    for (auto &event : handleEventFunc_) {
        HILOG_DEBUG("Add event: %{public}s", event.first.c_str());
        matchingSkills.AddEvent(event.first);
    }

    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    accessibilityCommonEventSubscriber_ =
        std::make_shared<AccessibilityCommonEventSubscriber>(subscribeInfo, eventHandles_);

    int retry = 3;
    do {
        subscribeResult = CommonEventManager::SubscribeCommonEvent(accessibilityCommonEventSubscriber_);
        if (subscribeResult) {
            break;
        } else {
            HILOG_DEBUG("SubscribeCommonEvent failed, retry %{public}d", retry);
            retry--;
            sleep(1);
        }
    } while (retry);

    return subscribeResult;
}

void AccessibilityCommonEventRegistry::UnRegister()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    eventHandles_.clear();
    if (accessibilityCommonEventSubscriber_ != nullptr) {
        bool subscribeResult = CommonEventManager::UnSubscribeCommonEvent(accessibilityCommonEventSubscriber_);
        HILOG_DEBUG("subscribeResult = %{public}d", subscribeResult);
        accessibilityCommonEventSubscriber_.reset();
    }
    HILOG_DEBUG("UnRegister end.");
}

void AccessibilityCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    HandleEvent(data.GetWant());
}

void AccessibilityCommonEventSubscriber::HandleEvent(const Want &want)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    auto action = want.GetAction();
    auto it = eventHandles_.find(action);
    if (it == eventHandles_.end()) {
        HILOG_ERROR("Ignore event: %{public}s", action.c_str());
        return;
    }
    HILOG_DEBUG("Handle event: %{public}s", action.c_str());
    it->second(want);
}

void AccessibilityCommonEventRegistry::HandleRemovedUser(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);

    int32_t accountId = want.GetIntParam(CommonEventSupport::COMMON_EVENT_USER_REMOVED, -1);
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->RemovedUser(accountId);
}

void AccessibilityCommonEventRegistry::HandlePresentUser(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->PresentUser();
}

void AccessibilityCommonEventRegistry::HandlePackageRemoved(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    string bundleName = want.GetBundle();
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->PackageRemoved(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageAdd(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    string bundleName = want.GetBundle();
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->PackageAdd(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageUpdateFinished(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    string bundleName = want.GetBundle();
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->PackageUpdateFinished(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageChanged(const Want &want) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    string bundleName = want.GetBundle();
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->PackageChanged(bundleName);
}
}  // namespace Accessibility
}  // namespace OHOS