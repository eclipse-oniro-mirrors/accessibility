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

#include <memory>
#include "gesture_simulation.h"
#include "hilog_wrapper.h"
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {

GesturePathPositionDefine::GesturePathPositionDefine(float positionX, float positionY)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    positionX_ = positionX;
    positionY_ = positionY;
}

float GesturePathPositionDefine::GetPositionX()
{
    HILOG_DEBUG("%{public}s start and positionX_ is %{public}f", __func__, positionX_);
    return positionX_;
}

float GesturePathPositionDefine::GetPositionY()
{
    HILOG_DEBUG("%{public}s start and positionY_ is %{public}f", __func__, positionY_);
    return positionY_;
}

void GesturePathPositionDefine::SetPositionX(float positionX)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    positionX_ = positionX;
}

void GesturePathPositionDefine::SetPositionY(float positionY)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    positionY_ = positionY;
}

bool GesturePathPositionDefine::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, positionX_);
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, positionY_);
    return true;
}

bool GesturePathPositionDefine::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, positionX_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Float, parcel, positionY_);
    return true;
}

GesturePathPositionDefine *GesturePathPositionDefine::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    GesturePathPositionDefine *gesturePathPositionDefine = new GesturePathPositionDefine();

    if (gesturePathPositionDefine && !gesturePathPositionDefine->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel GesturePathPositionDefine failed.");
        delete gesturePathPositionDefine;
        gesturePathPositionDefine = nullptr;
    }

    return gesturePathPositionDefine;
}

GesturePathDefine::GesturePathDefine(GesturePathPositionDefine &startPosition,
                                    GesturePathPositionDefine &endPosition, uint32_t durationTime)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (startPosition.GetPositionX() < 0 || startPosition.GetPositionY() < 0) {
        HILOG_ERROR("startPosition: (%{public}f , %{public}f) is not allowed.",
            startPosition.GetPositionX(), startPosition.GetPositionY());
        return;
    }
    startPosition_ = startPosition;

    if (endPosition.GetPositionX() < 0 || endPosition.GetPositionY() < 0) {
        HILOG_ERROR("endPosition : (%{public}f , %{public}f) is not allowed.",
            endPosition.GetPositionX(), endPosition.GetPositionY());
        return;
    }
    endPosition_ = endPosition;

    durationTime_ = durationTime;
}

uint32_t GesturePathDefine::GetDurationTime()
{
    HILOG_DEBUG("%{public}s start and max stroke duration is %{public}d", __func__, durationTime_);
    return durationTime_;
}

GesturePathPositionDefine &GesturePathDefine::GetEndPosition()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return endPosition_;
}

uint32_t GesturePathDefine::GetMaxStrokeDuration()
{
    HILOG_DEBUG("%{public}s start and max stroke duration is %{public}d", __func__, MAX_STROKE_DURATION);
    return MAX_STROKE_DURATION;
}

uint32_t GesturePathDefine::GetMaxStrokes()
{
    HILOG_DEBUG("%{public}s start and max strokes is %{public}d", __func__, MAX_STROKES);
    return MAX_STROKES;
}

GesturePathPositionDefine &GesturePathDefine::GetStartPosition()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return startPosition_;
}

void GesturePathDefine::SetDurationTime(uint32_t durationTime)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    durationTime_ = durationTime;
}

void GesturePathDefine::SetEndPosition(GesturePathPositionDefine &endPosition)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    endPosition_ = endPosition;
}

void GesturePathDefine::SetStartPosition(GesturePathPositionDefine &startPosition)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    startPosition_ = startPosition;
}

bool GesturePathDefine::ReadFromParcel(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    std::unique_ptr<GesturePathPositionDefine> startPosition(parcel.ReadParcelable<GesturePathPositionDefine>());
    if (!startPosition) {
        HILOG_ERROR("ReadParcelable startPosition failed.");
        return false;
    }
    startPosition_ = *startPosition;

    std::unique_ptr<GesturePathPositionDefine> endPosition(parcel.ReadParcelable<GesturePathPositionDefine>());
    if (!endPosition) {
        HILOG_ERROR("ReadParcelable endPosition failed.");
        return false;
    }
    endPosition_ = *endPosition;

    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, durationTime_);

    return true;
}

bool GesturePathDefine::Marshalling(Parcel &parcel) const
{
    HILOG_DEBUG("%{public}s start.", __func__);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &startPosition_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &endPosition_);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Uint32, parcel, durationTime_);
    return true;
}

GesturePathDefine *GesturePathDefine::Unmarshalling(Parcel &parcel)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    GesturePathDefine *gesturePathDefine = new GesturePathDefine();

    if (gesturePathDefine && !gesturePathDefine->ReadFromParcel(parcel)) {
        HILOG_ERROR("ReadFromParcel GesturePathDefine failed.");
        delete gesturePathDefine;
        gesturePathDefine = nullptr;
    }

    return gesturePathDefine;
}

GestureResultListenerInfo::GestureResultListenerInfo(std::vector<GesturePathDefine> gesturePathDefine,
                                                    std::shared_ptr<GestureResultListener>& gestureResultListener)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (gesturePathDefine.empty()) {
        HILOG_ERROR("There is no gesture path.");
        gestureResultListener = nullptr;
    } else {
        for (auto gesturePath : gesturePathDefine) {
            gesturePathDefine_.push_back(gesturePath);
        }
        gestureResultListener_ = gestureResultListener;
    }
}

std::shared_ptr<GestureResultListener>& GestureResultListenerInfo::GetGestureResultListener()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return gestureResultListener_;
}

std::vector<GesturePathDefine> GestureResultListenerInfo::GetGesturePathDefine()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return gesturePathDefine_;
}

} // namespace Accessibility
} // namespace OHOS
