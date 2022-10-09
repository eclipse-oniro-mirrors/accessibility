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
#include <map>
#include <memory>
#include "accessibility_common_helper.h"
#include "accessibility_mouse_autoclick.h"
#include "accessibility_ut_helper.h"
#include "accessible_ability_manager_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t DELAY_TIME = 100;
} // namespace

class AccessibilityMouseAutoclickUnitTest : public ::testing::Test {
public:
    AccessibilityMouseAutoclickUnitTest()
    {}
    ~AccessibilityMouseAutoclickUnitTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AccessibilityMouseAutoclick> mouseAutoclick_ = nullptr;
};

void AccessibilityMouseAutoclickUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityMouseAutoclickUnitTest Start ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
}

void AccessibilityMouseAutoclickUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### AccessibilityMouseAutoclickUnitTest End ######################";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityMouseAutoclickUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "SetUp";
    mouseAutoclick_ = std::make_shared<AccessibilityMouseAutoclick>();
}

void AccessibilityMouseAutoclickUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "TearDown";
    mouseAutoclick_ = nullptr;
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(1);
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    
    MMI::PointerEvent::PointerItem item1;
    item1.SetPointerId(1);
    item1.SetDisplayX(1);
    item1.SetDisplayY(1);
    event->UpdatePointerItem(1, item1);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_002 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        return;
    }
    accountData->GetConfig()->SetMouseAutoClick(DELAY_TIME);
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    event->SetPointerId(1);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(1);
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);
    mouseAutoclick_->OnPointerEvent(*event);
    sleep(2);

    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_003 end";
}

/**
 * @tc.number: AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 * @tc.require: issueI5NTXC
 */
HWTEST_F(AccessibilityMouseAutoclickUnitTest, AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004 start";
    if (!mouseAutoclick_) {
        return;
    }
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    if (!event) {
        return;
    }
    event->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    event->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
    MMI::PointerEvent::PointerItem item;
    event->AddPointerItem(item);
    mouseAutoclick_->OnPointerEvent(*event);

    GTEST_LOG_(INFO) << "AccessibilityMouseAutoclick_Unittest_OnPointerEvent_004 end";
}
} // namespace Accessibility
} // namespace OHOS