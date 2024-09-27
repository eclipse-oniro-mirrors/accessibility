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
#include "A11y_config.h"
#include "accesstoken_kit.h"
#include "ffrt_inner.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace OHOS::A11yConfig;
using namespace OHOS::Security::AccessToken;

namespace {
    class A11yConfigObserverImpl : public OHOS::A11yConfig::A11yConfigObserver {
    public:
        A11yConfigObserverImpl() = default;

        void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override
        {
            complete_.set_value();
        }

        void SetCompletePromise(ffrt::promise<void> &promise)
        {
            complete_ = std::move(promise);
        }

    private:
        ffrt::promise<void> complete_;
    };

    static bool g_flag = true;
    void AddPermission()
    {
        if (g_flag) {
            const char *perms[2];
            perms[0] = OHOS::A11y::OHOS_PERMISSION_READ_A11y_CONFIG.c_str();
            perms[1] = OHOS::A11y::OHOS_PERMISSION_WRITE_A11y_CONFIG.c_str();
            NativeTokenInfoParams infoInstance = {
                .dcapsNum = 0,
                .permsNum = 2,
                .aclsNum = 0,
                .dcaps = nullptr,
                .perms = perms,
                .acls = nullptr,
                .processName = "com.A11y.config.benchmark.test",
                .aplStr = "normal",
            };
            uint64_t tokenId = GetAccessTokenId(&infoInstance);
            SetSelfTokenID(tokenId);
            AccessTokenKit::ReloadNativeTokenInfo();
            g_flag = false;
        }
    }

    /**
     * @tc.name: BenchmarkTestForSetScreenMagnificationState
     * @tc.desc: Testcase for testing 'SetScreenMagnificationState' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSetScreenMagnificationState(benchmark::State &state)
    {
        bool value = false;
        std::shared_ptr<A11yConfigObserverImpl> configObserver =
            std::make_shared<A11yConfigObserverImpl>();
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();

        config.SubscribeConfigObserver(CONFIG_SCREEN_MAGNIFICATION, configObserver, false);
        config.GetScreenMagnificationState(value);
        for (auto _ : state) {
            /* @tc.steps: step1.call SetScreenMagnificationState in loop */
            ffrt::promise<void> complete;
            ffrt::future syncFuture = complete.get_future();
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
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();
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
        std::shared_ptr<A11yConfigObserverImpl> configObserver =
            std::make_shared<A11yConfigObserverImpl>();
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();

        config.SubscribeConfigObserver(CONFIG_SHORT_KEY_TARGET, configObserver, false);

        for (auto _ : state) {
            /* @tc.steps: step1.call SetShortkeyTarget in loop */
            ffrt::promise<void> complete;
            ffrt::future syncFuture = complete.get_future();
            configObserver->SetCompletePromise(complete);
            config.SetShortkeyTarget(nameStr);
            syncFuture.wait();
            if (!nameStr.compare("aaa")) {
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
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();
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
        std::shared_ptr<A11yConfigObserverImpl> configObserver =
            std::make_shared<A11yConfigObserverImpl>();
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();

        config.SubscribeConfigObserver(CONFIG_CONTENT_TIMEOUT, configObserver, false);
        config.GetContentTimeout(value);
        for (auto _ : state) {
            /* @tc.steps: step1.call SetContentTimeout in loop */
            value++;
            ffrt::promise<void> complete;
            ffrt::future syncFuture = complete.get_future();
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
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();
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
        std::shared_ptr<A11yConfigObserverImpl> configObserver =
            std::make_shared<A11yConfigObserverImpl>();
        auto &config = OHOS::A11yConfig::A11yConfig::GetInstance();
        (void)config.InitializeContext();
        AddPermission();

        for (auto _ : state) {
            /* @tc.steps: step1.call SubscribeConfigObserver in loop */
            config.SubscribeConfigObserver(CONFIG_SCREEN_MAGNIFICATION, configObserver, false);
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