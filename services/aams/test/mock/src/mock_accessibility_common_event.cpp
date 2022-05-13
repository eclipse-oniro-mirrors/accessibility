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
AccessibilityCommonEvent::AccessibilityCommonEvent()
{}

AccessibilityCommonEvent::~AccessibilityCommonEvent()
{}

void AccessibilityCommonEvent::SubscriberEvent(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    (void)handler;
}

void AccessibilityCommonEvent::UnSubscriberEvent()
{
}

void AccessibilityCommonEvent::OnReceiveEvent(const Want& want)
{
    (void)want;
}

void AccessibilityCommonEvent::HandleUserAdded(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEvent::HandleUserRemoved(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEvent::HandleUserSwitched(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEvent::HandlePackageRemoved(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEvent::HandlePackageAdd(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEvent::HandlePackageChanged(const Want& want) const
{
    (void)want;
}
} // namespace Accessibility
} // namespace OHOS