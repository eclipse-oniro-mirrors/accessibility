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

#ifndef ACCESSIBILITY_ABILITY_MANAGER_SERVICE_EVENT_HANDLER_H
#define ACCESSIBILITY_ABILITY_MANAGER_SERVICE_EVENT_HANDLER_H

#include "event_handler.h"

namespace OHOS {
namespace Accessibility {
class AAMSEventHandler : public AppExecFwk::EventHandler {
public:
    explicit AAMSEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~AAMSEventHandler() override;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ABILITY_MANAGER_SERVICE_EVENT_HANDLER_H