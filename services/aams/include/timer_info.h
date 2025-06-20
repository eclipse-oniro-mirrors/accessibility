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
 
#ifndef TIMER_INFO_H
#define TIMER_INFO_H
 
#include "itimer_info.h"
 
namespace OHOS {
namespace Accessibility {
 
class TimerInfo : public OHOS::MiscServices::ITimerInfo {
public:
    TimerInfo();
    virtual ~TimerInfo();
    void OnTrigger() override;
    void SetType(const int &type) override;
    void SetRepeat(bool repeat) override;
    void SetInterval(const uint64_t &_interval) override;
    void SetWantAgent(std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent) override;
    void SetCallbackInfo(const std::function<void()> &callBack);
    void SetTimerId(uint64_t timeId);
private:
    std::function<void()> callBack { nullptr };
};
 
TimerInfo::TimerInfo()
{
}
 
TimerInfo::~TimerInfo()
{
}
 
void TimerInfo::OnTrigger()
{
    if (this->callBack != nullptr) {
        this->callBack();
    }
}
 
void TimerInfo::SetCallbackInfo(const std::function<void()> &callBack)
{
    this->callBack = callBack;
}
 
void TimerInfo::SetType(const int &type)
{
    this->type = type;
}
 
void TimerInfo::SetRepeat(bool repeat)
{
    this->repeat = repeat;
}
 
void TimerInfo::SetInterval(const uint64_t &interval)
{
    this->interval = interval;
}
 
void TimerInfo::SetWantAgent(std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent)
{
    this->wantAgent = wantAgent;
}
} // namespace Accessibility
} // namespace OHOS
#endif // TIMER_INFO_H