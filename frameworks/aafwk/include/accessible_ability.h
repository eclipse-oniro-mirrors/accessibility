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

#ifndef ACCESSIBLE_ABILITY_H
#define ACCESSIBLE_ABILITY_H

#include <memory>
#include <optional>
#include <stdint.h>
#include <vector>
#include <map>

#include "key_event.h"
#include "accessible_ability_listener.h"
#include "accessibility_element_info.h"
#include "accessibility_window_info.h"
#include "accessibility_event_info.h"
#include "iremote_object.h"
#include "accessible_ability_manager_service_interface.h"
#include "display_resize_controller.h"
#include "fingerprint_controller.h"
#include "gesture_simulation.h"
#include "accessible_ability_client_stub_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

    // The types of swipe gestures which are performed on the touch screen.
    enum GestureTypes : uint32_t {
        GESTURE_SWIPE_UP = 0x0001,
        GESTURE_SWIPE_DOWN = 0x0002,
        GESTURE_SWIPE_LEFT = 0x0003,
        GESTURE_SWIPE_RIGHT = 0x0004,
        GESTURE_SWIPE_LEFT_THEN_RIGHT = 0x0005,
        GESTURE_SWIPE_RIGHT_THEN_LEFT = 0x0006,
        GESTURE_SWIPE_UP_THEN_DOWN = 0x0007,
        GESTURE_SWIPE_DOWN_THEN_UP = 0x0008,
        GESTURE_SWIPE_LEFT_THEN_UP = 0x0009,
        GESTURE_SWIPE_LEFT_THEN_DOWN = 0x000A,
        GESTURE_SWIPE_RIGHT_THEN_UP = 0x000B,
        GESTURE_SWIPE_RIGHT_THEN_DOWN = 0x000C,
        GESTURE_SWIPE_UP_THEN_LEFT = 0x000D,
        GESTURE_SWIPE_UP_THEN_RIGHT = 0x000E,
        GESTURE_SWIPE_DOWN_THEN_LEFT = 0x000F,
        GESTURE_SWIPE_DOWN_THEN_RIGHT = 0x0010,

        GESTURE_TAP = 0x0011,
        GESTURE_DOUBLETAP = 0x0012,
        GESTURE_DOUBLETAP_HOLD = 0x0013,
        GESTURE_DRAG = 0x0014,
        GESTURE_SWIPE_SLOWLY = 0x0015,
        GESTURE_TRIPLETAP = 0x0016,
        GESTURE_SCALE = 0x0017,
        GESTURE_SCROLL = 0x0018,
        GESTURE_GESTURE_ALL = 0xFFFFFFFF
    };

    // The global action types
    enum GlobalAction : uint32_t {
        GLOBAL_ACTION_INVALID = 0x0000,
        GLOBAL_ACTION_BACK = 0x0001,
        GLOBAL_ACTION_HOME = 0x0002,
        GLOBAL_ACTION_RECENT = 0x0003,
        GLOBAL_ACTION_NOTIFICATION = 0x0004,
        GLOBAL_ACTION_POP_UP_POWER_DIALOG = 0x0006,
        GLOBAL_ACTION_DIVIDE_SCREEN = 0x0007,
        GLOBAL_ACTION_LOCK_SCREEN = 0x0008,
        GLOBAL_ACTION_CAPTURE_SCREEN = 0x0009
    };

class AccessibleAbility {
public:

    /**
     * @brief Gets an instance of AccessibleAbility.
     * @param
     * @return Return an instance of AccessibleAbility.
     */
    static AccessibleAbility& GetInstance();

    /**
     * @brief Register listener if you want to receive notification.
     * @param listener The listener used to receive notification.
     * @return
     */
    void RegisterListener(const std::shared_ptr<AccessibleAbilityListener> &listener);

    /**
     * @brief Connect to AAMS.
     * @param
     * @return
     */
    void ConnectToAAMS();

    /**
     * @brief Disable the accessible ability.
     * @param
     * @return
     */
    void DisableAbility();

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The elementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetFocusElementInfo(uint32_t focusType, std::optional<AccessibilityElementInfo>& elementInfo);

    /**
     * @brief Sends simulate gestures to the screen.
     * @param gesturePathDefineList The gesture which need to send.
     * @param listener The listener of the gesture.
     * @return Return true if the gesture sends successfully, else return false.
     */
    bool GestureSimulate(std::vector<GesturePathDefine>& gesturePathDefineList,
                            std::shared_ptr<GestureResultListener>& listener);

    /**
     * @brief Obtains the default displayResize controller.
     * @param
     * @return Return the default displayResize controller.
     */
    std::shared_ptr<DisplayResizeController>& GetDisplayResizeController();

    /**
     * @brief Obtains the specified displayResize controller by displayId.
     * @param displayId The id of display.
     * @return Return the specified displayResize controller.
     */
    std::shared_ptr<DisplayResizeController>& GetDisplayResizeController(uint32_t displayId);

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetRootElementInfo(std::optional<AccessibilityElementInfo>& elementInfo);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param
     * @return The infomation of windows.
     */
    std::vector<AccessibilityWindowInfo>& GetWindows();

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::GlobalAction.
     * @return Return true if performs action successfully, else return false.
     */
    bool PerformCommonAction(uint32_t action);

    /**
     * @brief Obtains the controller for fingerprint operations.
     * @param 
     * @return Return the fingerprint controller.
     */
    std::shared_ptr<FingerprintController>& GetFingerprintController();

    /**
     * @brief Dispatch the result of simulation gesture.
     * @param sequence The sequence of gesture.
     * @param result The result of gesture completion.
     * @return
     */
    void DispatchOnSimulationGestureResult(uint32_t sequence, bool result);

    /**
     * @brief Set channelId.
     * @param channelId The id of channel.
     * @return
     */
    void SetChannelId(uint32_t channelId);

private:
    AccessibleAbility();
    ~AccessibleAbility() = default;

    /**
     * @brief Init AccessibleAbility.
     * @param
     * @return Return true if init successfully, else return false.
     */
    bool Init();

    /**
     * @brief Check the type of common action.
     * @param action The type of common action.
     * @return Return true if the type of common action is right, else return false.
     */
    bool CheckCommonAction(uint32_t action);

    std::vector<AccessibilityWindowInfo> accessibilityWindow_ {};
    std::map<uint32_t, std::shared_ptr<GestureResultListenerInfo>> gestureResultListenerInfos_{};
    std::map<uint32_t, std::shared_ptr<DisplayResizeController>> displayResizeControllers_{};
    std::shared_ptr<FingerprintController> fingerprintController_ = nullptr;
    std::shared_ptr<AccessibleAbilityEventHandler> accessibleAbilityEventHandler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;

    sptr<IAccessibleAbilityManagerServiceClient> proxy_ = nullptr;
    sptr<AccessibleAbilityClientStubImpl> stub_ = nullptr;
 
    uint32_t channelId_ = AccessibleAbilityClientStubImpl::INVALID_CHANNEL_ID;
    uint32_t gestureStatusListenerSequence_ = 0;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBLE_ABILILTY_H