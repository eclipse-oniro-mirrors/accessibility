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

#ifndef ACCESSIBILITY_SHORT_KEY_DIALOG_H
#define ACCESSIBILITY_SHORT_KEY_DIALOG_H

#include "ability_connection.h"
#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "message_parcel.h"
#include "nocopyable.h"
#include <cstdint>

namespace OHOS {
namespace Accessibility {

class ShortkeyAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ShortkeyAbilityConnection(const std::string commandStr)
    {
        commandStr_ = commandStr;
    }

    virtual ~ShortkeyAbilityConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    std::string GetCommandString();

private:
    std::string commandStr_;
};

class AccessibilityShortkeyDialog {
public:
    AccessibilityShortkeyDialog();
    ~AccessibilityShortkeyDialog();
    bool ConnectDialog();

private:
    bool ConnectExtension();
    bool ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr);
    bool DisconnectExtension() const;
    std::string BuildStartCommand();

private:
    sptr<ShortkeyAbilityConnection> connection_ {nullptr};
};

} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SHORT_KEY_DIALOG_H