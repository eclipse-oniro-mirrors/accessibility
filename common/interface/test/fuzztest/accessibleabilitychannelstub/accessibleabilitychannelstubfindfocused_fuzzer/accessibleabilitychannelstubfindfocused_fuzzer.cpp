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

#include "accessibleabilitychannelstubfindfocused_fuzzer.h"
#include "accessible_ability_channel_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "parcel/accessibility_gesture_inject_path_parcel.h"
#include "securec.h"
#include "abilit_channel_impl_fuzz_test.h"

namespace OHOS {
namespace Accessibility {


bool FuzzHandleFindFocusedElementInfo(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t focusType = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    position += GetObject<int32_t>(focusType, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(focusType);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::FIND_FOCUSED_ELEMENTINFO),
        mdata, reply, option);
    return true;
}

bool FuzzHandleFocusMoveSearch(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t direction = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    position += GetObject<int32_t>(direction, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(direction);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::FOCUS_MOVE_SEARCH),
        mdata, reply, option);
    return true;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzHandleFindFocusedElementInfo(data, size);
    FuzzHandleFocusMoveSearch(data, size);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}