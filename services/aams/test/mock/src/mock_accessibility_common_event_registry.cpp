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

#include "accessibility_common_event_registry.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

using namespace std;
using namespace OHOS::EventFwk;
using namespace OHOS::AAFwk;

namespace OHOS {
namespace Accessibility {
AccessibilityCommonEventRegistry::AccessibilityCommonEventRegistry(
    const std::shared_ptr<AppExecFwk::EventHandler> &handler) : eventHandler_(handler)
{}

bool AccessibilityCommonEventRegistry::StartRegister()
{
    return true;
}

bool AccessibilityCommonEventRegistry::RegisterSubscriber()
{
    return true;
}

void AccessibilityCommonEventRegistry::UnRegister()
{
}

void AccessibilityCommonEventRegistry::OnReceiveEvent(const Want& want)
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandleEvent(const Want& want)
{
    (void)want;
    eventHandles_.find("action");
}

void AccessibilityCommonEventRegistry::HandleRemovedUser(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandlePresentUser(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandlePackageRemoved(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandlePackageAdd(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandlePackageUpdateFinished(const Want& want) const
{
    (void)want;
}

void AccessibilityCommonEventRegistry::HandlePackageChanged(const Want& want) const
{
    (void)want;
}
} // namespace Accessibility
} // namespace OHOS