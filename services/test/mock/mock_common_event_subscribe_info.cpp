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

#include <gtest/gtest.h>
#include "common_event_subscribe_info.h"

namespace OHOS {
namespace EventFwk {
CommonEventSubscribeInfo::CommonEventSubscribeInfo(const MatchingSkills& matchingSkills)
    : matchingSkills_(matchingSkills), priority_(0), threadMode_(CommonEventSubscribeInfo::ASYNC)
{}

CommonEventSubscribeInfo::CommonEventSubscribeInfo() : priority_(0), threadMode_(CommonEventSubscribeInfo::ASYNC)
{}

CommonEventSubscribeInfo::CommonEventSubscribeInfo(const CommonEventSubscribeInfo& commonEventSubscribeInfo)
{
    matchingSkills_ = commonEventSubscribeInfo.matchingSkills_;
    priority_ = commonEventSubscribeInfo.priority_;
    permission_ = commonEventSubscribeInfo.permission_;
    deviceId_ = commonEventSubscribeInfo.deviceId_;
    threadMode_ = commonEventSubscribeInfo.threadMode_;
}

CommonEventSubscribeInfo::~CommonEventSubscribeInfo()
{}

void CommonEventSubscribeInfo::SetPriority(const int32_t& priority)
{
    priority_ = priority;
}

int32_t CommonEventSubscribeInfo::GetPriority() const
{
    return priority_;
}

void CommonEventSubscribeInfo::SetPermission(const std::string& permission)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo SetPermission";
    permission_ = permission;
}

std::string CommonEventSubscribeInfo::GetPermission() const
{
    return permission_;
}

CommonEventSubscribeInfo::ThreadMode CommonEventSubscribeInfo::GetThreadMode() const
{
    return threadMode_;
}

void CommonEventSubscribeInfo::SetThreadMode(CommonEventSubscribeInfo::ThreadMode threadMode)
{
    threadMode_ = threadMode;
}

void CommonEventSubscribeInfo::SetDeviceId(const std::string& deviceId)
{
    deviceId_ = deviceId;
}

std::string CommonEventSubscribeInfo::GetDeviceId() const
{
    return deviceId_;
}

const MatchingSkills& CommonEventSubscribeInfo::GetMatchingSkills() const
{
    return matchingSkills_;
}

bool CommonEventSubscribeInfo::Marshalling(Parcel& parcel) const
{
    return true;
}

bool CommonEventSubscribeInfo::ReadFromParcel(Parcel& parcel)
{
    return true;
}

CommonEventSubscribeInfo* CommonEventSubscribeInfo::Unmarshalling(Parcel& parcel)
{
    CommonEventSubscribeInfo* commonEventSubscribeInfo = new CommonEventSubscribeInfo();
    return commonEventSubscribeInfo;
}
} // namespace EventFwk
} // namespace OHOS