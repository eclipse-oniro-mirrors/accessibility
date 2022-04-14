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
#include "bundlemgr/bundle_mgr_interface.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "event_handler.h"
#include "want.h"

using EventHandle = std::function<void(const OHOS::AAFwk::Want&)>;

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonEventRegistry {
public:
    explicit AccessibilityCommonEventRegistry(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    ~AccessibilityCommonEventRegistry() = default;

    bool StartRegister();
    void UnRegister();
    void OnReceiveEvent(const AAFwk::Want &want);

private:
    class AccessibilityCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        explicit AccessibilityCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo,
            AccessibilityCommonEventRegistry &registry)
            : CommonEventSubscriber(subscriberInfo), registry_(registry) {}
        ~AccessibilityCommonEventSubscriber() = default;

        void OnReceiveEvent(const EventFwk::CommonEventData &data) override
        {
            registry_.OnReceiveEvent(data.GetWant());
        }

    private:
        AccessibilityCommonEventRegistry &registry_;
    };

    bool RegisterSubscriber();

    void HandleEvent(const AAFwk::Want &want);
    void HandleRemovedUser(const AAFwk::Want &want) const;
    void HandlePresentUser(const AAFwk::Want &want) const;

    void HandlePackageRemoved(const AAFwk::Want &want) const;
    void HandlePackageUpdateFinished(const AAFwk::Want &want) const;
    void HandlePackageChanged(const AAFwk::Want &want) const;
    void HandlePackageAdd(const AAFwk::Want &want) const;

    typedef void (AccessibilityCommonEventRegistry::*HandleEventFunc)(const AAFwk::Want&) const;
    std::map<std::string, EventHandle> eventHandles_;
    std::map<std::string, HandleEventFunc> handleEventFunc_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    std::shared_ptr<AccessibilityCommonEventSubscriber> subscriber_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif  // ACCESSIBILITY_COMMON_EVENT_REGISTRY_H