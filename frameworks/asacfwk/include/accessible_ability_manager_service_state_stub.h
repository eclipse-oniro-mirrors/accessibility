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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_STATE_STUB_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_STATE_STUB_H

#include <cstdint>
#include <list>

#include "accessible_ability_manager_service_state_interface.h"
#include "accessibility_errorcode.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceStateStub : public IRemoteStub<IAccessibleAbilityManagerServiceState> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    AccessibleAbilityManagerServiceStateStub();

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceStateStub() = default;

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @param option The option paramter of IPC,such as: async,sync
     * @return
     */
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registed..
     * @param stateType The state type and value.
     *                  state type: Refer to AccessibilityStateEventType.
     *                  value: STATE_ACCESSIBILITY_ENABLED/STATE_EXPLORATION_ENABLED/
     *                         STATE_ACCESSIBILITY_DISABLED/STATE_EXPLORATION_DISABLED
     * @return
     */
    void OnStateChanged(const uint32_t stateType) override;
private:
    /**
     * @brief Handle the IPC request for the function:HandleOnStateChanged.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleOnStateChanged(MessageParcel &data, MessageParcel &reply);

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityManagerServiceStateStub);
};
} // namespace Accessibility
} // namespace OHOS
#endif
