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

#ifndef ACCESSIBILE_ABILITY_MANAMGER_SERVICE_CAPTION_PROPERTY_PROXY_H
#define ACCESSIBILE_ABILITY_MANAMGER_SERVICE_CAPTION_PROPERTY_PROXY_H

#include <cstdint>

#include "accessible_ability_manager_service_caption_property_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityManagerServiceCaptionPropertyProxy : public IRemoteProxy<IAccessibleAbilityManagerServiceCaptionProperty> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibleAbilityManagerServiceCaptionPropertyProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibleAbilityManagerServiceCaptionPropertyProxy() override;

    /**
     * @brief Receive the caption property notify from AAMS and send it to the observer registed..
     * @param caption the caption property
     * @return
     */
    virtual void OnCaptionPropertyChanged(const CaptionProperty &caption) override;
   private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<AccessibleAbilityManagerServiceCaptionPropertyProxy> delegator;
};

} //namespace Accessibility
} //namespace OHOS
#endif