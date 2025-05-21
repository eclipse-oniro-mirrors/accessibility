/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "accessibility_gesture_inject_path_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class AccessibilityGestureInjectPathParcelTest : public ::testing::Test {
public:
    AccessibilityGestureInjectPathParcelTest()
    {}
    ~AccessibilityGestureInjectPathParcelTest()
    {}
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityGestureInjectPathParcelTest Start";
    }
    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "AccessibilityGestureInjectPathParcelTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilityGestureInjectPathParcelTest SetUp() Start";
        AccessibilityGestureInjectPath gestureInfo;
        gestureInfoParcel_ = std::make_shared<AccessibilityGestureInjectPathParcel>(gestureInfo);
        GTEST_LOG_(INFO) << "AccessibilityGestureInjectPathParcelTest SetUp() End";
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilityGestureInjectPathParcelTest TearDown()";
        gestureInfoParcel_ = nullptr;
    }

    std::shared_ptr<AccessibilityGestureInjectPathParcel> gestureInfoParcel_ = nullptr;
};

/**
 * @tc.number: Gesture_Info_Marshalling_001
 * @tc.name: Gesture_Info_Marshalling
 * @tc.desc: Test function Marshalling
 */
HWTEST_F(AccessibilityGestureInjectPathParcelTest, Gesture_Info_Marshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Gesture_Info_Marshalling_001 start";
    EXPECT_TRUE(GesturgestureInfoParcel_e_Info);
   
    Parcel parcel;
    bool ret = GesturgestureInfoParcel_e_Info->Marshalling(parcel);
    EXPECT_EQ(ret, true);
    GTEST_LOG_(INFO) << "Gesture_Info_Marshalling_001 end";
}

/**
 * @tc.number: Gesture_Info_Unmarshalling_001
 * @tc.name: Gesture_Info_Unmarshalling
 * @tc.desc: Test function Unmarshalling
 */
HWTEST_F(AccessibilityGestureInjectPathParcelTest, Gesture_Info_Unmarshalling_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "Gesture_Info_Unmarshalling_001 start";
    EXPECT_TRUE(GesturgestureInfoParcel_e_Info);    

    Parcel parcel;
    sptr<AccessibilityGestureInjectPathParcel> gestureInfoParcel = GesturgestureInfoParcel_e_Info->Unmarshalling(parcel);
    EXPECT_EQ(true, gestureInfoParcel == nullptr);
    GTEST_LOG_(INFO) << "Gesture_Info_Unmarshalling_001 end";
}
} // namespace Accessibility
} // namespace OHOS