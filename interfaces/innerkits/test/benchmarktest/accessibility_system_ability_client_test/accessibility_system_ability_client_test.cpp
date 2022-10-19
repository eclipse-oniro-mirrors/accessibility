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
#include "accessibility_system_ability_client.h"

using namespace OHOS::Accessibility;
namespace {
    /**
     * @tc.name: BenchmarkTestForIsEnabled
     * @tc.desc: Testcase for testing 'IsEnabled' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForIsEnabled(benchmark::State &state)
    {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (!asaClient) {
            return;
        }
        bool isEnable = false;
        for (auto _ : state) {
            /* @tc.steps: step1.call IsEnabled in loop */
            (void)asaClient->IsEnabled(isEnable);
        }
    }

	/**
     * @tc.name: BenchmarkTestForGetEnabledAbilities
     * @tc.desc: Testcase for testing 'GetEnabledAbilities' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetEnabledAbilities(benchmark::State &state)
    {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (!asaClient) {
            return;
        }
        for (auto _ : state) {
            /* @tc.steps: step1.call GetEnabledAbilities in loop */
            std::vector<std::string> enabledAbilities;
            (void)asaClient->GetEnabledAbilities(enabledAbilities);
        }
    }

    /**
     * @tc.name: BenchmarkTestForSendEvent
     * @tc.desc: Testcase for testing 'SendEvent' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForSendEvent(benchmark::State &state)
    {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (!asaClient) {
            return;
        }
        for (auto _ : state) {
            /* @tc.steps: step1.call SendEvent in loop */
            AccessibilityEventInfo event;
            event.SetEventType(TYPE_VIEW_TEXT_UPDATE_EVENT);
            (void)asaClient->SendEvent(event);
        }
    }

    BENCHMARK(BenchmarkTestForIsEnabled)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForGetEnabledAbilities)->Iterations(1000)->ReportAggregatesOnly();
    BENCHMARK(BenchmarkTestForSendEvent)->Iterations(1000)->ReportAggregatesOnly();
}

BENCHMARK_MAIN();