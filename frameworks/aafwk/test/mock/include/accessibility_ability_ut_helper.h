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

#ifndef ACCESSIBILITY_ABILITY_UT_HELPER_H
#define ACCESSIBILITY_ABILITY_UT_HELPER_H

#include <cstdint>

namespace OHOS {
namespace Accessibility {
class AccessibilityAbilityUtHelper {
public:
    static AccessibilityAbilityUtHelper& GetInstance()
    {
        static AccessibilityAbilityUtHelper helper;
        return helper;
    }

    void SetAbilityClientNullFlag(bool flag)
    {
        isNeedAbilityClientNull_ = flag;
    }

    bool GetAbilityClientNullFlag() const
    {
        return isNeedAbilityClientNull_;
    }

    void SetCacheMode(int32_t cacheMode)
    {
        cacheMode_ = cacheMode;
    }

    int32_t GetCacheMode() const
    {
        return cacheMode_;
    }

private:
    bool isNeedAbilityClientNull_ = false;
    int32_t cacheMode_ = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_UT_HELPER_H