/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_common_event_registry.h"
#include <unistd.h>
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
static int32_t RETRY_SUBSCRIBER = 3;
AccessibilityCommonEventRegistry::AccessibilityCommonEventRegistry(
    const std::shared_ptr<AppExecFwk::EventHandler> &handler) : eventHandler_(handler)
{
    HILOG_DEBUG("AccessibilityCommonEventRegistry AccessibilityCommonEventRegistry");
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED] =
        &AccessibilityCommonEventRegistry::HandleRemovedUser;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_USER_PRESENT] =
        &AccessibilityCommonEventRegistry::HandlePresentUser;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED] =
        &AccessibilityCommonEventRegistry::HandlePackageAdd;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED] =
        &AccessibilityCommonEventRegistry::HandlePackageRemoved;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED] =
        &AccessibilityCommonEventRegistry::HandlePackageChanged;
}

bool AccessibilityCommonEventRegistry::StartRegister()
{
    HILOG_DEBUG("start.");
    if (!eventHandles_.empty()) {
        HILOG_DEBUG("eventHandles_ is not empty");
        eventHandles_.clear();
    }

    for (auto it = handleEventFunc_.begin(); it != handleEventFunc_.end(); ++it) {
        HILOG_DEBUG("Add event: %{public}s", it->first.c_str());
        eventHandles_.emplace(it->first, std::bind(it->second, this, std::placeholders::_1));
    }

    if (!RegisterSubscriber()) {
        HILOG_ERROR("RegisterSubscriber failed");
        return false;
    }

    return true;
}

bool AccessibilityCommonEventRegistry::RegisterSubscriber()
{
    HILOG_DEBUG("start.");

    EventFwk::MatchingSkills matchingSkills;
    for (auto &event : handleEventFunc_) {
        HILOG_DEBUG("Add event: %{public}s", event.first.c_str());
        matchingSkills.AddEvent(event.first);
    }

    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscriber_ = std::make_shared<AccessibilityCommonEventSubscriber>(subscribeInfo, *this);

    int32_t retry = RETRY_SUBSCRIBER;
    do {
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        if (subscribeResult) {
            return true;
        } else {
            HILOG_DEBUG("SubscribeCommonEvent failed, retry %{public}d", retry);
            retry--;
            sleep(1);
        }
    } while (retry);

    return false;
}

void AccessibilityCommonEventRegistry::UnRegister()
{
    HILOG_DEBUG("start.");
    eventHandles_.clear();
    if (subscriber_ != nullptr) {
        bool subscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        HILOG_DEBUG("subscribeResult = %{public}d", subscribeResult);
        subscriber_.reset();
    }
    HILOG_DEBUG("UnRegister end.");
}

void AccessibilityCommonEventRegistry::OnReceiveEvent(const AAFwk::Want &want)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    std::function<void()> onReceiveEventFunc = std::bind(&AccessibilityCommonEventRegistry::HandleEvent,
        this, want);
    eventHandler_->PostTask(onReceiveEventFunc, "TASK_ON_RECEIVE_EVENT");
}

void AccessibilityCommonEventRegistry::HandleEvent(const AAFwk::Want &want)
{
    HILOG_DEBUG("start.");
    auto action = want.GetAction();
    auto it = eventHandles_.find(action);
    if (it == eventHandles_.end()) {
        HILOG_ERROR("Ignore event: %{public}s", action.c_str());
        return;
    }
    HILOG_DEBUG("Handle event: %{public}s", action.c_str());
    it->second(want);
}

void AccessibilityCommonEventRegistry::HandleRemovedUser(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    int32_t accountId = want.GetIntParam(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED, -1);
    aams->RemovedUser(accountId);
}

void AccessibilityCommonEventRegistry::HandlePresentUser(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    aams->PresentUser();
}

void AccessibilityCommonEventRegistry::HandlePackageRemoved(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    std::string bundleName = want.GetBundle();
    aams->PackageRemoved(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageAdd(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    std::string bundleName = want.GetBundle();
    aams->PackageAdd(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageUpdateFinished(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    std::string bundleName = want.GetBundle();
    aams->PackageUpdateFinished(bundleName);
}

void AccessibilityCommonEventRegistry::HandlePackageChanged(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    std::string bundleName = want.GetBundle();
    aams->PackageChanged(bundleName);
}
} // namespace Accessibility
} // namespace OHOS