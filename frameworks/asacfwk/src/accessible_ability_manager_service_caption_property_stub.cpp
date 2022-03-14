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
#include "accessible_ability_manager_service_caption_property_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "accessibility_system_ability_client.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceCaptionPropertyStub::AccessibleAbilityManagerServiceCaptionPropertyStub()
{
    HILOG_DEBUG("start");
}

int AccessibleAbilityManagerServiceCaptionPropertyStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityManagerServiceCaptionPropertyStub::OnRemoteRequest,\
        cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerServiceCaptionPropertyStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    if (code ==
        static_cast<uint32_t>(IAccessibleAbilityManagerServiceCaptionProperty::Message::ON_CAPTION_PROPERTY_CHANGED)) {
        return HandleOnCaptionPropertyChanged(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerServiceCaptionPropertyStub::HandleOnCaptionPropertyChanged(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG("start");
    CaptionProperty* caption = data.ReadParcelable<CaptionProperty>();
    if (!caption) {
            HILOG_ERROR("ReadParcelable<CaptionProperty> failed");
            return ERROR;
    }
    OnCaptionPropertyChanged(*caption);

    return NO_ERROR;
}

void AccessibleAbilityManagerServiceCaptionPropertyStub::OnCaptionPropertyChanged(const CaptionProperty& caption)
{
    HILOG_DEBUG("start");
    std::shared_ptr<AccessibilitySystemAbilityClient>  instance = AccessibilitySystemAbilityClient::GetInstance();
    if (instance == nullptr) {
        HILOG_DEBUG("Can't get asac instance");
        return;
    }
    instance->UpdatecaptionProperty(caption);
}
} // namespace Accessibility
} // namespace OHOS