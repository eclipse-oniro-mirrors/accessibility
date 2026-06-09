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

#include "accessibilitysystemabilityclientsetsearchresult_fuzzer.h"
#include "accessibility_system_ability_client_impl.h"
#include "securec.h"
#include "element_operator_fuzz_test.h"

namespace OHOS {
namespace Accessibility {

static AccessibilitySystemAbilityClientImpl g_asacImpl_;

bool SetSearchElementInfoByAccessibilityIdResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    std::list<AccessibilityElementInfo> infos;
    g_asacImpl_.SetSearchElementInfoByAccessibilityIdResult(infos, requestId);
    return true;
}

bool SetSearchElementInfoByTextResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    std::list<AccessibilityElementInfo> infos;
    g_asacImpl_.SetSearchElementInfoByTextResult(infos, requestId);
    return true;
}

bool SetFindFocusedElementInfoResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    AccessibilityElementInfo info;
    g_asacImpl_.SetFindFocusedElementInfoResult(info, requestId);
    return true;
}

bool SetFocusMoveSearchResultFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t requestId = 0;
    GetObject<int32_t>(requestId, &data[0], size);
    AccessibilityElementInfo info;
    g_asacImpl_.SetFocusMoveSearchResult(info, requestId);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::SetSearchElementInfoByAccessibilityIdResultFuzzTest(data, size);
    OHOS::Accessibility::SetSearchElementInfoByTextResultFuzzTest(data, size);
    OHOS::Accessibility::SetFindFocusedElementInfoResultFuzzTest(data, size);
    OHOS::Accessibility::SetFocusMoveSearchResultFuzzTest(data, size);
    return 0;
}