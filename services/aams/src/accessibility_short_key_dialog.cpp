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
#include "hilog_wrapper.h"

#include <algorithm>
#include "ability_connection.h"
#include "ability_connect_callback.h"
#include "bool_wrapper.h"
#include "double_wrapper.h"
#include "extension_manager_client.h"
#include "int_wrapper.h"
#include "ipc_skeleton.h"
#include "refbase.h"
#include "string_wrapper.h"
#include "want_params_wrapper.h"
#include "nlohmann/json.hpp"

constexpr int32_t SHORTKEY_DIALOG_PARAM_NUM = 3;
constexpr int32_t DEFAULT_VALUE_MINUS_ONE = -1;

namespace OHOS {
namespace Accessibility {
// ability_connection
void ShortkeyAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_DEBUG("on ability connected");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInt32(SHORTKEY_DIALOG_PARAM_NUM);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.ohos.settings");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"AccessibilityShortKeyDialog");
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(GetCommandString()));

    if (!data.WriteParcelable(&element)) {
        HILOG_ERROR("Connect done element error.");
        return;
    }

    if (!data.WriteRemoteObject(remoteObject)) {
        HILOG_ERROR("Connect done remote object error.");
        return;
    }

    if (!data.WriteInt32(resultCode)) {
        HILOG_ERROR("Connect done result code error.");
        return;
    }

    int32_t errCode = remoteObject->SendRequest(
        AAFwk::IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    HILOG_DEBUG("AbilityConnectionWrapperProxy::OnAbilityConnectDone result %{public}d", errCode);
}

void ShortkeyAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
    int32_t resultCode)
{
    HILOG_DEBUG("on ability disconnected");
}

std::string ShortkeyAbilityConnection::GetCommandString()
{
    return commandStr_;
}

// dialog
AccessibilityShortkeyDialog::AccessibilityShortkeyDialog() {}

AccessibilityShortkeyDialog::~AccessibilityShortkeyDialog()
{
    HILOG_DEBUG("release AccessibilityShortkeyDialog");
}

bool AccessibilityShortkeyDialog::ConnectDialog()
{
    HILOG_DEBUG("AccessibilityShortkeyDialog dialog");
    if (!ConnectExtension()) {
        HILOG_ERROR("failed to connect dialog.");
        return false;
    }
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr)
{
    connection_ = sptr<ShortkeyAbilityConnection>(new (std::nothrow) ShortkeyAbilityConnection(commandStr));
    if (connection_ == nullptr) {
        HILOG_ERROR("connection_ is nullptr.");
        return false;
    }
    // reset current callingIdentify
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want, connection_, nullptr,
        DEFAULT_VALUE_MINUS_ONE);
    HILOG_DEBUG("ret is: %{public}d.", ret);
    // set current callingIdentify back.
    IPCSkeleton::SetCallingIdentity(identity);
    if (ret != ERR_OK) {
        HILOG_ERROR("ret isn't ERR_OK");
        return false;
    }
    return true;
}

bool AccessibilityShortkeyDialog::ConnectExtension()
{
    std::string tmp = BuildStartCommand();
    HILOG_DEBUG("start command: %{public}s", tmp.c_str());

    AAFwk::Want want;
    std::string bundleName = "com.ohos.systemui";
    std::string abilityName = "com.ohos.systemui.dialog";
    want.SetElementName(bundleName, abilityName);
    bool ret = ConnectExtensionAbility(want, tmp);
    if (!ret) {
        HILOG_ERROR("ConnectExtensionAbility failed.");
        return false;
    }
    HILOG_DEBUG("ConnectExtensionAbility successed.");
    return true;
}

bool AccessibilityShortkeyDialog::DisconnectExtension() const
{
    if (connection_ == nullptr) {
        return true;
    }
    ErrCode ret = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(connection_);
    if (ret != ERR_OK) {
        HILOG_ERROR("disconnect extension ability failed ret: %{public}d.", ret);
        return false;
    }
    return true;
}

std::string AccessibilityShortkeyDialog::BuildStartCommand()
{
    nlohmann::json root;
    std::string tmp = "sysDialog/common";
    root["ability.want.params.uiExtensionType"] = tmp;
    std::string cmdData = root.dump();
    HILOG_DEBUG("cmdData is: %{public}s.", cmdData.c_str());
    return cmdData;
}
} // namespace Accessibility
} // namespace OHOS