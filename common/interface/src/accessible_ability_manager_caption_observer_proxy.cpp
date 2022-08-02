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

#include "accessible_ability_manager_caption_observer_proxy.h"
#include "accessibility_caption_parcel.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerCaptionObserverProxy::AccessibleAbilityManagerCaptionObserverProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerCaptionObserver>(impl)
{}

AccessibleAbilityManagerCaptionObserverProxy::~AccessibleAbilityManagerCaptionObserverProxy()
{}

bool AccessibleAbilityManagerCaptionObserverProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibleAbilityManagerCaptionObserverProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibleAbilityManagerCaptionObserverProxy::OnPropertyChanged(
    const AccessibilityConfig::CaptionProperty& property)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    CaptionPropertyParcel captionParcel(property);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&captionParcel)) {
        HILOG_ERROR("fail, connection write caption property error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerCaptionObserver::Message::ON_PROPERTY_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnPropertyChanged fail, error: %{public}d", error);
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS