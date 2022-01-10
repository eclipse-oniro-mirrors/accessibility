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
#include "accessibility_interaction_operation_callback_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityInteractionOperationCallbackProxy::AccessibilityInteractionOperationCallbackProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityInteractionOperationCallback>(impl)
{}

AccessibilityInteractionOperationCallbackProxy::~AccessibilityInteractionOperationCallbackProxy()
{}
bool AccessibilityInteractionOperationCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG("%{public}s" , __func__);
    if (!data.WriteInterfaceToken(AccessibilityInteractionOperationCallbackProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AccessibilityInteractionOperationCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!WriteParcelableVector(infos, data)) {
        HILOG_ERROR("write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_BY_ACCESSIBILITY_ID),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchAccessibilityInfoByAccessibility fail, error: %d", error);
    }
}

void AccessibilityInteractionOperationCallbackProxy::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!WriteParcelableVector(infos, data)) {
        HILOG_ERROR("write failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_BY_TEXT),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SearchAccessibilityInfoByAccessibility fail, error: %d", error);
    }
}

void AccessibilityInteractionOperationCallbackProxy::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteParcelable(&info)) {
        HILOG_ERROR("%{public}s fail, connection write info", __func__);
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_FOCUSED_INFO),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetFindFocusedElementInfo fail, error: %d", error);
    }
}

void AccessibilityInteractionOperationCallbackProxy::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int requestId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteParcelable(&info)) {
        HILOG_ERROR("%{public}s fail, connection write info", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_FOCUS_MOVE),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetFocusMoveSearchResult fail, error: %d", error);
    }
}

void AccessibilityInteractionOperationCallbackProxy::SetPerformActionResult(const bool succeeded, const int requestId)
{
    HILOG_DEBUG("%{public}s" , __func__);

    int error = NO_ERROR;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s fail, connection write Token", __func__);
        return;
    }

    if (!data.WriteBool(succeeded)) {
        HILOG_ERROR("%{public}s fail, connection write succeeded", __func__);
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("%{public}s fail, connection write requestId", __func__);
        return;
    }

    error = Remote()->SendRequest(
        static_cast<uint32_t>(IAccessibilityInteractionOperationCallback::Message::SET_RESULT_PERFORM_ACTION),
        data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("SetPerformActionResult fail, error: %d", error);
    }
}

template<typename T>
bool AccessibilityInteractionOperationCallbackProxy::WriteParcelableVector(const std::vector<T> &parcelableVector,
    Parcel &reply)
{
    if (!reply.WriteInt32(parcelableVector.size())) {
        HILOG_ERROR("write ParcelableVector failed");
        return false;
    }

    for (auto &parcelable : parcelableVector) {
        if (!reply.WriteParcelable(&parcelable)) {
            HILOG_ERROR("write ParcelableVector failed");
            return false;
        }
    }
    return true;
}

} //namespace Accessibility
} //namespace OHOS
