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
#include "accessibility_account_data.h"
#include "accessibility_common_helper.h"
#include "accessibility_element_operator_proxy.h"
#include "accessibility_ut_helper.h"
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
namespace {
    constexpr int RETRY_TIMES = 10;
    constexpr int WINDOW_ID = 2;
    constexpr int WINDOWS_SIZE = 2;
    constexpr int SEND_EVENT_TIMES = 2;
    constexpr int32_t ACCOUNT_ID = 100;
} // namespace

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
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
}

void AccessibilityWindowManagerTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest TearDownTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
}

void AccessibilityWindowManagerTest::SetUp()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest SetUp";
    AccessibilityAbilityHelper::GetInstance().ClearSendEventTimes();
}

void AccessibilityWindowManagerTest::TearDown()
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManagerTest TearDown";
}

sptr<Rosen::AccessibilityWindowInfo> GetRosenWindowInfo(Rosen::WindowType windowType)
{
    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    if (rosen_winInfo != nullptr) {
        rosen_winInfo->type_ = windowType;
        rosen_winInfo->wid_ = 1;
        rosen_winInfo->windowRect_.width_ = 1;
        rosen_winInfo->windowRect_.height_ = 1;
        rosen_winInfo->windowRect_.posX_ = 1;
        rosen_winInfo->windowRect_.posY_ = 1;
        rosen_winInfo->focused_ = true;
    }
    return rosen_winInfo;
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
    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(Rosen::WindowType::APP_MAIN_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);
    
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(rosen_winInfo);

    EXPECT_EQ(TYPE_APPLICATION, info.GetAccessibilityWindowType());
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
    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(rosen_winInfo);

    EXPECT_EQ(TYPE_SYSTEM, info.GetAccessibilityWindowType());
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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        static_cast<OHOS::Rosen::WindowType>(0xFFFFFFFF));
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = windowInfoManager.CreateAccessibilityWindowInfo(rosen_winInfo);

    EXPECT_EQ(TYPE_WINDOW_INVALID, info.GetAccessibilityWindowType());
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
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange001
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange001 start";
    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(Rosen::WindowType::APP_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);
    rosen_winInfo->focused_ = false;

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());

    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    int retryCount = 0;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        int counts = AccessibilityAbilityHelper::GetInstance().GetSendEventTimes();
        GTEST_LOG_(INFO) << "The counts of send event is : " << counts;
        if (counts == 1) {
            GTEST_LOG_(INFO) << "update window successful";
            break;
        }
        retryCount++;
    }
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(rosen_winInfo->wid_));
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
    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();;
    EXPECT_TRUE(rosen_winInfo != nullptr);
    rosen_winInfo->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    rosen_winInfo->wid_ = 1;
    rosen_winInfo->focused_ = true;

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());
    EXPECT_EQ(windowInfoManager.activeWindowId_, INVALID_WINDOW_ID);

    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    int retryCount = 0;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        int counts = AccessibilityAbilityHelper::GetInstance().GetSendEventTimes();
        GTEST_LOG_(INFO) << "The counts of send event is : " << counts;
        if (counts == 1) {
            GTEST_LOG_(INFO) << "update window successful";
            break;
        }
        retryCount++;
    }
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(rosen_winInfo->wid_));
    EXPECT_EQ(windowInfoManager.activeWindowId_, rosen_winInfo->wid_);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange003
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange003 start";
    // Clear window data
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());

    // Add a window
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfoFirst = new Rosen::AccessibilityWindowInfo();
    rosenWinInfoFirst->type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    rosenWinInfoFirst->wid_ = 1;
    rosenWinInfoFirst->focused_ = false;
    Rosen::Rect rect;
    rect.posX_ = 1;
    rect.posY_ = 1;
    rect.width_ = 1;
    rect.height_ = 1;
    rosenWinInfoFirst->windowRect_ = rect;
    rosenWinInfoFirst->mode_ = Rosen::WindowMode::WINDOW_MODE_PIP;
    rosenWinInfoFirst->isDecorEnable_ = false;
    rosenWinInfoFirst->displayId_ = 0;
    windowInfoManager.a11yWindows_.insert(std::make_pair(rosenWinInfoFirst->wid_,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfoFirst)));
    EXPECT_EQ(windowInfoManager.a11yWindows_.size(), 1);

    // Add another window
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfoSecond = new Rosen::AccessibilityWindowInfo();
    rosenWinInfoSecond->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    rosenWinInfoSecond->wid_ = WINDOW_ID;
    rosenWinInfoSecond->focused_ = false;
    rosenWinInfoSecond->windowRect_ = rect;
    rosenWinInfoSecond->mode_ = Rosen::WindowMode::WINDOW_MODE_PIP;
    rosenWinInfoSecond->isDecorEnable_ = false;
    rosenWinInfoSecond->displayId_ = 0;
    windowInfoManager.a11yWindows_.insert(std::make_pair(rosenWinInfoSecond->wid_,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfoSecond)));
    EXPECT_EQ(windowInfoManager.a11yWindows_.size(), WINDOWS_SIZE);

    // Remove the first window
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosenWinInfoFirst);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);

    int retryCount = 0;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        int counts = AccessibilityAbilityHelper::GetInstance().GetSendEventTimes();
        GTEST_LOG_(INFO) << "The counts of send event is : " << counts;
        if (counts == 1) {
            GTEST_LOG_(INFO) << "update window successful";
            break;
        }
        retryCount++;
    }
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_FALSE(windowInfoManager.a11yWindows_.count(rosenWinInfoFirst->wid_));
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(rosenWinInfoSecond->wid_));
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange003 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange004
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange004 start";
    // Clear window data
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());
    EXPECT_EQ(windowInfoManager.activeWindowId_, INVALID_WINDOW_ID);

    // Add an active window
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfoFirst = new Rosen::AccessibilityWindowInfo();
    rosenWinInfoFirst->type_ = Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
    rosenWinInfoFirst->wid_ = 1;
    rosenWinInfoFirst->focused_ = false;
    Rosen::Rect rect;
    rect.posX_ = 1;
    rect.posY_ = 1;
    rect.width_ = 1;
    rect.height_ = 1;
    rosenWinInfoFirst->windowRect_ = rect;
    rosenWinInfoFirst->mode_ = Rosen::WindowMode::WINDOW_MODE_PIP;
    rosenWinInfoFirst->isDecorEnable_ = false;
    rosenWinInfoFirst->displayId_ = 0;
    windowInfoManager.a11yWindows_.insert(std::make_pair(rosenWinInfoFirst->wid_,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfoFirst)));
    EXPECT_EQ(windowInfoManager.a11yWindows_.size(), 1);
    windowInfoManager.SetActiveWindow(rosenWinInfoFirst->wid_);
    EXPECT_EQ(windowInfoManager.activeWindowId_, rosenWinInfoFirst->wid_);

    // Add another normal window
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfoSecond = new Rosen::AccessibilityWindowInfo();
    rosenWinInfoSecond->type_ = Rosen::WindowType::APP_WINDOW_BASE;
    rosenWinInfoSecond->wid_ = WINDOW_ID;
    rosenWinInfoSecond->focused_ = false;
    rosenWinInfoSecond->windowRect_ = rect;
    rosenWinInfoSecond->mode_ = Rosen::WindowMode::WINDOW_MODE_PIP;
    rosenWinInfoSecond->isDecorEnable_ = false;
    rosenWinInfoSecond->displayId_ = 0;
    windowInfoManager.a11yWindows_.insert(std::make_pair(rosenWinInfoSecond->wid_,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfoSecond)));
    EXPECT_EQ(windowInfoManager.a11yWindows_.size(), WINDOWS_SIZE);

    // Remove the active window
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosenWinInfoFirst);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);

    int retryCount = 0;
    while (retryCount < RETRY_TIMES) {
        sleep(1);
        int counts = AccessibilityAbilityHelper::GetInstance().GetSendEventTimes();
        GTEST_LOG_(INFO) << "The counts of send event is : " << counts;
        if (counts == SEND_EVENT_TIMES) {
            GTEST_LOG_(INFO) << "update window successful";
            break;
        }
        retryCount++;
    }
    EXPECT_TRUE(windowInfoManager.a11yWindows_.size() == 1);
    EXPECT_FALSE(windowInfoManager.a11yWindows_.count(rosenWinInfoFirst->wid_));
    EXPECT_TRUE(windowInfoManager.a11yWindows_.count(rosenWinInfoSecond->wid_));
    EXPECT_EQ(windowInfoManager.activeWindowId_, INVALID_WINDOW_ID);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange004 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange005
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateBounds fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange005 start";

    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS);
    sleep(1);
    EXPECT_EQ(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED,
        AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange005 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange006
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateBounds success)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange006 start";

    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    infos.emplace_back(winInfo);
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType() == WINDOW_UPDATE_BOUNDS) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange006 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange007
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateFocused fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange007 start";

    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    sleep(1);
    EXPECT_EQ(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED,
        AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange007 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange008
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateFocused success(a11yWindows_ is not null))
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange008 start";

    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED);
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    // Add a window
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = 1;
    windowInfoManager.a11yWindows_.insert(std::make_pair(1,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(windowInfoManager.a11yWindows_.size(), 1);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = 1;
    infos.emplace_back(winInfo);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType() == WINDOW_UPDATE_FOCUSED) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange008 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange009
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateFocused success(a11yWindows_ is null))
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange009 start";

    AccessibilityAbilityHelper::GetInstance().SetEventWindowChangeType(WINDOW_UPDATE_ACCESSIBILITY_FOCUSED);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = 1;
    infos.emplace_back(winInfo);
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType() == WINDOW_UPDATE_FOCUSED) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange009 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange010
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateProperty fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange010 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    sleep(1);
    EXPECT_TRUE(!windowInfoManager.a11yWindows_[WINDOW_ID].IsFocused());
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange010 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange011
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateProperty fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange011 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = 1;
    winInfo->focused_ = true;
    infos.emplace_back(winInfo);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    sleep(1);
    EXPECT_TRUE(!windowInfoManager.a11yWindows_[WINDOW_ID].IsFocused());
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange011 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange012
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateProperty success)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange012, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange012 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = WINDOW_ID;
    winInfo->focused_ = true;
    infos.emplace_back(winInfo);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (Singleton<AccessibilityWindowManager>::GetInstance().a11yWindows_[WINDOW_ID].IsFocused()) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange012 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange013
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(size of parameter(infos) is 0)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange013, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange013 start";

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    EXPECT_EQ(0, windowInfoManager.a11yWindows_.size());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange013 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange014
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateAdded fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange014, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange014 start";

    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    windowInfoManager.a11yWindows_.clear();
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    sleep(1);
    EXPECT_EQ(0, windowInfoManager.a11yWindows_.size());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange014 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange015
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateAdded success)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange015, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange015 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo1 = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo1->wid_ = WINDOW_ID;
    rosenWinInfo1->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo1)));

    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo2 = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo2->wid_ = 1;
    rosenWinInfo2->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(1,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo2)));
    windowInfoManager.activeWindowId_ = 1;

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = WINDOW_ID;
    winInfo->focused_ = true;
    infos.emplace_back(winInfo);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType() == WINDOW_UPDATE_ACTIVE) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange015 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange016
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateRemoved fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange016, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange016 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    sleep(1);
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange016 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange017
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateRemoved success)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange017, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange017 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());
    windowInfoManager.a11yFocusedWindowId_ = WINDOW_ID;

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    sptr<Rosen::AccessibilityWindowInfo> winInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    winInfo->wid_ = WINDOW_ID;
    winInfo->focused_ = true;
    infos.emplace_back(winInfo);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED);
    bool ret = AccessibilityCommonHelper::GetInstance().WaitForLoop(std::bind([]() -> bool {
        if (AccessibilityAbilityHelper::GetInstance().GetEventWindowChangeType() == WINDOW_UPDATE_REMOVED) {
            return true;
        } else {
            return false;
        }
        }), 1);
    EXPECT_TRUE(ret);
    EXPECT_EQ(0, windowInfoManager.a11yWindows_.size());
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange017 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_OnWindowChange018
 * @tc.name: OnWindowChange
 * @tc.desc: Test function OnWindowChange(WindowUpdateActive fail)
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_OnWindowChange018, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange018 start";

    // Add window
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    sptr<Rosen::AccessibilityWindowInfo> rosenWinInfo = new Rosen::AccessibilityWindowInfo();
    rosenWinInfo->wid_ = WINDOW_ID;
    rosenWinInfo->focused_ = false;
    windowInfoManager.a11yWindows_.insert(std::make_pair(WINDOW_ID,
        windowInfoManager.CreateAccessibilityWindowInfo(rosenWinInfo)));
    EXPECT_EQ(1, windowInfoManager.a11yWindows_.size());
    windowInfoManager.activeWindowId_ = WINDOW_ID;

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(nullptr);
    windowInfoManager.OnWindowUpdate(infos, Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    sleep(1);
    EXPECT_EQ(WINDOW_ID, windowInfoManager.activeWindowId_);
    windowInfoManager.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_OnWindowChange018 end";
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
 * @tc.number: AccessibilityWindowManager_Unittest_SetActiveWindow005
 * @tc.name: SetActiveWindow
 * @tc.desc: Test function SetActiveWindow
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_SetActiveWindow005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow005 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info1;
    AccessibilityWindowInfo info2;
    mgr.activeWindowId_ = ACTIVE_WINDOW_ID;
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    int32_t windowId = ANY_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info1));
    mgr.a11yWindows_.insert(std::make_pair(windowId, info2));
    EXPECT_EQ(2, (int)mgr.a11yWindows_.size());
    /* SetActiveWindow */
    mgr.SetActiveWindow(windowId);
    /* test */
    EXPECT_EQ(INVALID_WINDOW_ID, mgr.a11yFocusedWindowId_);
    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_SetActiveWindow005 end";
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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(rosen_winInfo);
    info.SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(windowId, info));
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());

    /* SetAccessibilityFocusedWindow */
    mgr.SetAccessibilityFocusedWindow(windowId);

    /* GetAccessibilityWindows */
    std::vector<AccessibilityWindowInfo> windows = mgr.GetAccessibilityWindows();
    ASSERT_EQ(1, (int)windows.size());
    static Accessibility::AccessibilityWindowType type = windows.begin()->GetAccessibilityWindowType();
    EXPECT_EQ(AccessibilityWindowType::TYPE_APPLICATION, type);

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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(rosen_winInfo);
    info.SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(rosen_winInfo);
    info.SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(rosen_winInfo);
    info.SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
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

    sptr<Rosen::AccessibilityWindowInfo> rosen_winInfo = GetRosenWindowInfo(
        Rosen::WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_TRUE(rosen_winInfo != nullptr);

    std::vector<sptr<Rosen::AccessibilityWindowInfo>> infos;
    infos.emplace_back(rosen_winInfo);

    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info = mgr.CreateAccessibilityWindowInfo(rosen_winInfo);
    info.SetAccessibilityWindowType(AccessibilityWindowType::TYPE_APPLICATION);
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

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ClearAccessibilityFocused001
 * @tc.name: ClearAccessibilityFocused
 * @tc.desc: Test function ClearAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ClearAccessibilityFocused001,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused001 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info1;
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info1));

    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(true);
    /* ClearAccessibilityFocused */
    mgr.ClearAccessibilityFocused();
    AccessibilityAbilityHelper::GetInstance().SetNeedAccountDataNullFlag(false);
    /* test */
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused001 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ClearAccessibilityFocused002
 * @tc.name: ClearAccessibilityFocused
 * @tc.desc: Test function ClearAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ClearAccessibilityFocused002,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused002 start";
    /* map insert value */
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    AccessibilityWindowInfo info1;
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    EXPECT_EQ(0, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.insert(std::make_pair(ACTIVE_WINDOW_ID, info1));
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    sptr<AccessibilityWindowConnection> windowConnection =
        new(std::nothrow) AccessibilityWindowConnection(ACTIVE_WINDOW_ID, nullptr, ACCOUNT_ID);
    accountData->RemoveAccessibilityWindowConnection(ACTIVE_WINDOW_ID);
    accountData->AddAccessibilityWindowConnection(ACTIVE_WINDOW_ID, windowConnection);
    /* ClearAccessibilityFocused */
    mgr.ClearAccessibilityFocused();
    /* test */
    EXPECT_EQ(1, (int)mgr.a11yWindows_.size());
    mgr.a11yWindows_.clear();
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused002 end";
}

/**
 * @tc.number: AccessibilityWindowManager_Unittest_ClearAccessibilityFocused003
 * @tc.name: ClearAccessibilityFocused
 * @tc.desc: Test function ClearAccessibilityFocused
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_ClearAccessibilityFocused003,
    TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused003 start";
    AccessibilityWindowManager& mgr = Singleton<AccessibilityWindowManager>::GetInstance();
    mgr.a11yFocusedWindowId_ = ACTIVE_WINDOW_ID;
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        GTEST_LOG_(INFO) << "accountData is null";
        return;
    }
    sptr<IAccessibilityElementOperator> proxy = new(std::nothrow) AccessibilityElementOperatorProxy(nullptr);
    sptr<AccessibilityWindowConnection> windowConnection =
        new(std::nothrow) AccessibilityWindowConnection(ACTIVE_WINDOW_ID, proxy, ACCOUNT_ID);
    accountData->RemoveAccessibilityWindowConnection(ACTIVE_WINDOW_ID);
    accountData->AddAccessibilityWindowConnection(ACTIVE_WINDOW_ID, windowConnection);
    /* ClearAccessibilityFocused */
    mgr.ClearAccessibilityFocused();
    /* test */
    EXPECT_EQ(ACTIVE_WINDOW_ID, AccessibilityAbilityHelper::GetInstance().GetEventWindowId());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_ClearAccessibilityFocused003 end";
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
 * @tc.number: AccessibilityWindowManager_Unittest_DeInit001
 * @tc.name: DeInit
 * @tc.desc: Test function DeInit
 */
HWTEST_F(AccessibilityWindowManagerTest, AccessibilityWindowManager_Unittest_DeInit001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeInit001 start";
    AccessibilityWindowManager& windowInfoManager = Singleton<AccessibilityWindowManager>::GetInstance();
    int32_t windowId = WINDOW_ID;
    AccessibilityWindowInfo winInfo;

    windowInfoManager.activeWindowId_ = windowId;
    windowInfoManager.a11yFocusedWindowId_ = windowId;
    windowInfoManager.a11yWindows_.emplace(windowId, winInfo);
    windowInfoManager.DeInit();

    EXPECT_EQ(windowInfoManager.activeWindowId_, INVALID_WINDOW_ID);
    EXPECT_EQ(windowInfoManager.a11yFocusedWindowId_, INVALID_WINDOW_ID);
    EXPECT_TRUE(!windowInfoManager.a11yWindows_.size());
    GTEST_LOG_(INFO) << "AccessibilityWindowManager_Unittest_DeInit001 end";
}
} // namespace Accessibility
} // namespace OHOS