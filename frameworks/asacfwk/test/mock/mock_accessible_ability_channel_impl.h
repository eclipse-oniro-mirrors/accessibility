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

#ifndef MOCK_ACCESSIBLE_ABILITY_CHANNEL_IMPL_H
#define MOCK_ACCESSIBLE_ABILITY_CHANNEL_IMPL_H

#include <string>
#include <vector>
#include <map>

#include "accessibility_interaction_operation_callback_interface.h"
#include "accessibility_element_info.h"
#include "accessibility_window_info.h"
#include "gesture_simulation.h"
#include "iremote_broker.h"
#include "refbase.h"
#include "accessible_ability_channel_interface.h"

namespace OHOS {
namespace Accessibility {

class MockAccessibleAbilityChannel : public IAccessibleAbilityChannel {
public:
    MockAccessibleAbilityChannel() {}
    bool SearchElementInfoByAccessibilityId(const int accessibilityWindowId, 
        const long elementId, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback,
        const int mode) override;

    bool SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
        const std::string &text, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    bool FindFocusedElementInfo(const int accessibilityWindowId, const long elementId, 
        const int focusType, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    bool FocusMoveSearch(const int accessibilityWindowId, const long elementId, 
        const int direction, const int requestId, const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    bool PerformAction(const int accessibilityWindowId, const long elementId, const int action, 
        std::map<std::string, std::string> &actionArguments, const int requestId, 
        const sptr<IAccessibilityInteractionOperationCallback> &callback) override;

    std::vector<AccessibilityWindowInfo> GetWindows() override;

    bool PerformCommonAction(const int action) override;

    void DisableAbility() override;

    void SetOnKeyPressEventResult(const bool handled, const int sequence) override;

    float GetDisplayResizeScale(const int displayId) override;

    float GetDisplayResizeCenterX(const int displayId) override;

    float GetDisplayResizeCenterY(const int displayId) override;

    Rect GetDisplayResizeRect(const int displayId) override;

    bool ResetDisplayResize(const int displayId, const bool animate) override;

    bool SetDisplayResizeScaleAndCenter(const int displayId, const float scale, const float centerX, 
        const float centerY, const bool animate) override;

    void SendSimulateGesture(const int sequence, const std::vector<GesturePathDefine> &gestureSteps) override;

    bool IsFingerprintGestureDetectionValid() override;
    sptr<IRemoteObject> AsObject() override;

};

} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CHANNEL_IMPL_H