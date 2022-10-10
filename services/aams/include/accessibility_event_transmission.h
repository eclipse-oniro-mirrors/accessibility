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

#ifndef OHOS_EVENT_TRANSMISSION_H
#define OHOS_EVENT_TRANSMISSION_H

#include <cstdint>
#include "accessibility_event_info.h"
#include "key_event.h"
#include "pointer_event.h"
#include "refbase.h"

namespace OHOS {
namespace Accessibility {
class EventTransmission : public RefBase {
public:
    EventTransmission() {};
    virtual ~EventTransmission() {};
    virtual bool OnKeyEvent(MMI::KeyEvent &event);
    virtual bool OnPointerEvent(MMI::PointerEvent &event);
    virtual void OnMoveMouse(int32_t offsetX, int32_t offsetY);
    void SetNext(const sptr<EventTransmission> &next);
    sptr<EventTransmission> GetNext();
    virtual void DestroyEvents();
private:
    sptr<EventTransmission> next_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // OHOS_EVENT_TRANSMISSION_H