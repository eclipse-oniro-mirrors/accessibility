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
#ifndef ACCESSIBILITY_INTERACTION_CONNECTION_H
#define ACCESSIBILITY_INTERACTION_CONNECTION_H
#include <string>
#include "accessibility_interaction_operation_interface.h"

namespace OHOS {
namespace Accessibility{

class AccessibilityInteractionConnection : public RefBase {
public:
    AccessibilityInteractionConnection(const int windowId, const sptr<IAccessibilityInteractionOperation> &connection,
                                       const int accountId);
    ~AccessibilityInteractionConnection();

    inline sptr<IAccessibilityInteractionOperation> GetProxy() {return proxy_;}

private:
    int windowId_;
    int accountId_;
    sptr<IAccessibilityInteractionOperation> proxy_;
};

}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_INTERACTION_CONNECTION_H