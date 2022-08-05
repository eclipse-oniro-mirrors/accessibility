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

#ifndef MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H
#define MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H

#include <memory>
#include "accessible_ability_client_stub.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityClientStubImpl : public AccessibleAbilityClientStub {
public:
    MockAccessibleAbilityClientStubImpl();
    virtual ~MockAccessibleAbilityClientStubImpl();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

    virtual void Init(const sptr<IAccessibleAbilityChannel>& channel, const int32_t channelId) override;
    virtual void Disconnect(const int32_t channelId) override;
    virtual void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override;
    virtual void OnKeyPressEvent(const MMI::KeyEvent& keyEvent, const int32_t sequence) override;

private:
    ErrCode HandleInit(MessageParcel& data, MessageParcel& reply);
    ErrCode HandleDisconnect(MessageParcel& data, MessageParcel& reply);
    ErrCode HandleOnAccessibilityEvent(MessageParcel& data, MessageParcel& reply);
    ErrCode HandleOnKeyPressEvent(MessageParcel& data, MessageParcel& reply);
    DISALLOW_COPY_AND_MOVE(MockAccessibleAbilityClientStubImpl);
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H