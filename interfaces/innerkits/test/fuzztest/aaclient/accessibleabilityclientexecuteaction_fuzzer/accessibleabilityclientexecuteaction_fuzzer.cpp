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

#include "accessibleabilityclientexecuteaction_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

bool DoSomethingInterestingWithExecuteAction(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    startPos += GenerateAccessibilityElementInfo(sourceElementInfo, &data[startPos], size - startPos);
    int32_t action = 0;
    startPos += GetObject<int32_t>(action, &data[startPos], size - startPos);
    std::map<std::string, std::string> actionArguments;
    for (size_t count = 0; count < MAP_SIZE; count++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        for (size_t i = 0; i < LEN; i++) {
            startPos += GetObject<char>(name[i], &data[startPos], size - startPos);
        }
        std::string action1(name);
        for (size_t i = 0; i < LEN; i++) {
            startPos += GetObject<char>(name[i], &data[startPos], size - startPos);
        }
        std::string action2(name);
        actionArguments.insert(std::make_pair(action1, action2));
    }
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->ExecuteAction(
        sourceElementInfo, static_cast<OHOS::Accessibility::ActionType>(action), actionArguments);

    return true;
}

bool DoSomethingInterestingWithSetTargetBundleName(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    std::vector<std::string> targetBundleNames;
    for (size_t count = 0; count < VEC_SIZE; count++) {
        char name[LEN + 1];
        name[LEN] = END_CHAR;
        for (size_t i = 0; i < LEN; i++) {
            startPos += GetObject<char>(name[i], &data[startPos], size - startPos);
        }
        std::string targetBundleName(name);
        targetBundleNames.push_back(targetBundleName);
    }
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->SetTargetBundleName(targetBundleNames);

    return true;
}

bool DoSomethingInterestingWithSetCacheMode(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    int32_t cacheMode = 0;
    GetObject<int32_t>(cacheMode, &data[startPos], size - startPos);
    OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->SetCacheMode(cacheMode);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithExecuteAction(data, size);
    OHOS::DoSomethingInterestingWithSetTargetBundleName(data, size);
    OHOS::DoSomethingInterestingWithSetCacheMode(data, size);
    return 0;
}