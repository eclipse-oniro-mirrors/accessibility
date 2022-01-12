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
    return true;
}

void AccessibilityInteractionOperationCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityInteractionOperationCallbackProxy::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityInteractionOperationCallbackProxy::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityInteractionOperationCallbackProxy::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int requestId)
{}

void AccessibilityInteractionOperationCallbackProxy::SetPerformActionResult(const bool succeeded, const int requestId)
{}

template<typename T>
bool AccessibilityInteractionOperationCallbackProxy::WriteParcelableVector(const std::vector<T> &parcelableVector,
    Parcel &reply)
{
    return true;
}

} //namespace Accessibility
} //namespace OHOS
