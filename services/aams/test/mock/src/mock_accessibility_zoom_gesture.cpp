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

#include "accessibility_zoom_gesture.h"

namespace OHOS {
namespace Accessibility {
/**
 * Triple: DOWN UP DOWN UP DOWN
 */
AccessibilityZoomGesture::AccessibilityZoomGesture()
{
    Initialize();
}

AccessibilityZoomGesture::~AccessibilityZoomGesture()
{}

void AccessibilityZoomGesture::Initialize()
{
    distance_ = GetSysDistance();
    timeout_ = GetSysTimeout();
}

int32_t AccessibilityZoomGesture::GetSysDistance() const
{
    return 0;
}

int64_t AccessibilityZoomGesture::GetSysTimeout() const
{
    return 0;
}

bool AccessibilityZoomGesture::ValidDown(MMI::PointerEvent& event)
{
    (void)event;
    return true;
}

bool AccessibilityZoomGesture::Triple(MMI::PointerEvent& event)
{
    (void)event;
    return false;
}

void AccessibilityZoomGesture::Up()
{}

void AccessibilityZoomGesture::Reset(const MMI::PointerEvent& event)
{
    (void)event;
}

void AccessibilityZoomGesture::Clear()
{
    upCount_ = 0;
    downCount_ = 0;
}
} // namespace Accessibility
} // namespace OHOS