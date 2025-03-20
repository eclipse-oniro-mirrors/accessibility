/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "accessibilityapireporter_fuzzer.h"
#include "securec.h"
#include "api_reporter_helper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 100;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
    constexpr int32_t REPORTER_THRESHOLD_VALUE = 10;
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

bool ApiEventRepoterFuzzTest(const uint8_t* databuffer, size_t size)
{
    if (databuffer == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t startPos = 0;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        startPos += GetObject<char>(name[i], &databuffer[startPos], size - startPos);
    }
    std::string ApiName(name);
#ifdef ACCESSIBILITY_EMULATOR_DEFINED
    ApiReportHelper reporter(ApiName, REPORTER_THRESHOLD_VALUE);
#endif // ACCESSIBILITY_EMULATOR_DEFINED
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::ApiEventRepoterFuzzTest(data, size);
    return 0;
}