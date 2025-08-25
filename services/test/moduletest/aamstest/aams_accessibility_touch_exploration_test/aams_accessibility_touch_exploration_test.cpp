/*
 * Copyright (C) 2024-2025 Huawei Device Co., Ltd.
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

#define private public
#define protected public
#include <gtest/gtest.h>
#include <unistd.h>
#include "accessibility_touch_exploration.h"
#include "accessibility_account_data.h"
#include "accessibility_common_helper.h"
#include "accessibility_display_manager.h"
#include "accessibility_element_operator_stub.h"
#include "accessibility_input_interceptor.h"
#include "accessibility_mt_helper.h"
#include "accessible_ability_channel.h"
#include "accessible_ability_connection.h"
#include "accessible_ability_manager_service.h"
#include "iservice_registry.h"
#include "mock_accessibility_element_operator_callback.h"
#include "mock_accessibility_element_operator_impl.h"
#include "mock_accessibility_element_operator_proxy.h"
#include "mock_input_manager.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t POINT_ID_0 = 0;
    constexpr int32_t POINT_ID_1 = 1;
    constexpr int32_t POINT_ID_2 = 2;
    const int32_t SLEEP_TIME_3 = 3;
    const int32_t TAP_TIMES_1 = 1;
    const int32_t TAP_TIMES_2 = 2;
    const int32_t TAP_TIMES_3 = 3;
    const int64_t TAP_TIME_INTERVAL = 100000;
    const int32_t POINTER_ACTION_INVALID = -1;
} // namespace

class AamsTouchExplorationTest : public testing::Test {
public:
    AamsTouchExplorationTest()
    {}
    ~AamsTouchExplorationTest()
    {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    static void TouchExplorationPointSet(MMI::PointerEvent::PointerItem &point, int id, int x, int y);
    bool OnPointerEventOnePointsTest1(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4);
    bool OnPointerEventOnePointsTest3(std::vector<MMI::PointerEvent::PointerItem> &point,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4);
    bool OnPointerEventOnePointsTest7(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3);
    bool TwoFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, int32_t tapTimes,
        bool holdFlag);
    bool TwoFingerMoveEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
        MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4);
    bool OneFingerTapAndTwoFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, bool isSeparateFlag);
    bool TwoFingerTapAndOneFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2);
    bool MultiFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        std::vector<MMI::PointerEvent::PointerItem> &pointsVec, int32_t tapTimes, bool holdFlag);
    bool MultiFingerTapAndMoveEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
        std::vector<MMI::PointerEvent::PointerItem> &pointStartVec,
        std::vector<MMI::PointerEvent::PointerItem> &pointEndVec);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<MMI::PointerEvent> CreateMoveEvent(int32_t pointerCount, int32_t pointId);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action, int32_t pointId);
    std::shared_ptr<MMI::PointerEvent> CreateTouchEvent(int32_t action,
        std::vector<MMI::PointerEvent::PointerItem> &point, int64_t occurredTime, int64_t startTime,
        int32_t pointId);

    sptr<AccessibleAbilityChannel> aastub_ = nullptr;
    std::shared_ptr<MMI::IInputEventConsumer> interceptorId_ = nullptr;
    void WritefileAll(const char* fname, const char* data);
    void AddAccessibilityWindowConnection();
};

bool AamsTouchExplorationTest::OnPointerEventOnePointsTest1(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
    MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4)
{
    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_2);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

bool AamsTouchExplorationTest::OnPointerEventOnePointsTest3(std::vector<MMI::PointerEvent::PointerItem> &point,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
    MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4)
{
    point.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, point, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        // GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent003 inputEventConsumer is null";
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    point.clear();
    point.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, point, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

bool AamsTouchExplorationTest::OnPointerEventOnePointsTest7(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
    MMI::PointerEvent::PointerItem point3)
{
    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        return false;
    }
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
    inputEventConsumer->OnInputEvent(event);
    return true;
}

bool AamsTouchExplorationTest::TwoFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2, int32_t tapTimes, bool holdFlag)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest TwoFingerTapEventProduce inputEventConsumer is nullptr";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    int64_t occurredTime = 0;
    for (int32_t tapIndex = 1; tapIndex <= tapTimes; tapIndex++) {
        points.clear();
        points.emplace_back(point1);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, occurredTime, 0, POINT_ID_0);
        inputEventConsumer->OnInputEvent(event);

        points.emplace_back(point2);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, occurredTime, 0, POINT_ID_1);
        inputEventConsumer->OnInputEvent(event);

        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_0);
        inputEventConsumer->OnInputEvent(event);

        if (holdFlag && tapIndex == tapTimes) {
            sleep(1);
        }

        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
        inputEventConsumer->OnInputEvent(event);

        points.clear();
        points.emplace_back(point2);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_1);
        inputEventConsumer->OnInputEvent(event);

        occurredTime += TAP_TIME_INTERVAL;
    }
    return true;
}

bool AamsTouchExplorationTest::TwoFingerMoveEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
    MMI::PointerEvent::PointerItem point1, MMI::PointerEvent::PointerItem point2,
    MMI::PointerEvent::PointerItem point3, MMI::PointerEvent::PointerItem point4)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest TwoFingerMoveEventProduce inputEventConsumer is null";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    points.clear();
    points.emplace_back(point1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point3);
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point4);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    return true;
}

bool AamsTouchExplorationTest::OneFingerTapAndTwoFingerTapEventProduce(
    std::vector<MMI::PointerEvent::PointerItem> &points, MMI::PointerEvent::PointerItem point1,
    MMI::PointerEvent::PointerItem point2, bool isSeparateFlag)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest inputEventConsumer is null";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    // one finger event start
    points.clear();
    points.emplace_back(point1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    if (isSeparateFlag) {
        sleep(1);
    }
    // two finger event start
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, TAP_TIME_INTERVAL, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, TAP_TIME_INTERVAL, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    return true;
}

bool AamsTouchExplorationTest::TwoFingerTapAndOneFingerTapEventProduce(
    std::vector<MMI::PointerEvent::PointerItem> &points, MMI::PointerEvent::PointerItem point1,
    MMI::PointerEvent::PointerItem point2)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest inputEventConsumer is null";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    // two finger event start
    points.clear();
    points.emplace_back(point1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    points.clear();
    points.emplace_back(point2);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_1);
    inputEventConsumer->OnInputEvent(event);

    // one finger event start
    points.clear();
    points.emplace_back(point1);
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, TAP_TIME_INTERVAL, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, POINT_ID_0);
    inputEventConsumer->OnInputEvent(event);

    return true;
}

bool AamsTouchExplorationTest::MultiFingerTapEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
    std::vector<MMI::PointerEvent::PointerItem> &pointsVec, int32_t tapTimes, bool holdFlag)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest MultiFingerTapEventProduce inputEventConsumer is null";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    int64_t occurredTime = 0;
    for (int32_t tapIndex = 1; tapIndex <= tapTimes; tapIndex++) {
        points.clear();
        for (auto iter : pointsVec) {
            points.emplace_back(iter);
            event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, occurredTime, 0,
                iter.GetPointerId());
            inputEventConsumer->OnInputEvent(event);
        }

        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, 0, 0,
            pointsVec.front().GetPointerId());
        inputEventConsumer->OnInputEvent(event);

        if (holdFlag && tapIndex == tapTimes) {
            sleep(1);
        }

        for (int32_t pointsVecIndex = 0; pointsVecIndex < pointsVec.size(); pointsVecIndex++) {
            event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, occurredTime, 0,
                points.back().GetPointerId());
            points.pop_back();
            inputEventConsumer->OnInputEvent(event);
        }

        occurredTime += TAP_TIME_INTERVAL;
    }
    return true;
}

bool AamsTouchExplorationTest::MultiFingerTapAndMoveEventProduce(std::vector<MMI::PointerEvent::PointerItem> &points,
    std::vector<MMI::PointerEvent::PointerItem> &pointStartVec,
    std::vector<MMI::PointerEvent::PointerItem> &pointEndVec)
{
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest MultiFingerTapAndMoveEventProduce inputEventConsumer is null";
        return false;
    }

    std::shared_ptr<MMI::PointerEvent> event;
    int64_t occurredTime = 0;
    points.clear();
    for (auto iter : pointStartVec) {
        points.emplace_back(iter);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, occurredTime, 0, iter.GetPointerId());
        inputEventConsumer->OnInputEvent(event);
    }

    points.clear();
    for (auto iter : pointEndVec) {
        points.emplace_back(iter);
    }
    event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_MOVE, points, occurredTime, 0,
        pointStartVec.front().GetPointerId());
    inputEventConsumer->OnInputEvent(event);

    for (int32_t pointsEndVecIndex = 0; pointsEndVecIndex < pointEndVec.size(); pointsEndVecIndex++) {
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, occurredTime, 0,
            points.back().GetPointerId());
        points.pop_back();
        inputEventConsumer->OnInputEvent(event);
    }

    return true;
}

void AamsTouchExplorationTest::TouchExplorationPointSet(MMI::PointerEvent::PointerItem &point, int id, int x, int y)
{
    point.SetPointerId(id);
    point.SetDisplayX(x);
    point.SetDisplayY(y);
}

void AamsTouchExplorationTest::WritefileAll(const char* fname, const char* data)
{
    FILE* fp = nullptr;
    if (!(fp = fopen(fname, "w"))) {
        printf("open file %s fail \n", fname);
        return;
    }

    (void)fprintf(fp, "%s", data);
    (void)fclose(fp);
}

void AamsTouchExplorationTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest SetUpTestCase";
    Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
    AccessibilityCommonHelper::GetInstance().WaitForServicePublish();
    Singleton<AccessibleAbilityManagerService>::GetInstance().SwitchedUser(AccessibilityHelper::accountId_);
    GTEST_LOG_(INFO) << "AccessibleAbilityManagerService is published";
}

void AamsTouchExplorationTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest TearDownTestCase";
    AccessibilityHelper::GetInstance().SetGestureId(0);
    AccessibilityHelper::GetInstance().GetEventType().clear();
    AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
    Singleton<AccessibleAbilityManagerService>::GetInstance().runner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().handler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().actionRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().actionHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().sendEventRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().sendEventHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().channelRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().channelHandler_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().inputManagerRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().gestureRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().hoverEnterRunner_.reset();
    Singleton<AccessibleAbilityManagerService>::GetInstance().hoverEnterHandler_.reset();
}

void AamsTouchExplorationTest::SetUp()
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest SetUp";

    interceptorId_ = std::make_shared<AccessibilityInputEventConsumer>();
    MMI::InputManager::GetInstance()->AddInterceptor(interceptorId_);

    // add an ability connection client
    AccessibilityAbilityInitParams initParams;
    std::shared_ptr<AccessibilityAbilityInfo> abilityInfo = std::make_shared<AccessibilityAbilityInfo>(initParams);
    abilityInfo->SetAccessibilityAbilityType(AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_ALL);
    abilityInfo->SetCapabilityValues(Capability::CAPABILITY_TOUCH_GUIDE);
    AppExecFwk::ElementName elementName("deviceId", "bundleName", "name");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    accountData->AddInstalledAbility(*abilityInfo);
    sleep(1);
    sptr<AccessibleAbilityConnection> connection =
        new AccessibleAbilityConnection(accountData->GetAccountId(), 0, *abilityInfo);
    aastub_ = new AccessibleAbilityChannel(accountData->GetAccountId(), abilityInfo->GetId());
    connection->OnAbilityConnectDoneSync(elementName, aastub_);

    AddAccessibilityWindowConnection();
}

void AamsTouchExplorationTest::TearDown()
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest TearDown";

    Singleton<AccessibleAbilityManagerService>::GetInstance().DeregisterElementOperatorByWindowId(0);
    sleep(SLEEP_TIME_3);
    aastub_ = nullptr;
    interceptorId_ = nullptr;
    AccessibilityHelper::GetInstance().SetGestureId(0);
    AccessibilityHelper::GetInstance().GetEventType().clear();
    MMI::MockInputManager::ClearTouchActions();
}

std::shared_ptr<MMI::PointerEvent> AamsTouchExplorationTest::CreateTouchEvent(int32_t action, int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> touchEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    item.SetPointerId(1);
    item.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
    touchEvent->AddPointerItem(item);
    touchEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    touchEvent->SetPointerAction(action);
    touchEvent->SetPointerId(pointId);

    return touchEvent;
}

std::shared_ptr<MMI::PointerEvent> AamsTouchExplorationTest::CreateMoveEvent(int32_t pointerCount, int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> touchEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item = {};

    for (int32_t i = 0; i < pointerCount; i++) {
        item.SetPointerId(i + 1);
        item.SetPressed(true);
        touchEvent->AddPointerItem(item);
    }
    touchEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    touchEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    touchEvent->SetPointerId(pointId);

    return touchEvent;
}

std::shared_ptr<MMI::PointerEvent> AamsTouchExplorationTest::CreateTouchEvent(int32_t action,
    std::vector<MMI::PointerEvent::PointerItem> &points, int64_t occurredTime, int64_t startTime,
    int32_t pointId)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    for (auto &point : points) {
        if (point.GetPointerId() == pointId) {
            point.SetPressed(action == MMI::PointerEvent::POINTER_ACTION_UP ? false : true);
        } else {
            point.SetPressed(true);
        }
        pointerEvent->AddPointerItem(point);
    }
    pointerEvent->SetPointerId(pointId);
    pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetActionStartTime(startTime);
    pointerEvent->SetActionTime(occurredTime);

    return pointerEvent;
}

void AamsTouchExplorationTest::AddAccessibilityWindowConnection()
{
    GTEST_LOG_(INFO) << "aamsAccessibleAbilityChannelTest AddAccessibilityWindowConnection";
    // accessibility interaction connection
    int32_t windowId = 0;
    std::shared_ptr<MockAccessibilityElementOperatorCallback> mockCallback =
        std::make_shared<MockAccessibilityElementOperatorCallback>();
    sptr<AccessibilityElementOperatorStub> stub =
        new MockAccessibilityElementOperatorImpl(windowId, nullptr, *mockCallback);
    sptr<IAccessibilityElementOperator> proxy = new MockAccessibilityElementOperatorProxy(stub);
    GTEST_LOG_(INFO) << "aams  RegisterElementOperatorByWindowId";
    Singleton<AccessibleAbilityManagerService>::GetInstance().RegisterElementOperatorByWindowId(windowId, proxy);
}

/**
 * @tc.number: OnPointerEvent001
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the event that two fingers moving in same directions in dragging state.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent001 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 500, 500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 2, 1000, 500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 2000, 500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 2500, 500);

    bool flag = OnPointerEventOnePointsTest1(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent001 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);

        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_DOWN);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_UP);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent001 ENDs";
}

/**
 * @tc.number: OnPointerEvent002
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the event that move slowly with one finger.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent002, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent002 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent002 inputEventConsumer is null";
    } else {
        inputEventConsumer->OnInputEvent(event);

        sleep(1);
        event = CreateMoveEvent(1, 1);
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, 1);
        inputEventConsumer->OnInputEvent(event);

        sleep(1);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent002 ENDs";
}

/**
 * @tc.number: OnPointerEvent003
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_LEFT_THEN_RIGHT gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent003, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent003 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 1500, 2500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 0, 2500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 2500, 2250);

    OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);

    sleep(1);
    // gesture type
    EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
        static_cast<int32_t>(GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT));

    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent003 ENDs";
}

/**
 * @tc.number: OnPointerEvent004
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_DOWN_THEN_UP gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent004 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 2500, 3500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 2500, 5000);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 2250, 2500);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent003 inputEventConsumer is null";
    } else {
        sleep(1);
        // gesture type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_DOWN_THEN_UP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent004 ENDs";
}

/**
 * @tc.number: OnPointerEvent005
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_RIGHT_THEN_LEFT gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent005 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 3500, 2500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 5000, 2500);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 2500, 2250);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent005 inputEventConsumer is null";
    } else {
        sleep(1);
        // gesture type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent005 ENDs";
}

/**
 * @tc.number: OnPointerEvent006
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the GESTURE_SWIPE_UP_THEN_DOWN gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent006 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 2500, 1500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 2500, 0);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 2250, 2500);

    bool flag = OnPointerEventOnePointsTest3(points, point1, point2, point3, point4);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent006 inputEventConsumer is null";
    } else {
        sleep(1);
        // gesture type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP_THEN_DOWN));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent006 ENDs";
}

/**
 * @tc.number: OnPointerEvent007
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the UP gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent007, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent007 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 1, 2500, 2500);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 2500, 1500);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 1, 2500, 0);

    bool flag = OnPointerEventOnePointsTest7(points, point1, point2, point3);
    if (!flag) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent007 inputEventConsumer is null";
    } else {
        sleep(1);
        // gesture type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_SWIPE_UP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent007 ENDs";
}

/**
 * @tc.number: OnPointerEvent008
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the single tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent008, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent008 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::shared_ptr<MMI::PointerEvent> event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent009 inputEventConsumer is null";
    } else {
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, 1);
        inputEventConsumer->OnInputEvent(event);
        sleep(1);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent008 ends";
}

/**
 * @tc.number: OnPointerEvent009
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the double tap and long press event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent009, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent009 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    point1.SetDisplayX(300);
    point1.SetDisplayY(500);
    point1.SetPointerId(1);

    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent009 inputEventConsumer is null";
    } else {
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 40, 0, 1);
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 200, 0, 1);
        inputEventConsumer->OnInputEvent(event);
        sleep(SLEEP_TIME_3);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
        inputEventConsumer->OnInputEvent(event);

        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3), EventType::TYPE_TOUCH_END);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent009 ends";
}

/**
 * @tc.number: OnPointerEvent010
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the double-tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent010 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    point1.SetDisplayX(300);
    point1.SetDisplayY(500);
    point1.SetPointerId(1);

    points.emplace_back(point1);
    std::shared_ptr<MMI::PointerEvent> event =
        CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 0, 0, 1);
    auto inputEventConsumer = MMI::MockInputManager::GetInputEventConsumer();
    if (!inputEventConsumer) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent010 inputEventConsumer is null";
    } else {
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_DOWN, points, 100, 0, 1);
        inputEventConsumer->OnInputEvent(event);
        event = CreateTouchEvent(MMI::PointerEvent::POINTER_ACTION_UP, points, 0, 0, 1);
        inputEventConsumer->OnInputEvent(event);

        sleep(1);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), POINTER_ACTION_INVALID);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent010 ends";
}

/**
 * @tc.number: OnPointerEvent011
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent011 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();
    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapEventProduce(points, point1, point2, TAP_TIMES_1, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0011 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), GestureType::GESTURE_TWO_FINGER_SINGLE_TAP);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent011 ends";
}

/**
 * @tc.number: OnPointerEvent012
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger double tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent012 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapEventProduce(points, point1, point2, TAP_TIMES_2, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0011 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent012 ends";
}

/**
 * @tc.number: OnPointerEvent013
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger triple tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent013 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapEventProduce(points, point1, point2, TAP_TIMES_3, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0011 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent013 ends";
}

/**
 * @tc.number: OnPointerEvent014
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger double tap and hold event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent014 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapEventProduce(points, point1, point2, TAP_TIMES_2, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0014 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent014 ends";
}

/**
 * @tc.number: OnPointerEvent015
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger triple tap and hold event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent015 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapEventProduce(points, point1, point2, TAP_TIMES_3, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0015 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent015 ends";
}

/**
 * @tc.number: OnPointerEvent016
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger move event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent016, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent016 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 0, 100, 400);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 1, 200, 400);

    bool eventProduceRst = TwoFingerMoveEventProduce(points, point1, point2, point3, point4);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0016 inputEventConsumer is null";
    } else {
        sleep(1);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_DOWN);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_MOVE);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(2), MMI::PointerEvent::POINTER_ACTION_UP);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent016 ends";
}

/**
 * @tc.number: OnPointerEvent017
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the one finger tap and then two finger tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent017, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent017 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = OneFingerTapAndTwoFingerTapEventProduce(points, point1, point2, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0017 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3), EventType::TYPE_TOUCH_END);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_MOVE);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent017 ends";
}

/**
 * @tc.number: OnPointerEvent018
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the two finger tap and then one finger tap event, expect do not recognize as any gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent018, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent018 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = TwoFingerTapAndOneFingerTapEventProduce(points, point1, point2);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0018 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_INVALID));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent018 ends";
}

/**
 * @tc.number: OnPointerEvent019
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the one finger tap and then two finger tap event, interval > 300ms, expect two gesture event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent019, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent019 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);

    bool eventProduceRst = OneFingerTapAndTwoFingerTapEventProduce(points, point1, point2, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0019 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(2), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(3), EventType::TYPE_TOUCH_END);
        // action type
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(0), MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER);
        EXPECT_EQ(MMI::MockInputManager::GetTouchActionOfTargetIndex(1), MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(), GestureType::GESTURE_TWO_FINGER_SINGLE_TAP);
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent019 ends";
}

/**
 * @tc.number: OnPointerEvent020
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger single tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent020, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent020 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 1, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0020 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SINGLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent020 ends";
}

/**
 * @tc.number: OnPointerEvent021
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger single tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent021, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent021 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3, point4};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 1, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0021 inputEventConsumer is null";
    } else {
        sleep(1);
        // event type
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(0), EventType::TYPE_TOUCH_BEGIN);
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetEventTypeOfTargetIndex(1), EventType::TYPE_TOUCH_END);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent021 ends";
}

/**
 * @tc.number: OnPointerEvent022
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger double tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent022, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent022 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 2, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0022 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent022 ends";
}

/**
 * @tc.number: OnPointerEvent023
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger double tap and hold event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent023, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent023 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 2, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0023 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent023 ends";
}

/**
 * @tc.number: OnPointerEvent024
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger triple tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent024, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent024 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 3, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0024 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent024 ends";
}

/**
 * @tc.number: OnPointerEvent025
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger triple tap and hold event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent025, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent025 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 3, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0025 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent025 ends";
}

/**
 * @tc.number: OnPointerEvent026
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger double tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent026, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent026 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3, point4};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 2, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0026 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent026 ends";
}

/**
 * @tc.number: OnPointerEvent027
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger double tap event and hold.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent027, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent027 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3, point4};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 2, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0027 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent027 ends";
}

/**
 * @tc.number: OnPointerEvent028
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger triple tap event.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent028, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent028 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3, point4};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 3, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0028 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_LE(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent028 ends";
}

/**
 * @tc.number: OnPointerEvent029
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger triple tap event and hold.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent029, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent029 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3, point4};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 3, true);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0029 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent029 ends";
}

/**
 * @tc.number: OnPointerEvent030
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger swipe down.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent030, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent030 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 800);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 200, 800);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 300, 800);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0030 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent030 ends";
}

/**
 * @tc.number: OnPointerEvent031
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger swipe down.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent031, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent031 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 100);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3, point4};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 800);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 200, 800);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 300, 800);
    MMI::PointerEvent::PointerItem point44 = {};
    TouchExplorationPointSet(point44, 3, 400, 800);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33, point44};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0031 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_SWIPE_DOWN));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent031 ends";
}

/**
 * @tc.number: OnPointerEvent032
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger swipe up.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent032, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent032 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 800);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 800);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 800);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 200, 100);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0032 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SWIPE_UP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent032 ends";
}

/**
 * @tc.number: OnPointerEvent033
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger swipe left.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent033, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent033 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 600, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 600, 200);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 600, 300);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 100, 200);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 100, 300);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0033 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SWIPE_LEFT));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent033 ends";
}

/**
 * @tc.number: OnPointerEvent034
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the three finger swipe right.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent034, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent034 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 100, 200);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 100, 300);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 600, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 600, 200);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 600, 300);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0034 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SWIPE_RIGHT));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent034 ends";
}

/**
 * @tc.number: OnPointerEvent035
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger swipe up.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent035, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent035 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 800);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 200, 800);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 300, 800);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 400, 800);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3, point4};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 200, 100);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 300, 100);
    MMI::PointerEvent::PointerItem point44 = {};
    TouchExplorationPointSet(point44, 3, 400, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33, point44};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0035 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_SWIPE_UP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent035 ends";
}

/**
 * @tc.number: OnPointerEvent036
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger swipe left.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent036, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent036 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 600, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 600, 200);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 600, 300);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 600, 400);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3, point4};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 100, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 100, 200);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 100, 300);
    MMI::PointerEvent::PointerItem point44 = {};
    TouchExplorationPointSet(point44, 3, 100, 400);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33, point44};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0036 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_SWIPE_LEFT));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent036 ends";
}

/**
 * @tc.number: OnPointerEvent037
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the four finger swipe right.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent037, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent037 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 0, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 1, 100, 200);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 2, 100, 300);
    MMI::PointerEvent::PointerItem point4 = {};
    TouchExplorationPointSet(point4, 3, 100, 400);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3, point4};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 0, 600, 100);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 1, 600, 200);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 2, 600, 300);
    MMI::PointerEvent::PointerItem point44 = {};
    TouchExplorationPointSet(point44, 3, 600, 400);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33, point44};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0037 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_FOUR_FINGER_SWIPE_RIGHT));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent037 ends";
}

/**
 * @tc.number: OnPointerEvent038
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the simulated swipe gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent038, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent038 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 10000, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 10001, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 10002, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointStartVec{point1, point2, point3};

    MMI::PointerEvent::PointerItem point11 = {};
    TouchExplorationPointSet(point11, 10000, 100, 800);
    MMI::PointerEvent::PointerItem point22 = {};
    TouchExplorationPointSet(point22, 10001, 200, 800);
    MMI::PointerEvent::PointerItem point33 = {};
    TouchExplorationPointSet(point33, 10002, 300, 800);
    std::vector<MMI::PointerEvent::PointerItem> pointEndVec{point11, point22, point33};

    bool eventProduceRst = MultiFingerTapAndMoveEventProduce(points, pointStartVec, pointEndVec);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0030 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent038 ends";
}

/**
 * @tc.number: OnPointerEvent039
 * @tc.name:OnPointerEvent
 * @tc.desc: Check the simulated multi-tap gesture.
 */
HWTEST_F(AamsTouchExplorationTest, AamsTouchExplorationTest_Moduletest_OnPointerEvent039, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent039 starts";

    AccessibilityHelper::GetInstance().GetEventType() = {};
    MMI::MockInputManager::ClearTouchActions();

    std::vector<MMI::PointerEvent::PointerItem> points = {};
    MMI::PointerEvent::PointerItem point1 = {};
    TouchExplorationPointSet(point1, 10000, 100, 100);
    MMI::PointerEvent::PointerItem point2 = {};
    TouchExplorationPointSet(point2, 10001, 200, 100);
    MMI::PointerEvent::PointerItem point3 = {};
    TouchExplorationPointSet(point3, 10002, 300, 100);
    std::vector<MMI::PointerEvent::PointerItem> pointVec{point1, point2, point3};

    bool eventProduceRst = MultiFingerTapEventProduce(points, pointVec, 2, false);
    if (!eventProduceRst) {
        GTEST_LOG_(INFO) << "AamsTouchExplorationTest OnPointerEvent0022 inputEventConsumer is null";
    } else {
        sleep(1);
        // gestureId
        EXPECT_EQ(AccessibilityHelper::GetInstance().GetGestureId(),
            static_cast<int32_t>(GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP));
    }
    GTEST_LOG_(INFO) << "AamsTouchExplorationTest AamsTouchExplorationTest_Moduletest_OnPointerEvent039 ends";
}
} // namespace Accessibility
} // namespace OHOS