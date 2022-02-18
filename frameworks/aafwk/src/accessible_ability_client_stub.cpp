/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "accessible_ability_client_stub.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {

AccessibleAbilityClientStub::AccessibleAbilityClientStub()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::INIT)] =
        &AccessibleAbilityClientStub::HandleInit;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::DISCONNECT)] =
        &AccessibleAbilityClientStub::HandleDisconnect;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_ACCESSIBILITY_EVENT)] =
        &AccessibleAbilityClientStub::HandleOnAccessibilityEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_KEY_PRESS_EVENT)] =
        &AccessibleAbilityClientStub::HandleOnKeyPressEvent;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_DISPALYRESIZE_CHANGED)] =
        &AccessibleAbilityClientStub::HandleOnDisplayResized;
    memberFuncMap_[static_cast<uint32_t>(IAccessibleAbilityClient::Message::ON_GESTURE_SIMULATE_RESULT)] =
        &AccessibleAbilityClientStub::HandleOnGestureSimulateResult;
}

AccessibleAbilityClientStub::~AccessibleAbilityClientStub()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    memberFuncMap_.clear();
}

int AccessibleAbilityClientStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                        MessageOption &option)
{
    HILOG_DEBUG("AccessibleAbilityClientStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityClientStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOG_WARN("AccessibleAbilityClientStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityClientStub::HandleInit(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (!object) {
        HILOG_ERROR("object is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibleAbilityChannel> channel = iface_cast<IAccessibleAbilityChannel>(object);
    if (!channel) {
        HILOG_ERROR("channel is nullptr.");
        return ERR_INVALID_VALUE;
    }
    int channelId = data.ReadInt32();

    Init(channel, channelId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleDisconnect(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    int channelId = data.ReadInt32();
    Disconnect(channelId);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnAccessibilityEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    std::unique_ptr<AccessibilityEventInfo> eventInfo(data.ReadParcelable<AccessibilityEventInfo>());
    if (!eventInfo) {
        HILOG_ERROR("ReadParcelable<AccessibilityEventInfo> failed");
        return ERR_INVALID_VALUE;
    }

    OnAccessibilityEvent(*eventInfo);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnKeyPressEvent(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
#if 0 // TODO: Use AccessibilityKeyEvent to instead MMI::KeyEvent
    std::unique_ptr<MMI::KeyEvent> keyEvent(data.ReadParcelable<MMI::KeyEvent>());
    if (!keyEvent) {
        HILOG_ERROR("ReadParcelable<MMI::KeyEvent> failed");
        return ERR_INVALID_VALUE;
    }

    int sequence = data.ReadInt32();

    OnKeyPressEvent(*keyEvent, sequence);
#endif
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnDisplayResized(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    int displayId = data.ReadInt32();

    sptr<Rect> rect = data.ReadStrongParcelable<Rect>();
    if (!rect) {
        HILOG_ERROR("ReadStrongParcelable<Rect> failed");
        return ERR_INVALID_VALUE;
    }

    float scale = data.ReadFloat();
    float centerX = data.ReadFloat();
    float centerY = data.ReadFloat();

    OnDisplayResized(displayId, *rect, scale, centerX, centerY);
    return NO_ERROR;
}

ErrCode AccessibleAbilityClientStub::HandleOnGestureSimulateResult(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    int sequence = data.ReadInt32();
    bool completedSuccessfully = data.ReadBool();

    OnGestureSimulateResult(sequence, completedSuccessfully);
    return NO_ERROR;
}

} // namespace Accessibility
} // namespace OHOS