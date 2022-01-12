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
#include "accessibility_system_ability_client.h"
#include "ability_context.h"
#include "accessible_ability_manager_service_proxy.h"
#include "accessibility_interaction_operation.h"
#include "accessible_ability_manager_service_state_stub.h"
#include "accessibility_interaction_operation_stub.h"
#include "accessibility_element_info.h"
#include "hilog_wrapper.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"

using namespace OHOS::AppExecFwk;


using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {

class AccessibilityInteractionOperationTest : public Accessibility::AccessibilityInteractionOperation{
public:
    AccessibilityInteractionOperationTest() {}
    virtual ~AccessibilityInteractionOperationTest() = default;
    void SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback, const int mode)override;
    void SearchElementInfosByText(const long elementId, const std::string &text,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void PerformAction(const long elementId, const int action, const std::map<std::string, std::string> actionArguments,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)override;
    void ClearFocus()override;
    void OutsideTouch()override;
};

void AccessibilityInteractionOperationTest::SearchElementInfoByAccessibilityId(const long elementId,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback, const int mode)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(100);
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    callback.SetSearchElementInfoByAccessibilityIdResult(infos,requestId);
}

void AccessibilityInteractionOperationTest::SearchElementInfosByText(const long elementId, const std::string &text,
    const int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(200);
    std::list<AccessibilityElementInfo> infos{};
    infos.push_back(info);
    callback.SetSearchElementInfoByTextResult(infos, requestId);
}

void AccessibilityInteractionOperationTest::FindFocusedElementInfo(const long elementId, const int focusType, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(300);
    callback.SetFindFocusedElementInfoResult(info, requestId);
}

void AccessibilityInteractionOperationTest::FocusMoveSearch(const long elementId, const int direction, const int requestId,
    Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(400);

    callback.SetFocusMoveSearchResult(info, requestId);
}

void AccessibilityInteractionOperationTest::PerformAction(const long elementId, const int action, const std::map<std::string, std::string> actionArguments,
    int requestId, Accessibility::AccessibilityInteractionOperationCallback &callback)
{
    HILOG_INFO("%{public}s start", __func__);
    AccessibilityElementInfo info;
    info.SetWindowId(500);
    callback.SetPerformActionResult(true, requestId);
}

void AccessibilityInteractionOperationTest::ClearFocus()
{
    HILOG_INFO("%{public}s start", __func__);
}

void AccessibilityInteractionOperationTest::OutsideTouch()
{
    HILOG_INFO("%{public}s start", __func__);
}


class ASACClientSystemTest : public ::testing::Test {
public:
    ASACClientSystemTest() {}
    ~ASACClientSystemTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACClientSystemTest Start";
        Install("asacSystemTestA");
        sleep(2);
    }
    static void TearDownTestCase() {
        Uninstall("asacSystemTestA");
        GTEST_LOG_(INFO) << "ASACClientSystemTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACClientSystemTest SetUp()";
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "ASACClientSystemTest TearDown()";
    }
    static void Install(std::string hapName);
    static void Uninstall(std::string bundleName);
};

void ASACClientSystemTest::Install(std::string hapName)
{
    system(("bm install -p /system/vendor/" + hapName + ".hap > /dev/null 2>&1").c_str());
}

void ASACClientSystemTest::Uninstall(std::string bundleName)
{
    system(("bm uninstall -n " + bundleName + " > /dev/null 2>&1").c_str());
}
/**
 * @tc.number: RegisterInteractionOperation_001
 * @tc.name: Function
 * @tc.desc: Test RegisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, RegisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();
    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(100, interactionOperation, 1);

    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(100);
    eventInfo.SetEventType(eventType);
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

}

/**
 * @tc.number: RegisterInteractionOperation_002
 * @tc.name: Function
 * @tc.desc: Test RegisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, RegisterInteractionOperation_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(200, interactionOperation, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(200);
    eventInfo.SetEventType(eventType);
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

}

/**
 * @tc.number: RegisterInteractionOperation_003
 * @tc.name: Function
 * @tc.desc: Test RegisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, RegisterInteractionOperation_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_003 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(300, interactionOperation, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(300);
    eventInfo.SetEventType(eventType);
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

}

/**
 * @tc.number: RegisterInteractionOperation_004
 * @tc.name: Function
 * @tc.desc: Test RegisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, RegisterInteractionOperation_004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_004 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(400, interactionOperation, 1);AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(400);
    eventInfo.SetEventType(eventType);
    instance->SendEvent(eventInfo);

    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

}

/**
 * @tc.number: RegisterInteractionOperation_005
 * @tc.name: Function
 * @tc.desc: Test RegisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, RegisterInteractionOperation_005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterInteractionOperation_005 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(500, interactionOperation, 1);
    AccessibilityEventInfo eventInfo;
    auto eventType = Accessibility::EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT;
    eventInfo.SetWindowId(500);
    eventInfo.SetEventType(eventType);
    instance->SendEvent(eventInfo);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

}

/**
 * @tc.number: DeregisterInteractionOperation_001
 * @tc.name: Function
 * @tc.desc: Test DeregisterInteractionOperation
 */
HWTEST_F(ASACClientSystemTest, DeregisterInteractionOperation_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterInteractionOperation_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    std::shared_ptr<AccessibilityInteractionOperationTest> interactionOperation(new AccessibilityInteractionOperationTest());
    GTEST_LOG_(INFO) << "RegisterInteractionOperation start";
    instance->RegisterInteractionOperation(100, interactionOperation, 1);
    GTEST_LOG_(INFO) << "RegisterInteractionOperation end";

    GTEST_LOG_(INFO) << "DeregisterInteractionOperation start";
    instance->DeregisterInteractionOperation(1);
    GTEST_LOG_(INFO) << "DeregisterInteractionOperation end";

}

/**
 * @tc.number: GetSuggestedInterval_001
 * @tc.name: Function
 * @tc.desc: Test GetSuggestedInterval
 */
HWTEST_F(ASACClientSystemTest, GetSuggestedInterval_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSuggestedInterval_001 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetSuggestedInterval start";
    instance->GetSuggestedInterval(1, AccessibilityControlType::CONTENT_CONTROLS);
    GTEST_LOG_(INFO) << "GetSuggestedInterval end";
}

/**
 * @tc.number: GetSuggestedInterval_002
 * @tc.name: Function
 * @tc.desc: Test GetSuggestedInterval
 */
HWTEST_F(ASACClientSystemTest, GetSuggestedInterval_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSuggestedInterval_002 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetSuggestedInterval start";
    instance->GetSuggestedInterval(1, AccessibilityControlType::CONTENT_ICONS);
    GTEST_LOG_(INFO) << "GetSuggestedInterval end";
}

/**
 * @tc.number: GetSuggestedInterval_003
 * @tc.name: Function
 * @tc.desc: Test GetSuggestedInterval
 */
HWTEST_F(ASACClientSystemTest, GetSuggestedInterval_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetSuggestedInterval_003 start";
    shared_ptr<AccessibilitySystemAbilityClient> instance = AccessibilitySystemAbilityClient::GetInstance();

    GTEST_LOG_(INFO) << "GetSuggestedInterval start";
    instance->GetSuggestedInterval(1, AccessibilityControlType::CONTENT_TEXT);
    GTEST_LOG_(INFO) << "GetSuggestedInterval end";
}

} //namespace Accessibility
} //namespace OHOS