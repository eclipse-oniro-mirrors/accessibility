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

#ifndef ACCESSIBLE_ABILITY_MANAGER_STATE_OBSERVER_STUB_H
#define ACCESSIBLE_ABILITY_MANAGER_STATE_OBSERVER_STUB_H

#include "i_accessible_ability_manager_state_observer.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerStateObserverStub : public IRemoteStub<IAccessibleAbilityManagerStateObserver> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    AccessibleAbilityManagerStateObserverStub();

    /**
     * @brief destruct function
     */
    virtual ~AccessibleAbilityManagerStateObserverStub() = default;

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @param option The option parameter of IPC,such as: async,sync
     */
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    /**
     * @brief Handle the IPC request for the function:HandleOnStateChanged.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleOnStateChanged(MessageParcel &data, MessageParcel &reply);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_MANAGER_STATE_OBSERVER_STUB_H