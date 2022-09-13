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

#include <gtest/gtest.h>
#include "accessibility_account_data.h"
#include "accessibility_event_info.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel_stub.h"
#include "accessible_ability_client_proxy.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject>& object)
    : IRemoteProxy<IAccessibleAbilityClient>(object)
{}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel& data)
{
    return true;
}

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel>& channel, const int32_t channelId)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxy Init";
    AccessibilityHelper::GetInstance().SetTestStub(channel);
    AccessibilityHelper::GetInstance().SetTestChannalId(channelId);
}

void AccessibleAbilityClientProxy::Disconnect(const int32_t channelId)
{}

void AccessibleAbilityClientProxy::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientProxy OnAccessibilityEvent";
    AccessibilityHelper::GetInstance().SetTestWindowId(eventInfo.GetWindowId());
    AccessibilityHelper::GetInstance().SetTestWindowChangeTypes(eventInfo.GetWindowChangeTypes());
    AccessibilityHelper::GetInstance().SetTestEventType(int32_t(eventInfo.GetEventType()));
    AccessibilityHelper::GetInstance().SetGestureId(int32_t(eventInfo.GetGestureType()));

    switch (eventInfo.GetEventType()) {
        case TYPE_TOUCH_GUIDE_GESTURE_BEGIN:
        case TYPE_TOUCH_GUIDE_GESTURE_END:
        case TYPE_TOUCH_GUIDE_BEGIN:
        case TYPE_TOUCH_GUIDE_END:
        case TYPE_TOUCH_BEGIN:
        case TYPE_TOUCH_END:
            break;
        default:
            return;
    }
    AccessibilityHelper::GetInstance().PushEventType(eventInfo.GetEventType());
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent& keyEvent, const int32_t sequence)
{
    MessageParcel data;
    MessageParcel reply;
    HILOG_DEBUG();
    HILOG_DEBUG("start.----------sequence--%{public}d ----------mock Proxy Start ", sequence);
    AccessibilityHelper::GetInstance().SetTestKeyPressEvent(sequence);
    HILOG_DEBUG("start.-----------------------------mock Proxy end ");
}
} // namespace Accessibility
} // namespace OHOS