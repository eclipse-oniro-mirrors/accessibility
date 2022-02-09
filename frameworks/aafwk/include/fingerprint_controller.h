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

#ifndef FINGERPRINT_CONTROLLER_H
#define FINGERPRINT_CONTROLLER_H

#include <vector>
#include <stdint.h>
#include "refbase.h"
#include "accessible_ability_client_stub_impl.h"

namespace OHOS {
namespace Accessibility {

class FingerprintListener {
public:

    /**
     * @brief Callback when the validity status of the fingerprint sensor's gesture detection changes.
     * @param isActive The validity status of the fingerprint sensor's gesture detection.
     * @return
     */
    virtual void OnFingerprintGestureStatusChanged(bool isActive) = 0;

    /**
     * @brief Callback when the fingerprint sensor detects a gesture.
     * @param gesture The gesture which is detected by fingerprint sensor.
     * @return
     */
    virtual void OnFingerprintGesture(uint32_t gesture) = 0;
};

class FingerprintController {
public:
    /**
     * @brief The constructor of FingerprintController.
     * @param channelId The id of channel.
     */
    FingerprintController(uint32_t channelId);

    /**
     * @brief Judge whether the gesture detection function of the fingerprint sensor is available.
     * @param
     * @return Return true if the fingerprint sensor is available, else return false.
     */
    bool IsFingerprintGestureDetectionValid();

    /**
     * @brief Add listener of fingerprint gesture.
     * @param listener The listener to add.
     * @return
     */
    void AddFingerprintListener(std::shared_ptr<FingerprintListener>& listener);

    /**
     * @brief Delete listener of fingerprint gesture.
     * @param listener The listener to delete.
     * @return
     */
    void DeleteFingerprintListener(std::shared_ptr<FingerprintListener>& listener);

    /**
     * @brief Dispatch the validity status change of the fingerprint sensor's gesture detection
     *        to listeners who has been registered.
     * @param isActive The validity status of the fingerprint sensor's gesture detection.
     * @return
     */
    void DispatchOnFingerprintGestureStatusChanged(bool isActive);

    /**
     * @brief Dispatch the gesture which is detected by fingerprint sensor to listeners who has been registered.
     * @param gesture The gesture which is detected by fingerprint sensor.
     * @return
     */
    void DispatchOnFingerprintGesture(uint32_t gesture);

private:
    std::shared_ptr<FingerprintListener> fingerprintListener_ = nullptr;
    uint32_t channelId_ = AccessibleAbilityClientStubImpl::INVALID_CHANNEL_ID;
};

} // namespace Accessibility
} // namespace OHOS

#endif // FINGERPRINT_CONTROLLER_H