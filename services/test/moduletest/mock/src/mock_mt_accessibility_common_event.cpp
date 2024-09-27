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

#include "accessibility_common_event.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

using namespace std;
using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;

namespace OHOS {
namespace Accessibility {
MockAccessibilityCommonEvent::AccessibilityCommonEvent()
{}

MockAccessibilityCommonEvent::~AccessibilityCommonEvent()
{}

void MockAccessibilityCommonEvent::SubscriberEvent(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    (void)handler;
}

void MockAccessibilityCommonEvent::UnSubscriberEvent()
{
}

void MockAccessibilityCommonEvent::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandleUserAdded(const EventFwk::CommonEventData &data) const
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandleUserRemoved(const EventFwk::CommonEventData &data) const
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandleUserSwitched(const EventFwk::CommonEventData &data) const
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandlePackageRemoved(const EventFwk::CommonEventData &data) const
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandlePackageAdd(const EventFwk::CommonEventData &data) const
{
    (void)data;
}

void MockAccessibilityCommonEvent::HandlePackageChanged(const EventFwk::CommonEventData &data) const
{
    (void)data;
}
} // namespace Accessibility
} // namespace OHOS