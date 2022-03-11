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
#include "accessibility_element_operator_callback_proxy.h"
#include "hilog_wrapper.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorCallbackProxy::AccessibilityElementOperatorCallbackProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperatorCallback>(impl)
{}

AccessibilityElementOperatorCallbackProxy::~AccessibilityElementOperatorCallbackProxy()
{}

bool AccessibilityElementOperatorCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityElementOperatorCallbackProxy::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int requestId)
{}

void AccessibilityElementOperatorCallbackProxy::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int requestId)
{}

void AccessibilityElementOperatorCallbackProxy::SetFocusMoveSearchResult(const AccessibilityElementInfo &info,
    const int requestId)
{}

void AccessibilityElementOperatorCallbackProxy::SetExecuteActionResult(const bool succeeded, const int requestId)
{}

template<typename T>
bool AccessibilityElementOperatorCallbackProxy::WriteParcelableVector(const std::vector<T> &parcelableVector,
    Parcel &reply)
{
    return true;
}
} // namespace Accessibility
} // namespace OHOS
