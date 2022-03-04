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

#ifndef ACCESSIBILITY_SYSTEM_CLIENT_PROXY_H
#define ACCESSIBILITY_SYSTEM_CLIENT_PROXY_H

#include <cstdint>

#include "accessible_ability_manager_service_state_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceStateProxy : public IRemoteProxy<IAccessibleAbilityManagerServiceState> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibleAbilityManagerServiceStateProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceStateProxy() override;

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registered.
     * @param stateType The state type and value.
     *                  state type: Refer to AccessibilityStateEventType.
     *                  value: STATE_ACCESSIBILITY_ENABLED/STATE_EXPLORATION_ENABLED/
     *                         STATE_ACCESSIBILITY_DISABLED/STATE_EXPLORATION_DISABLED
     * @return
     */
    virtual void OnStateChanged(const uint32_t stateType) override;
private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<AccessibleAbilityManagerServiceStateProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif