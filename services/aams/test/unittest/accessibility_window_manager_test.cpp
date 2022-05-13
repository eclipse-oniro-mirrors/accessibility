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
#include "accessibility_ability_helper.h"
#include "accessibility_account_data.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessible_ability_manager_service.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_accessibility_element_operator_stub.h"
#include "mock_bundle_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowManagerTest : public testing::Test {
public:
    AccessibilityWindowManagerTest()
    {}
    ~AccessibilityWindowManagerTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

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
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityAbilityHelper::GetInstance().WaitForServicePublish();
}

void AccessibilityWindowManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest TearDown";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ConvertToRealWindowId001
 * @tc.name: ConvertToRealWindowId
 * @tc.desc: Test function ConvertToRealWindowId
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ConvertToRealWindowId001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId001 start";
    int32_t windowId = ACTIVE_WINDOW_ID;
    int32_t focusType = 0;
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    int32_t realId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.activeWindowId_, realId);
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
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_ACCESSIBILITY;
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    int32_t realId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.a11yFocusedWindowId_, realId);
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
    int32_t windowId = ANY_WINDOW_ID;
    int32_t focusType = FOCUS_TYPE_INPUT;
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    int32_t realId = mgr.ConvertToRealWindowId(windowId, focusType);

    EXPECT_EQ(mgr.activeWindowId_, realId);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ConvertToRealWindowId003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001 start";
    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::APP_MAIN_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);

    EXPECT_EQ(TYPE_APPLICATION, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002 start";
    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::WINDOW_TYPE_LAUNCHER_RECENT;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);

    EXPECT_EQ(TYPE_SYSTEM, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003
 * @tc.name: CreateAccessibilityWindowInfo
 * @tc.desc: Test function CreateAccessibilityWindowInfo
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003 start";

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = (OHOS::Rosen::WindowType)0xFFFFFFFF;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);

    EXPECT_EQ(TYPE_WINDOW_INVALID, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_RegisterWindowListener001
 * @tc.name: RegisterWindowListener
 * @tc.desc: Test function RegisterWindowListener
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_RegisterWindowListener001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_RegisterWindowListener001 start";

    Singleton<AccessibilityWindowManager>::GetInstance().RegisterWindowListener(nullptr);

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_RegisterWindowListener001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_DeregisterWindowListener001
 * @tc.name: RegisterWindowListener
 * @tc.desc: Test function RegisterWindowListener
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_DeregisterWindowListener001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowListener001 start";

    Singleton<AccessibilityWindowManager>::GetInstance().DeregisterWindowListener();

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowListener001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange001
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange001 start";

    sptr<AccessibilityElementOperatorStub> stub = new MockAccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy = new AccessibilityElementOperatorProxy(stub);
    /* add asacConnections */
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    ASSERT_TRUE(accountData);
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperator(1, proxy);
    auto map = accountData->GetAsacConnections();
    GTEST_LOG_(INFO) << "RegisterElementOperator OK";

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::APP_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();

    if (!windowInfoManager.a11yWindows_.empty()) {
        GTEST_LOG_(INFO) << "a11yWindows_ is empty";
        windowInfoManager.a11yWindows_.clear();
    }
    /* OnWindowChange */
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());

    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
    windowInfoManager.a11yWindows_.insert(std::make_pair(rosen_winInfo.wid_, info));
    windowInfoManager.OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(rosen_winInfo.wid_));
    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperator(0);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange002
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowchange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange002 start";

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    mgr.activeWindowId_ = INVALID_WINDOW_ID;
    mgr.a11yWindows_.clear();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
    int id = 2;
    EXPECT_EQ(0, mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_EQ(1, mgr.a11yWindows_.size());

    std::map<int32_t, AccessibilityWindowInfo> map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id == wid */
    rosen_winInfo.wid_ = 2;
    mgr.OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);

    EXPECT_TRUE(mgr.activeWindowId_ == INVALID_WINDOW_ID);
    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange003
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy003 start";

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
    int id = 2;
    EXPECT_TRUE(!mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int32_t, AccessibilityWindowInfo> map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id != wid != INVALID_WINDOW_ID */
    rosen_winInfo.wid_ = 1;
    mgr.OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange004
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowDestroy003 start";

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    mgr.activeWindowId_ = INVALID_WINDOW_ID;
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
    int id = -1;
    EXPECT_TRUE(!mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int32_t, AccessibilityWindowInfo> map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id == wid == INVALID_WINDOW_ID */
    rosen_winInfo.wid_ = -1;
    mgr.OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    EXPECT_TRUE(mgr.activeWindowId_ == INVALID_WINDOW_ID);
    mgr.a11yWindows_.clear();
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
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int32_t windowId = INVALID_WINDOW_ID;
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
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int32_t windowId = INVALID_WINDOW_ID;
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
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    int32_t windowId = ACTIVE_WINDOW_ID;
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
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    int32_t windowId = ANY_WINDOW_ID;
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int32_t windowId = INVALID_WINDOW_ID;
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int32_t windowId = INVALID_WINDOW_ID;
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info;
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ANY_WINDOW_ID, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    /* SetAccessibilityFocusedWindow */
    int32_t windowId = ACTIVE_WINDOW_ID;
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.a11yFocusedWindowId_ = ANY_WINDOW_ID;
    int32_t windowId = ACTIVE_WINDOW_ID;
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
 * @tc.number: AccessibilityWindowManager_Unittest_GetAccessibilityWindows001
 * @tc.name: GetAccessibilityWindows
 * @tc.desc: Test function GetAccessibilityWindows
 */
HWTEST_F(
    AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindows001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindows001 start";
    /* map insert value */
    int32_t windowId = ANY_WINDOW_ID;

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
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
    int32_t windowId = ANY_WINDOW_ID;

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
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
    int32_t windowId = ANY_WINDOW_ID;

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
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
    int32_t windowId = ANY_WINDOW_ID;

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
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
    int32_t windowId = ANY_WINDOW_ID;

    Rosen::WindowInfo rosen_winInfo;
    rosen_winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    rosen_winInfo.wid_ = 1;
    rosen_winInfo.windowRect_.width_ = 1;
    rosen_winInfo.windowRect_.height_ = 1;
    rosen_winInfo.windowRect_.posX_ = 1;
    rosen_winInfo.windowRect_.posY_ = 1;
    rosen_winInfo.focused_ = true;

    sptr<Rosen::AccessibilityWindowInfo> winInfo = new Rosen::AccessibilityWindowInfo();
    winInfo->currentWindowInfo_ = &rosen_winInfo;

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo->currentWindowInfo_);
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
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    int32_t windowId = ANY_WINDOW_ID;
    AccessibilityWindowInfo info;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* SetWindowSize */
    Rect rect(1, 2, 3, 4);
    mgr.SetWindowSize(windowId, rect);
    AccessibilityWindowInfo mapInfo;
    for (auto& window : mgr.a11yWindows_) {
        mapInfo = window.second;
    }
    EXPECT_EQ(rect.GetLeftTopXScreenPostion(), mapInfo.GetRectInScreen().GetLeftTopXScreenPostion());
    EXPECT_EQ(rect.GetRightBottomXScreenPostion(), mapInfo.GetRectInScreen().GetRightBottomXScreenPostion());

    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetWindowSize001 end";
}
} // namespace Accessibility
} // namespace OHOS