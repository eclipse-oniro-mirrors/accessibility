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

#ifndef ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_PROXY_H
#define ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_PROXY_H

#include "i_accessible_ability_manager_caption_observer.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerCaptionObserverProxy : public IRemoteProxy<IAccessibleAbilityManagerCaptionObserver> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    explicit AccessibleAbilityManagerCaptionObserverProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     */
    virtual ~AccessibleAbilityManagerCaptionObserverProxy() override;

    /**
     * @brief Receive the caption property notify from AAMS and send it to the observer registered.
     * @param caption the caption property
     */
    virtual void OnPropertyChanged(const AccessibilityConfig::CaptionProperty &property) override;
private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<AccessibleAbilityManagerCaptionObserverProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_PROXY_H