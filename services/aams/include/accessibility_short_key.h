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

#ifndef ACCESSIBILITY_SHORT_KEY_H
#define ACCESSIBILITY_SHORT_KEY_H

#include "accessibility_event_transmission.h"
#include "event_handler.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityShortKey : public EventTransmission {
public:
    AccessibilityShortKey();
    virtual ~AccessibilityShortKey();

    bool OnKeyEvent(MMI::KeyEvent &event) override;
    void SendKeyEventToNext();
    void DestroyEvents() override;

private:
    class ShortKeyEventHandler : public AppExecFwk::EventHandler {
    public:
        ShortKeyEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, AccessibilityShortKey &shortKey);
        virtual ~ShortKeyEventHandler() = default;
        virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;

    private:
        AccessibilityShortKey &shortKey_;
    };

    void AddCachedKeyEvent(std::shared_ptr<MMI::KeyEvent> &event);
    void RecognizeShortKey(MMI::KeyEvent &event);
    void ClearCachedEventsAndMsg();
    bool IsUpValid();
    bool IsTriplePress();
    void OnShortKey();

    std::vector<std::shared_ptr<MMI::KeyEvent>> cachedKeyEvents_;
    std::shared_ptr<ShortKeyEventHandler> timeoutHandler_ = nullptr;
    int32_t lastKeyAction_ = MMI::KeyEvent::KEY_ACTION_UNKNOWN;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SHORT_KEY_H