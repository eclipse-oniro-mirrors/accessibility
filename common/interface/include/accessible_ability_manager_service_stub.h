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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_CLIENT_STUB_H

#include <map>
#include "i_accessible_ability_manager_service.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Accessibility {
/*
 * The class define the interface to call ABMS API.
 */
class AccessibleAbilityManagerServiceStub : public IRemoteStub<IAccessibleAbilityManagerService> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    AccessibleAbilityManagerServiceStub();

    /**
     * @brief destruct function
     */
    virtual ~AccessibleAbilityManagerServiceStub();

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @param option The option parameter of IPC,such as: async,sync
     */
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    /**
     * @brief Check the caller has permissions.
     * @param permission The description of permission
     * @return Check result.
     */
    bool CheckPermission(const std::string &permission);

    /**
     * @brief Handle the IPC request for the function:HandleSendEvent.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSendEvent(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:HandleRegisterStateCallback.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleRegisterStateCallback(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:HandleGetAbilityList.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleGetAbilityList(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:HandleRegisterAccessibilityElementOperator.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleRegisterAccessibilityElementOperator(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:HandleDeregisterAccessibilityElementOperator.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleDeregisterAccessibilityElementOperator(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetCaptionProperty(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetCaptionProperty(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetCaptionState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleRegisterCaptionPropertyCallback(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetEnabled(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetCaptionState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetTouchGuideState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetGestureState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetKeyEventObserverState(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleEnableAbility(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetEnabledAbilities(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleDisableAbility(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleEnableUITestAbility(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleDisableUITestAbility(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetActiveWindow(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleSetScreenMagnificationState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetShortKeyState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetMouseKeyState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetMouseAutoClick(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetShortkeyTarget(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetHighContrastTextState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetInvertColorState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetAnimationOffState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetAudioMonoState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetDaltonizationColorFilter(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetContentTimeout(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetBrightnessDiscount(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleSetAudioBalance(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetScreenMagnificationState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetShortKeyState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetMouseKeyState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetMouseAutoClick(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetShortkeyTarget(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetHighContrastTextState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetInvertColorState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetAnimationOffState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetAudioMonoState(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetDaltonizationColorFilter(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetContentTimeout(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetBrightnessDiscount(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetAudioBalance(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleGetAllConfigs(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleRegisterEnableAbilityListsObserver(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleRegisterConfigCallback(MessageParcel &data, MessageParcel &reply);

    using AccessibilityAbilityManagerServiceFunc = ErrCode (AccessibleAbilityManagerServiceStub::*)(
        MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AccessibilityAbilityManagerServiceFunc> memberFuncMap_;
};
} // namespace Accessibility
} // namespace OHOS
#endif