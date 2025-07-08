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

#ifndef ANI_ACCESSIBILITY_CONFIG_H
#define ANI_ACCESSIBILITY_CONFIG_H

#include <vector>
#include <map>
#include <ani.h>
#include "accessibility_config.h"
#include "ffrt.h"

class ANIAccessibilityConfig {
public:
    static void SetHighContrastText(ani_env *env, ani_object object, ani_boolean state);
    static ani_boolean GetHighContrastText(ani_env *env, ani_object object);

private:
    ANIAccessibilityConfig() = default;
    ~ANIAccessibilityConfig() = default;
};

#endif // ANI_ACCESSIBILITY_CONFIG_H