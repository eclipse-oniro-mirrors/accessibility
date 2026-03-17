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

#include "accessibleabilityclientgetfocus_fuzzer.h"
#include "accessible_ability_client_fuzz.h"
#include <chrono>
#include <thread>

namespace OHOS {

bool DoSomethingInterestingWithGetFocus(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t focusType = 0;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    GetObject<int32_t>(focusType, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetFocus(focusType, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetFocusByElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t focusType = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    startPos += GetObject<int32_t>(focusType, &data[startPos], size - startPos);
    GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetFocusByElementInfo(
        sourceElementInfo, focusType, resultElementInfo);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithGetFocus(data, size);
    OHOS::DoSomethingInterestingWithGetFocusByElementInfo(data, size);
    return 0;
}