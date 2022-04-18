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
#include "common_event_data.h"

namespace OHOS {
namespace EventFwk {
CommonEventData::CommonEventData() : code_(0)
{}

CommonEventData::CommonEventData(const Want& want) : want_(want), code_(0)
{
    (void)want;
}

CommonEventData::CommonEventData(const Want& want, const int& code, const std::string& data)
    : want_(want), code_(code), data_(data)
{
    (void)want;
    (void)code;
    (void)data;
}

CommonEventData::~CommonEventData()
{}

void CommonEventData::SetCode(const int& code)
{
    code_ = code;
}

int CommonEventData::GetCode() const
{
    return code_;
}

void CommonEventData::SetData(const std::string& data)
{
    data_ = data;
}

std::string CommonEventData::GetData() const
{
    return data_;
}

void CommonEventData::SetWant(const Want& want)
{
    GTEST_LOG_(INFO) << "MOCK CommonEventData SetWant";
    want_ = want;
}

const Want& CommonEventData::GetWant() const
{
    GTEST_LOG_(INFO) << "MOCK CommonEventData GetWant";
    return want_;
}

bool CommonEventData::Marshalling(Parcel& parcel) const
{
    (void)parcel;
    return true;
}

bool CommonEventData::ReadFromParcel(Parcel& parcel)
{
    (void)parcel;
    return true;
}

CommonEventData* CommonEventData::Unmarshalling(Parcel& parcel)
{
    (void)parcel;
    CommonEventData* commonEventData = new CommonEventData();
    return commonEventData;
}
} // namespace EventFwk
} // namespace OHOS