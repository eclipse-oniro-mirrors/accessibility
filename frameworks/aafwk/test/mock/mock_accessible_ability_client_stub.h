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

#ifndef MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_H
#define MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "accessible_ability_client_interface.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityClientStubMock : public IRemoteStub<IAccessibleAbilityClient> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAccessibleAbilityClientMock");

    AccessibleAbilityClientStubMock() : code_(0)
    {}
    virtual ~AccessibleAbilityClientStubMock()
    {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock InvokeSendRequest";
        code_ = code;
        return NO_ERROR;
    }

    int InvokeErrorSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock InvokeErrorSendRequest";
        code_ = code;
        return ERROR;
    }

    virtual void Init(const sptr<IAccessibleAbilityChannel> &channel, const int channelId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock Init";
    }

    virtual void Disconnect(const int channelId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock Disconnect";
    }

    virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnAccessibilityEvent";
    }

    virtual void OnInterrupt()
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnInterrupt";
    }

    virtual void OnGesture(const int gestureId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnGesture";
    }

    virtual void OnKeyPressEvent(const KeyEvent &keyEvent, const int sequence)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnKeyPressEvent";
    }

    virtual void OnDisplayResizeChanged(const int displayId, const Rect &rect, const float scale, const float centerX,
        const float centerY)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnDisplayResizeChanged";
    }

    virtual void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnGestureSimulateResult";
    }

    virtual void OnFingerprintGestureValidityChanged(const bool validity)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnFingerprintGestureValidityChanged";
    }

    virtual void OnFingerprintGesture(const int gesture)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityClientStubMock OnFingerprintGesture";
    }

    int code_ = 0;
};

} // namespace Accessibility
} // namespace OHOS

#endif // MOCK_ACCESSIBLE_ABILITY_CLIENT_STUB_H