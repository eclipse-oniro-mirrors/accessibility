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

#ifndef ACCESSIBILITY_SEC_COMP_RAWDATA_H
#define ACCESSIBILITY_SEC_COMP_RAWDATA_H

#include "securec.h"

namespace OHOS {
namespace Accessibility {
constexpr int32_t MAX_RAW_DATA_SIZE = 4096;
class AccessibilitySecCompRawdata {
public:
    uint32_t size = 0;
    uint8_t* data = nullptr;

    int32_t RawDataCpy(const void* readData)
    {
        if ((size == 0) || (size >= MAX_RAW_DATA_SIZE)) {
            return -1;
        }
        uint8_t* buffer = new (std::nothrow) uint8_t[size];
        if (buffer == nullptr) {
            return -1;
        }
        errno_t ret = memcpy_s(buffer, size, readData, size);
        if (ret != EOK) {
            delete[] buffer;
            return -1;
        }
        data = buffer;
        return 0;
    }
};
}  // namespace Accessibility
}  // namespace OHOS
#endif // ACCESSIBILITY_SEC_COMP_RAWDATA_H