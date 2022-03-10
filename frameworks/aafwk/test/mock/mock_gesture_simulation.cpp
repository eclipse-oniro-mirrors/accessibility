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

#include <memory>
#include <gtest/gtest.h>

#include "gesture_simulation.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"
#include "mock_gesture_simulation.h"

namespace OHOS {
namespace Accessibility {
GesturePathPositionDefine::GesturePathPositionDefine(float positionX, float positionY)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine";
}

float GesturePathPositionDefine::GetPositionX()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine GetPositionX";
    return positionX_;
}

float GesturePathPositionDefine::GetPositionY()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine GetPositionY";
    return positionY_;
}

void GesturePathPositionDefine::SetPositionX(float positionX)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine SetPositionX";
}

void GesturePathPositionDefine::SetPositionY(float positionY)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine SetPositionY";
}

bool GesturePathPositionDefine::ReadFromParcel(Parcel &parcel)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine ReadFromParcel";
    return true;
}

bool GesturePathPositionDefine::Marshalling(Parcel &parcel) const
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine Marshalling";
    return true;
}

GesturePathPositionDefine *GesturePathPositionDefine::Unmarshalling(Parcel &parcel)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathPositionDefine Unmarshalling";
    return nullptr;
}

GesturePathDefine::GesturePathDefine(GesturePathPositionDefine &startPosition,
    GesturePathPositionDefine &endPosition, int64_t durationTime)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine";
}

int64_t GesturePathDefine::GetDurationTime()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine GetDurationTime";
    return durationTime_;
}

GesturePathPositionDefine &GesturePathDefine::GetEndPosition()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine GetEndPosition";
    return endPosition_;
}

int64_t GesturePathDefine::GetMaxStrokeDuration()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine GetMaxStrokeDuration";
    return MAX_STROKE_DURATION;
}

uint32_t GesturePathDefine::GetMaxStrokes()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine GetMaxStrokes";
    return MAX_STROKES;
}

GesturePathPositionDefine &GesturePathDefine::GetStartPosition()
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine GetStartPosition";
    return startPosition_;
}

void GesturePathDefine::SetDurationTime(int64_t durationTime)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine SetDurationTime";
    durationTime_ = durationTime;
}

void GesturePathDefine::SetEndPosition(GesturePathPositionDefine &endPosition)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine SetEndPosition";
}

void GesturePathDefine::SetStartPosition(GesturePathPositionDefine &startPosition)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine SetStartPosition";
}

bool GesturePathDefine::ReadFromParcel(Parcel &parcel)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine ReadFromParcel";
    return true;
}

bool GesturePathDefine::Marshalling(Parcel &parcel) const
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine Marshalling";
    return true;
}

GesturePathDefine *GesturePathDefine::Unmarshalling(Parcel &parcel)
{
    GTEST_LOG_(INFO) << "MOCK GesturePathDefine Unmarshalling";
    return nullptr;
}

void MockGestureResultListener::OnGestureInjectResult(uint32_t sequence, bool result)
{
    GTEST_LOG_(INFO) << "MOCK MockGestureResultListener OnGestureInjectResult";
}
} // namespace Accessibility
} // namespace OHOS
