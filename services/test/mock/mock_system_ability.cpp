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
#include "accessibility_common_helper.h"
#include "accessibility_mt_helper.h"
#include "system_ability.h"

namespace OHOS {
SystemAbility::SystemAbility(bool runOnCreate)
{
    isRunning_ = false;
    isRunOnCreate_ = runOnCreate;
    isDistributed_ = false;
    dumpLevel_ = 0;
    capability_ = u"";
}

SystemAbility::SystemAbility(int32_t systemAbilityId, bool runOnCreate) : SystemAbility(runOnCreate)
{
    saId_ = systemAbilityId;
}

SystemAbility::~SystemAbility()
{
}

bool SystemAbility::MakeAndRegisterAbility(SystemAbility* systemAbility)
{
    (void)systemAbility;
    Accessibility::AccessibilityCommonHelper::GetInstance().SetIsServicePublished(false);
    return true;
}

bool SystemAbility::AddSystemAbilityListener(int32_t systemAbilityId)
{
    OnAddSystemAbility(systemAbilityId, "");
    return true;
}

bool SystemAbility::RemoveSystemAbilityListener(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return true;
}

bool SystemAbility::Publish(sptr<IRemoteObject> systemAbility)
{
    (void)systemAbility;
    systemAbility.ForceSetRefPtr(nullptr);
    Accessibility::AccessibilityCommonHelper::GetInstance().SetIsServicePublished(true);
    return true;
}

void SystemAbility::StopAbility(int32_t systemAbilityId)
{
    return;
}

void SystemAbility::Start()
{
    OnStart();
    isRunning_ = true;
}

void SystemAbility::Stop()
{
    if (!isRunning_) {
        return;
    }

    OnStop();
    isRunning_ = false;
}

void SystemAbility::SADump()
{
    OnDump();
}

int32_t SystemAbility::GetSystemAbilitId() const
{
    return saId_;
}

void SystemAbility::SetLibPath(const std::u16string& libPath)
{
    libPath_ = libPath;
}

const std::u16string& SystemAbility::GetLibPath() const
{
    return libPath_;
}

void SystemAbility::SetDependSa(const std::vector<std::u16string>& dependSa)
{
    dependSa_ = dependSa;
}

const std::vector<std::u16string>& SystemAbility::GetDependSa() const
{
    return dependSa_;
}

void SystemAbility::SetRunOnCreate(bool isRunOnCreate)
{
    isRunOnCreate_ = isRunOnCreate;
}

bool SystemAbility::IsRunOnCreate() const
{
    return isRunOnCreate_;
}

void SystemAbility::SetDistributed(bool isDistributed)
{
    isDistributed_ = isDistributed;
}

bool SystemAbility::GetDistributed() const
{
    return isDistributed_;
}

bool SystemAbility::GetRunningStatus() const
{
    return isRunning_;
}

void SystemAbility::SetDumpLevel(uint32_t dumpLevel)
{
    dumpLevel_ = dumpLevel;
}

uint32_t SystemAbility::GetDumpLevel() const
{
    return dumpLevel_;
}

void SystemAbility::SetDependTimeout(int32_t dependTimeout)
{
    (void)dependTimeout;
}

int32_t SystemAbility::GetDependTimeout() const
{
    return dependTimeout_;
}

// The details should be implemented by subclass
void SystemAbility::OnDump()
{
}

// The details should be implemented by subclass
void SystemAbility::OnStart()
{
}

// The details should be implemented by subclass
void SystemAbility::OnStop()
{
}

// The details should be implemented by subclass
void SystemAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

// The details should be implemented by subclass
void SystemAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    (void)systemAbilityId;
    (void)deviceId;
}

sptr<IRemoteObject> SystemAbility::GetSystemAbility(int32_t systemAbilityId)
{
    (void)systemAbilityId;
    return nullptr;
}

void SystemAbility::SetCapability(const std::u16string& capability)
{
    capability_ = capability;
}

const std::u16string& SystemAbility::GetCapability() const
{
    return capability_;
}

void SystemAbility::SetPermission(const std::u16string& permission)
{
    permission_ = permission;
}
}