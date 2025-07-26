/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ext_utils.h"
#include <hisysevent.h>
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

bool ExtUtils::IsInRect(int32_t posX, int32_t posY, Rosen::Rect rect)
{
    return (posX >= rect.posX_ && posY >= rect.posY_ && posX <= rect.posX_ + static_cast<int32_t>(rect.width_) &&
        posY <= rect.posY_ + static_cast<int32_t>(rect.height_));
}

void ExtUtils::RecordMagnificationUnavailableEvent(const std::string &name)
{
    std::ostringstream oss;
    oss << "magnification run failed" << ", caused by: " << name << ";";
    HILOG_DEBUG("starting accessibility: %{public}s", oss.str().c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY,
        "UNAVAILABLE",
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "MSG", oss.str());
    if (ret != 0) {
        HILOG_ERROR("Write HiSysEvent error, ret:%{public}d", ret);
    }
}
} // namespace Accessibility
} // namespace OHOS