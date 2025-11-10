/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <functional>

namespace OHOS::Accessibility {
class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> onExitScope) : onExitScope_(onExitScope), dismissed_(false) {}

    ~ScopeGuard()
    {
    }

    void Dismiss()
    {
        dismissed_ = true;
    }

    void Commit()
    {
        if (!dismissed_) {
            onExitScope_();
            dismissed_ = true;
        }
    }

private:
    std::function<void()> onExitScope_;
    bool dismissed_;

private: // noncopyable
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;
};
} // namespace OHOS::Accessibility

#endif // SCOPE_GUARD_H