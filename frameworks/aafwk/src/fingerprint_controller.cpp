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

#include "fingerprint_controller.h"

using namespace std;

namespace OHOS {
namespace Accessibility {

FingerprintController::FingerprintController(uint32_t channelId)
{
    HILOG_DEBUG("%{public}s start and channelId is %{public}d.", __func__, channelId);
    channelId_ = channelId;
}

bool FingerprintController::IsFingerprintGestureDetectionValid()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    return AccessibilityOperator::GetInstance().IsFingerprintGestureDetectionValid(channelId_);
}

void FingerprintController::AddFingerprintListener(std::shared_ptr<FingerprintListener>& listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }
    fingerprintListener_ = listener;
}

void FingerprintController::DeleteFingerprintListener(std::shared_ptr<FingerprintListener>& listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }
    if (fingerprintListener_ != listener) {
        HILOG_ERROR("There is not this listener.");
        return;
    }
    fingerprintListener_ = nullptr;
}

void FingerprintController::DispatchOnFingerprintGestureStatusChanged(bool isActive)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (!fingerprintListener_) {
        HILOG_ERROR("fingerprintListener_ is nullptr.");
        return;
    }
    fingerprintListener_->OnFingerprintGestureStatusChanged(isActive);
}

void FingerprintController::DispatchOnFingerprintGesture(uint32_t gesture)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (!fingerprintListener_) {
        HILOG_ERROR("fingerprintListener_ is nullptr.");
        return;
    }
    fingerprintListener_->OnFingerprintGesture(gesture);
}

} // namespace Accessibility
} // namespace OHOS