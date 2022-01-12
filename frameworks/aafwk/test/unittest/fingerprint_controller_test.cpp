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
#include "fingerprint_controller.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Accessibility;
using namespace std;

class UnitTestFingerprintListener : public FingerprintListener {
public:
    UnitTestFingerprintListener() {}
    virtual ~UnitTestFingerprintListener() {}

    void OnFingerprintGestureStatusChanged(bool isActive) override
    {
        GTEST_LOG_(INFO) << "UnitTestFingerprintListener OnFingerprintGestureStatusChanged";
    }

    void OnFingerprintGesture(uint32_t gesture) override
    {
        GTEST_LOG_(INFO) << "UnitTestFingerprintListener OnFingerprintGesture";
    }
};

class FingerprintControllerUnitTest : public ::testing::Test {
public:
    FingerprintControllerUnitTest() {}
    ~FingerprintControllerUnitTest() {}

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    shared_ptr<FingerprintController> fingerprintController_ = nullptr;
};

void FingerprintControllerUnitTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "###################### FingerprintControllerUnitTest Start ######################";
}

void FingerprintControllerUnitTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "###################### FingerprintControllerUnitTest End ######################";
}

void FingerprintControllerUnitTest::SetUp()
{
    GTEST_LOG_(INFO) << "UT SetUp";
    fingerprintController_ = make_shared<FingerprintController>(0);
}

void FingerprintControllerUnitTest::TearDown()
{
    GTEST_LOG_(INFO) << "UT TearDown";
    fingerprintController_ = nullptr;
}

/**
 * @tc.number: FingerprintController_Unittest_IsFingerprintGestureDetectionValid_001
 * @tc.name: IsFingerprintGestureDetectionValid
 * @tc.desc: Test function IsFingerprintGestureDetectionValid
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_IsFingerprintGestureDetectionValid_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_IsFingerprintGestureDetectionValid_001 start";

    EXPECT_TRUE(fingerprintController_->IsFingerprintGestureDetectionValid());

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_IsFingerprintGestureDetectionValid_001 end";
}

/**
 * @tc.number: FingerprintController_Unittest_AddFingerprintListener_001
 * @tc.name: AddFingerprintListener
 * @tc.desc: Test function AddFingerprintListener
 *           Add a register which is null.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_AddFingerprintListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_AddFingerprintListener_001 start";

    shared_ptr<FingerprintListener> listener = nullptr;
    fingerprintController_->AddFingerprintListener(listener);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_AddFingerprintListener_001 end";
}

/**
 * @tc.number: FingerprintController_Unittest_AddFingerprintListener_002
 * @tc.name: AddFingerprintListener
 * @tc.desc: Test function AddFingerprintListener
 *           Add a register which is not null.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_AddFingerprintListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_AddFingerprintListener_002 start";

    shared_ptr<FingerprintListener> listener = make_shared<UnitTestFingerprintListener>();
    fingerprintController_->AddFingerprintListener(listener);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_AddFingerprintListener_002 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DeleteFingerprintListener_001
 * @tc.name: DeleteFingerprintListener
 * @tc.desc: Test function DeleteFingerprintListener
 *           Delete a register which is null.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DeleteFingerprintListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_001 start";

    shared_ptr<FingerprintListener> listener = nullptr;
    fingerprintController_->DeleteFingerprintListener(listener);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_001 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DeleteFingerprintListener_002
 * @tc.name: DeleteFingerprintListener
 * @tc.desc: Test function DeleteFingerprintListener
 *           Delete a register which hasn't been added.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DeleteFingerprintListener_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_002 start";

    shared_ptr<FingerprintListener> listener = make_shared<UnitTestFingerprintListener>();
    fingerprintController_->DeleteFingerprintListener(listener);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_002 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DeleteFingerprintListener_003
 * @tc.name: DeleteFingerprintListener
 * @tc.desc: Test function DeleteFingerprintListener
 *           Delete a register which has been added.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DeleteFingerprintListener_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_003 start";

    shared_ptr<FingerprintListener> listener = make_shared<UnitTestFingerprintListener>();
    fingerprintController_->AddFingerprintListener(listener);
    fingerprintController_->DeleteFingerprintListener(listener);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DeleteFingerprintListener_003 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_001
 * @tc.name: DispatchOnFingerprintGestureStatusChanged
 * @tc.desc: Test function DispatchOnFingerprintGestureStatusChanged
 *           Dispatch result without register.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_001 start";

    bool isActive = true;
    fingerprintController_->DispatchOnFingerprintGestureStatusChanged(isActive);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_001 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_002
 * @tc.name: DispatchOnFingerprintGestureStatusChanged
 * @tc.desc: Test function DispatchOnFingerprintGestureStatusChanged
 *           Dispatch result with register.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_002 start";

    bool isActive = true;
    shared_ptr<FingerprintListener> listener = make_shared<UnitTestFingerprintListener>();

    fingerprintController_->AddFingerprintListener(listener);
    fingerprintController_->DispatchOnFingerprintGestureStatusChanged(isActive);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGestureStatusChanged_002 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DispatchOnFingerprintGesture_001
 * @tc.name: DispatchOnFingerprintGesture
 * @tc.desc: Test function DispatchOnFingerprintGesture
 *           Dispatch result without register.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DispatchOnFingerprintGesture_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGesture_001 start";

    uint32_t gesture = 1;
    fingerprintController_->DispatchOnFingerprintGesture(gesture);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGesture_001 end";
}

/**
 * @tc.number: FingerprintController_Unittest_DispatchOnFingerprintGesture_002
 * @tc.name: DispatchOnFingerprintGesture
 * @tc.desc: Test function DispatchOnFingerprintGesture
 *           Dispatch result with register.
 */
HWTEST_F(FingerprintControllerUnitTest, FingerprintController_Unittest_DispatchOnFingerprintGesture_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGesture_002 start";

    uint32_t gesture = 1;
    shared_ptr<FingerprintListener> listener = make_shared<UnitTestFingerprintListener>();

    fingerprintController_->AddFingerprintListener(listener);
    fingerprintController_->DispatchOnFingerprintGesture(gesture);

    GTEST_LOG_(INFO) << "FingerprintController_Unittest_DispatchOnFingerprintGesture_002 end";
}