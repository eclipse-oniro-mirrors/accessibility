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

#include "accessibleabilityclientregister_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

bool DoSomethingInterestingWithRegisterAbilityListener()
{
    std::shared_ptr<AccessibleAbilityListenerForFuzzTest> listener =
        std::make_shared<AccessibleAbilityListenerForFuzzTest>();
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);
    return true;
}

bool FuzzWithConnect()
{
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->Connect();
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->Disconnect();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithRegisterAbilityListener();
    OHOS::FuzzWithConnect();
    return 0;
}