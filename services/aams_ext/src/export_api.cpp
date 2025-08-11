/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed On an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "accessibility_notification_helper.h"
#include "visibility.h"
#include "time_service_client.h"

extern "C" {
API_EXPORT int32_t PublishIgnoreRepeatClickReminder()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::PublishIgnoreRepeatClickReminder();
}
API_EXPORT void CancelNotification()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::CancelNotification();
}
API_EXPORT int32_t RegisterTimers(uint64_t beginTime)
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::RegisterTimers(beginTime);
}
API_EXPORT void DestoryTimers()
{
    return OHOS::Accessibility::IgnoreRepeatClickNotification::DestoryTimers();
}
API_EXPORT int64_t GetWallTimeMs()
{
    if (OHOS::MiscServices::TimeServiceClient::GetInstance()) {
        return OHOS::MiscServices::TimeServiceClient::GetInstance()->GetWallTimeMs();
    }
    return 0;
}
}