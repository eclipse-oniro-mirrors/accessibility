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

#include <vector>
#include <memory>
#include <gtest/gtest.h>
#include "parcel.h"
#include "gesture_simulation.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

const uint32_t MAX_STROKE_DURATION = 60 * 1000;
const uint32_t MAX_STROKES = 10;

class UnitTestGestureResultListener : public GestureResultListener {
public:
    UnitTestGestureResultListener() {}
    virtual ~UnitTestGestureResultListener() {}
    void OnCancelled(std::vector<GesturePathDefine> &gesturePathDefine) override
    {
        GTEST_LOG_(INFO) << "UnitTestGestureResultListener OnCancelled";
    }

    void OnCompleted(std::vector<GesturePathDefine> &gesturePathDefine) override
    {
        GTEST_LOG_(INFO) << "UnitTestGestureResultListener OnCompleted";
    }
};

class GestureSimulationUnitTest : public ::testing::Test {
public:
    GestureSimulationUnitTest() {}
    ~GestureSimulationUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GestureSimulationUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### GestureSimulationUnitTest Start ######################";
}

void GestureSimulationUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### GestureSimulationUnitTest End ######################";
}

void GestureSimulationUnitTest::SetUp()
{}

void GestureSimulationUnitTest::TearDown()
{}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionX_001
 * @tc.name: GesturePathPositionDefine_GetPositionX
 * @tc.desc: Test function GetPositionX in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionX_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionX_001 start";

    GesturePathPositionDefine position {};
    float positionX = 1.1;

    position.SetPositionX(positionX);
    EXPECT_EQ(position.GetPositionX(), positionX);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionX_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionY_001
 * @tc.name: GesturePathPositionDefine_GetPositionY
 * @tc.desc: Test function GetPositionY in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionY_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionY_001 start";

    GesturePathPositionDefine position {};
    float positionY = 1.1;

    position.SetPositionY(positionY);
    EXPECT_EQ(position.GetPositionY(), positionY);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_GetPositionY_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionX_001
 * @tc.name: GesturePathPositionDefine_SetPositionX
 * @tc.desc: Test function SetPositionX in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionX_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionX_001 start";

    GesturePathPositionDefine position {};
    float positionX = 1.1;
    position.SetPositionX(positionX);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionX_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionY_001
 * @tc.name: GesturePathPositionDefine_SetPositionY
 * @tc.desc: Test function SetPositionY in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionY_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionY_001 start";

    GesturePathPositionDefine position {};
    float positionY = 1.1;
    position.SetPositionY(positionY);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_SetPositionY_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_ReadFromParcel_001
 * @tc.name: GesturePathPositionDefine_ReadFromParcel
 * @tc.desc: Test function ReadFromParcel in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_ReadFromParcel_001 start";

    GesturePathPositionDefine position {};
    Parcel parcel;

    position.Marshalling(parcel);
    EXPECT_TRUE(position.ReadFromParcel(parcel));

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_ReadFromParcel_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_Marshalling_001
 * @tc.name: GesturePathPositionDefine_Marshalling
 * @tc.desc: Test function Marshalling in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_Marshalling_001 start";

    GesturePathPositionDefine position {};
    Parcel parcel;
    EXPECT_TRUE(position.Marshalling(parcel));

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_Marshalling_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathPositionDefine_Unmarshalling_001
 * @tc.name: GesturePathPositionDefine_Unmarshalling
 * @tc.desc: Test function Unmarshalling in class GesturePathPositionDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathPositionDefine_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_Unmarshalling_001 start";

    GesturePathPositionDefine position {};
    position.SetPositionX(1.0);
    position.SetPositionY(1.0);

    Parcel parcel;
    position.Marshalling(parcel);
    GesturePathPositionDefine *obj = position.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->GetPositionX(), position.GetPositionX());
    EXPECT_EQ(obj->GetPositionY(), position.GetPositionY());

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathPositionDefine_Unmarshalling_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_001
 * @tc.name: GesturePathDefine
 * @tc.desc: Test function GesturePathDefine in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_001 start";

    GesturePathPositionDefine startPosition {};
    GesturePathPositionDefine endPosition {};
    uint32_t durationTime = 1;
    startPosition.SetPositionX(-1);
    shared_ptr<GesturePathDefine> path = make_shared<GesturePathDefine>(startPosition, endPosition, durationTime);
    EXPECT_NE(path->GetStartPosition().GetPositionX(), -1);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_002
 * @tc.name: GesturePathDefine
 * @tc.desc: Test function GesturePathDefine in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_002 start";

    GesturePathPositionDefine startPosition {};
    GesturePathPositionDefine endPosition {};
    uint32_t durationTime = 1;
    endPosition.SetPositionX(-1);
    shared_ptr<GesturePathDefine> path = make_shared<GesturePathDefine>(startPosition, endPosition, durationTime);
    EXPECT_NE(path->GetEndPosition().GetPositionX(), -1);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_002 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokeDuration_001
 * @tc.name: GesturePathDefine_GetMaxStrokeDuration
 * @tc.desc: Test function GetMaxStrokeDuration in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokeDuration_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokeDuration_001 start";

    GesturePathDefine path {};
    EXPECT_EQ(path.GetMaxStrokeDuration(), MAX_STROKE_DURATION);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokeDuration_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokes_001
 * @tc.name: GesturePathDefine_GetMaxStrokes
 * @tc.desc: Test function GetMaxStrokes in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokes_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokes_001 start";

    GesturePathDefine path {};
    EXPECT_EQ(path.GetMaxStrokes(), MAX_STROKES);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetMaxStrokes_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_GetDurationTime_001
 * @tc.name: GesturePathDefine_GetDurationTime
 * @tc.desc: Test function GetDurationTime in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_GetDurationTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetDurationTime_001 start";

    GesturePathDefine path {};
    uint32_t durationTime = 1;

    path.SetDurationTime(durationTime);
    EXPECT_EQ(path.GetDurationTime(), durationTime);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetDurationTime_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_GetEndPosition_001
 * @tc.name: GesturePathDefine_GetEndPosition
 * @tc.desc: Test function GetEndPosition in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_GetEndPosition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetEndPosition_001 start";

    GesturePathPositionDefine endposition {};
    endposition.SetPositionX(1.0);
    endposition.SetPositionY(1.0);

    GesturePathDefine path {};
    path.SetEndPosition(endposition);

    EXPECT_EQ(path.GetEndPosition().GetPositionX(), endposition.GetPositionX());
    EXPECT_EQ(path.GetEndPosition().GetPositionY(), endposition.GetPositionY());

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetEndPosition_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_GetStartPosition_001
 * @tc.name: GesturePathDefine_GetStartPosition
 * @tc.desc: Test function GetStartPosition in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_GetStartPosition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetStartPosition_001 start";

    GesturePathPositionDefine startposition {};
    startposition.SetPositionX(1.0);
    startposition.SetPositionY(1.0);

    GesturePathDefine path {};
    path.SetStartPosition(startposition);

    EXPECT_EQ(path.GetStartPosition().GetPositionX(), startposition.GetPositionX());
    EXPECT_EQ(path.GetStartPosition().GetPositionY(), startposition.GetPositionY());

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_GetStartPosition_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_SetDurationTime_001
 * @tc.name: GesturePathDefine_SetDurationTime
 * @tc.desc: Test function SetDurationTime in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_SetDurationTime_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetDurationTime_001 start";

    GesturePathDefine path {};
    uint32_t durationTime = 1;

    path.SetDurationTime(durationTime);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetDurationTime_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_SetEndPosition_001
 * @tc.name: GesturePathDefine_SetEndPosition
 * @tc.desc: Test function SetEndPosition in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_SetEndPosition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetEndPosition_001 start";

    GesturePathPositionDefine endposition {};
    GesturePathDefine path {};
    path.SetEndPosition(endposition);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetEndPosition_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_SetStartPosition_001
 * @tc.name: GesturePathDefine_SetStartPosition
 * @tc.desc: Test function SetStartPosition in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_SetStartPosition_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetStartPosition_001 start";

    GesturePathPositionDefine startposition {};
    GesturePathDefine path {};
    path.SetStartPosition(startposition);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_SetStartPosition_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_ReadFromParcel_001
 * @tc.name: GesturePathDefine_ReadFromParcel
 * @tc.desc: Test function ReadFromParcel in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_ReadFromParcel_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_ReadFromParcel_001 start";

    GesturePathDefine path {};
    Parcel parcel;

    path.Marshalling(parcel);
    EXPECT_TRUE(path.ReadFromParcel(parcel));

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_ReadFromParcel_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_Marshalling_001
 * @tc.name: GesturePathDefine_Marshalling
 * @tc.desc: Test function Marshalling in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_Marshalling_001 start";

    GesturePathDefine path {};
    Parcel parcel;
    EXPECT_TRUE(path.Marshalling(parcel));

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_Marshalling_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GesturePathDefine_Unmarshalling_001
 * @tc.name: GesturePathDefine_Unmarshalling
 * @tc.desc: Test function Unmarshalling in class GesturePathDefine
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GesturePathDefine_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_Unmarshalling_001 start";

    GesturePathPositionDefine startposition {};
    GesturePathPositionDefine endposition {};
    startposition.SetPositionX(1.0);
    startposition.SetPositionY(1.0);
    endposition.SetPositionX(2.0);
    endposition.SetPositionY(2.0);
    uint32_t durationTime = 1;

    GesturePathDefine path {};
    path.SetDurationTime(durationTime);
    path.SetStartPosition(startposition);
    path.SetEndPosition(endposition);

    Parcel parcel;
    path.Marshalling(parcel);
    GesturePathDefine *obj = path.Unmarshalling(parcel);
    ASSERT_TRUE(obj);
    EXPECT_EQ(obj->GetDurationTime(), durationTime);
    EXPECT_EQ(obj->GetStartPosition().GetPositionX(), startposition.GetPositionX());
    EXPECT_EQ(obj->GetStartPosition().GetPositionY(), startposition.GetPositionY());
    EXPECT_EQ(obj->GetEndPosition().GetPositionX(), endposition.GetPositionX());
    EXPECT_EQ(obj->GetEndPosition().GetPositionY(), endposition.GetPositionY());

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GesturePathDefine_Unmarshalling_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GestureResultListenerInfo_GetGestureResultListener_001
 * @tc.name: GestureResultListenerInfo_GetGestureResultListener
 * @tc.desc: Test function GetGestureResultListener in class GestureResultListenerInfo
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GestureResultListenerInfo_GetGestureResultListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGestureResultListener_001 start";

    vector<GesturePathDefine> paths {};
    GesturePathDefine path {};
    paths.push_back(path);
    shared_ptr<GestureResultListener> listener = make_shared<UnitTestGestureResultListener>();
    shared_ptr<GestureResultListenerInfo> listenerInfo = make_shared<GestureResultListenerInfo>(paths, listener);

    EXPECT_NE(listenerInfo->GetGestureResultListener(), nullptr);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGestureResultListener_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_001
 * @tc.name: GestureResultListenerInfo_GetGesturePathDefine
 * @tc.desc: Test function GetGesturePathDefine in class GestureResultListenerInfo
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_001 start";

    vector<GesturePathDefine> paths {};
    GesturePathDefine path {};
    paths.push_back(path);
    shared_ptr<GestureResultListener> listener = make_shared<UnitTestGestureResultListener>();
    shared_ptr<GestureResultListenerInfo> listenerInfo = make_shared<GestureResultListenerInfo>(paths, listener);

    EXPECT_EQ((int)listenerInfo->GetGesturePathDefine().size(), 1);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_001 end";
}

/**
 * @tc.number: GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_002
 * @tc.name: GestureResultListenerInfo_GetGesturePathDefine
 * @tc.desc: Test function GetGesturePathDefine in class GestureResultListenerInfo
 *           Get path when there is no path in listenerInfos.
 */
HWTEST_F(GestureSimulationUnitTest, GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_002 start";

    vector<GesturePathDefine> paths {};
    shared_ptr<GestureResultListener> listener = make_shared<UnitTestGestureResultListener>();
    shared_ptr<GestureResultListenerInfo> listenerInfo = make_shared<GestureResultListenerInfo>(paths, listener);

    EXPECT_EQ((int)listenerInfo->GetGesturePathDefine().size(), 0);
    EXPECT_EQ(listenerInfo->GetGestureResultListener(), nullptr);

    GTEST_LOG_(INFO) << "GestureSimulation_Unittest_GestureResultListenerInfo_GetGesturePathDefine_002 end";
}