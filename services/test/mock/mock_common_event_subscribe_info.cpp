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
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo CommonEventSubscribeInfo";
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
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo SetPriority";
    priority_ = priority;
}

int32_t CommonEventSubscribeInfo::GetPriority() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo GetPriority";
    return priority_;
}

void CommonEventSubscribeInfo::SetPermission(const std::string& permission)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo SetPermission";
    permission_ = permission;
}

std::string CommonEventSubscribeInfo::GetPermission() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo GetPermission";
    return permission_;
}

CommonEventSubscribeInfo::ThreadMode CommonEventSubscribeInfo::GetThreadMode() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo GetThreadMode";
    return threadMode_;
}

void CommonEventSubscribeInfo::SetThreadMode(CommonEventSubscribeInfo::ThreadMode threadMode)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo SetThreadMode";
    threadMode_ = threadMode;
}

void CommonEventSubscribeInfo::SetDeviceId(const std::string& deviceId)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo SetDeviceId";
    deviceId_ = deviceId;
}

std::string CommonEventSubscribeInfo::GetDeviceId() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo GetDeviceId";
    return deviceId_;
}

const MatchingSkills& CommonEventSubscribeInfo::GetMatchingSkills() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventSubscribeInfo GetMatchingSkills";
    return matchingSkills_;
}

bool CommonEventSubscribeInfo::Marshalling(Parcel& parcel) const
{
    (void)parcel;
    return true;
}

bool CommonEventSubscribeInfo::ReadFromParcel(Parcel& parcel)
{
    (void)parcel;
    return true;
}

CommonEventSubscribeInfo* CommonEventSubscribeInfo::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    CommonEventSubscribeInfo* commonEventSubscribeInfo = new CommonEventSubscribeInfo();
    return commonEventSubscribeInfo;
}
} // namespace EventFwk
} // namespace OHOS