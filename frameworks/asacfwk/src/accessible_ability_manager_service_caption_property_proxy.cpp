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
#include "accessible_ability_manager_service_caption_property_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceCaptionPropertyProxy::AccessibleAbilityManagerServiceCaptionPropertyProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerServiceCaptionProperty>(impl)
{}

AccessibleAbilityManagerServiceCaptionPropertyProxy::~AccessibleAbilityManagerServiceCaptionPropertyProxy()
{}

bool AccessibleAbilityManagerServiceCaptionPropertyProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("start");
    if (!data.WriteInterfaceToken(AccessibleAbilityManagerServiceCaptionPropertyProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibleAbilityManagerServiceCaptionPropertyProxy::OnCaptionPropertyChanged(const CaptionProperty &caption)
{
    HILOG_DEBUG("start");

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteParcelable(&caption)) {
        HILOG_ERROR("fail, connection write caption property error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceCaptionProperty::Message::ON_CAPTION_PROPERTY_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnCaptionPropertyChanged fail, error: %{public}d", error);
        return;
    }
}
} // namespace Accessibility
} // namespace OHOS