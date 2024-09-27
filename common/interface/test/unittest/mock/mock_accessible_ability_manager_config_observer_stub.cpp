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

#include "accessible_ability_manager_config_observer_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerConfigObserverStubMock::AccessibleAbilityManagerConfigObserverStubMock()
{
    HILOG_DEBUG();
}

int AccessibleAbilityManagerConfigObserverStubMock::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibleAbilityManagerConfigObserverStubMock::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CONFIG_STATE_CHANGED)) {
        return HandleOnConfigStateChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(
                    AccessibilityInterfaceCode::ON_DALTONIZATION_COLOR_FILTER_CHANGED)) {
        return HandleOnDaltonizationColorFilterChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CONTENT_TIMEOUT_CHANGED)) {
        return HandleOnContentTimeoutChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_MOUSE_AUTOCLICK_CHANGED)) {
        return HandleOnMouseAutoClickChanged(data, reply);
    }
    if (code ==
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_BRIGHTNESS_DISCOUNT_CHANGED)) {
        return HandleOnBrightnessDiscountChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_AUDIO_BALANCE_CHANGED)) {
        return HandleOnAudioBalanceChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_SHORTKEY_TARGET_CHANGED)) {
        return HandleOnShortkeyTargetChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_SHORTKEY_MULTI_TARGET_CHANGED)) {
        return HandleOnShortkeyMultiTargetChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CLICK_RESPONSE_TIME)) {
        return HandleOnClickResponseTimeChanged(data, reply);
    }
    if (code == static_cast<uint32_t>(AccessibilityInterfaceCode::ON_IGNORE_REPEAT_CLICK_TIME)) {
        return HandleOnIgnoreRepeatClickTimeChanged(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnConfigStateChanged(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
    uint32_t stateType = data.ReadUint32();
    OnConfigStateChanged(stateType);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnAudioBalanceChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    float audioBalance = data.ReadFloat();
    OnAudioBalanceChanged(audioBalance);

    return NO_ERROR;
}
ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnBrightnessDiscountChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    float brightnessDiscount = data.ReadFloat();
    OnBrightnessDiscountChanged(brightnessDiscount);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnContentTimeoutChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    uint32_t contentTimeout = data.ReadUint32();
    OnContentTimeoutChanged(contentTimeout);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnDaltonizationColorFilterChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    uint32_t filterType = data.ReadUint32();
    OnDaltonizationColorFilterChanged(filterType);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnMouseAutoClickChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    int32_t mouseAutoClick = data.ReadInt32();
    OnMouseAutoClickChanged(mouseAutoClick);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnShortkeyTargetChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    std::string shortkeyTarget = data.ReadString();
    OnShortkeyTargetChanged(shortkeyTarget);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnShortkeyMultiTargetChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    std::vector<std::string> shortkeyMultiTarget;
    if (!data.ReadStringVector(&shortkeyMultiTarget)) {
        return ERR_TRANSACTION_FAILED;
    }
    OnShortkeyMultiTargetChanged(shortkeyMultiTarget);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnClickResponseTimeChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    uint32_t time = data.ReadUint32();
    OnClickResponseTimeChanged(time);

    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerConfigObserverStubMock::HandleOnIgnoreRepeatClickTimeChanged(
    MessageParcel& data, MessageParcel& reply)
{
    HILOG_DEBUG();
    uint32_t time = data.ReadUint32();
    OnIgnoreRepeatClickTimeChanged(time);

    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS