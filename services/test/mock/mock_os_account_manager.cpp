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

#include "accessibility_helper.h"
#include "os_account_manager.h"

namespace OHOS {
namespace AccountSA {
ErrCode OsAccountManager::QueryActiveOsAccountIds(std::vector<int32_t>& ids)
{
    ids.push_back(Accessibility::AccessibilityHelper::accountId_);
    return ERR_OK;
}

ErrCode OsAccountManager::SubscribeOsAccount(const std::shared_ptr<OsAccountSubscriber> &subscriber)
{
    (void)subscriber;
    return ERR_OK;
}

OsAccountSubscriber::OsAccountSubscriber()
{}

OsAccountSubscriber::OsAccountSubscriber(const OsAccountSubscribeInfo &subscribeInfo) : subscribeInfo_(subscribeInfo)
{}

OsAccountSubscriber::~OsAccountSubscriber()
{}

ErrCode OsAccountSubscriber::GetSubscribeInfo(OsAccountSubscribeInfo &subscribeInfo) const
{
    subscribeInfo = subscribeInfo_;

    return ERR_OK;
}

OsAccountSubscribeInfo::OsAccountSubscribeInfo()
    : osAccountSubscribeType_(ACTIVATING), name_("")
{}

OsAccountSubscribeInfo::OsAccountSubscribeInfo(OS_ACCOUNT_SUBSCRIBE_TYPE &osAccountSubscribeType, std::string &name)
    : osAccountSubscribeType_(osAccountSubscribeType), name_(name)
{}

OsAccountSubscribeInfo::~OsAccountSubscribeInfo()
{}

ErrCode OsAccountSubscribeInfo::GetOsAccountSubscribeType(OS_ACCOUNT_SUBSCRIBE_TYPE &osAccountSubscribeType) const
{
    osAccountSubscribeType = osAccountSubscribeType_;

    return ERR_OK;
}

ErrCode OsAccountSubscribeInfo::SetOsAccountSubscribeType(const OS_ACCOUNT_SUBSCRIBE_TYPE &osAccountSubscribeType)
{
    osAccountSubscribeType_ = osAccountSubscribeType;

    return ERR_OK;
}

ErrCode OsAccountSubscribeInfo::GetName(std::string &name) const
{
    name = name_;

    return ERR_OK;
}

ErrCode OsAccountSubscribeInfo::SetName(const std::string &name)
{
    name_ = name;

    return ERR_OK;
}

bool OsAccountSubscribeInfo::Marshalling(Parcel &parcel) const
{
    return false;
}

OsAccountSubscribeInfo *OsAccountSubscribeInfo::Unmarshalling(Parcel &parcel)
{
    return nullptr;
}

bool OsAccountSubscribeInfo::ReadFromParcel(Parcel &parcel)
{
    return false;
}
} // AccountSA
} // OHOS