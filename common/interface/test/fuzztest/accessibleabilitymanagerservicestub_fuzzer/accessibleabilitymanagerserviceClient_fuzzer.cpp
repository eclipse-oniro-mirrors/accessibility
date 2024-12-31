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

#include "accessibility_config_impl.h"
#include "accessibility_def.h"
#include "accessibility_element_operator_fuzz_impl.h"
#include "accessibility_element_operator_impl.h"
#include "accessibility_event_info_parcel.h"
#include "accessibility_ipc_interface_code.h"
#include "accessibility_system_ability_client_impl.h"
#include "accessible_ability_manager_service.h"
#include "accessible_ability_manager_service_stub.h"
#include "accessibleabilitymanagerservicestub_fuzzer.h"
#include "addaamstoken_fuzzer.h"
#include "singleton.h"

#define private public

namespace OHOS {
namespace Accessibility {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr size_t BASE_CODE = 800;
constexpr size_t MESSAGE_SIZE = SERVICE_CODE_END - SEND_EVENT;
constexpr size_t FUZZ_NUM1 = 1;
constexpr size_t FUZZ_NUM2 = 2;
constexpr size_t FUZZ_NUM3 = 3;
constexpr size_t FUZZ_NUM8 = 8;
constexpr size_t FUZZ_NUM16 = 16;
constexpr size_t FUZZ_NUM24 = 24;
constexpr uint8_t DEVISOR_TWO = 2;
} // namespace

uint32_t GetU32Data(const uint8_t *ptr)
{
    return (ptr[0] << FUZZ_NUM24) | (ptr[FUZZ_NUM1] << FUZZ_NUM16) | (ptr[FUZZ_NUM2] << FUZZ_NUM8) | (ptr[FUZZ_NUM3]);
}

static bool g_initialized = false;

bool InitService()
{
    if (!g_initialized) {
        DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->handler_) {
            g_initialized = true;
        }
    }
    return g_initialized;
}

/*
* Sort the functions from AccessibilityInterfaceCode 800 in ascending order
*/
bool HandleRegisterStateCallbackTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    request.WriteInterfaceToken(descriptor);
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }

    auto *impl = new AccessibilityConfig::AccessibilityConfig::Impl();
    sptr<AccessibleAbilityManagerConfigObserverImpl> callback = new (std::nothrow)
        AccessibleAbilityManagerConfigObserverImpl(impl);
    if (!request.WriteRemoteObject(callback->AsObject())) {
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::REGISTER_STATE_CALLBACK), request, reply, option);
    return true;
}

bool HandleGetAbilityListTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    request.WriteInterfaceToken(descriptor);
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }

    uint32_t abilityTypes = ACCESSIBILITY_ABILITY_TYPE_ALL;
    int32_t stateType = data[0];
    auto *impl = new AccessibilityConfig::AccessibilityConfig::Impl();
    if (!request.WriteUint32(abilityTypes)) {
        return false;
    }

    if (!request.WriteInt32(stateType)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::GET_ABILITYLIST), request, reply, option);
    return true;
}

bool HandleRegisterAccessibilityElementOperatorTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    request.WriteInterfaceToken(descriptor);
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }

    int32_t windowId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(windowId)) {
        return false;
    }
    std::shared_ptr<AccessibilityElementOperatorFuzzImpl> operation =
        std::make_shared<AccessibilityElementOperatorFuzzImpl>();
    auto *impl = new AccessibilitySystemAbilityClientImpl();
    sptr<AccessibilityElementOperatorImpl> optr = new (std::nothrow) AccessibilityElementOperatorImpl(
        windowId, operation, reinterpret_cast<AccessibilityElementOperatorCallback &>(impl));
    if (optr == nullptr) {
        return false;
    }
    if (!request.WriteRemoteObject(optr->AsObject())) {
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::REGISTER_INTERACTION_CONNECTION), request, reply, option);
    return true;
}

bool HandleMultiRegisterAccessibilityElementOperatorTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    request.WriteInterfaceToken(descriptor);
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }

    int32_t windowId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(windowId)) {
        return false;
    }
    int32_t parentWindowId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(parentWindowId)) {
        return false;
    }
    int32_t parentTreeId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(parentTreeId)) {
        return false;
    }
    int32_t elementId = static_cast<int32_t>(*data);
    if (!request.WriteInt64(elementId)) {
        return false;
    }
    std::shared_ptr<AccessibilityElementOperatorFuzzImpl> operation =
        std::make_shared<AccessibilityElementOperatorFuzzImpl>();
    auto *impl = new AccessibilitySystemAbilityClientImpl();
    sptr<AccessibilityElementOperatorImpl> optr = new (std::nothrow) AccessibilityElementOperatorImpl(
        windowId, operation, reinterpret_cast<AccessibilityElementOperatorCallback &>(impl));
    if (optr == nullptr) {
        return false;
    }
    if (!request.WriteRemoteObject(optr->AsObject())) {
        return false;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::CARDREGISTER_INTERACTION_CONNECTION), request, reply, option);
    return true;
}

bool HandleDeregisterAccessibilityElementOperatorTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    request.WriteInterfaceToken(descriptor);
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }

    int32_t windowId = static_cast<int32_t>(*data);
    if (!request.WriteInt32(windowId)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::DEREGISTER_INTERACTION_CONNECTION), request, reply, option);
    return true;
}

bool HandleSendEventTest(const uint8_t *data, size_t size)
{
    if (!InitService()) {
        return false;
    }
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }
    AccessibilityEventInfo event;
    AccessibilityEventInfoParcel eventParcel(event);
    if (!request.WriteParcelable(&eventParcel)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::SEND_EVENT), request, reply, option);
    return true;
}

bool HandleSetDaltonizationStateTest(const uint8_t *data, size_t size)
{
    MessageParcel request;
    std::u16string descriptor = AccessibleAbilityManagerServiceStub::GetDescriptor();
    if (!request.WriteInterfaceToken(descriptor)) {
        return false;
    }
    bool isSetDaltonizationState = data[0] % DEVISOR_TWO;
    if (!request.WriteBool(isSetDaltonizationState)) {
        return false;
    }
    MessageParcel reply;
    MessageOption option;

    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::SET_DALTONIZATION_STATE), request, reply, option);
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
    DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::REMOVE_REQUEST_ID), request, reply, option);
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

    OHOS::Accessibility::HandleRegisterStateCallbackTest(data, size);
    OHOS::Accessibility::HandleGetAbilityListTest(data, size);
    OHOS::Accessibility::HandleRegisterAccessibilityElementOperatorTest(data, size);
    OHOS::Accessibility::HandleMultiRegisterAccessibilityElementOperatorTest(data, size);
    OHOS::Accessibility::HandleDeregisterAccessibilityElementOperatorTest(data, size);
    OHOS::Accessibility::HandleSendEventTest(data, size);
    OHOS::Accessibility::HandleSetDaltonizationStateTest(data, size);
    OHOS::Accessibility::HandleRemoveRequestIdTest(data, size);
    return 0;
}