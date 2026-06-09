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

#include "accessibilitysystemabilityclientregister_fuzzer.h"
#include "accessibility_system_ability_client_impl.h"
#include "securec.h"
#include "element_operator_fuzz_test.h"

namespace OHOS {
namespace Accessibility {

bool RegisterElementOperatorFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    int32_t windowId = 0;
    position += GetObject<int32_t>(windowId, &data[position], size - position);
    std::shared_ptr<ElementOperatorForFuzzTest> elementOperator = std::make_shared<ElementOperatorForFuzzTest>();
    instance->RegisterElementOperator(windowId, elementOperator);

    GetObject<int32_t>(windowId, &data[position], size - position);
    instance->DeregisterElementOperator(windowId);
    return true;
}

bool SubscribeStateObserverFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto instance = AccessibilitySystemAbilityClient::GetInstance();
    if (!instance) {
        return false;
    }

    size_t position = 0;
    uint32_t eventTypes = 0;
    position += GetObject<uint32_t>(eventTypes, &data[position], size - position);
    std::shared_ptr<StateObserverForFuzzTest> observer = std::make_shared<StateObserverForFuzzTest>();
    instance->SubscribeStateObserver(observer, eventTypes);

    GetObject<uint32_t>(eventTypes, &data[position], size - position);
    instance->UnsubscribeStateObserver(observer, eventTypes);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::RegisterElementOperatorFuzzTest(data, size);
    OHOS::Accessibility::SubscribeStateObserverFuzzTest(data, size);
    return 0;
}