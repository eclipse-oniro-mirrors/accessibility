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

#include "gesture_simulation.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
GesturePathPositionDefine::GesturePathPositionDefine(float positionX, float positionY)
{
    positionX_ = positionX;
    positionY_ = positionY;
}

float GesturePathPositionDefine::GetPositionX()
{
    return positionX_;
}

float GesturePathPositionDefine::GetPositionY()
{
    return positionY_;
}

void GesturePathPositionDefine::SetPositionX(float positionX)
{
    positionX_ = positionX;
}

void GesturePathPositionDefine::SetPositionY(float positionY)
{
    positionY_ = positionY;
}

bool GesturePathPositionDefine::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool GesturePathPositionDefine::Marshalling(Parcel &parcel) const
{
    return true;
}

GesturePathPositionDefine *GesturePathPositionDefine::Unmarshalling(Parcel &parcel)
{
    GesturePathPositionDefine *gesturePathPositionDefine = new GesturePathPositionDefine();
    return gesturePathPositionDefine;
}

GesturePathDefine::GesturePathDefine(GesturePathPositionDefine &startPosition,
    GesturePathPositionDefine &endPosition, int64_t durationTime)
{}

int64_t GesturePathDefine::GetDurationTime()
{
    return durationTime_;
}

GesturePathPositionDefine &GesturePathDefine::GetEndPosition()
{
    return endPosition_;
}

int64_t GesturePathDefine::GetMaxStrokeDuration()
{
    return MAX_STROKE_DURATION;
}

uint32_t GesturePathDefine::GetMaxStrokes()
{
    return MAX_STROKES;
}

GesturePathPositionDefine &GesturePathDefine::GetStartPosition()
{
    return startPosition_;
}

void GesturePathDefine::SetDurationTime(int64_t durationTime)
{
    durationTime_ = durationTime;
}

void GesturePathDefine::SetEndPosition(GesturePathPositionDefine &endPosition)
{
    endPosition_ = endPosition;
}

void GesturePathDefine::SetStartPosition(GesturePathPositionDefine &startPosition)
{
    startPosition_ = startPosition;
}

bool GesturePathDefine::ReadFromParcel(Parcel &parcel)
{
    return true;
}

bool GesturePathDefine::Marshalling(Parcel &parcel) const
{
    return true;
}

GesturePathDefine *GesturePathDefine::Unmarshalling(Parcel &parcel)
{
    GesturePathDefine *gesturePathDefine = new GesturePathDefine();
    return gesturePathDefine;
}
} // namespace Accessibility
} // namespace OHOS