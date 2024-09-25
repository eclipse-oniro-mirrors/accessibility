/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include <map>
#include <memory>
#include "accessibility_common_helper.h"
#include "accessibility_screen_touch.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t CLICK_RESPONSE_TIME_SHORT = 0; // ms
    constexpr uint32_t CLICK_RESPONSE_TIME_MEDIUM = 300; // ms
    constexpr uint32_t CLICK_RESPONSE_TIME_LONG = 600; // ms
    constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_SHORTEST = 100; // ms
    constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_MEDIUM = 700; // ms
    constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_LONGEST = 1300; // ms
    constexpr uint32_t CLICK_RESPONSE_DELAY_SHORT = 0;
    constexpr uint32_t CLICK_RESPONSE_DELAY_MEDIUM = 1;
    constexpr uint32_t CLICK_RESPONSE_DELAY_LONG = 2;
    constexpr uint32_t IGNORE_REPEAT_CLICK_SHORTEST = 0;
    constexpr uint32_t IGNORE_REPEAT_CLICK_MEDIUM = 2;
    constexpr uint32_t IGNORE_REPEAT_CLICK_LONGEST = 4;
    constexpr uint32_t TIMESTAMP_800 = 800;
    constexpr uint32_t TIMESTAMP_1200 = 1200;
    constexpr uint32_t TIMESTAMP_1500 = 1500;
    constexpr uint32_t TIMESTAMP_1600 = 1600;
    constexpr uint32_t TIMESTAMP_1700 = 1700;
    constexpr uint32_t TIMESTAMP_2500 = 2500;
    constexpr uint32_t TIMESTAMP_2700 = 2700;
    constexpr uint32_t TIMESTAMP_2800 = 2800;
    constexpr uint32_t SLEEP_TIME_MS = 500;
} // namespace

class AccessibilityScreenTouchUnitTest : public ::testing::Test {
public:
    AccessibilityScreenTouchUnitTest()
    {}
    ~AccessibilityScreenTouchUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<MMI::PointerEvent> SetPointerEvent(uint32_t time, uint32_t action);
    std::shared_ptr<MMI::PointerEvent> SetPointerEvent(uint32_t time, uint32_t action,
        std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId);

    std::shared_ptr<AccessibilityScreenTouch> screenTouch_ = nullptr;
    static uint32_t lastUpTime_;
};

uint32_t AccessibilityScreenTouchUnitTest::lastUpTime_ = 0;

void AccessibilityScreenTouchUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityScreenTouchUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityScreenTouchUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityScreenTouchUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityScreenTouchUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
}

void AccessibilityScreenTouchUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    screenTouch_ = nullptr;
    AccessibilityAbilityHelper::GetInstance().ClearTouchEventActionVector();

    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }
    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
}

std::shared_ptr<MMI::PointerEvent> AccessibilityScreenTouchUnitTest::SetPointerEvent(uint32_t time, uint32_t action)
{
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(action);
    event->SetPointerId(0);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(0);
    event->AddPointerItem(item);
    event->SetActionTime(time);
    return event;
}

std::shared_ptr<MMI::PointerEvent> AccessibilityScreenTouchUnitTest::SetPointerEvent(uint32_t time, uint32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int32_t pointerId)
{
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();

    for (auto &point : points) {
        if (point.GetPointerId() == pointerId) {
            point.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
        } else {
            point.SetPressed(true);
        }
        event->AddPointerItem(point);
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(action);
    event->SetPointerId(pointerId);
    event->SetActionTime(time);
    return event;
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetClickResponseTime_001
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealClickResponseTime(), CLICK_RESPONSE_TIME_SHORT);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetClickResponseTime_002
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetClickResponseTime_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealClickResponseTime(), CLICK_RESPONSE_TIME_MEDIUM);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetClickResponseTime_003
 * @tc.name: SetClickResponseTime
 * @tc.desc: Test function SetClickResponseTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetClickResponseTime_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealClickResponseTime(), CLICK_RESPONSE_TIME_LONG);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetClickResponseTime_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_001
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickState(), true);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_002
 * @tc.name: SetIgnoreRepeatClickState
 * @tc.desc: Test function SetIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickState(), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickState_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_001
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_SHORTEST);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickTime(), IGNORE_REPEAT_CLICK_TIME_SHORTEST);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_002
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_MEDIUM);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickTime(), IGNORE_REPEAT_CLICK_TIME_MEDIUM);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_003
 * @tc.name: SetIgnoreRepeatClickTime
 * @tc.desc: Test function SetIgnoreRepeatClickTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickTime(), IGNORE_REPEAT_CLICK_TIME_LONGEST);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SetIgnoreRepeatClickTime_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_001 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(0);
    event->SetPointerId(0);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);   
    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), true);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_002 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(0);
    event->SetPointerId(0);
    event->AddPointerItem(pointer); 
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_003 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);   
    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_004 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    pointer.SetToolType(MMI::PointerEvent::TOOL_TYPE_KNUCKLE);
    event->SetPointerId(1);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);

    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_004 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_005
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_005 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    pointer.SetToolType(MMI::PointerEvent::TOOL_TYPE_KNUCKLE);
    event->SetPointerId(2);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);

    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_005 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_006
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_006 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    pointer.SetToolType(MMI::PointerEvent::TOOL_TYPE_KNUCKLE);
    event->SetPointerId(1);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);

    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), false);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_006 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_007
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_007 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    event->SetPointerId(1);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    screenTouch_->OnPointerEvent(*event);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_007 end.";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_008
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_008 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(1);
    event->SetPointerId(2);
    event->AddPointerItem(pointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    screenTouch_->OnPointerEvent(*event);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_008 end.";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_009
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_009 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (screenTouch_ == nullptr || event == nullptr) {
        GTEST_LOG_(INFO) << "null pointer";
    }

    MMI::PointerEvent::PointerItem pointer = {};
    pointer.SetPointerId(0);
    MMI::PointerEvent::PointerItem anotherPointer = {};
    anotherPointer.SetPointerId(1);
    event->SetPointerId(1);
    event->AddPointerItem(pointer);
    event->AddPointerItem(anotherPointer);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);
    screenTouch_->OnPointerEvent(*event);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_009 end.";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_001
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ = TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_002
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_003
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_SHORTEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_004
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_004 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_004 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_005
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_005,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_005 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_005 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_006
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_006,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_006 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_006 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_007
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_007,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_007 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    auto eventDownContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDownContinue);

    auto eventMoveContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMoveContinue);

    auto eventUpContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2800) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUpContinue);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_007 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_008
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_008,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_008 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    auto eventDownContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDownContinue);

    auto eventMoveContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMoveContinue);

    auto eventUpContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_2800) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUpContinue);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 6);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(4);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(5);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_008 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_009
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_009,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_009 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    auto eventDownContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDownContinue);

    auto eventMoveContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMoveContinue);

    auto eventUpContinue = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUpContinue);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_009 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_010
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_010,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_010 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 6);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(5);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_010 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_011
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_011,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_011 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 6);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_011 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_012
 * @tc.name: HandleIgnoreRepeatClickState
 * @tc.desc: Test function HandleIgnoreRepeatClickState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_012,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_012 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1200) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleIgnoreRepeatClickState_012 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_001
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    sleep(1);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_002
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    sleep(1);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_003
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    sleep(1);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 2);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_004
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_004 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    usleep(SLEEP_TIME_MS);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_004 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_005
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_005,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_005 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    usleep(SLEEP_TIME_MS);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_005 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_006
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_006,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_006 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    usleep(SLEEP_TIME_MS);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 0);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_006 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_007
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_007,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_007 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(0);
    point.SetDisplayX(0);
    point.SetDisplayY(0);
    points.emplace_back(point);
    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*eventDown);

    points.clear();
    point.SetDisplayX(500);
    point.SetDisplayY(500);
    points.emplace_back(point);
    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 0);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_007 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_008
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_008,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_008 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(0);
    point.SetDisplayX(0);
    point.SetDisplayY(0);
    points.emplace_back(point);
    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*eventDown);

    points.clear();
    point.SetDisplayX(500);
    point.SetDisplayY(500);
    points.emplace_back(point);
    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 0);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_008 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_009
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_009,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_009 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point = {};
    point.SetPointerId(0);
    point.SetDisplayX(0);
    point.SetDisplayY(0);
    points.emplace_back(point);
    auto eventDown = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*eventDown);

    points.clear();
    point.SetDisplayX(500);
    point.SetDisplayY(500);
    points.emplace_back(point);
    auto eventMove = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 0);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_009 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_010
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_010,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_010 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 4);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_010 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_011
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_011,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_011 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 6);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(4);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(5);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_011 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_012
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_012,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_012 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 4);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_012 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_013
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_013,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_013 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    sleep(1);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_013 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_014
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_014,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_014 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_SHORT);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    sleep(1);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 6);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(4);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_014 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleResponseDelayState_015
 * @tc.name: HandleResponseDelayState
 * @tc.desc: Test function HandleResponseDelayState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleResponseDelayState_015,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_015 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent(TIMESTAMP_1500 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    sleep(1);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent(TIMESTAMP_1600 * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    auto secondUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent(TIMESTAMP_1700 * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleResponseDelayState_015 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_001
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    sleep(1);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_002
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_002 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_MEDIUM);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    sleep(1);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_002 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_003
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_003 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_MEDIUM);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    auto eventDown = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    sleep(1);

    auto eventMove = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS, MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*eventUp);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 3);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(2);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_003 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_004
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_004 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 4);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_004 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_005
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_005 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_MEDIUM);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 4);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_005 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_HandleBothState_006
 * @tc.name: HandleBothState
 * @tc.desc: Test function HandleBothState
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_HandleBothState_006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_006 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (accountData == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_MEDIUM);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_MEDIUM);

    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (screenTouch_ == nullptr) {
        return;
    }

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem firstPoint = {};
    firstPoint.SetPointerId(0);
    points.emplace_back(firstPoint);
    auto firstDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0);
    screenTouch_->OnPointerEvent(*firstDownEvent);

    auto firstMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1500) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0);
    screenTouch_->OnPointerEvent(*firstMoveEvent);

    MMI::PointerEvent::PointerItem secondPoint = {};
    secondPoint.SetPointerId(1);
    points.emplace_back(secondPoint);
    auto secondDownEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_DOWN, points, 1);
    screenTouch_->OnPointerEvent(*secondDownEvent);

    auto secondMoveEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1600) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_MOVE, points, 1);
    screenTouch_->OnPointerEvent(*secondMoveEvent);

    sleep(1);

    auto secondUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP, points, 1);
    screenTouch_->OnPointerEvent(*secondUpEvent);

    auto firstUpEvent = SetPointerEvent((lastUpTime_ + TIMESTAMP_1700) * US_TO_MS,
        MMI::PointerEvent::POINTER_ACTION_UP);
    screenTouch_->OnPointerEvent(*firstUpEvent);

    EXPECT_EQ(AccessibilityAbilityHelper::GetInstance().GetTouchEventActionVector().size(), 4);
    int32_t touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(0);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(1);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_DOWN);
    touchAction = AccessibilityAbilityHelper::GetInstance().GetTouchEventActionOfTargetIndex(3);
    EXPECT_EQ(touchAction, MMI::PointerEvent::POINTER_ACTION_UP);

    lastUpTime_ += TIMESTAMP_1700;

    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_HandleBothState_006 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_GetRealClickResponseTime_001
 * @tc.name: GetRealClickResponseTime
 * @tc.desc: Test function GetRealClickResponseTime
 */
HWTEST_F(AccessibilityScreenTouchUnitTest,
    AccessibilityScreenTouch_Unittest_GetRealClickResponseTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_GetRealClickResponseTime_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(900);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealClickResponseTime(), 0);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_GetRealClickResponseTime_001 end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_GetRealIgnoreRepeatClickTime_001
 * @tc.name: GetRealIgnoreRepeatClickTime
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest,
    AccessibilityScreenTouch_Unittest_GetRealIgnoreRepeatClickTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_GetRealIgnoreRepeatClickTime_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetIgnoreRepeatClickTime(2000);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickTime(), IGNORE_REPEAT_CLICK_TIME_SHORTEST);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_GetRealIgnoreRepeatClickTime end";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_SendInterceptedEvent_001
 * @tc.name: SendInterceptedEvent
 * @tc.desc: Test function SendInterceptedEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_SendInterceptedEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SendInterceptedEvent_001 start";
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    screenTouch_->SendInterceptedEvent();
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_SendInterceptedEvent_001 end";
}
} // namespace Accessibility
} // namespace OHOS