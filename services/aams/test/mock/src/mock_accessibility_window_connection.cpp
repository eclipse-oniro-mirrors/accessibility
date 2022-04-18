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

#include "accessibility_window_connection.h"

namespace OHOS {
namespace Accessibility {
AccessibilityWindowConnection::AccessibilityWindowConnection(
    const int32_t windowId, const sptr<IAccessibilityElementOperator>& connection, const int32_t accountId)
{
    windowId_ = windowId;
    proxy_ = connection;
    accountId_ = accountId;
}

AccessibilityWindowConnection::~AccessibilityWindowConnection()
{}
} // namespace Accessibility
} // namespace OHOS