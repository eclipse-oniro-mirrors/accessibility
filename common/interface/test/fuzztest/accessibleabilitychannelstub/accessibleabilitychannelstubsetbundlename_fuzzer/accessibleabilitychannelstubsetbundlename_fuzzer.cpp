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

#include "accessibleabilitychannelstubsetbundlename_fuzzer.h"
#include "accessible_ability_channel_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "parcel/accessibility_gesture_inject_path_parcel.h"
#include "securec.h"
#include "abilit_channel_impl_fuzz_test.h"

namespace OHOS {
namespace Accessibility {

bool FuzzHandleSetOnKeyPressEventResult()
{
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SET_ON_KEY_PRESS_EVENT_RESULT),
        mdata, reply, option);
    return true;
}

bool FuzzHandleNotifyDisconnect()
{
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::NOTIFY_DISCONNECT),
        mdata, reply, option);
    return true;
}

bool FuzzHandleFocusMoveSearchWithCondition(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t action = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    GetObject<int32_t>(action, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(action);
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::FOCUS_MOVE_SEARCH_WITH_CONDITION),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSearchElementInfoBySpecificProperty(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t action = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    GetObject<int32_t>(action, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(action);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(
        AccessibilityInterfaceCode::SEARCH_ELEMENTINFOS_BY_SPECIFIC_PROPERTY), mdata, reply, option);
    return true;
}

bool FuzzHandleConfigureEvents(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint32_t eventSize = 0;
    uint32_t accessibilityEvent = 0;
    uint32_t a11yEvent = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<uint32_t>(eventSize, &data[position], size - position);
    position += GetObject<uint32_t>(accessibilityEvent, &data[position], size - position);
    GetObject<uint32_t>(a11yEvent, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteUint32(eventSize);
    mdata.WriteUint32(accessibilityEvent);
    mdata.WriteUint32(a11yEvent);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::CONFIGURE_EVENTS),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSearchDefaultFocusedByWindowId(const uint8_t *data, size_t size)
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
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SEARCH_DEFAULTFOCUSED_BY_WINDOW_ID),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSetTargetBundleName(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t positions = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        positions += GetObject<char>(name[i], data + positions, size - positions);
    }
    std::string bundleName(name);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    if (chanImp == nullptr) {
        return false;
    }
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(LEN);
    for (size_t i = 0; i < LEN; i++) {
        mdata.WriteString(bundleName);
    }
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SET_TARGET_BUNDLE_NAME),
        mdata, reply, option);
    FuzzHandleSetOnKeyPressEventResult();
    FuzzHandleNotifyDisconnect();
    return true;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzHandleSetTargetBundleName(data, size);
    FuzzHandleFocusMoveSearchWithCondition(data, size);
    FuzzHandleSearchElementInfoBySpecificProperty(data, size);
    FuzzHandleConfigureEvents(data, size);
    FuzzHandleSearchDefaultFocusedByWindowId(data, size);
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