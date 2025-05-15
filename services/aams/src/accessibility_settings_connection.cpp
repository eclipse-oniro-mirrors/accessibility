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

#include "accessibility_settings_connection.h"

#include <string>
#include "message_parcel.h"
#include "hilog_wrapper.h"
#include "nlohmann/json.hpp"
#include "extension_manager_client.h"

namespace OHOS {
namespace Accessibility {

const int64_t WAIT_INTERVAL = 5000; // 5s
constexpr int32_t DEFAULT_USER_ID = -1;
const std::string SETTINGS_BUNDLE_NAME = "com.huawei.hmos.settings";
const std::string SERVICE_EXTENSION_ABILITY_NAME = "SettingsExtService";
const std::string VOICE_RECOGNITION = "voiceRecognition";
const std::string METHOD_KEY = "method";
const std::string EXTRA_KEY = "extra";
const std::string EXTRA_VALUE_KEY = "soundType";
constexpr int32_t SETTINGS_VOICE_RECOGNITION_CODE = 7;

void AccessibilitySettingsConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_INFO("on ability connected done");
    if (resultCode != ERR_OK) {
        HILOG_ERROR("ability connect failed: %{public}d", resultCode);
        return;
    }
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is null");
        return;
    }
    HILOG_INFO("got remoteObject");
    std::unique_lock<std::mutex> lock(mutex_);
    remoteObject_ = remoteObject;
    waitFlag_ = true;
    cv_.notify_all();
}

void AccessibilitySettingsConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_INFO("on ability disconnected done");
    std::unique_lock<std::mutex> lock(mutex_);
    waitFlag_ = false;
    remoteObject_ = nullptr;
}

sptr<IRemoteObject> AccessibilitySettingsConnection::GetRemoteObject()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.wait_for(lock, std::chrono::milliseconds(WAIT_INTERVAL), [this]() { return waitFlag_; })) {
        HILOG_ERROR("wait for remote object timeout");
        return nullptr;
    }
    return remoteObject_;
}

AccessibilitySettingsManager::AccessibilitySettingsManager()
{
    connection_ = sptr<AccessibilitySettingsConnection> (new (std::nothrow) AccessibilitySettingsConnection());
}

AccessibilitySettingsManager& AccessibilitySettingsManager::GetInstance()
{
    static AccessibilitySettingsManager instance;
    return instance;
}

void AccessibilitySettingsManager::VoiceRecognize(int32_t soundType)
{
    HILOG_INFO("called, soundType=%{public}d", soundType);
    sptr<IRemoteObject> remote = GetRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("cannot get remote object");
        return;
    }
    std::string message = CreateJsonMessage(soundType);
    SendRequestToSetting(remote, message);
    DisconnectSettingsExtService();
}

int32_t AccessibilitySettingsManager::ConnectSettingsExtService()
{
    HILOG_INFO("called");
    AAFwk::Want want;
    want.SetElementName(SETTINGS_BUNDLE_NAME, SERVICE_EXTENSION_ABILITY_NAME);
    int32_t ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want, connection_,
        nullptr, DEFAULT_USER_ID);
    return ret;
}

void AccessibilitySettingsManager::DisconnectSettingsExtService()
{
    auto disconnectResult = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(connection_);
    if (disconnectResult != ERR_OK) {
        HILOG_ERROR("disconnect failed, ret=%{public}d", disconnectResult);
    }
}

sptr<IRemoteObject> AccessibilitySettingsManager::GetRemoteObject()
{
    int32_t connectResult = ConnectSettingsExtService();
    if (connectResult != ERR_OK) {
        HILOG_ERROR("connect failed, ret=%{public}d", connectResult);
        return nullptr;
    }
    HILOG_INFO("connect success");
    return connection_->GetRemoteObject();
}

std::string AccessibilitySettingsManager::CreateJsonMessage(int32_t soundType)
{
    nlohmann::json jsonMessageObj;
    nlohmann::json jsonExtraValue;

    jsonExtraValue[EXTRA_VALUE_KEY] = soundType;
    jsonMessageObj[METHOD_KEY] = VOICE_RECOGNITION;
    jsonMessageObj[EXTRA_KEY] = jsonExtraValue;

    return jsonMessageObj.dump();
}

void AccessibilitySettingsManager::SendRequestToSetting(const sptr<IRemoteObject> &remoteObject,
    const std::string &message)
{
    if (message.empty()) {
        HILOG_ERROR("send message is empty!");
        return;
    }
    HILOG_INFO("send message=%{public}s", message.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteString16(Str8ToStr16(message))) {
        HILOG_ERROR("write message failed!");
        return;
    }
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is nullptr!");
        return;
    }
    int32_t ret = remoteObject->SendRequest(SETTINGS_VOICE_RECOGNITION_CODE, data, reply, option);
    auto replyCode = reply.ReadInt32();
    HILOG_INFO("sendRequest ret=%{public}d, replyCode=%{public}d", ret, replyCode);
}

} // namespace Accessibility
} // namespace OHOS
