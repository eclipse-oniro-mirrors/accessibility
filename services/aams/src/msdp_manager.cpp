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

#ifdef ACCESSIBILITY_USER_STATUS_AWARENESS
#include "user_status_client.h"
#endif // ACCESSIBILITY_USER_STATUS_AWARENESS
#include "msdp_manager.h"
#include "extension_ability_manager.cpp"

namespace OHOS {
namespace Accessibility {

const int32_t FEATURE_VOICE_RECOGNITION = 17;

int32_t MsdpManager::SubscribeVoiceRecognition()
{
    HILOG_INFO();
    int32_t ret = 0;
    #ifdef ACCESSIBILITY_USER_STATUS_AWARENESS
    std::function<void(int32_t, std::shared_ptr<Msdp::UserStatusAwareness::UserStatusData>)> func = [ = ](
        int32_t callbackId, std::shared_ptr<Msdp::UserStatusAwareness::UserStatusData> userStatusData) {
        HILOG_INFO("voice recognition callback, Id: %{public}d, GetFeature: %{public}d, GetResult: %{public}d",
            callbackId, userStatusData->GetFeature(), userStatusData->GetResult());

        ExtensionAbilityManager::GetInstance().VoiceRecognize(userStatusData->GetResult());
    };
    auto &userstatusClient = Msdp::UserStatusAwareness::UserStatusClient::GetInstance();
    ret = userstatusClient.SubscribeCallback(FEATURE_VOICE_RECOGNITION, func);
    userstatusClient.Subscribe(FEATURE_VOICE_RECOGNITION, {});
    HILOG_INFO("userstatusClient.SubscribeCallback RET: %{public}d", ret);
    #endif // ACCESSIBILITY_USER_STATUS_AWARENESS
    return ret;
}

void MsdpManager::UnSubscribeVoiceRecognition()
{
    HILOG_INFO();
    #ifdef ACCESSIBILITY_USER_STATUS_AWARENESS
    auto &userstatusClient = Msdp::UserStatusAwareness::UserStatusClient::GetInstance();
    userstatusClient.Unsubscribe(FEATURE_VOICE_RECOGNITION);
    HILOG_INFO("msdp userstatusClient.Unsubscribe");
    #endif // ACCESSIBILITY_USER_STATUS_AWARENESS
}
} // namespace Accessibility
} // namespace OHOS
