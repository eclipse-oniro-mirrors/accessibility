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

#include "accessibility_common_event.h"
#include <unistd.h>
#include "accessible_ability_manager_service.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t RETRY_SUBSCRIBER = 3;
} // namespace

AccessibilityCommonEvent::AccessibilityCommonEvent()
{
    HILOG_DEBUG("AccessibilityCommonEvent AccessibilityCommonEvent");
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED] =
        &AccessibilityCommonEvent::HandleUserAdded;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED] =
        &AccessibilityCommonEvent::HandleUserRemoved;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED] =
        &AccessibilityCommonEvent::HandleUserSwitched;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED] =
        &AccessibilityCommonEvent::HandlePackageAdd;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED] =
        &AccessibilityCommonEvent::HandlePackageRemoved;
    handleEventFunc_[EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED] =
        &AccessibilityCommonEvent::HandlePackageChanged;

    for (auto it = handleEventFunc_.begin(); it != handleEventFunc_.end(); ++it) {
        HILOG_DEBUG("Add event: %{public}s", it->first.c_str());
        eventHandles_.emplace(it->first, std::bind(it->second, this, std::placeholders::_1));
    }
}

AccessibilityCommonEvent::~AccessibilityCommonEvent()
{
    UnSubscriberEvent();
}

void AccessibilityCommonEvent::SubscriberEvent(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    HILOG_DEBUG("start.");

    if (subscriber_) {
        HILOG_DEBUG("Common Event is already subscribered!");
        return;
    }

    EventFwk::MatchingSkills matchingSkills;
    for (auto &event : handleEventFunc_) {
        HILOG_DEBUG("Add event: %{public}s", event.first.c_str());
        matchingSkills.AddEvent(event.first);
    }

    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscriber_ = std::make_shared<AccessibilityCommonEventSubscriber>(subscribeInfo, *this);
    eventHandler_ = handler;

    int32_t retry = RETRY_SUBSCRIBER;
    do {
        bool subscribeResult = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
        if (subscribeResult) {
            HILOG_DEBUG("SubscriberEvent success.");
            return;
        } else {
            HILOG_DEBUG("SubscriberEvent failed, retry %{public}d", retry);
            retry--;
            sleep(1);
        }
    } while (retry);

    HILOG_ERROR("SubscriberEvent failed.");
}

void AccessibilityCommonEvent::UnSubscriberEvent()
{
    HILOG_DEBUG("start.");
    eventHandles_.clear();
    if (subscriber_) {
        bool subscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        HILOG_DEBUG("subscribeResult = %{public}d", subscribeResult);
        subscriber_ = nullptr;
        eventHandler_ = nullptr;
    }
    HILOG_DEBUG("UnSubscriberEvent end.");
}

void AccessibilityCommonEvent::OnReceiveEvent(const AAFwk::Want &want)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    eventHandler_->PostTask(std::bind([this, want]() -> void {
        HILOG_DEBUG("start.");
        std::string action = want.GetAction();
        auto it = eventHandles_.find(action);
        if (it == eventHandles_.end()) {
            HILOG_ERROR("Ignore event: %{public}s", action.c_str());
            return;
        }
        HILOG_DEBUG("Handle event: %{public}s", action.c_str());
        it->second(want);
        }), "TASK_ON_RECEIVE_EVENT");
}

void AccessibilityCommonEvent::HandleUserAdded(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    int32_t accountId = want.GetIntParam(EventFwk::CommonEventSupport::COMMON_EVENT_USER_ADDED, -1);
    if (accountId == -1) {
        HILOG_ERROR("GetIntParam failed.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(accountId);
}

void AccessibilityCommonEvent::HandleUserRemoved(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    int32_t accountId = want.GetIntParam(EventFwk::CommonEventSupport::COMMON_EVENT_USER_REMOVED, -1);
    if (accountId == -1) {
        HILOG_ERROR("GetIntParam failed.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(accountId);
}

void AccessibilityCommonEvent::HandleUserSwitched(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    int32_t accountId = want.GetIntParam(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED, -1);
    if (accountId == -1) {
        HILOG_ERROR("GetIntParam failed.");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(accountId);
}

void AccessibilityCommonEvent::HandlePackageRemoved(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    std::string bundleName = want.GetBundle();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
}

void AccessibilityCommonEvent::HandlePackageAdd(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    std::string bundleName = want.GetBundle();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
}

void AccessibilityCommonEvent::HandlePackageChanged(const AAFwk::Want &want) const
{
    HILOG_DEBUG("start.");
    std::string bundleName = want.GetBundle();
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
}
} // namespace Accessibility
} // namespace OHOS