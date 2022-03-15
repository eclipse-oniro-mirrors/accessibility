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

#include "accessibility_extension.h"
#include "accessibility_extension_context.h"
#include "ability_local_record.h"
#include "hilog_wrapper.h"
#include "js_accessibility_extension.h"

namespace OHOS {
namespace Accessibility {
AccessibilityExtension* AccessibilityExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    if (!runtime) {
        return new AccessibilityExtension();
    }
    HILOG_INFO("AccessibilityExtension::Create runtime");
    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS:
            return JsAccessibilityExtension::Create(runtime);

        default:
            return new AccessibilityExtension();
    }
}

void AccessibilityExtension::Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
    std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<AccessibilityExtensionContext>::Init(record, application, handler, token);
    HILOG_INFO("AccessibilityExtension begin init");
}

std::shared_ptr<AccessibilityExtensionContext> AccessibilityExtension::CreateAndInitContext(
    const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
    std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    std::shared_ptr<AccessibilityExtensionContext> context =
        ExtensionBase<AccessibilityExtensionContext>::CreateAndInitContext(record, application, handler, token);
    if (!record) {
        HILOG_ERROR("AccessibilityExtension::CreateAndInitContext record is nullptr");
    }
    return context;
}
} // namespace Accessibility
} // namespace OHOS