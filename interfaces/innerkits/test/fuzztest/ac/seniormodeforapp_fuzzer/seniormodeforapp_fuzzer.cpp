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

#include "seniormodeforapp_fuzzer.h"
#include "accessibility_config.h"
#include "securec.h"

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 1;
    constexpr uint32_t LEN = 10;
    constexpr char END_CHAR = '\0';
}

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool SeniorModeForAppFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto& config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (!config.InitializeContext()) {
        return false;
    }
    size_t position = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string bundleName(name);

    int32_t appIndex = 0;
    position += GetObject<int32_t>(appIndex, &data[position], size - position);

    bool state = false;
    position += GetObject<bool>(state, &data[position], size - position);

    config.GetSeniorModeStateForApp(bundleName, appIndex, state);

    std::vector<AccessibilityConfig::AccessibilityBundleSeniorModeInfo> infos;
    AccessibilityConfig::AccessibilityBundleSeniorModeInfo info;
    info.bundleName_ = bundleName;
    info.appIndex_ = appIndex;
    info.seniorModeState_ = state;
    infos.push_back(info);
    config.SetSeniorModeStateForApp(infos);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SeniorModeForAppFuzzTest(data, size);
    return 0;
}
