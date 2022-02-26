/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <string>
#include <functional>
#include <memory>
#include "want.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "bundlemgr/bundle_mgr_interface.h"

using EventHandle = std::function<void(const OHOS::AAFwk::Want&)>;

namespace OHOS {
namespace Accessibility {
class AccessibilityCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    AccessibilityCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo,
        const std::map<std::string, EventHandle> &handles)
        : CommonEventSubscriber(subscriberInfo), eventHandles_(handles) {}

    ~AccessibilityCommonEventSubscriber() = default;

    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    void HandleEvent(const AAFwk::Want &want);
    const std::map<std::string, EventHandle> &eventHandles_;
};

class AccessibilityCommonEventRegistry {
public:
    AccessibilityCommonEventRegistry();
    ~AccessibilityCommonEventRegistry() = default;

    bool StartRegister();
    void UnRegister();

private:
    bool RegisterSubscriber();

    void HandleRemovedUser(const AAFwk::Want &want) const;
    void HandlePresentUser(const AAFwk::Want &want) const;

    void HandlePackageRemoved(const AAFwk::Want &want) const;
    void HandlePackageUpdateFinished(const AAFwk::Want &want) const;
    void HandlePackageChanged(const AAFwk::Want &want) const;
    void HandlePackageAdd(const AAFwk::Want &want) const;

    typedef void (AccessibilityCommonEventRegistry::*HandleEventFunc)(const AAFwk::Want&) const;
    std::map<std::string, HandleEventFunc> handleEventFunc_;
    std::shared_ptr<AccessibilityCommonEventSubscriber> accessibilityCommonEventSubscriber_;
    std::map<std::string, EventHandle> eventHandles_;
};
}  // namespace Accessibility
}  // namespace OHOS

#endif  // ACCESSIBILITY_COMMON_EVENT_REGISTRY_H