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
#ifndef ACCESSIBLE_APP_STATE_OBSERVER_H
#define ACCESSIBLE_APP_STATE_OBSERVER_H

#include "application_state_observer_stub.h"
#include "app_state_data.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAppStateObserver : public AppExecFwk::ApplicationStateObserverStub {
public:
    AccessibleAppStateObserver();
    virtual ~AccessibleAppStateObserver();

    void SetStateChangeCallback(std::function<void(const AppExecFwk::AppStateData&)> callback);

    virtual void OnAbilityStateChanged(const AppExecFwk::AbilityStateData &abilityStateData) override {};
    virtual void OnApplicationStateChanged(const AppExecFwk::AppStateData &appStateData) override {}
    virtual void OnAppStateChanged(const AppExecFwk::AppStateData &appStateData) override;

    virtual void OnForegroundApplicationChanged(const AppExecFwk::AppStateData &appStateData) override {};
    virtual void OnExtensionStateChanged(const AppExecFwk::AbilityStateData &abilityStateData) override {};
    virtual void OnProcessCreated(const AppExecFwk::ProcessData &processData) override {};
    virtual void OnProcessDied(const AppExecFwk::ProcessData &processData) override {};

private:
    std::function<void(const AppExecFwk::AppStateData&)> stateChangeCallback_;
    std::mutex callbackMutex_;
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBLE_APP_STATE_OBSERVER_H