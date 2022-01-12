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
#include "accessible_ability_manager_service_state_stub.h"
#include "accessible_ability_manager_service_state_proxy.h"
#include "accessibility_system_ability_client.h"

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace Accessibility {
class ASACStateProxyUnitTest : public ::testing::Test {
public:
    sptr<AccessibleAbilityManagerServiceStateStub> mockStub_ = nullptr;
    sptr<AccessibleAbilityManagerServiceStateProxy> mockProxy_ = nullptr;

    ASACStateProxyUnitTest() {}
    ~ASACStateProxyUnitTest() {}

    static void SetUpTestCase() {
        GTEST_LOG_(INFO) << "ASACStateProxyUnitTest Start";
    }
    static void TearDownTestCase() {
        GTEST_LOG_(INFO) << "ASACStateProxyUnitTest End";
    }
    void SetUp() {
        GTEST_LOG_(INFO) << "ASACStateProxyUnitTest SetUp()";
        mockStub_ = new AccessibleAbilityManagerServiceStateStub();
        mockProxy_ = new AccessibleAbilityManagerServiceStateProxy(mockStub_);
    };
    void TearDown() {
        GTEST_LOG_(INFO) << "ASACStateProxyUnitTest TearDown()";
        mockStub_ = nullptr;
    }

    void WriteInterfaceToken(MessageParcel &data) {
        GTEST_LOG_(INFO) << "ASACStateProxyUnitTest WriteInterfaceToken";
        data.WriteInterfaceToken(AccessibleAbilityManagerServiceStateStub::GetDescriptor());
    }    
};

/**
 * @tc.number: OnStateChanged_001
 * @tc.name: OnStateChanged
 * @tc.desc: Test function OnStateChanged
 */
HWTEST_F(ASACStateProxyUnitTest, OnStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnStateChanged_001 start";

    mockProxy_->OnStateChanged(static_cast<uint32_t>(IAccessibleAbilityManagerServiceState::Message::ON_STATE_CHANGED));

    GTEST_LOG_(INFO) << "OnStateChanged_001 end";
}

} //namespace Accessibility
} //namespace OHOS