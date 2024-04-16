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

#include "accessibilityelementoperatorstub_fuzzer.h"
#include "accessibility_element_operator_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 100;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

class ElementOperatorImplFuzzTest : public AccessibilityElementOperatorStub {
public:
    ElementOperatorImplFuzzTest() = default;
    ~ElementOperatorImplFuzzTest() = default;

    void SearchElementInfoByAccessibilityId(const int64_t elementId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
        bool isFilter) override {}
    void SearchElementInfosByText(const int64_t elementId, const std::string &text,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}
    void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}
    void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}
    void ExecuteAction(const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}
    void GetCursorPosition(const int64_t elementId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}
    void ClearFocus() override {}
    void OutsideTouch() override {}
};

template<class T>
size_t GetData(T &object, const uint8_t *databuffer, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, databuffer, objectSize) == EOK ? objectSize : 0;
}

bool EleOperatorOnRemoteRequestFuzzTest(const uint8_t* databuffer, size_t size)
{
    if (databuffer == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint32_t code = 0;
    MessageParcel eleOperatorData;
    MessageParcel eleOperatorReply;
    MessageOption option(MessageOption::TF_SYNC);

    GetData<uint32_t>(code, &databuffer[position], size - position);
    ElementOperatorImplFuzzTest callBack;
    eleOperatorData.WriteInterfaceToken(ElementOperatorImplFuzzTest::GetDescriptor());
    callBack.OnRemoteRequest(code, eleOperatorData, eleOperatorReply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::EleOperatorOnRemoteRequestFuzzTest(data, size);
    return 0;
}