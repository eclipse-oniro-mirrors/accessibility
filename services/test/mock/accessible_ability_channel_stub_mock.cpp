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

#include "accessible_ability_channel_stub.h"
#include "accessibility_account_data.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "parcel.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
AccessibleAbilityChannelStub::AccessibleAbilityChannelStub()
{}

AccessibleAbilityChannelStub::~AccessibleAbilityChannelStub()
{}

int32_t AccessibleAbilityChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    return 0;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleFocusMoveSearch(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetWindows(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleExecuteCommonAction(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetOnKeyPressEventResult(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG("%{public}s mock AccessibleAbilityChannelStub mock ------------------ start.", __func__);
    HILOG_DEBUG("%{public}s mock AccessibleAbilityChannelStub -------mock -----mock------------- end.", __func__);
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeScale(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterX(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeCenterY(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleGetDisplayResizeRect(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleResetDisplayResize(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSetDisplayResizeScaleAndCenter(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

ErrCode AccessibleAbilityChannelStub::HandleSendSimulateGesture(MessageParcel &data, MessageParcel &reply)
{
    return NO_ERROR;
}

} // namespace Accessibility
} // namespace OHOS