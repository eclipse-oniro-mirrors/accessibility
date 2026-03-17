/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "accessibilitysystemabilityclientistouch_fuzzer.h"
#include "accessibility_system_ability_client_impl.h"
#include "securec.h"
#include "element_operator_fuzz_test.h"

namespace OHOS {
namespace Accessibility {

bool IsTouchExplorationEnabledFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    bool isEnabled = false;
    GetObject<bool>(isEnabled, &data[0], size);
    instance->IsTouchExplorationEnabled(isEnabled);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::IsTouchExplorationEnabledFuzzTest(data, size);
    return 0;
}