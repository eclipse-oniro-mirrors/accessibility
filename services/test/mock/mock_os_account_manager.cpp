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

#include "accessibility_mt_helper.h"
#include "os_account_manager.h"

namespace OHOS {
namespace AccountSA {
const int32_t accountId = 100;

ErrCode OsAccountManager::QueryActiveOsAccountIds(std::vector<int32_t>& accountIds)
{
    accountIds.push_back(accountId);
    return ERR_OK;
}

ErrCode OsAccountManager::SubscribeOsAccount(const std::shared_ptr<OsAccountSubscriber>& subscriberAccount)
{
    (void)subscriberAccount;
    return ERR_OK;
}

OsAccountSubscriber::OsAccountSubscriber()
{
}

OsAccountSubscriber::OsAccountSubscriber(const OsAccountSubscribeInfo &subscribeInfo) : subscribeInfo_(subscribeInfo)
{
}

OsAccountSubscriber::~OsAccountSubscriber()
{
}

void OsAccountSubscriber::GetSubscribeInfo(OsAccountSubscribeInfo &info) const
{
    info = subscribeInfo_;
}

OsAccountSubscribeInfo::OsAccountSubscribeInfo()
    : osAccountSubscribeType_(ACTIVATING), name_("")
{}

OsAccountSubscribeInfo::OsAccountSubscribeInfo(const OS_ACCOUNT_SUBSCRIBE_TYPE &osAccountSubscribeType,
    const std::string &name) : osAccountSubscribeType_(osAccountSubscribeType), name_(name)
{
    (void)osAccountSubscribeType;
}

OsAccountSubscribeInfo::~OsAccountSubscribeInfo()
{}

void OsAccountSubscribeInfo::GetOsAccountSubscribeType(OS_ACCOUNT_SUBSCRIBE_TYPE &subscribetype) const
{
    subscribetype = osAccountSubscribeType_;
}

void OsAccountSubscribeInfo::SetOsAccountSubscribeType(const OS_ACCOUNT_SUBSCRIBE_TYPE &subscribetype)
{
    osAccountSubscribeType_ = subscribetype;
}

void OsAccountSubscribeInfo::GetName(std::string &subscribeName) const
{
    subscribeName = name_;
}

void OsAccountSubscribeInfo::SetName(const std::string &subscribeName)
{
    name_ = subscribeName;
}

bool OsAccountSubscribeInfo::Marshalling(Parcel &parcel) const
{
    (void)parcel;
    return false;
}

OsAccountSubscribeInfo *OsAccountSubscribeInfo::Unmarshalling(Parcel &parcel)
{
    (void)parcel;
    return nullptr;
}

bool OsAccountSubscribeInfo::ReadFromParcel(Parcel &parcel)
{
    (void)parcel;
    return false;
}
} // AccountSA
} // OHOS