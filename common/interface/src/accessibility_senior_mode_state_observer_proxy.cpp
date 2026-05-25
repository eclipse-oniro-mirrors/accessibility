/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "accessibility_senior_mode_state_observer_proxy.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityAppSeniorModeStateObserverProxy::AccessibilityAppSeniorModeStateObserverProxy(
    const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IAccessibilityAppSeniorModeStateObserver>(impl)
{}

AccessibilityAppSeniorModeStateObserverProxy::~AccessibilityAppSeniorModeStateObserverProxy() {}

bool AccessibilityAppSeniorModeStateObserverProxy::WriteInterfaceToken(MessageParcel& data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibilityAppSeniorModeStateObserverProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibilityAppSeniorModeStateObserverProxy::OnSeniorModeStateChanged(const std::string& bundleName,
    int32_t appIndex, bool state)
{
    HILOG_DEBUG();

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("write interface token failed");
        return;
    }
    if (!data.WriteString(bundleName)) {
        HILOG_ERROR("fail, connection write bundleName error");
        return;
    }
    if (!data.WriteInt32(appIndex)) {
        HILOG_ERROR("fail, connection write appIndex error");
        return;
    }
    if (!data.WriteBool(state)) {
        HILOG_ERROR("fail, connection write state error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_SENIOR_MODE_STATE_CHANGED), data, reply,
        option);
    if (result != NO_ERROR) {
        HILOG_ERROR("OnSeniorModeStateChanged fail, error: %{public}d", result);
    }
}
} // namespace Accessibility
} // namespace OHOS
