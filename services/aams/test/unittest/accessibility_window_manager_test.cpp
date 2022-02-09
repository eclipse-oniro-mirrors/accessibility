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

#include <gtest/gtest.h>
#include "accessibility_window_manager.h"
#include "accessibility_account_data.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_interaction_operation_proxy.h"
#include "accessible_ability_client_stub_impl.h"
#include "mock_bundle_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "dummy.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {

class AccessibilityWindowManagerTest : public testing::Test {
public:

    AccessibilityWindowManagerTest() {}
    ~AccessibilityWindowManagerTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<OHOS::AppExecFwk::BundleMgrService> mock_ = nullptr;
};

static shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_;

void AccessibilityWindowManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest SetUpTestCase";
}

void AccessibilityWindowManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest TearDownTestCase";
}

void AccessibilityWindowManagerTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest SetUp";
    /* register bundleService */
    mock_ = new OHOS::AppExecFwk::BundleMgrService();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::ISystemAbilityManager::SAExtraProp saExtraProp;
    systemAbilityManager->AddSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, mock_, saExtraProp);
    /* aams OnStart */
    ins_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    ins_->OnStart();
}

void AccessibilityWindowManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest TearDown";
    delete mock_;
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ConvertToRealWindowId001
 * @tc.name: ConvertToRealWindowId
 * @tc.desc: Test function ConvertToRealWindowId
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ConvertToRealWindowId001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId001 start";
    int windowId = ACTIVE_WINDOW_ID;
    int focusType = 0;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    int realWindowId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.activeWindowId_, realWindowId);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ConvertToRealWindowId002
 * @tc.name: ConvertToRealWindowId
 * @tc.desc: Test function ConvertToRealWindowId
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ConvertToRealWindowId002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId002 start";
    int windowId = ANY_WINDOW_ID;
    int focusType = FOCUS_TYPE_ACCESSIBILITY;
    auto a11yWindowMgr = AccessibilityWindowInfoManager::GetInstance();
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    int realWindowId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.a11yFocusedWindowId_, realWindowId);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ConvertToRealWindowId003
 * @tc.name: ConvertToRealWindowId
 * @tc.desc: Test function ConvertToRealWindowId
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ConvertToRealWindowId003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId003 start";
    int windowId = ANY_WINDOW_ID;
    int focusType = FOCUS_TYPE_INPUT;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    int realWindowId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.inputFocusedWindowId_, realWindowId);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001 start";
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_NORMAL;
    result.wid = 1;
    result.zorder = 1;
    result.width = 1;
    result.height = 1;
    result.positionX = 1;
    result.positionY = 1;
    result.anchorX = 1;
    result.anchorY = 1;
    result.isFocused = true;

    auto WindowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = WindowInfoManager.CreateAccessibilityWindowInfo(result);

    EXPECT_EQ(TYPE_APPLICATION, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002 start";
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_LAUNCHER;

    auto WindowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = WindowInfoManager.CreateAccessibilityWindowInfo(result);

    EXPECT_EQ(TYPE_SYSTEM, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003 start";
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_VIDEO;

    auto WindowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = WindowInfoManager.CreateAccessibilityWindowInfo(result);

    EXPECT_EQ(TYPE_WINDOW_INVALID, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001
 * @tc.name: RegisterWindowChangeListener
 * @tc.desc: Test function RegisterWindowChangeListener
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001 start";

    AccessibilityWindowInfoManager::GetInstance().RegisterWindowChangeListener();

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001
 * @tc.name: RegisterWindowChangeListener
 * @tc.desc: Test function RegisterWindowChangeListener
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001 start";

    AccessibilityWindowInfoManager::GetInstance().DeregisterWindowChangeListener();

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowCreate001
 * @tc.name: OnWindowCreate
 * @tc.desc: Test function OnWindowCreate
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowCreate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowCreate001 start";

    EXPECT_CALL(*mock_, GetUidByBundleName(_, _)).WillRepeatedly(Return(0));
    /* add asacConnections */
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    auto map = accountData->GetAsacConnections();
    EXPECT_EQ(0, int(map.size()));
    ins_->RegisterInteractionOperation(0,nullptr, 0);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation OK";
    map = accountData->GetAsacConnections();
    EXPECT_EQ(1, int(map.size()));

    auto WindowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    int32_t wid = 0;

    if (!WindowInfoManager.a11yWindows_.empty()) {
        GTEST_LOG_(INFO) << "a11yWindows_ is empty";
        WindowInfoManager.a11yWindows_.clear();
    }
    /* OnWindowCreate */
    EXPECT_TRUE(WindowInfoManager.a11yWindows_.size() == 0);
    WindowInfoManager.OnWindowCreate(wid);
    EXPECT_TRUE(WindowInfoManager.a11yWindows_.size() == 1);
    EXPECT_TRUE(WindowInfoManager.a11yWindows_.count(wid));
    ins_->DeregisterInteractionOperation(0);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowCreate001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowDestroy001
 * @tc.name: OnWindowDestroy
 * @tc.desc: Test function OnWindowDestroy
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowDestroy001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy001 start";

    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_NORMAL;

    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    int id = 2;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2,it->second.GetWindowType());
    }

    /* id == wid */
    int wid = 2;
    mgr.OnWindowDestroy(wid);

    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowDestroy002
 * @tc.name: OnWindowDestroy
 * @tc.desc: Test function OnWindowDestroy
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowDestroy002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy002 start";

    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_NORMAL;

    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    int id = 2;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2,it->second.GetWindowType());
    }

    /* id != wid != INVALID_WINDOW_ID */
    int wid = 1;
    mgr.OnWindowDestroy(wid);
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowDestroy003
 * @tc.name: OnWindowDestroy
 * @tc.desc: Test function OnWindowDestroy
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowDestroy003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy003 start";

    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_NORMAL;

    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    int id = -1;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2,it->second.GetWindowType());
    }

    /* id == wid == INVALID_WINDOW_ID */
    int wid = -1;
    mgr.OnWindowDestroy(wid);
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetActiveWindow001
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetActiveWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow001 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetActiveWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.activeWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetActiveWindow002
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetActiveWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow002 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetActiveWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ACTIVE_WINDOW_ID].IsActive();
    EXPECT_FALSE(test2);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.activeWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetActiveWindow003
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetActiveWindow003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow003 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int windowId = ACTIVE_WINDOW_ID;
    mgr.SetActiveWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(windowId)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(ACTIVE_WINDOW_ID, mgr.activeWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetActiveWindow004
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetActiveWindow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow004 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    int windowId = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info1));
    mgr.a11yWindows_.insert(std::make_pair(windowId, info2));
    EXPECT_EQ(2, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    mgr.SetActiveWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ACTIVE_WINDOW_ID].IsActive();
    EXPECT_FALSE(test2);
    EXPECT_EQ(ANY_WINDOW_ID, mgr.activeWindowId_);
    bool test3 = mgr.a11yWindows_[mgr.activeWindowId_].IsActive();
    EXPECT_TRUE(test3);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow004 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001
 * @tc.name: SetAccessibilityFocusedWindow
 * @tc.desc: Test function SetAccessibilityFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetAccessibilityFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ANY_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.a11yFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002
 * @tc.name: SetAccessibilityFocusedWindow
 * @tc.desc: Test function SetAccessibilityFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetAccessibilityFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ANY_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ANY_WINDOW_ID].IsAccessibilityFocused();
    EXPECT_FALSE(test2);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.a11yFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003
 * @tc.name: SetAccessibilityFocusedWindow
 * @tc.desc: Test function SetAccessibilityFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int windowId = ACTIVE_WINDOW_ID;
    mgr.SetAccessibilityFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(windowId)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(ACTIVE_WINDOW_ID, mgr.a11yFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004
 * @tc.name: SetAccessibilityFocusedWindow
 * @tc.desc: Test function SetAccessibilityFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    int windowId = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info1));
    mgr.a11yWindows_.insert(std::make_pair(windowId, info2));
    EXPECT_EQ(2, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    mgr.SetAccessibilityFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ANY_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ANY_WINDOW_ID].IsAccessibilityFocused();
    EXPECT_FALSE(test2);
    EXPECT_EQ(ACTIVE_WINDOW_ID, mgr.a11yFocusedWindowId_);
    bool test3 = mgr.a11yWindows_[mgr.a11yFocusedWindowId_].IsAccessibilityFocused();
    EXPECT_TRUE(test3);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetInputFocusedWindow001
 * @tc.name: SetInputFocusedWindow
 * @tc.desc: Test function SetInputFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetInputFocusedWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow001 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.inputFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetInputFocusedWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetInputFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.inputFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetInputFocusedWindow002
 * @tc.name: SetInputFocusedWindow
 * @tc.desc: Test function SetInputFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetInputFocusedWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow002 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.inputFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetInputFocusedWindow */
    int windowId = INVALID_WINDOW_ID;
    mgr.SetInputFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ACTIVE_WINDOW_ID].IsFocused();
    EXPECT_FALSE(test2);
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.inputFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetInputFocusedWindow003
 * @tc.name: SetInputFocusedWindow
 * @tc.desc: Test function SetInputFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetInputFocusedWindoww003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow003 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info;
    mgr.inputFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetInputFocusedWindow */
    int windowId = ACTIVE_WINDOW_ID;
    mgr.SetInputFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(windowId)) {
        test1 = true;
    }
    EXPECT_FALSE(test1);
    EXPECT_EQ(ACTIVE_WINDOW_ID, mgr.inputFocusedWindowId_);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetInputFocusedWindow004
 * @tc.name: SetInputFocusedWindow
 * @tc.desc: Test function SetInputFocusedWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetInputFocusedWindow004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow004 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.inputFocusedWindowId_ = ACTIVE_WINDOW_ID;
    int windowId = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info1));
    mgr.a11yWindows_.insert(std::make_pair(windowId, info2));
    EXPECT_EQ(2, (int)mgr.a11yWindows_.size());
    /* SetInputFocusedWindow */
    mgr.SetInputFocusedWindow(windowId);
    /* test */
    bool test1 = false;
    if (mgr.a11yWindows_.count(ACTIVE_WINDOW_ID)) {
        test1 = true;
    }
    EXPECT_TRUE(test1);
    bool test2 = mgr.a11yWindows_[ACTIVE_WINDOW_ID].IsFocused();
    EXPECT_FALSE(test2);
    EXPECT_EQ(ANY_WINDOW_ID, mgr.inputFocusedWindowId_);
    bool test3 = mgr.a11yWindows_[mgr.inputFocusedWindowId_].IsFocused();
    EXPECT_TRUE(test3);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow004 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_GetAccessibilityWindows001
 * @tc.name: GetAccessibilityWindows
 * @tc.desc: Test function GetAccessibilityWindows
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindows001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindows001 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_SYSTEM_UI;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* SetAccessibilityFocusedWindow */
    mgr.SetAccessibilityFocusedWindow(windowId);

    /* GetAccessibilityWindows */
    std::vector<AccessibilityWindowInfo> windows = mgr.GetAccessibilityWindows();
    EXPECT_EQ(1, (int)windows.size());
    static Accessibility::WindowType type = windows.begin()->GetWindowType();
    EXPECT_EQ(WindowType::TYPE_APPLICATION, type);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindows001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_GetAccessibilityWindow001
 * @tc.name: GetAccessibilityWindow
 * @tc.desc: Test function GetAccessibilityWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow001 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_SYSTEM_UI;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* GetAccessibilityWindow */
    bool window = mgr.GetAccessibilityWindow(windowId, info);
    EXPECT_TRUE(window);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_GetAccessibilityWindow002
 * @tc.name: GetAccessibilityWindow
 * @tc.desc: Test function GetAccessibilityWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow002 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_SYSTEM_UI;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* GetAccessibilityWindow */
    bool window = mgr.GetAccessibilityWindow(-1, info);
    EXPECT_FALSE(window);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_IsValidWindow001
 * @tc.name: IsValidWindow
 * @tc.desc: Test function IsValidWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_IsValidWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_IsValidWindow001 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_SYSTEM_UI;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* IsValidWindow */
    bool window = mgr.IsValidWindow(windowId);
    EXPECT_TRUE(window);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_IsValidWindow001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_IsValidWindow002
 * @tc.name: IsValidWindow
 * @tc.desc: Test function IsValidWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_IsValidWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_IsValidWindow002 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Accessibility::GetWindowInfoResult result;
    result.type = WMWindowType::WINDOW_TYPE_SYSTEM_UI;
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(result);
    info.SetWindowType(WindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* IsValidWindow */
    bool window = mgr.IsValidWindow(0);
    EXPECT_FALSE(window);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_IsValidWindow002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_SetWindowSize001
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetWindowSize001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetWindowSize001 start";
    /* map insert value */
    auto mgr = AccessibilityWindowInfoManager::GetInstance();
    int windowId = ANY_WINDOW_ID;
    AccessibilityWindowInfo info;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* SetWindowSize */
    Rect rect(1, 2, 3, 4);
    mgr.SetWindowSize(windowId, rect);
    AccessibilityWindowInfo mapInfo;
    for (auto &window : mgr.a11yWindows_) {
       mapInfo = window.second;
    }
    EXPECT_EQ(rect.GetLeftTopXScreenPostion(), mapInfo.GetRectInScreen().GetLeftTopXScreenPostion());
    EXPECT_EQ(rect.GetRightBottomXScreenPostion(), mapInfo.GetRectInScreen().GetRightBottomXScreenPostion());

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetWindowSize001 end";
}

}  // namespace Accessibility
}  // namespace OHOS
