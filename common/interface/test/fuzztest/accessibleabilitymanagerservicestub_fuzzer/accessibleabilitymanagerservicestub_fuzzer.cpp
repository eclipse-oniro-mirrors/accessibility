/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "accessibleabilitymanagerservicestub_fuzzer.h"

#include "accessibility_ipc_interface_code.h"
#include "accessible_ability_manager_service_stub.h"
#include "securec.h"
#include "addaamstoken_fuzzer.h"

namespace OHOS {
namespace Accessibility {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr size_t BASE_CODE = 1;
constexpr size_t MESSAGE_SIZE = SERVICE_CODE_END - SEND_EVENT;
constexpr size_t FUZZ_NUM1 = 1;
constexpr size_t FUZZ_NUM2 = 2;
constexpr size_t FUZZ_NUM3 = 3;
constexpr size_t FUZZ_NUM8 = 8;
constexpr size_t FUZZ_NUM16 = 16;
constexpr size_t FUZZ_NUM24 = 24;
constexpr uint8_t DEVISOR_TWO = 2;
} // namespace

class AccessibleAbilityManagerServiceStubFuzzTest : public AccessibleAbilityManagerServiceStub {
public:
    AccessibleAbilityManagerServiceStubFuzzTest() = default;
    ~AccessibleAbilityManagerServiceStubFuzzTest() = default;

    ErrCode SendEvent(const AccessibilityEventInfoParcel& eventInfoParcel, int32_t flag) override
    {
        return RET_OK;
    }

    ErrCode RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback,
        uint32_t &state) override
    {
        return RET_OK;
    }

    ErrCode RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver> &callback) override
    {
        return RET_OK;
    }

    ErrCode RegisterEnableAbilityListsObserver(const sptr<IAccessibilityEnableAbilityListsObserver> &observer) override
    {
        return RET_OK;
    }

    ErrCode GetAbilityList(
        uint32_t abilityTypes, int32_t stateType, std::vector<AccessibilityAbilityInfoParcel>& infos) override
    {
        return RET_OK;
    }

    ErrCode RegisterElementOperatorByWindowId(
        const int32_t windowId, const sptr<IAccessibilityElementOperator> &elementOperation) override
    {
        return RET_OK;
    }

    ErrCode RegisterElementOperatorByParameter(
        const RegistrationPara& parameter, const sptr<IAccessibilityElementOperator>& elementOperation) override
    {
        return RET_OK;
    }

    ErrCode DeregisterElementOperatorByWindowId(const int32_t windowId) override
    {
        return RET_OK;
    }

    ErrCode DeregisterElementOperatorByWindowIdAndTreeId(const int32_t windowId, const int32_t treeId) override
    {
        return RET_OK;
    }

    ErrCode GetCaptionProperty(CaptionPropertyParcel &caption) override
    {
        return RET_OK;
    }
    ErrCode GetCaptionState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetScreenReaderState(bool &state) override
    {
        return RET_OK;
    }

    ErrCode SetCaptionProperty(const CaptionPropertyParcel &caption) override
    {
        return RET_OK;
    }
    ErrCode SetCaptionState(bool state) override
    {
        return RET_OK;
    }

    ErrCode EnableAbility(const std::string &name, const uint32_t capabilities) override
    {
        return RET_OK;
    }
    ErrCode GetEnabledAbilities(std::vector<std::string> &enabledAbilities) override
    {
        return RET_OK;
    }
    ErrCode DisableAbility(const std::string &name) override
    {
        return RET_OK;
    }
    ErrCode SetMagnificationState(const bool state) override
    {
        return RET_OK;
    }
    ErrCode GetActiveWindow(int32_t &windowId) override
    {
        return RET_OK;
    }
    ErrCode GetActiveWindow(int32_t &windowId, bool systemApi) override
    {
        return RET_OK;
    }
    ErrCode CheckExtensionAbilityPermission(std::string& processName) override
    {
        return RET_OK;
    }
    ErrCode EnableUITestAbility(const sptr<IRemoteObject> &obj) override
    {
        return RET_OK;
    }
    ErrCode DisableUITestAbility() override
    {
        return RET_OK;
    }

    ErrCode SetScreenMagnificationState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetShortKeyState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetMouseKeyState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetMouseAutoClick(int32_t time) override
    {
        return RET_OK;
    }
    ErrCode SetShortkeyTarget(const std::string &name) override
    {
        return RET_OK;
    }
    ErrCode SetShortkeyMultiTarget(const std::vector<std::string> &name) override
    {
        return RET_OK;
    }
    ErrCode SetHighContrastTextState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetInvertColorState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetAnimationOffState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetAudioMonoState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetDaltonizationState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetDaltonizationColorFilter(uint32_t filter) override
    {
        return RET_OK;
    }
    ErrCode SetContentTimeout(uint32_t time) override
    {
        return RET_OK;
    }
    ErrCode SetBrightnessDiscount(float discount) override
    {
        return RET_OK;
    }
    ErrCode SetAudioBalance(float balance) override
    {
        return RET_OK;
    }
    ErrCode SetClickResponseTime(uint32_t time) override
    {
        return RET_OK;
    }
    ErrCode SetIgnoreRepeatClickState(bool state) override
    {
        return RET_OK;
    }
    ErrCode SetIgnoreRepeatClickTime(uint32_t time) override
    {
        return RET_OK;
    }

    ErrCode GetScreenMagnificationState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetShortKeyState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetMouseKeyState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetMouseAutoClick(int32_t &time) override
    {
        return RET_OK;
    }
    ErrCode GetShortkeyTarget(std::string &name) override
    {
        return RET_OK;
    }
    ErrCode GetShortkeyMultiTarget(std::vector<std::string> &name) override
    {
        return RET_OK;
    }
    ErrCode GetHighContrastTextState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetInvertColorState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetAnimationOffState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetAudioMonoState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetDaltonizationState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetDaltonizationColorFilter(uint32_t &type) override
    {
        return RET_OK;
    }
    ErrCode GetContentTimeout(uint32_t &timer) override
    {
        return RET_OK;
    }
    ErrCode GetBrightnessDiscount(float &brightness) override
    {
        return RET_OK;
    }
    ErrCode GetAudioBalance(float &balance) override
    {
        return RET_OK;
    }
    ErrCode GetClickResponseTime(uint32_t &time) override
    {
        return RET_OK;
    }
    ErrCode GetIgnoreRepeatClickState(bool &state) override
    {
        return RET_OK;
    }
    ErrCode GetIgnoreRepeatClickTime(uint32_t &time) override
    {
        return RET_OK;
    }
    ErrCode GetAllConfigs(AccessibilityConfigData& configData, CaptionPropertyParcel& caption) override
    {
        return RET_OK;
    }
    ErrCode GetRealWindowAndElementId(int32_t &windowId, int64_t &elementId) override
    {
        return RET_OK;
    }
    ErrCode GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t &innerWid) override
    {
        return RET_OK;
    }
    ErrCode GetFocusedWindowId(int32_t &focusedWindowId) override
    {
        return RET_OK;
    }
    ErrCode RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) override
    {
        return RET_OK;
    }
    void PostDelayUnloadTask()
    {
    }
    ErrCode RemoveRequestId(int32_t requestId) override
    {
        return RET_OK;
    }
    ErrCode GetRootParentId(int32_t windowsId, int32_t treeId, int64_t &parentId) override
    {
        return 0;
    }
    ErrCode GetRootParentId(int32_t windowsId, int32_t treeId, int64_t &parentId, bool systemApi) override
    {
        return 0;
    }
    int32_t SetEnhanceConfig(const AccessibilitySecCompRawdata& rawData) override
    {
        return 0;
    }
};

uint32_t GetU32Data(const uint8_t *ptr)
{
    return (ptr[0] << FUZZ_NUM24) | (ptr[FUZZ_NUM1] << FUZZ_NUM16) | (ptr[FUZZ_NUM2] << FUZZ_NUM8) | (ptr[FUZZ_NUM3]);
}

bool OnRemoteRequestSvcFuzzTest(const uint8_t *data, size_t size)
{
    uint32_t code = (GetU32Data(data) % MESSAGE_SIZE) + BASE_CODE;
    MessageParcel datas;
    std::u16string descriptor = AccessibleAbilityManagerServiceStubFuzzTest::GetDescriptor();
    datas.WriteInterfaceToken(descriptor);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    AccessibleAbilityManagerServiceStubFuzzTest serviceStub;
    serviceStub.OnRemoteRequest(code, datas, reply, option);
    return true;
}

bool HandleSetDaltonizationStateTest(const uint8_t *data, size_t size)
{
    MessageParcel datas;
    std::u16string descriptor = AccessibleAbilityManagerServiceStubFuzzTest::GetDescriptor();
    if (!datas.WriteInterfaceToken(descriptor)) {
        return false;
    }
    bool isSetDaltonizationState = data[0] % DEVISOR_TWO;
    if (!datas.WriteBool(isSetDaltonizationState)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    AccessibleAbilityManagerServiceStubFuzzTest serviceStub;
    uint32_t TRANSACTION_CODE = 0x30;

    serviceStub.OnRemoteRequest(TRANSACTION_CODE, datas, reply, option);
    return true;
}

bool HandleRemoveRequestIdTest(const uint8_t *data, size_t size)
{
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }
    int32_t requestId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(requestId)) {
        return false;
    }

    MessageParcel reply;
    MessageOption option;
    AccessibleAbilityManagerServiceStubFuzzTest serviceStub;
    uint32_t TRANSACTION_CODE = 0x61;
    serviceStub.OnRemoteRequest(TRANSACTION_CODE, request, reply, option);
    return true;
}

bool HandleGetScreenReaderStateTest()
{
    MessageParcel datas;
    std::u16string descriptor = AccessibleAbilityManagerServiceStubFuzzTest::GetDescriptor();
    if (!datas.WriteInterfaceToken(descriptor)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    AccessibleAbilityManagerServiceStubFuzzTest serviceStub;
    uint32_t TRANSACTION_CODE = 0x63;

    serviceStub.OnRemoteRequest(TRANSACTION_CODE, datas, reply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::AddAAMSTokenFuzzer token;
    return 0;
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::Accessibility::U32_AT_SIZE) {
        return 0;
    }

    OHOS::Accessibility::OnRemoteRequestSvcFuzzTest(data, size);
    OHOS::Accessibility::HandleSetDaltonizationStateTest(data, size);
    OHOS::Accessibility::HandleGetScreenReaderStateTest();
    OHOS::Accessibility::HandleRemoveRequestIdTest(data, size);
    return 0;
}