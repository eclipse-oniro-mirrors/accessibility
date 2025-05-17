/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef EXTENSION_ABILITY_CONNECTION_H
#define EXTENSION_ABILITY_CONNECTION_H

#include <condition_variable>
#include "ability_connect_callback_stub.h"

namespace OHOS {
namespace Accessibility {
using namespace OHOS::AppExecFwk;

class ExtensionAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr <IRemoteObject> &remoteObject,
        int32_t resultCode) override;

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    sptr<IRemoteObject> GetRemoteObject();

private:
    sptr <IRemoteObject> remoteObject_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool waitFlag_ = false;
};

class ExtensionAbilityManager {
public:
    static ExtensionAbilityManager &GetInstance();

    void VoiceRecognize(int32_t soundType);

    ExtensionAbilityManager(const ExtensionAbilityManager&) = delete;

    ExtensionAbilityManager(ExtensionAbilityManager&&) = delete;

    ExtensionAbilityManager& operator=(const ExtensionAbilityManager&) = delete;

    ExtensionAbilityManager& operator=(ExtensionAbilityManager&&) = delete;

private:
    ExtensionAbilityManager();

    ~ExtensionAbilityManager() = default;

    int32_t ConnectSettingsExtService();

    void DisconnectSettingsExtService();

    sptr<IRemoteObject> GetRemoteObject();

    std::string CreateJsonMessage(int32_t soundType);

    void SendRequestToSetting(const sptr<IRemoteObject> &remoteObject, const std::string &message);

    sptr <ExtensionAbilityConnection> connection_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // EXTENSION_ABILITY_CONNECTION_H
