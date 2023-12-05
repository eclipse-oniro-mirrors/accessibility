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

#include "accessible_ability_manager_config_observer_proxy.h"
#include "accessibility_ipc_interface_code.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerConfigObserverProxy::AccessibleAbilityManagerConfigObserverProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibleAbilityManagerConfigObserver>(impl)
{}

AccessibleAbilityManagerConfigObserverProxy::~AccessibleAbilityManagerConfigObserverProxy()
{}

bool AccessibleAbilityManagerConfigObserverProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();
    if (!data.WriteInterfaceToken(AccessibleAbilityManagerConfigObserverProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibleAbilityManagerConfigObserverProxy::OnConfigStateChanged(const uint32_t stateType)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(stateType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CONFIG_STATE_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnConfigStateChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnAudioBalanceChanged(const float audioBalance)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteFloat(audioBalance)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_AUDIO_BALANCE_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnAudioBalanceChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnBrightnessDiscountChanged(const float brightnessDiscount)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteFloat(brightnessDiscount)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_BRIGHTNESS_DISCOUNT_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnBrightnessDiscountChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnContentTimeoutChanged(const uint32_t contentTimeout)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(contentTimeout)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CONTENT_TIMEOUT_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnContentTimeoutChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnDaltonizationColorFilterChanged(const uint32_t filterType)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(filterType)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_DALTONIZATION_COLOR_FILTER_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnDaltonizationColorFilterChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnMouseAutoClickChanged(const int32_t mouseAutoClick)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt32(mouseAutoClick)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_MOUSE_AUTOCLICK_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnMouseAutoClickChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnShortkeyTargetChanged(const std::string &shortkeyTarget)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteString(shortkeyTarget)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_SHORTKEY_TARGET_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnShortkeyTargetChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnShortkeyMultiTargetChanged(
    const std::vector<std::string> &shortkeyMultiTarget)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteStringVector(shortkeyMultiTarget)) {
        HILOG_ERROR("connection write argument shortkeyMultiTarget failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_SHORTKEY_MULTI_TARGET_CHANGED),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnShortkeyMultiTargetChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnClickResponseTimeChanged(const uint32_t clickResponseTime)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(clickResponseTime)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_CLICK_RESPONSE_TIME),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnClickResponseTimeChanged fail, error: %{public}d", error);
        return;
    }
}

void AccessibleAbilityManagerConfigObserverProxy::OnIgnoreRepeatClickTimeChanged(const uint32_t ignoreRepeatClickTime)
{
    HILOG_DEBUG();

    int32_t error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteUint32(ignoreRepeatClickTime)) {
        HILOG_ERROR("fail, connection write stateType error");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        HILOG_ERROR("fail to send transact cmd due to remote object");
        return;
    }

    error = remote->SendRequest(
        static_cast<uint32_t>(AccessibilityInterfaceCode::ON_IGNORE_REPEAT_CLICK_TIME),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("OnIgnoreRepeatClickTimeChanged fail, error: %{public}d", error);
        return;
    }
}

} // namespace Accessibility
} // namespace OHOS