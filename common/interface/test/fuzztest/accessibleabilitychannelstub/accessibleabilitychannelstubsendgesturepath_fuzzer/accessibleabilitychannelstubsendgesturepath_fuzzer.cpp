/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "accessibleabilitychannelstubsendgesturepath_fuzzer.h"
#include "accessible_ability_channel_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "parcel/accessibility_gesture_inject_path_parcel.h"
#include "securec.h"
#include "abilit_channel_impl_fuzz_test.h"

namespace OHOS {
namespace Accessibility {

bool FuzzHandleSendSimulateGesturePath(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    float positionX = 0.0;
    float positionY = 0.0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    position += GetObject<float>(positionX, &data[position], size - position);
    position += GetObject<float>(positionY, &data[position], size - position);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());

    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    if (gesturePath == nullptr) {
        return false;
    }
    AccessibilityGesturePosition point { positionX, positionY };
    gesturePath->AddPosition(point);
    gesturePath->SetDurationTime(DURATION_TIME);
    sptr<AccessibilityGestureInjectPathParcel> path = new(
        std::nothrow) AccessibilityGestureInjectPathParcel(*gesturePath);
    if (path == nullptr) {
        return false;
    }
    mdata.WriteStrongParcelable(path);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SEND_SIMULATE_GESTURE_PATH),
        mdata, reply, option);
    return true;
}

bool FuzzHandleGetCursorPosition(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t treeId = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    position += GetObject<int32_t>(treeId, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(treeId);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::GET_CURSOR_POSITION),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSetIsRegisterDisconnectCallback(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t treeId = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    position += GetObject<int32_t>(treeId, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(treeId);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SET_IS_REGISTER_DISCONNECT_CALLBACK),
        mdata, reply, option);
    return true;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzHandleSendSimulateGesturePath(data, size);
    FuzzHandleGetCursorPosition(data, size);
    FuzzHandleSetIsRegisterDisconnectCallback(data, size);
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