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

#ifndef ANI_CONFIG_UITLS_H
#define ANI_CONFIG_UITLS_H

#include <ani.h>
#include <vector>
#include <functional>
#include <map>
#include "accessibility_def.h"

class ANIConfigUtils {
public:
    static NAccessibilityErrMsg QueryRetMsg(OHOS::Accessibility::RetError errorCode);
    static void ThrowBusinessError(ani_env *env, NAccessibilityErrMsg errMsg);

private:
    ANIConfigUtils() = default;
    ~ANIConfigUtils() = default;
};

#endif // ANI_CONFIG_UITLS_H