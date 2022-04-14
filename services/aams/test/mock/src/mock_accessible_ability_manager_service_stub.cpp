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

#include "accessible_ability_manager_service_stub.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityManagerServiceStub::AccessibleAbilityManagerServiceStub()
{}
AccessibleAbilityManagerServiceStub::~AccessibleAbilityManagerServiceStub()
{}
int AccessibleAbilityManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return true;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleSendEvent(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterStateCallback(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleGetAbilityList(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleRegisterAccessibilityElementOperator(
    MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}

ErrCode AccessibleAbilityManagerServiceStub::HandleDeregisterAccessibilityElementOperator(
    MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS