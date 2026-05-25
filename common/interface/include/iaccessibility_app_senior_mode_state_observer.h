/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef INTERFACE_ACCESSIBILITY_SENIOR_MODE_STATE_OBSERVER_H
#define INTERFACE_ACCESSIBILITY_SENIOR_MODE_STATE_OBSERVER_H

#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
class IAccessibilityAppSeniorModeStateObserver : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibilityAppSeniorModeStateObserver");

    /**
     * @brief Receive the senior mode state notify from AAMS and send it to the observer registered.
     * @param bundleName The bundle name of the application
     * @param appIndex The index of the application
     * @param state The senior mode state
     */
    virtual void OnSeniorModeStateChanged(const std::string& bundleName, int32_t appIndex, bool state) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // INTERFACE_ACCESSIBILITY_SENIOR_MODE_STATE_OBSERVER_H
