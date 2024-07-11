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

void ReConfirmAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("start.");
    (void)element;
    (void)remoteObject;
    (void)resultCode;
}

void ReConfirmAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    HILOG_DEBUG("start.");
    (void)element;
    (void)resultCode;
}

std::string ReConfirmAbilityConnection::GetCommandString()
{
    HILOG_DEBUG("start.");
    return "";
}

// dialog
AccessibilityShortkeyDialog::AccessibilityShortkeyDialog() {}

AccessibilityShortkeyDialog::~AccessibilityShortkeyDialog()
{
}

bool AccessibilityShortkeyDialog::ConnectDialog(ShortKeyDialogType dialogType)
{
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr,
                                                          ShortKeyDialogType dialogType)
{
    (void)want;
    (void)commandStr;
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtension(ShortKeyDialogType dialogType)
{
    return true;
}

bool AccessibilityShortkeyDialog::DisconnectExtension(ShortKeyDialogType dialogType) const
{
    return true;
}

std::string AccessibilityShortkeyDialog::BuildStartCommand()
{
    return "";
}
} // namespace Accessibility
} // namespace OHOS