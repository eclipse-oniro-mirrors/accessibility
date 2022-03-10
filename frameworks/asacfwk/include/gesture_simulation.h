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

#ifndef GESTURE_SIMULATION_H
#define GESTURE_SIMULATION_H

#include <cstdint>
#include <vector>
#include "parcel_util.h"

namespace OHOS {
namespace Accessibility {
class GesturePathPositionDefine : public Parcelable {
public:
    GesturePathPositionDefine() {}
    ~GesturePathPositionDefine() {}

    GesturePathPositionDefine(float positionX, float positionY);

    /**
     * @brief Obtains the X coordinate of the position.
     * @param
     * @return Return the X coordinate of the position.
     */
    float GetPositionX();

    /**
     * @brief Obtains the Y coordinate of the position.
     * @param
     * @return Return the Y coordinate of the position.
     */
    float GetPositionY();

    /**
     * @brief Sets the X coordinate of the position.
     * @param positionX The X coordinate of the position.
     * @return
     */
    void SetPositionX(float positionX);

    /**
     * @brief Sets the Y coordinate of the position.
     * @param positionX The Y coordinate of the position.
     * @return
     */
    void SetPositionY(float positionY);

    /**
     * @brief read this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return true if read successfully, else return false.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this sequenceable object into a Parcel.
     * @param parcel Indicates the Parcel object to which the sequenceable object will be marshaled.
     * @return Return true if Marshal successfully, else return false.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return a sequenceable object of GesturePathPositionDefine.
     */
    static GesturePathPositionDefine *Unmarshalling(Parcel &parcel);

private:
    float positionX_ = .0f;
    float positionY_ = .0f;
};

class GesturePathDefine : public Parcelable {
public:
    GesturePathDefine() {}
    ~GesturePathDefine() {}

    GesturePathDefine(GesturePathPositionDefine &startPosition,
        GesturePathPositionDefine &endPosition, int64_t durationTime);

    /**
     * @brief Obtains the duration for completing the maximum number of gesture strokes.
     * @param
     * @return Return the duration for completing the maximum number of gesture strokes.
     */
    static int64_t GetMaxStrokeDuration();

    /**
     * @brief Obtains the maximum number of strokes in this gesture path.
     * @param
     * @return Return the maximum number of strokes in this gesture path.
     */
    static uint32_t GetMaxStrokes();

    /**
     * @brief Obtains the duration in which this gesture path continues.
     * @param
     * @return Return the duration in which this gesture path continues.
     */
    int64_t GetDurationTime();

    /**
     * @brief Obtains the end position of this gesture path.
     * @param
     * @return Return the end position of this gesture path.
     */
    GesturePathPositionDefine &GetEndPosition();

    /**
     * @brief Obtains the start position of this gesture path.
     * @param
     * @return Return the start position of this gesture path.
     */
    GesturePathPositionDefine &GetStartPosition();

    /**
     * @brief Sets the duration for this gesture path to continue.
     * @param durationTime The duration for this gesture path to continue.
     * @return
     */
    void SetDurationTime(int64_t durationTime);

    /**
     * @brief Sets the end position of this gesture path.
     * @param endPosition The end position of this gesture path.
     * @return
     */
    void SetEndPosition(GesturePathPositionDefine &endPosition);

    /**
     * @brief Sets the start position of this gesture path.
     * @param startPosition The start position of this gesture path.
     * @return
     */
    void SetStartPosition(GesturePathPositionDefine &startPosition);

    /**
     * @brief read this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return true if read successfully, else return false.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this sequenceable object into a Parcel.
     * @param parcel Indicates the Parcel object to which the sequenceable object will be marshaled.
     * @return Return true if Marshal successfully, else return false.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this sequenceable object from a Parcel.
     * @param parcel Indicates the Parcel object into which the sequenceable object has been marshaled.
     * @return Return a sequenceable object of GesturePathDefine.
     */
    static GesturePathDefine *Unmarshalling(Parcel &parcel);

private:
    const static int64_t MAX_STROKE_DURATION = 60 * 1000;
    const static uint32_t MAX_STROKES = 10;

    GesturePathPositionDefine startPosition_;
    GesturePathPositionDefine endPosition_;
    int64_t durationTime_ = 0;
};

class GestureResultListener {
public:
    /**
     * @brief Called when the gesture is finished.
     * @param sequence
     * @param result Return true if the gesture which is listened is injected successfully, else return false.
     * @return
     */
    virtual void OnGestureInjectResult(uint32_t sequence, bool result) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // GESTURE_SIMULATION_H