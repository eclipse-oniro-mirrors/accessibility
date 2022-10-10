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

#include <gtest/gtest.h>
#include "accessibility_event_transmission.h"
#include "accessibility_ut_helper.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
bool EventTransmission::OnPointerEvent(MMI::PointerEvent& event)
{
    HILOG_DEBUG();
    int32_t touchAction = event.GetPointerAction();
    AccessibilityAbilityHelper::GetInstance().SetTouchEventActionVectors(touchAction);
    GTEST_LOG_(INFO) << "##########EventTransmission OnPointerEvent touchAction=" << touchAction;
    return false;
}

bool EventTransmission::OnKeyEvent(MMI::KeyEvent& event)
{
    AccessibilityAbilityHelper::GetInstance().SetKeyCode(event.GetKeyCode());
    return false;
}

void EventTransmission::OnMoveMouse(int32_t offsetX, int32_t offsetY)
{
    (void)offsetX;
    (void)offsetY;
}

void EventTransmission::DestroyEvents()
{
    AccessibilityAbilityHelper::GetInstance().SetDestroyState();
}
} // namespace Accessibility
} // namespace OHOS