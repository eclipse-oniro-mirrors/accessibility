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

#ifndef ACCESSIBLE_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_INTERFACE_H
#define ACCESSIBLE_ABILITY_MANAGER_SERVICE_CAPTION_PROPERTY_INTERFACE_H

#include <cstdint>

#include "iremote_broker.h"
#include "iremote_object.h"
#include "accessibility_caption.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityManagerServiceCaptionProperty: public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerServiceCaptionProperty");

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registed..
     * @param caption The caption properties.
     * @return
     */
    virtual void OnCaptionPropertyChanged(const CaptionProperty& caption) = 0;

    enum class Message {
        ON_CAPTION_PROPERTY_CHANGED,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif