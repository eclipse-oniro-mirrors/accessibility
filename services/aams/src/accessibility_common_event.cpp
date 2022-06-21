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
    HILOG_DEBUG();
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
    HILOG_DEBUG();

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
            HILOG_INFO("SubscriberEvent success.");
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
    HILOG_DEBUG();
    eventHandles_.clear();
    if (subscriber_) {
        bool unSubscribeResult = EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        HILOG_DEBUG("unSubscribeResult = %{public}d", unSubscribeResult);
        subscriber_ = nullptr;
        eventHandler_ = nullptr;
    }
}

void AccessibilityCommonEvent::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    HILOG_DEBUG();
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    eventHandler_->PostTask(std::bind([this, data]() -> void {
        HILOG_DEBUG();
        std::string action = data.GetWant().GetAction();
        HILOG_INFO("Handle event: %{public}s", action.c_str());
        auto it = eventHandles_.find(action);
        if (it == eventHandles_.end()) {
            HILOG_ERROR("Ignore event: %{public}s", action.c_str());
            return;
        }
        it->second(data);
        }), "TASK_ON_RECEIVE_EVENT");
}

void AccessibilityCommonEvent::HandleUserAdded(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO("account id is %{public}d", accountId);
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().AddedUser(accountId);
}

void AccessibilityCommonEvent::HandleUserRemoved(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO("account id is %{public}d", accountId);
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().RemovedUser(accountId);
}

void AccessibilityCommonEvent::HandleUserSwitched(const EventFwk::CommonEventData &data) const
{
    int32_t accountId = data.GetCode();
    HILOG_INFO("account id is %{public}d", accountId);
    if (accountId == -1) {
        HILOG_ERROR("account id is wrong");
        return;
    }
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(accountId);
}

void AccessibilityCommonEvent::HandlePackageRemoved(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageRemoved(bundleName);
}

void AccessibilityCommonEvent::HandlePackageAdd(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageAdd(bundleName);
}

void AccessibilityCommonEvent::HandlePackageChanged(const EventFwk::CommonEventData &data) const
{
    std::string bundleName = data.GetWant().GetBundle();
    HILOG_INFO("bundleName is %{public}s", bundleName.c_str());
    Singleton<AccessibleAbilityManagerService>::GetInstance().PackageChanged(bundleName);
}
} // namespace Accessibility
} // namespace OHOS