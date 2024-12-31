/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "accessibleabilitymanagerstateobserverstub_fuzzer.h"
#include "accessible_ability_manager_state_observer_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 100;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

class ManagerStateObserverImplFuzzTest : public AccessibleAbilityManagerStateObserverStub {
public:
    ManagerStateObserverImplFuzzTest() = default;
    ~ManagerStateObserverImplFuzzTest() = default;

    void OnStateChanged(const uint32_t stateType) override {}
};

template<class T>
size_t GetData(T &code, const uint8_t *databuffer, size_t size)
{
    size_t objectSize = sizeof(code);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&code, objectSize, databuffer, objectSize) == EOK ? objectSize : 0;
}

bool StateObserverOnRemoteRequestFuzzTest(const uint8_t* databuffer, size_t size)
{
    if (databuffer == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint32_t code = 0;
    MessageParcel stateObserverData;
    MessageParcel stateObserverReply;
    MessageOption option(MessageOption::TF_SYNC);

    GetData<uint32_t>(code, &databuffer[position], size - position);
    ManagerStateObserverImplFuzzTest callBack;
    stateObserverData.WriteInterfaceToken(ManagerStateObserverImplFuzzTest::GetDescriptor());
    callBack.OnRemoteRequest(code, stateObserverData, stateObserverReply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::StateObserverOnRemoteRequestFuzzTest(data, size);
    return 0;
}