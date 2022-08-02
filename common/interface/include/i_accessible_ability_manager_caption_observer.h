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

#ifndef INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_H
#define INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_H

#include "accessibility_caption.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
class IAccessibleAbilityManagerCaptionObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibleAbilityManagerCaptionObserver");

    /**
     * @brief Receive the state notify from AAMS and send it to the observer registered.
     * @param caption The caption properties.
     */
    virtual void OnPropertyChanged(const AccessibilityConfig::CaptionProperty &property) = 0;

    enum class Message {
        ON_PROPERTY_CHANGED,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBLE_ABILITY_MANAGER_CAPTION_OBSERVER_H