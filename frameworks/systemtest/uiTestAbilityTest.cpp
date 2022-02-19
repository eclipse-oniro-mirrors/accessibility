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

#include <gtest/gtest.h>
#include <future>
#include <memory>

#include "accessibility_ui_test_ability.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
class UITestAbilityTest : public testing::Test {
public:

    UITestAbilityTest() {}
    ~UITestAbilityTest() {}

    void SetUp() override;
    void TearDown() override;

public:
    class UITestAbilityListenerImpl : public IAccessibleUITestAbilityListener {
    public:
        virtual ~UITestAbilityListenerImpl() = default;

        virtual void OnAbilityConnected() override
        {
            std::printf("%s \n", __FUNCTION__);
            connected_ = true;
            // promise_.set_value();
        }
        virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override
        {
            std::printf("%s \n", __FUNCTION__);
        }
        virtual bool OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int sequence) override
        {
            std::printf("%s \n", __FUNCTION__);
            return true;
        }
        virtual void OnGestureSimulateResult(const int sequence, const bool completedSuccessfully) override
        {
            std::printf("%s \n", __FUNCTION__);
        }

        bool connected_ = false;
        // std::promise<void> promise_;
    };

public:
    std::shared_ptr<UITestAbilityListenerImpl> listener_ = nullptr;
};

void UITestAbilityTest::SetUp()
{
    GTEST_LOG_(INFO) << "UITestAbilityTest SetUp";

    listener_ = std::make_shared<UITestAbilityListenerImpl>();
}

void UITestAbilityTest::TearDown()
{
    GTEST_LOG_(INFO) << "UITestAbilityTest TearDown";
    listener_ = nullptr;
}

HWTEST_F(UITestAbilityTest, Extension_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UITestAbilityTest Extension_001 start";

    bool ret = AccessibilityUITestAbility::GetInstance().RegisterListener(listener_);
    EXPECT_EQ(ret, true);

    ret = AccessibilityUITestAbility::GetInstance().Connect();
    EXPECT_EQ(ret, true);

    // std::promise<void> connectPromise;
    // std::future<void> connectFuture = connectPromise.get_future();
    // listener_->promise_ = std::move(connectPromise);
    // connectFuture.wait();
    sleep(3);

    EXPECT_EQ(listener_->connected_, true);

    ret = AccessibilityUITestAbility::GetInstance().Disconnect();
    EXPECT_EQ(ret, true);

    GTEST_LOG_(INFO) << "UITestAbilityTest Extension_001 end";
}
} // namespace Accessibility
} // namespace OHOS