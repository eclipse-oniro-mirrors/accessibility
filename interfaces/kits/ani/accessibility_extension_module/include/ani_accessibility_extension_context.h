/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ANI_ACCESSIBILITY_EXTENSION_CONTEXT_H
#define ANI_ACCESSIBILITY_EXTENSION_CONTEXT_H

#include "accessibility_extension_context.h"
#include "accessibility_element_info.h"
#include "ani.h"
#include "extension_base.h"

namespace OHOS {
namespace Accessibility {
ani_object CreateAniAccessibilityExtensionContext(ani_env *env, std::shared_ptr<AccessibilityExtensionContext> context,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application);
} // namespace Accessibility
} // namespace OHOS
#endif // ANI_ACCESSIBILITY_EXTENSION_CONTEXT_H