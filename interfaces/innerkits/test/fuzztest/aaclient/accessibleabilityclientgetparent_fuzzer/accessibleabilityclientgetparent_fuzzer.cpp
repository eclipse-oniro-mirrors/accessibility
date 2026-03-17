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

#include "accessibleabilityclientgetparent_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

bool DoSomethingInterestingWithGetByContent(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> resultElementInfos;
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name[i], &data[startPos], size - startPos);
    }
    std::string text(name);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetByContent(
        sourceElementInfo, text, resultElementInfos);

    return true;
}

bool DoSomethingInterestingWithGetSource(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityEventInfo sourceEventInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    GenerateAccessibilityEventInfo(sourceEventInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetSource(sourceEventInfo, resultElementInfo);

    return true;
}

bool DoSomethingInterestingWithGetParentElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetParentElementInfo(
        sourceElementInfo, resultElementInfo);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithGetByContent(data, size);
    OHOS::DoSomethingInterestingWithGetSource(data, size);
    OHOS::DoSomethingInterestingWithGetParentElementInfo(data, size);
    return 0;
}