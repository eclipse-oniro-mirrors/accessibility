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

#include <array>
#include <iostream>
#include "ani_accessibility_config.h"
#include "ani_utils.h"
#include "hilog_wrapper.h"

void ANIAccessibilityConfig::SetHighContrastText(ani_env *env, ani_object object, ani_boolean state)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    auto ret = instance.SetHighContrastTextState(state);
    if (ret != OHOS::Accessibility::RetError::RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("SetHighContrastText ret = %{public}d", static_cast<int32_t>(ret));
    return;
}

ani_boolean ANIAccessibilityConfig::GetHighContrastText(ani_env *env, ani_object object)
{
    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    auto ret = instance.GetHighContrastTextState(state);
    if (ret != OHOS::Accessibility::RetError::RET_OK) {
        ANIUtils::ThrowBusinessError(env, ANIUtils::QueryRetMsg(ret));
    }
    HILOG_INFO("GetHighContrastText ret = %{public}d", static_cast<int32_t>(ret));
    return static_cast<ani_boolean>(state);
}