/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MOCK_GESTURE_SIMULATION_H
#define MOCK_GESTURE_SIMULATION_H

#include <vector>
#include <stdint.h>
#include "parcel_util.h"
#include "gesture_simulation.h"

namespace OHOS {
namespace Accessibility {

class MockGestureResultListener : public GestureResultListener {
public:
    MockGestureResultListener() {}
    virtual ~MockGestureResultListener() {}
    void OnCancelled(std::vector<GesturePathDefine> &gesturePathDefine) override;
    void OnCompleted(std::vector<GesturePathDefine> &gesturePathDefine) override;
};

} // namespace Accessibility
} // namespace OHOS

#endif // MOCK_GESTURE_SIMULATION_H