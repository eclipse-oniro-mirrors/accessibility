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
    constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_SHORTEST = 100; // ms
    constexpr uint32_t CLICK_RESPONSE_TIME_LONG = 600; // ms
    constexpr uint32_t IGNORE_REPEAT_CLICK_TIME_LONGEST = 1300; // ms
    constexpr uint32_t CLICK_RESPONSE_DELAY_LONG = 2;
    constexpr uint32_t IGNORE_REPEAT_CLICK_LONGEST = 4;
    constexpr uint32_t TIMESTAMP_800 = 800;
    constexpr uint32_t TIMESTAMP_1200 = 1200;
    constexpr uint32_t TIMESTAMP_1500 = 1500;
    constexpr uint32_t TIMESTAMP_1600 = 1600;
    constexpr uint32_t TIMESTAMP_1700 = 1700;
    constexpr uint32_t TIMESTAMP_2700 = 2700;
    constexpr uint32_t TIMESTAMP_2800 = 2800;
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

    std::shared_ptr<AccessibilityScreenTouch> screenTouch_ = nullptr;
};

void AccessibilityScreenTouchUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityScreenTouchUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
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

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_001 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();

    EXPECT_EQ(screenTouch_->GetRealClickResponseTime(), CLICK_RESPONSE_TIME_LONG);
    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickTime(), IGNORE_REPEAT_CLICK_TIME_LONGEST);
    EXPECT_EQ(screenTouch_->GetRealIgnoreRepeatClickState(), true);
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
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_CANCEL);

    EXPECT_EQ(screenTouch_->OnPointerEvent(*event), true);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_003 end.";
}

/**
 * @tc.number: AccessibilityScreenTouch_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(AccessibilityScreenTouchUnitTest, AccessibilityScreenTouch_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_004 start";
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (accountData == nullptr || screenTouch_ == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    auto eventDown = SetPointerEvent(TIMESTAMP_1500, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700, MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(screenTouch_->OnPointerEvent(*eventUp), true);
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
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (accountData == nullptr || screenTouch_ == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(0);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    auto eventDown = SetPointerEvent(TIMESTAMP_1200, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove = SetPointerEvent(TIMESTAMP_1600, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700, MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(screenTouch_->OnPointerEvent(*eventUp), true);
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
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (accountData == nullptr || screenTouch_ == nullptr) {
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(false);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    auto eventDown = SetPointerEvent(0, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove1 = SetPointerEvent(TIMESTAMP_800, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove1);

    auto eventMove2 = SetPointerEvent(TIMESTAMP_1600, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove2);

    auto eventUp = SetPointerEvent(TIMESTAMP_1700, MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(screenTouch_->OnPointerEvent(*eventUp), true);
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
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    screenTouch_ = std::make_shared<AccessibilityScreenTouch>();
    if (accountData == nullptr || screenTouch_ == nullptr) {
        GTEST_LOG_(INFO) << "accountData is nullptr or screenTouch_ is nullptr";
        return;
    }

    accountData->GetConfig()->SetClickResponseTime(CLICK_RESPONSE_DELAY_LONG);
    accountData->GetConfig()->SetIgnoreRepeatClickState(true);
    accountData->GetConfig()->SetIgnoreRepeatClickTime(IGNORE_REPEAT_CLICK_LONGEST);

    auto eventDown = SetPointerEvent(TIMESTAMP_1600, MMI::PointerEvent::POINTER_ACTION_DOWN);
    screenTouch_->OnPointerEvent(*eventDown);

    auto eventMove1 = SetPointerEvent(TIMESTAMP_1700, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove1);

    auto eventMove2 = SetPointerEvent(TIMESTAMP_2700, MMI::PointerEvent::POINTER_ACTION_MOVE);
    screenTouch_->OnPointerEvent(*eventMove2);

    auto eventUp = SetPointerEvent(TIMESTAMP_2800, MMI::PointerEvent::POINTER_ACTION_UP);
    EXPECT_EQ(screenTouch_->OnPointerEvent(*eventUp), true);
    GTEST_LOG_(INFO) << "AccessibilityScreenTouch_Unittest_OnPointerEvent_007 end";
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
} // namespace Accessibility
} // namespace OHOS