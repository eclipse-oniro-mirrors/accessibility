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
#include <future>
#include "accessibility_ui_test_ability.h"
#include "accessible_ability_listener.h"

using namespace OHOS::Accessibility;

namespace {
    const std::string APP_NAME = "计算器";

    class AccessibleAbilityListenerImpl : public AccessibleAbilityListener {
    public:
        AccessibleAbilityListenerImpl() = default;
        ~AccessibleAbilityListenerImpl() = default;

        virtual void OnAbilityConnected() override
        {
            complete_.set_value();
        }

        virtual void OnAbilityDisconnected() override
        {
            complete_.set_value();
        }

        virtual void OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo) override
        {
            if (eventInfo.GetEventType() == EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT) {
                complete_.set_value();
            }
        }

        virtual bool OnKeyPressEvent(const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent) override
        {
            return false;
        }

        void SetCompletePromise(std::promise<void> &promise)
        {
            complete_ = std::move(promise);
        }

    private:
        std::promise<void> complete_;
    };

    class AccessibilityGestureResultListenerForBenchmark : public AccessibilityGestureResultListener {
    public:
        virtual ~AccessibilityGestureResultListenerForBenchmark() = default;
        void OnGestureInjectResult(uint32_t sequence, bool result) override {}
    };

    /**
     * @tc.name: BenchmarkTestForConnect
     * @tc.desc: Testcase for testing 'Connect' function and 'Disconnect' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForConnect(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        for (auto _ : state) {
            // Connect
            std::promise<void> connected;
            std::future syncFuture = connected.get_future();
            listener->SetCompletePromise(connected);
            AccessibilityUITestAbility::GetInstance()->Connect();
            syncFuture.wait();

            // Disconnect
            std::promise<void> disconnected;
            syncFuture = disconnected.get_future();
            listener->SetCompletePromise(disconnected);
            AccessibilityUITestAbility::GetInstance()->Disconnect();
            syncFuture.wait();
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetRoot
     * @tc.desc: Testcase for testing 'GetRoot' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetRoot(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        for (auto _ : state) {
            AccessibilityElementInfo elementInfo;
            AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetWindows
     * @tc.desc: Testcase for testing 'GetWindows' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetWindows(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        for (auto _ : state) {
            std::vector<AccessibilityWindowInfo> windows;
            AccessibilityUITestAbility::GetInstance()->GetWindows(windows);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetWindow
     * @tc.desc: Testcase for testing 'GetWindow' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetWindow(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        std::vector<AccessibilityWindowInfo> windows;
        AccessibilityUITestAbility::GetInstance()->GetWindows(windows);
        if (windows.empty()) {
            return;
        }
        int32_t windowId = windows[0].GetWindowId();

        for (auto _ : state) {
            AccessibilityWindowInfo windowInfo;
            AccessibilityUITestAbility::GetInstance()->GetWindow(windowId, windowInfo);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetChildren
     * @tc.desc: Testcase for testing 'GetChildren' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetChildren(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        AccessibilityElementInfo elementInfo;
        AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);

        for (auto _ : state) {
            std::vector<AccessibilityElementInfo> children;
            AccessibilityUITestAbility::GetInstance()->GetChildren(elementInfo, children);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForSetEventTypeFilter
     * @tc.desc: Testcase for testing 'SetEventTypeFilter' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSetEventTypeFilter(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        for (auto _ : state) {
            uint32_t filter = TYPES_ALL_MASK;
            AccessibilityUITestAbility::GetInstance()->SetEventTypeFilter(filter);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetByContent
     * @tc.desc: Testcase for testing 'GetByContent' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetByContent(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        AccessibilityElementInfo elementInfo;
        AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);

        for (auto _ : state) {
            std::vector<AccessibilityElementInfo> resultElementInfos;
            AccessibilityUITestAbility::GetInstance()->GetByContent(elementInfo, APP_NAME, resultElementInfos);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForExecuteAction
     * @tc.desc: Testcase for testing 'ExecuteAction' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForExecuteAction(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        AccessibilityElementInfo elementInfo;
        AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);

        std::vector<AccessibilityElementInfo> resultElementInfos;
        AccessibilityUITestAbility::GetInstance()->GetByContent(elementInfo, APP_NAME, resultElementInfos);
        if (resultElementInfos.empty()) {
            return;
        }

        for (auto _ : state) {
            std::promise<void> event;
            std::future syncFuture = event.get_future();
            listener->SetCompletePromise(event);
            std::map<std::string, std::string> actionArguments;
            AccessibilityUITestAbility::GetInstance()->ExecuteAction(resultElementInfos[0],
                ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, actionArguments);
            syncFuture.wait();
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetFocus
     * @tc.desc: Testcase for testing 'GetFocus' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetFocus(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        AccessibilityElementInfo elementInfo;
        AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);

        std::vector<AccessibilityElementInfo> resultElementInfos;
        AccessibilityUITestAbility::GetInstance()->GetByContent(elementInfo, APP_NAME, resultElementInfos);
        if (resultElementInfos.empty()) {
            return;
        }

        std::promise<void> event;
        syncFuture = event.get_future();
        listener->SetCompletePromise(event);
        std::map<std::string, std::string> actionArguments;
        AccessibilityUITestAbility::GetInstance()->ExecuteAction(resultElementInfos[0],
            ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, actionArguments);
        syncFuture.wait();

        for (auto _ : state) {
            AccessibilityElementInfo foucsElementInfo;
            AccessibilityUITestAbility::GetInstance()->GetFocus(FOCUS_TYPE_ACCESSIBILITY, foucsElementInfo);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForGetNext
     * @tc.desc: Testcase for testing 'GetNext' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetNext(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        AccessibilityElementInfo elementInfo;
        AccessibilityUITestAbility::GetInstance()->GetRoot(elementInfo);

        std::vector<AccessibilityElementInfo> resultElementInfos;
        AccessibilityUITestAbility::GetInstance()->GetByContent(elementInfo, APP_NAME, resultElementInfos);
        if (resultElementInfos.empty()) {
            return;
        }

        std::promise<void> event;
        syncFuture = event.get_future();
        listener->SetCompletePromise(event);
        std::map<std::string, std::string> actionArguments;
        AccessibilityUITestAbility::GetInstance()->ExecuteAction(resultElementInfos[0],
            ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, actionArguments);
        syncFuture.wait();

        AccessibilityElementInfo foucsElementInfo;
        AccessibilityUITestAbility::GetInstance()->GetFocus(FOCUS_TYPE_ACCESSIBILITY, foucsElementInfo);

        for (auto _ : state) {
            AccessibilityElementInfo nextElementInfo;
            AccessibilityUITestAbility::GetInstance()->GetNext(foucsElementInfo,
                FocusMoveDirection::UP, nextElementInfo);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    /**
     * @tc.name: BenchmarkTestForInjectGesture
     * @tc.desc: Testcase for testing 'InjectGesture' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForInjectGesture(benchmark::State &state)
    {
        std::shared_ptr<AccessibleAbilityListenerImpl> listener = std::make_shared<AccessibleAbilityListenerImpl>();
        AccessibilityUITestAbility::GetInstance()->RegisterAbilityListener(listener);

        // Connect
        std::promise<void> connected;
        std::future syncFuture = connected.get_future();
        listener->SetCompletePromise(connected);
        AccessibilityUITestAbility::GetInstance()->Connect();
        syncFuture.wait();

        uint32_t sequence = 0;
        for (auto _ : state) {
            sequence++;
            std::shared_ptr<AccessibilityGestureInjectPath> gesturePath =
                std::make_shared<AccessibilityGestureInjectPath>();
            int64_t durationTime = 20;
            gesturePath->SetDurationTime(durationTime);
            AccessibilityGesturePosition position = {200.0f, 200.0f};
            gesturePath->AddPosition(position);

            std::shared_ptr<AccessibilityGestureResultListenerForBenchmark> listener =
                std::make_shared<AccessibilityGestureResultListenerForBenchmark>();
            AccessibilityUITestAbility::GetInstance()->InjectGesture(sequence, gesturePath, listener);
        }

        // Disconnect
        std::promise<void> disconnected;
        syncFuture = disconnected.get_future();
        listener->SetCompletePromise(disconnected);
        AccessibilityUITestAbility::GetInstance()->Disconnect();
        syncFuture.wait();
    }

    BENCHMARK(BenchmarkTestForConnect)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetRoot)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetWindows)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetWindow)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetChildren)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForSetEventTypeFilter)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetByContent)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForExecuteAction)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetFocus)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetNext)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForInjectGesture)->Iterations(1000)->ReportAggregatesOnly();
}

BENCHMARK_MAIN();