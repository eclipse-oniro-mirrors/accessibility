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

#include "setenhanceconfig_fuzzer.h"
#include "accessibility_config.h"
#include "securec.h"

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 1;
}

bool SetEnhanceConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto& config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (!config.InitializeContext()) {
        return false;
    }

    // 使用fuzz数据作为配置数据
    uint32_t cfgLen = size;
    config.SetEnhanceConfig(const_cast<uint8_t*>(data), cfgLen);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SetEnhanceConfigFuzzTest(data, size);
    return 0;
}
