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
#include "accessibility_ipc_interface_code.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

class AbilityChannelImplFuzzTest : public AccessibleAbilityChannelStub {
public:
    AbilityChannelImplFuzzTest() = default;
    ~AbilityChannelImplFuzzTest() = default;

    RetError SearchElementInfoByAccessibilityId(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter) override
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
    RetError EnableScreenCurtain(bool isEnable) override
    {
        return RET_OK;
    }
    RetError GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t requestId,
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

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

bool FuzzHandleSearchElementInfoByAccessibilityId(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SEARCH_ELEMENTINFO_BY_ACCESSIBILITY_ID),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSearchElementInfosByText(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t accessibilityWindowId = 0;
    int64_t elementId = 0;
    int32_t requestId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    position += GetObject<int32_t>(accessibilityWindowId, &data[position], size - position);
    position += GetObject<int64_t>(elementId, &data[position], size - position);
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], data + position, size - position);
    }
    std::string text(name);
    GetObject<int32_t>(requestId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteString(text);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SEARCH_ELEMENTINFOS_BY_TEXT),
        mdata, reply, option);
    return true;
}

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
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(direction);
    mdata.WriteInt32(requestId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::FOCUS_MOVE_SEARCH),
        mdata, reply, option);
    return true;
}

bool FuzzHandleExecuteAction(const uint8_t *data, size_t size)
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
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(accessibilityWindowId);
    mdata.WriteInt64(elementId);
    mdata.WriteInt32(action);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::PERFORM_ACTION),
        mdata, reply, option);
    return true;
}

bool FuzzHandleGetWindow(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    int32_t windowId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    GetObject<int32_t>(windowId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(windowId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::GET_WINDOW),
        mdata, reply, option);
    return true;
}

bool FuzzHandleGetWindows(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::GET_WINDOWS),
        mdata, reply, option);
    return true;
}

bool FuzzHandleGetWindowsByDisplayId(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint64_t displayId = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    GetObject<uint64_t>(displayId, &data[position], size - position);
    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteUint64(displayId);
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::GET_WINDOWS_BY_DISPLAY_ID),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSendSimulateGesturePath(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SEND_SIMULATE_GESTURE_PATH),
        mdata, reply, option);
    return true;
}

bool FuzzHandleSetTargetBundleName(const uint8_t *data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    int32_t bundleNamesSize = 0;
    MessageParcel mdata;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::shared_ptr<AbilityChannelImplFuzzTest> chanImp = std::make_shared<AbilityChannelImplFuzzTest>();
    mdata.WriteInterfaceToken(AccessibleAbilityChannelStub::GetDescriptor());
    mdata.WriteInt32(bundleNamesSize);
    mdata.WriteString("test");
    chanImp->OnRemoteRequest(static_cast<uint32_t>(AccessibilityInterfaceCode::SET_TARGET_BUNDLE_NAME),
        mdata, reply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::FuzzHandleSearchElementInfoByAccessibilityId(data, size);
    OHOS::Accessibility::FuzzHandleSearchElementInfosByText(data, size);
    OHOS::Accessibility::FuzzHandleFindFocusedElementInfo(data, size);
    OHOS::Accessibility::FuzzHandleFocusMoveSearch(data, size);
    OHOS::Accessibility::FuzzHandleExecuteAction(data, size);
    OHOS::Accessibility::FuzzHandleGetWindow(data, size);
    OHOS::Accessibility::FuzzHandleGetWindows(data, size);
    OHOS::Accessibility::FuzzHandleGetWindowsByDisplayId(data, size);
    OHOS::Accessibility::FuzzHandleSendSimulateGesturePath(data, size);
    OHOS::Accessibility::FuzzHandleSetTargetBundleName(data, size);
    return 0;
}