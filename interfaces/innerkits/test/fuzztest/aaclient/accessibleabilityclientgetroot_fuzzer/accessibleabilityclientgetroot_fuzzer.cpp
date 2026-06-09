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

#include "accessibleabilityclientgetroot_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

bool FuzzWithGetRootByWindowBatch(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    bool isFilter = false;
    bool needCut = false;

    OHOS::Accessibility::AccessibilityWindowInfo windowInfo;
    GenerateAccessibilityWindowInfo(windowInfo, &data[startPos], size - startPos);
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> infos;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    infos.push_back(sourceElementInfo);

    startPos += GetObject<bool>(isFilter, &data[startPos], size - startPos);
    startPos += GetObject<bool>(needCut, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetRootByWindowBatch(
        windowInfo, infos, isFilter, needCut);

    CheckGenerateAccessibilityWindowInfo(windowInfo);
    CheckGenerateAccessibilityElementInfoP1(sourceElementInfo, &data[startPos], size - startPos, startPos);
    CheckGenerateAccessibilityElementInfoP2(sourceElementInfo);
    CheckGenerateAccessibilityElementInfoP3(sourceElementInfo, &data[startPos], size - startPos, startPos);
    CheckGridItemInfo();
    return true;
}

bool FuzzWithGetRootBatch(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> infos;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    infos.push_back(sourceElementInfo);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetRootBatch(infos);

    return true;
}

bool DoSomethingInterestingWithGetRoot(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    GenerateAccessibilityElementInfo(resultElementInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetRoot(resultElementInfo);
    FuzzWithGetRootBatch(data, size);
    return true;
}

bool DoSomethingInterestingWithGetRootByWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityWindowInfo sourceWindowInfo;
    OHOS::Accessibility::AccessibilityElementInfo resultElementInfo;
    GenerateAccessibilityWindowInfo(sourceWindowInfo, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetRootByWindow(
        sourceWindowInfo, resultElementInfo);
    FuzzWithGetRootByWindowBatch(data, size);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */

    OHOS::DoSomethingInterestingWithGetRoot(data, size);
    OHOS::DoSomethingInterestingWithGetRootByWindow(data, size);
    return 0;
}