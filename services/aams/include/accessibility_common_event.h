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

#ifndef ACCESSIBILITY_COMMON_EVENT_REGISTRY_H
#define ACCESSIBILITY_COMMON_EVENT_REGISTRY_H

#include <map>
#include <memory>
#include <functional>
#include <string>
#include "common_event_subscriber.h"
#include "event_handler.h"
#include "singleton.h"
#include "want.h"

using EventHandle = std::function<void(const OHOS::EventFwk::CommonEventData &)>;

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonEvent {
    DECLARE_SINGLETON(AccessibilityCommonEvent)
public:
    void SubscriberEvent(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    void UnSubscriberEvent();
    void OnReceiveEvent(const EventFwk::CommonEventData &data);

private:
    class AccessibilityCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        explicit AccessibilityCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo,
            AccessibilityCommonEvent &registry)
            : CommonEventSubscriber(subscriberInfo), registry_(registry) {}
        ~AccessibilityCommonEventSubscriber() = default;

        void OnReceiveEvent(const EventFwk::CommonEventData &data) override
        {
            registry_.OnReceiveEvent(data);
        }

    private:
        AccessibilityCommonEvent &registry_;
    };

    void HandleUserAdded(const EventFwk::CommonEventData &data) const;
    void HandleUserRemoved(const EventFwk::CommonEventData &data) const;
    void HandleUserSwitched(const EventFwk::CommonEventData &data) const;

    void HandlePackageRemoved(const EventFwk::CommonEventData &data) const;
    void HandlePackageChanged(const EventFwk::CommonEventData &data) const;
    void HandlePackageAdd(const EventFwk::CommonEventData &data) const;

    typedef void (AccessibilityCommonEvent::*HandleEventFunc)(const EventFwk::CommonEventData &) const;
    std::map<std::string, EventHandle> eventHandles_;
    std::map<std::string, HandleEventFunc> handleEventFunc_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    std::shared_ptr<AccessibilityCommonEventSubscriber> subscriber_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_COMMON_EVENT_REGISTRY_H