/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_enable_ability_lists_observer_proxy.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityEnableAbilityListsObserverProxy::AccessibilityEnableAbilityListsObserverProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityEnableAbilityListsObserver>(impl)
{}

AccessibilityEnableAbilityListsObserverProxy::~AccessibilityEnableAbilityListsObserverProxy()
{}

bool AccessibilityEnableAbilityListsObserverProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibilityEnableAbilityListsObserverProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibilityEnableAbilityListsObserverProxy::OnAccessibilityEnableAbilityListsChanged()
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write interface token failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    int32_t result = remote->SendRequest(static_cast<uint32_t>(
        IAccessibilityEnableAbilityListsObserver::Message::ON_ACCESSIBILITY_ENABLE_ABILITY_LISTS_CHANGED),
        data, reply, option);
    if (result != NO_ERROR) {
        HILOG_ERROR("OnAccessibilityEnableAbilityListsChanged fail, error: %{public}d", result);
    }
}
} // namespace Accessibility
} // namespace OHOS