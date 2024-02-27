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

#include "accessibleabilityclientstub_fuzzer.h"
#include "accessible_ability_client_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

class AccessibleAbilityClientTmplFuzzTest : public AccessibleAbilityClientStub {
public:
    AccessibleAbilityClientTmplFuzzTest() = default;
    ~AccessibleAbilityClientTmplFuzzTest() = default;
    void Init(const sptr<IAccessibleAbilityChannel> &channel, const int32_t channelId) override {};
    void Disconnect(const int32_t channelId) override {};
    void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override {};
    void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence) override {};
};

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool OnRemoteRequestFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint32_t cilentCode = 0;
    MessageParcel clientData;
    MessageParcel clientReply;
    MessageOption option(MessageOption::TF_SYNC);

    GetObject<uint32_t>(cilentCode, &data[position], size - position);
    AccessibleAbilityClientTmplFuzzTest callBackImp;
    clientData.WriteInterfaceToken(AccessibleAbilityClientTmplFuzzTest::GetDescriptor());
    callBackImp.OnRemoteRequest(cilentCode, clientData, clientReply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::OnRemoteRequestFuzzTest(data, size);
    return 0;
}