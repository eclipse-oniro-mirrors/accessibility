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

#include <gtest/gtest.h>
#include "matching_skills.h"
#include "string_ex.h"

namespace OHOS {
namespace EventFwk {
MatchingSkills::MatchingSkills()
{}

MatchingSkills::MatchingSkills(const MatchingSkills &matchingSkills)
{
    entities_ = matchingSkills.entities_;
    events_ = matchingSkills.events_;
    schemes_ = matchingSkills.schemes_;
}

MatchingSkills::~MatchingSkills()
{}

std::string MatchingSkills::GetEntity(int index) const
{
    std::string entity;
    return entity;
}

void MatchingSkills::AddEntity(const std::string &entity)
{}

bool MatchingSkills::HasEntity(const std::string &entity) const
{
    return true;
}

void MatchingSkills::RemoveEntity(const std::string &entity)
{}

int MatchingSkills::CountEntities() const
{
    return entities_.size();
}

void MatchingSkills::AddEvent(const std::string &event)
{
    GTEST_LOG_(INFO) << "MOCK MatchingSkills AddEvent";
}

int MatchingSkills::CountEvent() const
{
    return events_.size();
}

std::string MatchingSkills::GetEvent(int index) const
{
    std::string event;
    return event;
}

std::vector<std::string> MatchingSkills::GetEvents() const
{
    return events_;
}

void MatchingSkills::RemoveEvent(const std::string &event)
{}

bool MatchingSkills::HasEvent(const std::string &event) const
{
    return true;
}

std::string MatchingSkills::GetScheme(int index) const
{
    std::string schemes;
    return schemes;
}

void MatchingSkills::AddScheme(const std::string &scheme)
{}

bool MatchingSkills::HasScheme(const std::string &scheme) const
{
    return true;
}

void MatchingSkills::RemoveScheme(const std::string &scheme)
{}

int MatchingSkills::CountSchemes() const
{
    return 0;
}

bool MatchingSkills::Marshalling(Parcel &parcel) const
{
    return true;
}

bool MatchingSkills::ReadFromParcel(Parcel &parcel)
{
    return true;
}

MatchingSkills *MatchingSkills::Unmarshalling(Parcel &parcel)
{
    return nullptr;
}

bool MatchingSkills::MatchEvent(const std::string &event) const
{
    return true;
}

bool MatchingSkills::MatchEntity(const std::vector<std::string> &entities) const
{
    return true;
}

bool MatchingSkills::MatchScheme(const std::string &scheme) const
{
    return true;
}

bool MatchingSkills::Match(const Want &want) const
{
    return true;
}
}  // namespace EventFwk
}  // namespace OHOS