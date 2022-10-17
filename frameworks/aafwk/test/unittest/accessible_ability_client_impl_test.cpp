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
#include "accessibility_ut_helper.h"
#include "accessible_ability_client_impl.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessible_ability_channel_proxy.h"
#include "mock_accessible_ability_channel_stub.h"
#include "mock_accessible_ability_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string TEST = "test";
    constexpr int32_t CHANNEL_ID = 1;
    constexpr int32_t SEQUENCE = 1;
    constexpr int32_t INVALID_CHILD_ID = -1;
    constexpr int INVALID_ID = -1;
} // namespace

class AccessibleAbilityClientImplTest : public ::testing::Test {
public:
    AccessibleAbilityClientImplTest()
    {}
    ~AccessibleAbilityClientImplTest()
    {}

    std::shared_ptr<AccessibleAbilityClientImpl> instance_ = nullptr;
    std::shared_ptr<AccessibleAbilityListener> listener_ = nullptr;
    sptr<MockAccessibleAbilityChannelStub> stub_ = nullptr;
    sptr<IAccessibleAbilityChannel> channel_ = nullptr;

    static void SetUpTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
        GTEST_LOG_(INFO) << "AccessibleAbilityClientImplTest Start";
    }
    static void TearDownTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
        GTEST_LOG_(INFO) << "AccessibleAbilityClientImplTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityClientImplTest SetUp()";
        instance_ = std::make_shared<AccessibleAbilityClientImpl>();
        ASSERT_TRUE(instance_);
        listener_ = std::make_shared<MockAccessibleAbilityListener>();
        ASSERT_TRUE(listener_);
        stub_ = new MockAccessibleAbilityChannelStub();
        ASSERT_TRUE(stub_);
        channel_ = iface_cast<IAccessibleAbilityChannel>(stub_);
        ASSERT_TRUE(channel_);
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibleAbilityClientImplTest TearDown()";
        AccessibilityAbilityHelper::GetInstance().SetTestChannelId(INVALID_CHANNEL_ID);
        AccessibilityAbilityHelper::GetInstance().SetTestKeyPressEvent(INVALID_ID);
        AccessibilityAbilityHelper::GetInstance().SetTestEventType(INVALID_ID);
        stub_ = nullptr;
        channel_ = nullptr;
        listener_ = nullptr;
        instance_ = nullptr;
    }

    void Connect();
};

void AccessibleAbilityClientImplTest::Connect()
{
    EXPECT_EQ(instance_->RegisterAbilityListener(listener_), RET_OK);
    instance_->Init(channel_, CHANNEL_ID);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(CHANNEL_ID));
}

/**
 * @tc.number: Disconnect_001
 * @tc.name: Disconnect
 * @tc.desc: Test function Disconnect
 */
HWTEST_F(AccessibleAbilityClientImplTest, Disconnect_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Disconnect_001 start";
    Connect();
    instance_->Disconnect(CHANNEL_ID);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(INVALID_CHANNEL_ID));
    GTEST_LOG_(INFO) << "Disconnect_001 end";
}

/**
 * @tc.number: OnAccessibilityEvent_001
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientImplTest, OnAccessibilityEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnAccessibilityEvent_001 start";
    Connect();
    AccessibilityEventInfo eventInfo {};
    eventInfo.SetEventType(EventType::TYPE_TOUCH_BEGIN);
    instance_->OnAccessibilityEvent(eventInfo);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestEventType(),
        static_cast<int>(EventType::TYPE_TOUCH_BEGIN));
    GTEST_LOG_(INFO) << "OnAccessibilityEvent_001 end";
}

/**
 * @tc.number: OnAccessibilityEvent_002
 * @tc.name: OnAccessibilityEvent
 * @tc.desc: Test function OnAccessibilityEvent
 */
HWTEST_F(AccessibleAbilityClientImplTest, OnAccessibilityEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnAccessibilityEvent_002 start";
    AccessibilityEventInfo eventInfo;
    eventInfo.SetEventType(EventType::TYPE_GESTURE_EVENT);
    instance_->OnAccessibilityEvent(eventInfo);
    EXPECT_NE(AccessibilityAbilityHelper::GetInstance().GetTestEventType(), static_cast<int>(TYPE_GESTURE_EVENT));
    GTEST_LOG_(INFO) << "OnAccessibilityEvent_002 end";
}

/**
 * @tc.number: GetFocus_001
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_001 start";
    Connect();
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE_INPUT, info), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetFocus_001 end";
}

/**
 * @tc.number: GetFocus_002
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocus_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_002 start";
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE_INVALID, info), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "GetFocus_002 end";
}

/**
 * @tc.number: GetFocus_003
 * @tc.name: GetFocus
 * @tc.desc: Test function GetFocus
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocus_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocus_003 start";
    AccessibilityElementInfo info {};
    EXPECT_EQ(instance_->GetFocus(FOCUS_TYPE_INPUT, info), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetFocus_003 end";
}

/**
 * @tc.number: GetRemoteObject_001
 * @tc.name: GetRemoteObject
 * @tc.desc: Test function GetRemoteObject
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetRemoteObject_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRemoteObject_001 start";
    EXPECT_TRUE(AccessibleAbilityClient::GetInstance()->GetRemoteObject());
    GTEST_LOG_(INFO) << "GetRemoteObject_001 end";
}

/**
 * @tc.number: RegisterAbilityListener_001
 * @tc.name: RegisterAbilityListener
 * @tc.desc: Test function RegisterAbilityListener
 */
HWTEST_F(AccessibleAbilityClientImplTest, RegisterAbilityListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 start";
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_OK);
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_ERR_REGISTER_EXIST);
    GTEST_LOG_(INFO) << "RegisterAbilityListener_001 end";
}

/**
 * @tc.number: Init_001
 * @tc.name: Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibleAbilityClientImplTest, Init_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Init_001 start";
    sptr<IAccessibleAbilityChannel> channel = nullptr;
    instance_->Init(channel, CHANNEL_ID);
    EXPECT_NE(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(CHANNEL_ID));
    GTEST_LOG_(INFO) << "Init_001 end";
}

/**
 * @tc.number: Init_002
 * @tc.name: Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibleAbilityClientImplTest, Init_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Init_002 start";
    std::shared_ptr<AccessibleAbilityListener> listener = std::make_shared<MockAccessibleAbilityListener>();
    EXPECT_EQ(instance_->RegisterAbilityListener(listener), RET_OK);

    sptr<MockAccessibleAbilityChannelStub> stub = new MockAccessibleAbilityChannelStub();
    sptr<IAccessibleAbilityChannel> channel = new MockAccessibleAbilityChannelProxy(stub->AsObject());
    instance_->Init(channel, CHANNEL_ID);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(CHANNEL_ID));
    GTEST_LOG_(INFO) << "Init_002 end";
}

/**
 * @tc.number: Init_003
 * @tc.name: Init
 * @tc.desc: Test function Init
 */
HWTEST_F(AccessibleAbilityClientImplTest, Init_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Init_003 start";
    sptr<MockAccessibleAbilityChannelStub> stub = new MockAccessibleAbilityChannelStub();
    sptr<IAccessibleAbilityChannel> channel = new MockAccessibleAbilityChannelProxy(stub->AsObject());
    instance_->Init(channel, CHANNEL_ID);
    EXPECT_NE(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(CHANNEL_ID));
    GTEST_LOG_(INFO) << "Init_003 end";
}

/**
 * @tc.number: GetFocusByElementInfo_001
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocusByElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 start";
    Connect();
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE_INPUT, elementInfo), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_001 end";
}

/**
 * @tc.number: GetFocusByElementInfo_002
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocusByElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 start";
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE_INVALID, elementInfo), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_002 end";
}

/**
 * @tc.number: GetFocusByElementInfo_003
 * @tc.name: GetFocusByElementInfo
 * @tc.desc: Test function GetFocusByElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetFocusByElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_003 start";
    AccessibilityElementInfo sourceInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetFocusByElementInfo(sourceInfo, FOCUS_TYPE_INPUT, elementInfo), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetFocusByElementInfo_003 end";
}

/**
 * @tc.number: InjectGesture_001
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture
 */
HWTEST_F(AccessibleAbilityClientImplTest, InjectGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_001 start";
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "InjectGesture_001 end";
}

/**
 * @tc.number: InjectGesture_002
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture
 */
HWTEST_F(AccessibleAbilityClientImplTest, InjectGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_002 start";
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = nullptr;
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "InjectGesture_002 end";
}

/**
 * @tc.number: InjectGesture_003
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture
 */
HWTEST_F(AccessibleAbilityClientImplTest, InjectGesture_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_003 start";
    EXPECT_CALL(*stub_, SendSimulateGesture(_)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    AccessibilityGesturePosition position;
    gesturePath->AddPosition(position);
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_OK);
    GTEST_LOG_(INFO) << "InjectGesture_003 end";
}

/**
 * @tc.number: InjectGesture_004
 * @tc.name: InjectGesture
 * @tc.desc: Test function InjectGesture
 */
HWTEST_F(AccessibleAbilityClientImplTest, InjectGesture_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "InjectGesture_004 start";
    std::shared_ptr<AccessibilityGestureInjectPath> gesturePath = std::make_shared<AccessibilityGestureInjectPath>();
    AccessibilityGesturePosition position;
    gesturePath->AddPosition(position);
    EXPECT_EQ(instance_->InjectGesture(gesturePath), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "InjectGesture_004 end";
}

/**
 * @tc.number: GetRoot_001
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetRoot_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_001 start";
    Connect();
    AccessibilityElementInfo info {};
    instance_->SetCacheMode(0);
    EXPECT_EQ(instance_->GetRoot(info), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetRoot_001 end";
}

/**
 * @tc.number: GetRoot_002
 * @tc.name: GetRoot
 * @tc.desc: Test function GetRoot
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetRoot_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRoot_002 start";
    AccessibilityElementInfo info {};
    instance_->SetCacheMode(INVALID_ID);
    EXPECT_EQ(instance_->GetRoot(info), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetRoot_002 end";
}

/**
 * @tc.number: GetRootByWindow_001
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetRootByWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_001 start";
    Connect();
    AccessibilityElementInfo info {};
    AccessibilityWindowInfo windowInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, info), RET_ERR_TIME_OUT);

    GTEST_LOG_(INFO) << "GetRootByWindow_001 end";
}

/**
 * @tc.number: GetRootByWindow_002
 * @tc.name: GetRootByWindow
 * @tc.desc: Test function GetRootByWindow
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetRootByWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetRootByWindow_002 start";
    AccessibilityElementInfo info {};
    AccessibilityWindowInfo windowInfo {};
    EXPECT_EQ(instance_->GetRootByWindow(windowInfo, info), RET_ERR_NO_CONNECTION);

    GTEST_LOG_(INFO) << "GetRootByWindow_002 end";
}

/**
 * @tc.number: GetNext_001
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_001 start";
    Connect();
    AccessibilityElementInfo info {};
    AccessibilityElementInfo nextElementInfo {};
    FocusMoveDirection direction = DIRECTION_INVALID;
    EXPECT_EQ(instance_->GetNext(info, direction, nextElementInfo), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "GetNext_001 end";
}

/**
 * @tc.number: GetNext_002
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetNext_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_002 start";
    AccessibilityElementInfo info {};
    AccessibilityElementInfo nextElementInfo {};
    FocusMoveDirection direction = UP;
    EXPECT_EQ(instance_->GetNext(info, direction, nextElementInfo), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetNext_002 end";
}

/**
 * @tc.number: GetNext_003
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetNext_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetNext_003 start";
    Connect();
    AccessibilityElementInfo info {};
    AccessibilityElementInfo nextElementInfo {};
    FocusMoveDirection direction = DOWN;
    EXPECT_EQ(instance_->GetNext(info, direction, nextElementInfo), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetNext_003 end";
}

/**
 * @tc.number: GetByContent_001
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetByContent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_001 start";
    Connect();
    AccessibilityElementInfo elementInfo {};
    std::vector<AccessibilityElementInfo> inelementInfosfos;
    EXPECT_EQ(instance_->GetByContent(elementInfo, TEST, inelementInfosfos), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetByContent_001 end";
}

/**
 * @tc.number: GetByContent_002
 * @tc.name: GetByContent
 * @tc.desc: Test function GetByContent
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetByContent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetByContent_002 start";
    AccessibilityElementInfo elementInfo {};
    std::vector<AccessibilityElementInfo> inelementInfosfos;
    EXPECT_EQ(instance_->GetByContent(elementInfo, TEST, inelementInfosfos), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetByContent_002 end";
}

/**
 * @tc.number: GetSource_001
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetSource_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_001 start";
    Connect();
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetSource_001 end";
}

/**
 * @tc.number: GetSource_002
 * @tc.name: GetSource
 * @tc.desc: Test function GetSource
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetSource_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSource_002 start";
    AccessibilityEventInfo eventInfo {};
    AccessibilityElementInfo elementInfo {};
    EXPECT_EQ(instance_->GetSource(eventInfo, elementInfo), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetSource_002 end";
}

/**
 * @tc.number: GetParentElementInfo_001
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetParentElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_001 start";
    Connect();
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetParentElementInfo_001 end";
}

/**
 * @tc.number: GetParentElementInfo_002
 * @tc.name: GetParentElementInfo
 * @tc.desc: Test function GetParentElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetParentElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetParentElementInfo_002 start";
    AccessibilityElementInfo parent {};
    AccessibilityElementInfo child {};
    EXPECT_EQ(instance_->GetParentElementInfo(child, parent), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetParentElementInfo_002 end";
}

/**
 * @tc.number: ExecuteAction_001
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityClientImplTest, ExecuteAction_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_001 start";
    Connect();
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_INVALID;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "ExecuteAction_001 end";
}

/**
 * @tc.number: ExecuteAction_002
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityClientImplTest, ExecuteAction_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_002 start";
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_SELECT;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "ExecuteAction_002 end";
}

/**
 * @tc.number: ExecuteAction_003
 * @tc.name: ExecuteAction
 * @tc.desc: Test function ExecuteAction
 */
HWTEST_F(AccessibleAbilityClientImplTest, ExecuteAction_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ExecuteAction_003 start";
    Connect();
    AccessibilityElementInfo elementInfo {};
    ActionType action = ACCESSIBILITY_ACTION_SELECT;
    std::map<std::string, std::string> actionArguments {};
    EXPECT_EQ(instance_->ExecuteAction(elementInfo, action, actionArguments), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "ExecuteAction_003 end";
}

/**
 * @tc.number: ResetAAClient_001
 * @tc.name: ResetAAClient
 * @tc.desc: Test function ResetAAClient
 */
HWTEST_F(AccessibleAbilityClientImplTest, ResetAAClient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ResetAAClient_001 start";
    EXPECT_CALL(*stub_, GetWindows(_)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    wptr<IRemoteObject> remote = nullptr;
    instance_->ResetAAClient(remote);
    std::vector<AccessibilityWindowInfo> infos;
    EXPECT_EQ(instance_->GetWindows(infos), RET_OK);
    GTEST_LOG_(INFO) << "ResetAAClient_001 end";
}

/**
 * @tc.number: OnKeyPressEvent_001
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientImplTest, OnKeyPressEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnKeyPressEvent_001 start";
    Connect();
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(1);
    instance_->OnKeyPressEvent(*event, SEQUENCE);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestKeyPressEvent(), 1);
    GTEST_LOG_(INFO) << "OnKeyPressEvent_001 end";
}

/**
 * @tc.number: OnKeyPressEvent_002
 * @tc.name: OnKeyPressEvent
 * @tc.desc: Test function OnKeyPressEvent
 */
HWTEST_F(AccessibleAbilityClientImplTest, OnKeyPressEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnKeyPressEvent_002 start";
    std::shared_ptr<MMI::KeyEvent> event = MMI::KeyEvent::Create();
    event->SetKeyCode(1);
    instance_->OnKeyPressEvent(*event, SEQUENCE);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestKeyPressEvent(), INVALID_ID);
    GTEST_LOG_(INFO) << "OnKeyPressEvent_002 end";
}

/**
 * @tc.number: GetWindows_001
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindows_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_001 start";
    EXPECT_CALL(*stub_, GetWindows(_)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    std::vector<AccessibilityWindowInfo> infos;
    EXPECT_EQ(instance_->GetWindows(infos), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_001 end";
}

/**
 * @tc.number: GetWindows_002
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindows_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_002 start";
    EXPECT_CALL(*stub_, GetWindowsByDisplayId(_, _)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    std::vector<AccessibilityWindowInfo> infos;
    EXPECT_EQ(instance_->GetWindows(0, infos), RET_OK);
    GTEST_LOG_(INFO) << "GetWindows_002 end";
}

/**
 * @tc.number: GetWindows_003
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindows_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_003 start";
    std::vector<AccessibilityWindowInfo> infos;
    EXPECT_EQ(instance_->GetWindows(infos), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetWindows_003 end";
}

/**
 * @tc.number: GetWindows_004
 * @tc.name: GetWindows
 * @tc.desc: Test function GetWindows
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindows_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindows_004 start";
    std::vector<AccessibilityWindowInfo> infos;
    EXPECT_EQ(instance_->GetWindows(0, infos), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetWindows_004 end";
}

/**
 * @tc.number: GetChildElementInfo_001
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildElementInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_001 start";
    AccessibilityElementInfo parent;
    AccessibilityElementInfo child;
    EXPECT_EQ(instance_->GetChildElementInfo(0, parent, child), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetChildElementInfo_001 end";
}

/**
 * @tc.number: GetChildElementInfo_002
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildElementInfo_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_002 start";
    Connect();
    AccessibilityElementInfo parent;
    AccessibilityElementInfo child;
    EXPECT_EQ(instance_->GetChildElementInfo(0, parent, child), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "GetChildElementInfo_002 end";
}

/**
 * @tc.number: GetChildElementInfo_003
 * @tc.name: GetChildElementInfo
 * @tc.desc: Test function GetChildElementInfo
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildElementInfo_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildElementInfo_003 start";
    Connect();

    AccessibilityElementInfo parent;
    parent.AddChild(1);
    AccessibilityElementInfo child;
    EXPECT_EQ(instance_->GetChildElementInfo(0, parent, child), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetChildElementInfo_003 end";
}

/**
 * @tc.number: GetChildren_001
 * @tc.name: GetChildren
 * @tc.desc: Test function GetChildren
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildren_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildren_001 start";
    AccessibilityElementInfo parent;
    std::vector<AccessibilityElementInfo> children;
    EXPECT_EQ(instance_->GetChildren(parent, children), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetChildren_001 end";
}

/**
 * @tc.number: GetChildren_002
 * @tc.name: GetChildren
 * @tc.desc: Test function GetChildren
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildren_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildren_002 start";
    Connect();

    AccessibilityElementInfo parent;
    parent.AddChild(INVALID_CHILD_ID);
    std::vector<AccessibilityElementInfo> children;
    EXPECT_EQ(instance_->GetChildren(parent, children), RET_ERR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "GetChildren_002 end";
}

/**
 * @tc.number: GetChildren_003
 * @tc.name: GetChildren
 * @tc.desc: Test function GetChildren
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetChildren_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetChildren_003 start";
    Connect();

    AccessibilityElementInfo parent;
    parent.AddChild(1);
    std::vector<AccessibilityElementInfo> children;
    EXPECT_EQ(instance_->GetChildren(parent, children), RET_ERR_TIME_OUT);
    GTEST_LOG_(INFO) << "GetChildren_003 end";
}

/**
 * @tc.number: SetTargetBundleName_001
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityClientImplTest, SetTargetBundleName_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 start";
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "SetTargetBundleName_001 end";
}

/**
 * @tc.number: SetTargetBundleName_002
 * @tc.name: SetTargetBundleName
 * @tc.desc: Test function SetTargetBundleName
 */
HWTEST_F(AccessibleAbilityClientImplTest, SetTargetBundleName_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 start";
    EXPECT_CALL(*stub_, SetTargetBundleName(_)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    std::vector<std::string> targetBundleNames;
    EXPECT_EQ(instance_->SetTargetBundleName(targetBundleNames), RET_OK);
    GTEST_LOG_(INFO) << "SetTargetBundleName_002 end";
}

/**
 * @tc.number: GetWindow_001
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindow_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_001 start";
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(instance_->GetWindow(0, windowInfo), RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "GetWindow_001 end";
}

/**
 * @tc.number: GetWindow_002
 * @tc.name: GetWindow
 * @tc.desc: Test function GetWindow
 */
HWTEST_F(AccessibleAbilityClientImplTest, GetWindow_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetWindow_002 start";
    EXPECT_CALL(*stub_, GetWindow(_, _)).Times(1).WillOnce(Return(RET_OK));
    Connect();
    AccessibilityWindowInfo windowInfo;
    EXPECT_EQ(instance_->GetWindow(0, windowInfo), RET_OK);
    GTEST_LOG_(INFO) << "GetWindow_002 end";
}

/**
 * @tc.number: NotifyServiceDied_001
 * @tc.name: NotifyServiceDied
 * @tc.desc: Test function NotifyServiceDied
 */
HWTEST_F(AccessibleAbilityClientImplTest, NotifyServiceDied_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyServiceDied_001 start";
    Connect();
    wptr<IRemoteObject> remote = nullptr;
    instance_->NotifyServiceDied(remote);
    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTestChannelId(), static_cast<int>(CHANNEL_ID));
    GTEST_LOG_(INFO) << "NotifyServiceDied_001 end";
}
} // namespace Accessibility
} // namespace OHOS