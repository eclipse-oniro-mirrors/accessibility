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

#include <benchmark/benchmark.h>
#include "accessibility_ui_test_ability.h"
#include "accessible_ability_listener.h"
#include "ffrt_inner.h"

using namespace OHOS::Accessibility;

namespace {
    class AccessibleAbilityListenerImpl : public AccessibleAbilityListener {
    public:
        AccessibleAbilityListenerImpl() = default;
        ~AccessibleAbilityListenerImpl() = default;

        void OnAbilityConnected() override
        {
            complete_.set_value();
        }

        void OnAbilityDisconnected() override
        {
            complete_.set_value();
        }

        void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override {}

        bool OnKeyPressEvent(const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent) override
        {
            return false;
        }

        void SetCompletePromise(ffrt::promise<void> &promise)
        {
            complete_ = std::move(promise);
        }

    private:
        ffrt::promise<void> complete_;
    };

    class AccessibleAbilityClientTest : public benchmark::Fixture {
    public:
        AccessibleAbilityClientTest()
        {
            Iterations(iterations);
            Repetitions(repetitions);
            ReportAggregatesOnly();
        }

        ~AccessibleAbilityClientTest() override = default;

        void SetUp(const ::benchmark::State &state) override;
        void TearDown(const ::benchmark::State &state) override;

    protected:
        const int32_t repetitions = 3;
        const int32_t iterations = 1000;
        std::shared_ptr<AccessibleAbilityListenerImpl> listener_ = nullptr;
    };

    void AccessibleAbilityClientTest::SetUp(const ::benchmark::State &state)
    {
        listener_ = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener_);

        // Connect
        ffrt::promise<void> connected;
        ffrt::future syncFuture = connected.get_future();
        listener_->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();
    }

    void AccessibleAbilityClientTest::TearDown(const ::benchmark::State &state)
    {
        // Disconnect
        ffrt::promise<void> disconnect;
        ffrt::future syncFuture = disconnect.get_future();
        listener_->SetCompletePromise(disconnect);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: GetRootTestCase
     * @tc.desc: Testcase for testing 'GetRoot' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number. Set ace command first before test.
     */
    BENCHMARK_F(AccessibleAbilityClientTest, GetRootTestCase)(
        benchmark::State &state)
    {
        while (state.KeepRunning()) {
            AccessibilityElementInfo elementInfo;
            int32_t result = AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);
            if (result != 0) {
                state.SkipWithError("GetRoot failed. Maybe there is no command to set ace");
            }
        }
    }

    /**
     * @tc.name: GetWindowsTestCase
     * @tc.desc: Testcase for testing 'GetWindows' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    BENCHMARK_F(AccessibleAbilityClientTest, GetWindowsTestCase)(
        benchmark::State &state)
    {
        while (state.KeepRunning()) {
            std::vector<AccessibilityWindowInfo> windows;
            int32_t result = AccessibilityUITestAbility::GetInstance()->GetWindows(windows);
            if (result != 0) {
                state.SkipWithError("GetWindows failed.");
            }
        }
    }

    /**
     * @tc.name: GetWindowTestCase
     * @tc.desc: Testcase for testing 'GetWindow' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    BENCHMARK_F(AccessibleAbilityClientTest, GetWindowTestCase)(
        benchmark::State &state)
    {
        while (state.KeepRunning()) {
            int32_t windowId = 0;
            AccessibilityWindowInfo windowInfo;
            AccessibilityUITestAbility::GetInstance()->GetWindow(windowId, windowInfo);
        }
    }

    /**
     * @tc.name: InjectGestureTestCase
     * @tc.desc: Testcase for testing 'InjectGesture' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    BENCHMARK_F(AccessibleAbilityClientTest, InjectGestureTestCase)(
        benchmark::State &state)
    {
        while (state.KeepRunning()) {
            std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
                std::make_shared<AccessibilityGestureInjectPath>();
            int64_t durationTime = 20;
            gesturePath->SetDurationTime(durationTime);
            AccessibilityGesturePosition position = {200.0f, 200.0f};
            gesturePath->AddPosition(position);
            int32_t result = AccessibilityUITestAbility::GetInstance()->InjectGesture(gesturePath);
            if (result != 0) {
                state.SkipWithError("InjectGesture failed.");
            }
        }
    }
}

BENCHMARK_MAIN();