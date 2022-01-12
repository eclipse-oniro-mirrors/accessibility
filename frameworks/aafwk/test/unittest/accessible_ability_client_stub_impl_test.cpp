/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <memory>
#include <gtest/gtest.h>
#include "accessible_ability_listener.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

const string AA_UT_NAME = "AccessibleAbilityClientStubImplUnitTest";

class UnitTestAccessibleAbilityListener : public AccessibleAbilityListener {
public:
    UnitTestAccessibleAbilityListener() {}
    virtual ~UnitTestAccessibleAbilityListener() {}

    void OnAbilityConnected() override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAbilityConnected";
    }

    void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAccessibilityEvent";
    }

    void OnGesture(uint32_t gestureId) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnGesture";
    }

    void OnInterrupt() override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnInterrupt";
    }

    bool OnKeyPressEvent(const KeyEvent& keyEvent) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnKeyPressEvent";
        return true;
    }
};

class AccessibleAbilityClientStubImplUnitTest : public ::testing::Test {
public:
    AccessibleAbilityClientStubImplUnitTest() {}
    ~AccessibleAbilityClientStubImplUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void InitToSetChannelId();

    shared_ptr<AccessibleAbilityEventHandler> accessibleAbilityEventHandler_ =
        make_shared<AccessibleAbilityEventHandler>(AppExecFwk::EventRunner::Create(AA_UT_NAME));
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
};

void AccessibleAbilityClientStubImplUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientStubImplUnitTest Start ######################";
}

void AccessibleAbilityClientStubImplUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibleAbilityClientStubImplUnitTest End ######################";
}

void AccessibleAbilityClientStubImplUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    stub_ = new AccessibleAbilityClientStubImpl(accessibleAbilityEventHandler_);
}

void AccessibleAbilityClientStubImplUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    stub_ = nullptr;
}

void AccessibleAbilityClientStubImplUnitTest::InitToSetChannelId()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImplUnitTest InitToSetChannelId";
    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->Init(channel, channelId);
    sleep(1); // sleep for task completed.
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001
 * @tc.name: RegisterListenerImpl
 * @tc.desc: Test function RegisterListenerImpl
 *           Register a listener which is already exist.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    stub_->RegisterListenerImpl(listener);
    stub_->RegisterListenerImpl(listener);
    
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_002
 * @tc.name: RegisterListenerImpl
 * @tc.desc: Test function RegisterListenerImpl
 *           Register a listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_002 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    stub_->RegisterListenerImpl(listener);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_Init_001
 * @tc.name: Init
 * @tc.desc: Test function Init
 *           Init without listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_Init_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Init_001 start";

    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->Init(channel, channelId);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Init_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_Init_002
 * @tc.name: Init
 * @tc.desc: Test function Init
 *           Init with listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_Init_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Init_002 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->RegisterListenerImpl(listener);
    stub_->Init(channel, channelId);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Init_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Disconnect_001 start";

    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->Init(channel, channelId);
    sleep(1); // sleep for task Init completed.

    stub_->Disconnect(channelId);
    sleep(1); // sleep for task Disconnect completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Disconnect_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 *           Invoke OnAccessibilityEvent without listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001 start";

    AccessibilityEventInfo eventInfo {};

    InitToSetChannelId();
    stub_->OnAccessibilityEvent(eventInfo);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 *           Invoke OnAccessibilityEvent with listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_002 start";

    AccessibilityEventInfo eventInfo {};
    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();

    stub_->RegisterListenerImpl(listener);
    InitToSetChannelId();
    stub_->OnAccessibilityEvent(eventInfo);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_001
 * @tc.name: OnInterrupt
 * @tc.desc: Test function OnInterrupt
 *           Invoke OnInterrupt without listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_001 start";

    InitToSetChannelId();
    stub_->OnInterrupt();
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_002
 * @tc.name: OnInterrupt
 * @tc.desc: Test function OnInterrupt
 *           Invoke OnInterrupt with listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_002 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();

    stub_->RegisterListenerImpl(listener);
    InitToSetChannelId();
    stub_->OnInterrupt();
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnInterrupt_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnGesture_001
 * @tc.name: OnGesture
 * @tc.desc: Test function OnGesture
 *           Invoke OnGesture without listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGesture_001 start";

    int gestureId = 1;

    InitToSetChannelId();
    stub_->OnGesture(gestureId);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGesture_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnGesture_002
 * @tc.name: OnGesture
 * @tc.desc: Test function OnGesture
 *           Invoke OnGesture with listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGesture_002 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    int gestureId = 1;

    stub_->RegisterListenerImpl(listener);
    InitToSetChannelId();
    stub_->OnGesture(gestureId);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGesture_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 *           Invoke OnKeyPressEvent without listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001 start";

    KeyEvent keyEvent {};
    int sequence = 0;

    stub_->OnKeyPressEvent(keyEvent, sequence);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_002
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 *           Invoke OnKeyPressEvent with listener.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_002 start";

    shared_ptr<AccessibleAbilityListener> listener = make_shared<UnitTestAccessibleAbilityListener>();
    KeyEvent keyEvent {};
    int sequence = 0;

    stub_->RegisterListenerImpl(listener);
    stub_->OnKeyPressEvent(keyEvent, sequence);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnDisplayResizeChanged_001
 * @tc.name: OnDisplayResizeChanged
 * @tc.desc: Test function OnDisplayResizeChanged
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnDisplayResizeChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnDisplayResizeChanged_001 start";

    int displayId = 0;
    Rect rect {};
    float scale = 1;
    float centerX = 0;
    float centerY = 0;

    InitToSetChannelId();
    stub_->OnDisplayResizeChanged(displayId, rect, scale, centerX, centerY);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnDisplayResizeChanged_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001 start";

    int sequence = 0;
    bool completedSuccessfully = true;

    InitToSetChannelId();
    stub_->OnGestureSimulateResult(sequence, completedSuccessfully);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGestureValidityChanged_001
 * @tc.name: OnFingerprintGestureValidityChanged
 * @tc.desc: Test function OnFingerprintGestureValidityChanged
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGestureValidityChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGestureValidityChanged_001 start";

    bool validity = true;

    InitToSetChannelId();
    stub_->OnFingerprintGestureValidityChanged(validity);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGestureValidityChanged_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGesture_001
 * @tc.name: OnFingerprintGesture
 * @tc.desc: Test function OnFingerprintGesture
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest, AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGesture_001 start";

    int gesture = 0;

    InitToSetChannelId();
    stub_->OnFingerprintGesture(gesture);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnFingerprintGesture_001 end";
}
