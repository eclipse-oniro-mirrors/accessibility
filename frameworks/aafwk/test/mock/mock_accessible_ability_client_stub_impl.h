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

#ifndef MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H
#define MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "accessible_ability_client_stub.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityClientStubImplMock : public AccessibleAbilityClientStub {
public:
    AccessibleAbilityClientStubImplMock()
    {}
    virtual ~AccessibleAbilityClientStubImplMock()
    {}

    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock Init";
    }

    virtual void Disconnect(const int channelId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock Disconnect";
    }

    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock OnAccessibilityEvent";
    }

    virtual void OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock OnKeyPressEvent";
    }

    virtual void OnDisplayResized(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock OnDisplayResized";
    }

    virtual void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubImplMock OnGestureSimulateResult";
    }
};
} // namespace Accessibility
} // namespace OHOS

#endif // MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_IMPL_H