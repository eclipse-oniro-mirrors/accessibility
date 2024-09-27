/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "accessibility_enable_ability_lists_observer_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityEnableAbilityListsObserverStubTest::AccessibilityEnableAbilityListsObserverStubTest()
{
    HILOG_DEBUG();
}

int AccessibilityEnableAbilityListsObserverStubTest::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibilityEnableAbilityListsObserverStubTest::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    if (code == static_cast<uint32_t>(
        AccessibilityInterfaceCode::ON_ACCESSIBILITY_ENABLE_ABILITY_LISTS_CHANGED)) {
        return HandleOnAccessibilityEnableAbilityListsChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(
        AccessibilityInterfaceCode::ON_ACCESSIBILITY_INSTALL_ABILITY_LISTS_CHANGED)) {
        return HandleOnAccessibilityInstallAbilityListsChanged(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityEnableAbilityListsObserverStubTest::HandleOnAccessibilityEnableAbilityListsChanged(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    OnAccessibilityEnableAbilityListsChanged();
    return NO_ERROR;
}

ErrCode AccessibilityEnableAbilityListsObserverStubTest::HandleOnAccessibilityInstallAbilityListsChanged(
    MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    OnAccessibilityInstallAbilityListsChanged();
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS