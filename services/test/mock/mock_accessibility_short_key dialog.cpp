/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "accessibility_short_key_dialog.h"
#include <gtest/gtest.h>

namespace OHOS {
namespace Accessibility {
// ability_connection
void ShortkeyAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    (void)element;
    (void)remoteObject;
    (void)resultCode;
}

void ShortkeyAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    (void)element;
    (void)resultCode;
}

std::string ShortkeyAbilityConnection::GetCommandString()
{
    return "";
}

// dialog
AccessibilityShortkeyDialog::AccessibilityShortkeyDialog() {}

AccessibilityShortkeyDialog::~AccessibilityShortkeyDialog()
{
}

bool AccessibilityShortkeyDialog::ConnectDialog()
{
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr)
{
    (void)want;
    (void)commandStr;
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtension()
{
    return true;
}

bool AccessibilityShortkeyDialog::DisconnectExtension() const
{
    return true;
}

std::string AccessibilityShortkeyDialog::BuildStartCommand()
{
    return "";
}
} // namespace Accessibility
} // namespace OHOS