/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessibleabilitychannelstub_fuzzer.h"
#include "accessible_ability_channel_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr size_t BASE_CODE = 400;
constexpr size_t MESSAGE_SIZE = 13;
constexpr size_t TEM_NUM_1 = 1;
constexpr size_t TEM_NUM_2 = 2;
constexpr size_t TEM_NUM_3 = 3;
constexpr size_t TEM_NUM_8 = 8;
constexpr size_t TEM_NUM_16 = 16;
constexpr size_t TEM_NUM_24 = 24;
}

class AbilityChannelImplFuzzTest : public AccessibleAbilityChannelStub {
public:
    AbilityChannelImplFuzzTest() = default;
    ~AbilityChannelImplFuzzTest() = default;

    RetError SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode) override
    {
        return RET_OK;
    }
    RetError SearchElementInfosByText(const int32_t accessibilityWindowId, const int64_t elementId,
        const std::string &text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError FindFocusedElementInfo(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t direction,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo) override
    {
        return RET_OK;
    }
    RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows) override
    {
        return RET_OK;
    }
    RetError GetWindowsByDisplayId(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows) override
    {
        return RET_OK;
    }
    void SetOnKeyPressEventResult(const bool handled, const int32_t sequence) override {}
    RetError SendSimulateGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath) override
    {
        return RET_OK;
    }
    RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) override
    {
        return RET_OK;
    }
};

uint32_t GetU32Data(const char *ptr)
{
    return (ptr[0] << TEM_NUM_24) | (ptr[TEM_NUM_1] << TEM_NUM_16) |
     (ptr[TEM_NUM_2] << TEM_NUM_8) | (ptr[TEM_NUM_3]);
}

bool AbilityChannelOnRemoteRequestFuzzTest(const char *data, size_t size)
{
    uint32_t code = (GetU32Data(data) % MESSAGE_SIZE) + BASE_CODE;
    MessageParcel datas;
    std::u16string descriptor = AccessibleAbilityChannelStub::GetDescriptor();
    datas.WriteInterfaceToken(descriptor);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    AbilityChannelImplFuzzTest channelImp;
    channelImp.OnRemoteRequest(code, datas, reply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::Accessibility::U32_AT_SIZE) {
        return 0;
    }

    /* Validate the length of size */
    if (size > OHOS::Accessibility::FOO_MAX_LEN) {
        return 0;
    }

    char *ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }
    OHOS::Accessibility::AbilityChannelOnRemoteRequestFuzzTest(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}