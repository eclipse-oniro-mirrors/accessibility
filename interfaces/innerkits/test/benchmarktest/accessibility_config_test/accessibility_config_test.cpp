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
#include "accessibility_config.h"

using namespace OHOS::AccessibilityConfig;

namespace {
    class AccessibilityConfigObserverImpl : public OHOS::AccessibilityConfig::AccessibilityConfigObserver {
    public:
        AccessibilityConfigObserverImpl() = default;

        virtual void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
        {
            complete_.set_value();
        }

        void SetCompletePromise(std::promise<void> &promise)
        {
            complete_ = std::move(promise);
        }

    private:
        std::promise<void> complete_;
    };

    /**
     * @tc.name: BenchmarkTestForSetScreenMagnificationState
     * @tc.desc: Testcase for testing 'SetScreenMagnificationState' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSetScreenMagnificationState(benchmark::State &state)
    {
        bool value = false;
        std::shared_ptr<AccessibilityConfigObserverImpl> configObserver =
            std::make_shared<AccessibilityConfigObserverImpl>();
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();

        config.SubscribeConfigObserver(CONFIG_SCREEN_MAGNIFICATION, configObserver);
        config.GetScreenMagnificationState(value);
        for (auto _ : state) {
            /* @tc.steps: step1.call SetScreenMagnificationState in loop */
            std::promise<void> complete;
            std::future syncFuture = complete.get_future();
            configObserver->SetCompletePromise(complete);

            config.SetScreenMagnificationState(!value);
            syncFuture.wait();
            value = !value;
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetScreenMagnificationState
     * @tc.desc: Testcase for testing 'GetScreenMagnificationState' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetScreenMagnificationState(benchmark::State &state)
    {
        bool value = false;
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
        for (auto _ : state) {
            /* @tc.steps: step1.call GetScreenMagnificationState in loop */
            (void)config.GetScreenMagnificationState(value);
        }
    }

    /**
     * @tc.name: BenchmarkTestForSetShortkeyTarget
     * @tc.desc: Testcase for testing 'SetShortkeyTarget' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSetShortkeyTarget(benchmark::State &state)
    {
        std::string nameStr = "aaa";
        std::shared_ptr<AccessibilityConfigObserverImpl> configObserver =
            std::make_shared<AccessibilityConfigObserverImpl>();
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();

        config.SubscribeConfigObserver(CONFIG_SHORT_KEY_TARGET, configObserver);

        for (auto _ : state) {
            /* @tc.steps: step1.call SetShortkeyTarget in loop */
            std::promise<void> complete;
            std::future syncFuture = complete.get_future();
            configObserver->SetCompletePromise(complete);
            config.SetShortkeyTarget(nameStr);
            syncFuture.wait();
            if (!strcmp(nameStr.c_str(), "aaa")) {
                nameStr = "bbb";
            } else {
                nameStr = "aaa";
            }
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetShortkeyTarget
     * @tc.desc: Testcase for testing 'GetShortkeyTarget' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetShortkeyTarget(benchmark::State &state)
    {
        std::string value = "";
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
        for (auto _ : state) {
            /* @tc.steps: step1.call GetShortkeyTarget in loop */
            (void)config.GetShortkeyTarget(value);
        }
    }

        /**
     * @tc.name: BenchmarkTestForSetContentTimeout
     * @tc.desc: Testcase for testing 'SetContentTimeout' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSetContentTimeout(benchmark::State &state)
    {
        uint32_t value = 0;
        std::shared_ptr<AccessibilityConfigObserverImpl> configObserver =
            std::make_shared<AccessibilityConfigObserverImpl>();
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();

        config.SubscribeConfigObserver(CONFIG_CONTENT_TIMEOUT, configObserver);
        config.GetContentTimeout(value);
        for (auto _ : state) {
            /* @tc.steps: step1.call SetContentTimeout in loop */
            value++;
            std::promise<void> complete;
            std::future syncFuture = complete.get_future();
            configObserver->SetCompletePromise(complete);

            config.SetContentTimeout(value);
            syncFuture.wait();
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetContentTimeout
     * @tc.desc: Testcase for testing 'GetContentTimeout' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetContentTimeout(benchmark::State &state)
    {
        uint32_t value = 0;
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();
        for (auto _ : state) {
            /* @tc.steps: step1.call GetContentTimeout in loop */
            (void)config.GetContentTimeout(value);
        }
    }

    /**
     * @tc.name: BenchmarkTestForSubscribeConfigObserver
     * @tc.desc: Testcase for testing 'SubscribeConfigObserver' and 'UnsubscribeConfigObserver' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSubscribeConfigObserver(benchmark::State &state)
    {
        std::shared_ptr<AccessibilityConfigObserverImpl> configObserver =
            std::make_shared<AccessibilityConfigObserverImpl>();
        auto &config = OHOS::Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();

        for (auto _ : state) {
            /* @tc.steps: step1.call SubscribeConfigObserver in loop */
            config.SubscribeConfigObserver(CONFIG_SCREEN_MAGNIFICATION, configObserver);
            config.UnsubscribeConfigObserver(CONFIG_SCREEN_MAGNIFICATION, configObserver);
        }
    }

    BENCHMARK(BenchmarkTestForSetScreenMagnificationState)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetScreenMagnificationState)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForSetShortkeyTarget)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetShortkeyTarget)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForSetContentTimeout)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetContentTimeout)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForSubscribeConfigObserver)->Iterations(1000)->ReportAggregatesOnly();
}

BENCHMARK_MAIN();