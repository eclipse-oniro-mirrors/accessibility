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

#include "mock_accessible_ability_client_stub_impl.h"
#include <gtest/gtest.h>
#include "accessibility_ut_helper.h"

namespace OHOS {
namespace Accessibility {
MockAccessibleAbilityClientStubImpl::MockAccessibleAbilityClientStubImpl()
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl MockAccessibleAbilityClientStubImpl()";
}
MockAccessibleAbilityClientStubImpl::~MockAccessibleAbilityClientStubImpl()
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl ~MockAccessibleAbilityClientStubImpl()";
}
int MockAccessibleAbilityClientStubImpl::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl OnRemoteRequest";
    return 0;
}

ErrCode MockAccessibleAbilityClientStubImpl::HandleInit(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl HandleInit";
    return 0;
}
ErrCode MockAccessibleAbilityClientStubImpl::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl HandleDisconnect";
    return 0;
}
ErrCode MockAccessibleAbilityClientStubImpl::HandleOnAccessibilityEvent(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl HandleOnAccessibilityEvent";
    return 0;
}
ErrCode MockAccessibleAbilityClientStubImpl::HandleOnKeyPressEvent(MessageParcel& data, MessageParcel& reply)
{
    (void)data;
    (void)reply;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl HandleOnKeyPressEvent";
    return 0;
}
void MockAccessibleAbilityClientStubImpl::Init(const sptr<IAccessibleAbilityChannel>& channel, const int32_t channelId)
{
    (void)channel;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl Init";
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(channelId);
}
void MockAccessibleAbilityClientStubImpl::Disconnect(const int32_t channelId)
{
    (void)channelId;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl Disconnect";
    AccessibilityAbilityHelper::GetInstance().SetTestChannelId(INVALID_CHANNEL_ID);
}
void MockAccessibleAbilityClientStubImpl::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl OnAccessibilityEvent";
    AccessibilityAbilityHelper::GetInstance().SetTestEventType(int32_t(eventInfo.GetEventType()));
}
void MockAccessibleAbilityClientStubImpl::OnKeyPressEvent(const MMI::KeyEvent& keyEvent, const int32_t sequence)
{
    (void)keyEvent;
    (void)sequence;
    GTEST_LOG_(INFO) << "MockAccessibleAbilityClientStubImpl OnKeyPressEvent";
}
} // namespace Accessibility
} // namespace OHOS