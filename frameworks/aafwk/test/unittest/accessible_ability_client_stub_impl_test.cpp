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
#include "accessibility_extension.h"
#include "accessibility_extension_context.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class UnitTestAccessibleAbilityListener : public AccessibilityExtension {
public:
    UnitTestAccessibleAbilityListener()
    {
        GTEST_LOG_(INFO) << "mock aa listener's constructor";
        std::shared_ptr<AppExecFwk::AbilityLocalRecord> record = nullptr;
        std::shared_ptr<AppExecFwk::OHOSApplication> application = nullptr;
        std::shared_ptr<AppExecFwk::AbilityHandler> handler = nullptr;
        sptr<IRemoteObject> token = nullptr;
        Init(record, application, handler, token);
    }
    virtual ~UnitTestAccessibleAbilityListener() {}

    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener Init";
        AccessibilityExtension::Init(record, application, handler, token);
    }

    void OnAbilityConnected() override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAbilityConnected";
    }

    void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override
    {
        GTEST_LOG_(INFO) << "UnitTestAccessibleAbilityListener OnAccessibilityEvent";
    }

    bool OnKeyPressEvent(const MMI::KeyEvent& keyEvent) override
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
    std::shared_ptr<AbilityRuntime::ContextImpl> contextImpl = std::make_shared<AbilityRuntime::ContextImpl>();
    contextImpl->InitAppContext();
    stub_ = new AccessibleAbilityClientStubImpl();
}

void AccessibleAbilityClientStubImplUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    stub_ = nullptr;
}

void AccessibleAbilityClientStubImplUnitTest::InitToSetChannelId()
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImplUnitTest InitToSetChannelId";
    shared_ptr<AccessibilityExtension> listener = make_shared<UnitTestAccessibleAbilityListener>();
    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->RegisterListenerImpl(listener);
    stub_->Init(channel, channelId);
    sleep(1); // sleep for task completed.

    stub = nullptr;
    channel = nullptr;
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001
 * @tc.name: RegisterListenerImpl
 * @tc.desc: Test function RegisterListenerImpl
 *           Register a listener which is already exist.
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001 start";

    shared_ptr<AccessibilityExtension> listener = make_shared<UnitTestAccessibleAbilityListener>();
    stub_->RegisterListenerImpl(listener);
    stub_->RegisterListenerImpl(listener);

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_RegisterListenerImpl_001 end";
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

    stub = nullptr;
    channel = nullptr;

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

    shared_ptr<AccessibilityExtension> listener = make_shared<UnitTestAccessibleAbilityListener>();
    sptr<AccessibleAbilityChannelStubMock> stub = new AccessibleAbilityChannelStubMock();
    sptr<IAccessibleAbilityChannel> channel = new AccessibleAbilityChannelProxy(stub->AsObject());
    int channelId = 0;

    stub_->RegisterListenerImpl(listener);
    stub_->Init(channel, channelId);
    sleep(1); // sleep for task completed.

    stub = nullptr;
    channel = nullptr;

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Init_002 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Disconnect_001 start";

    int channelId = 0;
    InitToSetChannelId();
    stub_->Disconnect(channelId);
    sleep(1); // sleep for task Disconnect completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_Disconnect_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001 start";

    AccessibilityEventInfo eventInfo {};

    InitToSetChannelId();
    stub_->OnAccessibilityEvent(eventInfo);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001 start";

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    int sequence = 0;

    InitToSetChannelId();
    stub_->OnKeyPressEvent(*keyEvent, sequence);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnKeyPressEvent_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnDisplayResized_001
 * @tc.name: OnDisplayResized
 * @tc.desc: Test function OnDisplayResized
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_OnDisplayResized_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnDisplayResized_001 start";

    int displayId = 0;
    Rect rect {};
    float scale = 1;
    float centerX = 0;
    float centerY = 0;

    InitToSetChannelId();
    stub_->OnDisplayResized(displayId, rect, scale, centerX, centerY);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnDisplayResized_001 end";
}

/**
 * @tc.number: AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001
 * @tc.name: OnGestureSimulateResult
 * @tc.desc: Test function OnGestureSimulateResult
 */
HWTEST_F(AccessibleAbilityClientStubImplUnitTest,
    AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001 start";

    int sequence = 0;
    bool completedSuccessfully = true;

    InitToSetChannelId();
    stub_->OnGestureSimulateResult(sequence, completedSuccessfully);
    sleep(1); // sleep for task completed.

    GTEST_LOG_(INFO) << "AccessibleAbilityClientStubImpl_Unittest_OnGestureSimulateResult_001 end";
}
} // namespace Accessibility
} // namespace OHOS