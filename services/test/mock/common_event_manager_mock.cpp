/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "common_event_manager.h"
#include <gtest/gtest.h>
#include "singleton.h"
namespace OHOS {
namespace EventFwk {
static std::shared_ptr<CommonEventSubscriber> g_subscriberMOCK = nullptr;

bool CommonEventManager::PublishCommonEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventManager PublishCommonEvent";
    if (!g_subscriberMOCK) {
        GTEST_LOG_(INFO) << "g_subscriberMOCK is nullptr";
    }
    g_subscriberMOCK->OnReceiveEvent(data);
    return true;
}

bool CommonEventManager::PublishCommonEvent(const CommonEventData &data, const CommonEventPublishInfo &publishInfo)
{
    return true;
}

bool CommonEventManager::PublishCommonEvent(const CommonEventData &data, const CommonEventPublishInfo &publishInfo,
    const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return true;
}

bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventManager SubscribeCommonEvent";
    g_subscriberMOCK = subscriber;
    return true;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventManager UnSubscribeCommonEvent";
    if (g_subscriberMOCK == subscriber) {
        g_subscriberMOCK = nullptr;
    }

    return true;
}

bool CommonEventManager::GetStickyCommonEvent(const std::string &event, CommonEventData &commonEventData)
{
    return true;
}
}  //namespace EventFwk
}  // namespace OHOS