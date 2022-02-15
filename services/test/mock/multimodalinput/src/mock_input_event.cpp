/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
 
#include "input_event.h"
#include <chrono>

namespace OHOS {
namespace MMI {

InputEvent::InputEvent(int32_t eventType) : eventType_(eventType)
{
    Reset();
}

InputEvent::InputEvent(const InputEvent& other)
    : eventType_(other.eventType_), id_(other.id_), actionTime_(other.actionTime_),
    action_(other.action_), actionStartTime_(other.actionStartTime_),
    deviceId_(other.deviceId_), targetDisplayId_(other.targetDisplayId_),
    targetWindowId_(other.targetWindowId_), agentWindowId_(other.agentWindowId_),
    flag_(other.flag_), processedCallback_(other.processedCallback_)
{}

InputEvent::~InputEvent() {}

void InputEvent::Reset()
{
    int32_t conversionStep = 1000000;
    timespec ts = { 0, 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        actionTime_ = 0;
    }
    id_ = DEFALUTID;
    uint64_t nowTime = (ts.tv_sec * static_cast<uint64_t>(1e3)) + (ts.tv_nsec / conversionStep);
    int32_t actionTime = static_cast<int32_t>(nowTime);
    actionTime_ = actionTime;
    action_ = ACTION_UNKNOWN;
    actionStartTime_ = actionTime_;
    deviceId_ = DEFALUTID;
    targetDisplayId_ = DEFALUTID;
    targetWindowId_ = DEFALUTID;
    agentWindowId_ = DEFALUTID;
    flag_ = 0;
}

void InputEvent::SetActionStartTime(int32_t actionStartTime)
{
    actionStartTime_ = actionStartTime;
}

int32_t InputEvent::GetActionTime() const
{
    return actionTime_;
}

void InputEvent::SetActionTime(int32_t actionTime)
{
    actionTime_ = actionTime;
}

int32_t InputEvent::GetAction() const
{
    return action_;
}

void InputEvent::SetAction(int32_t action)
{
    action_ = action;
}

void InputEvent::AddFlag(int32_t flag)
{
    flag_ |= flag;
}

void InputEvent::ClearFlag()
{
    flag_ = 0X00000000;
}

}
}