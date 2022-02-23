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

#include "accessibility_window_manager.h"

#include <gtest/gtest.h>

#include "accessibility_account_data.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_operator_stub.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_bundle_manager.h"
#include "system_ability_definition.h"

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
    shared_ptr<OHOS::Accessibility::AccessibleAbilityManagerService> ins_ = nullptr;
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
    mock_ = nullptr;
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::APP_MAIN_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;

    AccessibilityWindowInfoManager& windowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo);

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
    Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::WINDOW_TYPE_LAUNCHER_RECENT;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;

    AccessibilityWindowInfoManager& windowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo);

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
    Rosen::WindowInfo winInfo;
    winInfo.type_ = (OHOS::Rosen::WindowType)0xFFFFFFFF;//Rosen::WindowType::WINDOW_TYPE_MEDIA;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;

    AccessibilityWindowInfoManager& windowInfoManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(winInfo);

    EXPECT_EQ(TYPE_WINDOW_INVALID, info.GetWindowType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_CreateAccessibilityWindowInfo003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001
 * @tc.name: RegisterWindowChangeListener
 * @tc.desc: Test function RegisterWindowChangeListener
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_RegisterWindowChangeListener001,
    TestSize.Level1)
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001 start";

    AccessibilityWindowInfoManager::GetInstance().DeregisterWindowChangeListener();

    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeregisterWindowChangeListener001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange001
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange001 start";

    EXPECT_CALL(*mock_, GetUidByBundleName(_, _)).WillRepeatedly(Return(0));
    sptr<AccessibilityElementOperatorStub> stub = new AccessibilityElementOperatorStub();
    sptr<IAccessibilityElementOperator> proxy =
        new AccessibilityElementOperatorProxy(stub);
    /* add asacConnections */
    sptr<AccessibilityAccountData> accountData = ins_->GetCurrentAccountData();
    ins_->RegisterElementOperator(1, proxy, 0);
    auto map = accountData->GetAsacConnections();
    GTEST_LOG_(INFO) << "RegisterElementOperator OK";
    EXPECT_EQ(1, int(map.size()));

    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    int32_t wid = 1;
    winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    winInfo->wid_ = wid;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;
    AccessibilityWindowInfoManager& windowInfoManager = AccessibilityWindowInfoManager::GetInstance();

    if (!windowInfoManager.a11yWindows_.empty()) {
        GTEST_LOG_(INFO) << "a11yWindows_ is empty";
        windowInfoManager.a11yWindows_.clear();
    }
    /* OnWindowChange */
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 0);

    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(*winInfo);
    windowInfoManager.a11yWindows_.insert(std::make_pair(winInfo->wid_, info));
    windowInfoManager.windowListener_->OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(wid));
    ins_->DeregisterElementOperator(0);
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

    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    winInfo->type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    winInfo->wid_ = 1;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;

    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    mgr.activeWindowId_ = INVALID_WINDOW_ID;
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(*winInfo);
    int id = 2;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id == wid */
    winInfo->wid_ = 2;
    mgr.windowListener_->OnWindowUpdate(winInfo, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);

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

    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    winInfo->type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    winInfo->wid_ = 1;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;

    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(*winInfo);
    int id = 2;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id != wid != INVALID_WINDOW_ID */
    winInfo->wid_ = 1;
    mgr.windowListener_->OnWindowUpdate(winInfo,
                       Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
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

    sptr<Rosen::WindowInfo> winInfo = new Rosen::WindowInfo();
    winInfo->type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    winInfo->wid_ = 1;
    winInfo->windowRect_.width_ = 1;
    winInfo->windowRect_.height_ = 1;
    winInfo->windowRect_.posX_ = 1;
    winInfo->windowRect_.posY_ = 1;
    winInfo->focused_ = true;

    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    mgr.activeWindowId_ = INVALID_WINDOW_ID;
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(*winInfo);
    int id = -1;
    EXPECT_TRUE(mgr.a11yWindows_.size() == 0);
    mgr.a11yWindows_.insert(std::make_pair(id, info));
    EXPECT_TRUE(mgr.a11yWindows_.size() == 1);

    std::map<int, AccessibilityWindowInfo>  map = mgr.a11yWindows_;
    for (auto it = map.begin(); it != map.end(); it++) {
        EXPECT_EQ(2, it->second.GetWindowType());
    }

    /* id == wid == INVALID_WINDOW_ID */
    winInfo->wid_ = -1;
    mgr.windowListener_->OnWindowUpdate(winInfo,
                       Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow001 start";
    /* map insert value */
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow002 start";
    /* map insert value */
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow003 start";
    /* map insert value */
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetAccessibilityFocusedWindow004 start";
    /* map insert value */
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetInputFocusedWindoww003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetInputFocusedWindow003 start";
    /* map insert value */
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindows001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindows001 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo);
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindow001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow001 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo);
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
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_GetAccessibilityWindow002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_GetAccessibilityWindow002 start";
    /* map insert value */
    int windowId = ANY_WINDOW_ID;
    OHOS::Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo);
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
    OHOS::Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo);
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
    OHOS::Rosen::WindowInfo winInfo;
    winInfo.type_ = Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE;
    winInfo.wid_ = 1;
    winInfo.windowRect_.width_ = 1;
    winInfo.windowRect_.height_ = 1;
    winInfo.windowRect_.posX_ = 1;
    winInfo.windowRect_.posY_ = 1;
    winInfo.focused_ = true;
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(winInfo);
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
    AccessibilityWindowInfoManager& mgr = AccessibilityWindowInfoManager::GetInstance();
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