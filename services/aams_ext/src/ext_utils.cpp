/*
 * Copyright (C) 2025-2026 Huawei Device Co., Ltd.
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
#include "parameters.h"

// LCOV_EXCL_START
namespace OHOS {
namespace Accessibility {

namespace {
const std::string FOLD_SCREEN_TYPE = system::GetParameter("const.window.foldscreen.type", "0,0,0,0");
const bool IS_WIDE_FOLD = (FOLD_SCREEN_TYPE == "4,2,0,0");
const bool IS_BIG_FOLD = (FOLD_SCREEN_TYPE == "1,2,0,0") || (FOLD_SCREEN_TYPE == "6,1,0,0");
const bool IS_SMALL_FOLD = (FOLD_SCREEN_TYPE == "2,2,0,0");
}  // namespace

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

int64_t ExtUtils::GetSystemTime()
{
    HILOG_DEBUG("start.");
 
    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    int64_t millisecond = static_cast<int64_t>(times.tv_sec * 1000 + times.tv_nsec / 1000000);
 
    return millisecond;
}
 
void ExtUtils::RecordOnZoomGestureEvent(const std::string &state, const bool &isFullType)
{
    std::string MSG_NAME = "on zoom gesture state";
    if (!isFullType) {
        MSG_NAME = "on partial zoom gesture state";
    }
    HILOG_DEBUG("starting RecordOnZoomGestureEvent on zoom gesture state: %{public}s", state.c_str());
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::ACCESSIBILITY_UE,
        "ZOOM_GESTURE_ACTION",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "MSG_NAME", MSG_NAME, "MSG_VALUE", state);
    if (ret != 0) {
        HILOG_ERROR("Write HiSysEvent RecordOnZoomGestureEvent error, ret:%{public}d", ret);
    }
}
 
bool ExtUtils::IsWideFold()
{
    return IS_WIDE_FOLD;
}
 
bool ExtUtils::IsBigFold()
{
    return IS_BIG_FOLD;
}
 
bool ExtUtils::IsSmallFold()
{
    return IS_SMALL_FOLD;
}
// LCOV_EXCL_STOP
} // namespace Accessibility
} // namespace OHOS