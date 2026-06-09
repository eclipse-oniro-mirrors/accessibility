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

#include "accessibleabilityclientgetwindow_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

bool DoSomethingInterestingWithGetWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t windowId = 0;
    OHOS::Accessibility::AccessibilityWindowInfo resultWindowInfo;
    GetObject<int32_t>(windowId, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetWindow(windowId, resultWindowInfo);

    return true;
}

bool DoSomethingInterestingWithGetWindows(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> resultWindowInfos;
    OHOS::Accessibility::AccessibilityWindowInfo windowInfo;
    GenerateAccessibilityWindowInfo(windowInfo, &data[startPos], size - startPos);
    resultWindowInfos.push_back(windowInfo);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetWindows(resultWindowInfos);
    return true;
}

bool DoSomethingInterestingWithGetWindowsByDisplayId(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    uint64_t displayId = 0;
    std::vector<OHOS::Accessibility::AccessibilityWindowInfo> resultWindowInfos;
    GetObject<uint64_t>(displayId, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetWindows(displayId, resultWindowInfos);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */

    OHOS::DoSomethingInterestingWithGetWindow(data, size);
    OHOS::DoSomethingInterestingWithGetWindows(data, size);
    OHOS::DoSomethingInterestingWithGetWindowsByDisplayId(data, size);
    return 0;
}