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

#include <cstdint>
#include <gtest/gtest.h>
#include "accessible_ability_client_proxy.h"
#include "accessibility_account_data.h"
#include "accessibility_errorcode.h"
#include "accessibility_event_info.h"
#include "accessible_ability_channel_stub.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
int g_testChannalId = -1;
int g_testEventType = -1;
int g_testWindowChangeTypes = -1;
int g_testWindowId = -1;
int g_testGesture = -1;
int g_testKeyPressEvent = -1;
int g_testDisplayId = -1;
int g_testGestureSimulateResult = -1;
OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> g_testStub = nullptr;
int g_testGestureSimulate = -1;

std::vector<EventType> g_mTeventType;
int g_mTgestureId = -1;

AccessibleAbilityClientProxy::AccessibleAbilityClientProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAccessibleAbilityClient>(object)
{
}

bool AccessibleAbilityClientProxy::WriteInterfaceToken(MessageParcel &data)
{
    return true;
}

void AccessibleAbilityClientProxy::Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
{
    g_testStub = channel;
    g_testChannalId = channelId;
}

void AccessibleAbilityClientProxy::Disconnect(const int channelId)
{
}

void AccessibleAbilityClientProxy::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    GTEST_LOG_(INFO) << "TouchGuider_Unittest_OnTouchEvent OnAccessibilityEvent";
    g_testWindowId = eventInfo.GetWindowId();
    g_testWindowChangeTypes = eventInfo.GetWindowChangeTypes();
    g_testEventType = int(eventInfo.GetEventType());
    g_mTgestureId = int(eventInfo.GetGestureType());

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
    g_mTeventType.push_back(eventInfo.GetEventType());
}

void AccessibleAbilityClientProxy::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
{
    MessageParcel data;
    MessageParcel reply;
    HILOG_DEBUG();
    HILOG_DEBUG("start.----------sequence--%{public}d ----------mock Proxy Start ", sequence);
    g_testKeyPressEvent = sequence;
    HILOG_DEBUG("start.-----------------------------mock Proxy end ");
}

void AccessibleAbilityClientProxy::OnDisplayResized(const int displayId, const Rect &rect, const float scale,
    const float centerX, const float centerY)
{
    g_testDisplayId = 1;
}

void AccessibleAbilityClientProxy::OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
{
    g_testGestureSimulateResult = 1;
    g_testGestureSimulate = sequence;
}
} // namespace Accessibility
} // namespace OHOS