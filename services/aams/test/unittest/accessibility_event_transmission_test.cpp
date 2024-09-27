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
#include <vector>
#include "accessibility_event_transmission.h"
#include "hilog_wrapper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class EventTransmissionTest : public ::testing::Test {
public:
    EventTransmissionTest()
    {}
    ~EventTransmissionTest()
    {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void EventTransmissionTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### EventTransmissionTest Start ######################";
}

void EventTransmissionTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### EventTransmissionTest End ######################";
}

void EventTransmissionTest::SetUp()
{}

void EventTransmissionTest::TearDown()
{}

/**
 * @tc.number: EventTransmission_Unittest_OnKeyEvent_001
 * @tc.name: OnKeyEvent
 * @tc.desc: Test function OnKeyEvent
 */
HWTEST_F(EventTransmissionTest,
    EventTransmission_Unittest_OnKeyEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnKeyEvent_001 start";

    Singleton<EventTransmission>::GetInstance().OnKeyEvent(nullptr);

    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnKeyEvent_001 end";
}

/**
 * @tc.number: EventTransmission_Unittest_OnPointerEvent_001
 * @tc.name: OnPointerEvent
 * @tc.desc: Test function OnPointerEvent
 */
HWTEST_F(EventTransmissionTest,
    EventTransmission_Unittest_OnPointerEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnPointerEvent_001 start";

    Singleton<EventTransmission>::GetInstance().OnPointerEvent();

    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnPointerEvent_001 end";
}

/**
 * @tc.number: EventTransmission_Unittest_OnMoveMouse_001
 * @tc.name: OnMoveMouse
 * @tc.desc: Test function OnMoveMouse
 */
HWTEST_F(EventTransmissionTest, EventTransmission_Unittest_OnMoveMouse_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnMoveMouse_001 start";
    std::vector<int> infos{1};
    auto display = Singleton<EventTransmission>::GetInstance().OnMoveMouse(0);
    EXPECT_EQ(infos.size(), 1);
    
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_OnMoveMouse_001 end";
}

/**
 * @tc.number: EventTransmission_Unittest_SetNext_001
 * @tc.name: SetNext
 * @tc.desc: Test function SetNext
 */
HWTEST_F(EventTransmissionTest, EventTransmission_Unittest_SetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_SetNext_001 start";
    std::vector<int> infos{1};
    auto displays = Singleton<EventTransmission>::GetInstance().SetNext();
    EXPECT_EQ(infos.size(), 1);
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_SetNext_001 end";
}

/**
 * @tc.number: EventTransmission_Unittest_GetNext_001
 * @tc.name: GetNext
 * @tc.desc: Test function GetNext
 */
HWTEST_F(
    EventTransmissionTest, EventTransmission_Unittest_GetNext_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_GetNext_001 start";

    auto display = Singleton<EventTransmission>::GetInstance().GetNext();

    GTEST_LOG_(INFO) << "EventTransmission_Unittest_GetNext_001 end";
}

/**
 * @tc.number: EventTransmission_Unittest_DestroyEvents_001
 * @tc.name: DestroyEvents
 * @tc.desc: Test function DestroyEvents
 */
HWTEST_F(
    EventTransmissionTest, EventTransmission_Unittest_DestroyEvents_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EventTransmission_Unittest_DestroyEvents_001 start";

    uint64_t displayId = Singleton<EventTransmission>::GetInstance().DestroyEvents();
    GTEST_LOG_(INFO) << "DefaultDisplayId" << displayId;

    GTEST_LOG_(INFO) << "EventTransmission_Unittest_DestroyEvents_001 end";
}
} // namespace Accessibility
} // namespace OHOS