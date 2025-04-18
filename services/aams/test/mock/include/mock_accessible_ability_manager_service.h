/*
 * Copyright (C) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef MOCK_ACCESSIBLE_ABILITY_MANAGER_SERVICE_H
#define MOCK_ACCESSIBLE_ABILITY_MANAGER_SERVICE_H

#include <gmock/gmock.h>
#include "accessible_ability_manager_service.h"
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibleAbilityManagerService : public AccessibleAbilityManagerService {
    DECLARE_DELAYED_SINGLETON(MockAccessibleAbilityManagerService)
    DECLEAR_SYSTEM_ABILITY(MockAccessibleAbilityManagerService)
public:
    MOCK_METHOD0(OnStart, void());
    MOCK_METHOD0(OnStop, void());
    MOCK_METHOD2(SendEvent, ErrCode(const AccessibilityEventInfoParcel& eventInfoParcel, int32_t flag));
    MOCK_METHOD2(RegisterStateCallback,
        uint32_t(const sptr<AccessibleAbilityManagerServiceStub>& callback, const int32_t accountId));
    MOCK_METHOD2(RegisterCaptionPropertyCallback,
        uint32_t(const sptr<IAccessibleAbilityManagerCaptionObserver>& callback, const int32_t accountId));
    MOCK_METHOD3(GetAbilityList, ErrCode(uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfoParcel>& infos));
    MOCK_METHOD2(RegisterElementOperatorByWindowId,
        ErrCode(const int32_t windowId, const sptr<IAccessibilityElementOperator>& elementOperator));
    MOCK_METHOD2(RegisterElementOperatorByParameter, ErrCode(const RegistrationPara& parameter,
        const sptr<IAccessibilityElementOperator>& elementOperator));
    MOCK_METHOD1(DeregisterElementOperatorByWindowId, ErrCode(const int32_t windowId));
    MOCK_METHOD2(DeregisterElementOperatorByWindowIdAndTreeId, ErrCode(const int32_t windowId,
        const int32_t treeId));
    MOCK_METHOD1(GetCaptionProperty, ErrCode(CaptionPropertyParcel& caption));
    MOCK_METHOD1(SetCaptionProperty, ErrCode(const CaptionPropertyParcel& caption));
    MOCK_METHOD1(SetCaptionState, ErrCode(bool state));
    MOCK_METHOD1(SetTouchEventInjector, void(const sptr<TouchEventInjector>& touchEventInjector));

    inline sptr<TouchEventInjector> GetTouchEventInjector()
    {
        return nullptr;
    }

    inline sptr<KeyEventFilter> GetKeyEventFilter()
    {
        return nullptr;
    }

    MOCK_METHOD1(SetKeyEventFilter, void(const sptr<KeyEventFilter>& keyEventFilter));
    MOCK_METHOD5(NotifyDisplayResizeStateChanged,
        void(int32_t displayId, Rect& rect, float scale, float centerX, float centerY));

    inline int32_t GetCurrentAccountId()
    {
        return 0;
    }

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AAMSEventHandler> handler_;
    inline std::shared_ptr<AAMSEventHandler>& GetMainHandler()
    {
        return handler_;
    }

    inline std::shared_ptr<AppExecFwk::EventRunner>& GetMainRunner()
    {
        return runner_;
    }

    MOCK_METHOD0(GetCurrentAccountData, sptr<AccessibilityAccountData>());
    MOCK_METHOD0(GetBundleMgrProxy, sptr<AppExecFwk::IBundleMgr>());
    MOCK_METHOD1(AddedUser, void(int32_t accountId));
    MOCK_METHOD1(RemovedUser, void(int32_t accountId));
    MOCK_METHOD1(SwitchedUser, void(int32_t accountId));
    MOCK_METHOD1(PackageChanged, void(const std::string& bundleName));
    MOCK_METHOD1(PackageRemoved, void(const std::string& bundleName));
    MOCK_METHOD1(PackageAdd, void(const std::string& bundleName));
    MOCK_METHOD0(UpdateAccessibilityManagerService, void());
    MOCK_METHOD0(HoldRunningLock, void());
    MOCK_METHOD0(UnholdRunningLock, void());
    MOCK_METHOD1(GetCaptionState, ErrCode(bool &state));
    MOCK_METHOD1(GetScreenReaderState, ErrCode(bool &state));
    MOCK_METHOD1(SetTouchGuideState, bool(const bool state));
    MOCK_METHOD1(SetGestureState, bool(const bool state));
    MOCK_METHOD1(SetKeyEventObserverState, bool(const bool state));
    MOCK_METHOD1(SetEnabledObj, bool(std::map<std::string, AppExecFwk::ElementName> it));
    MOCK_METHOD1(GetEnabledAbilities, ErrCode(std::vector<std::string> &enabledAbilities));
    MOCK_METHOD1(GetActiveWindow, ErrCode(int32_t &windowId));
    MOCK_METHOD2(GetRealWindowAndElementId, ErrCode(int32_t& windowId, int64_t& elementId));
    MOCK_METHOD3(GetSceneBoardInnerWinId, ErrCode(int32_t windowId, int64_t elementId, int32_t& innerWid));
    MOCK_METHOD1(ExecuteActionOnAccessibilityFocused, bool(const ActionType &action));
    MOCK_METHOD1(FindFocusedElement, bool(AccessibilityElementInfo &elementInfo));
    MOCK_METHOD1(GetFocusedWindowId, ErrCode(int32_t &focusedWindowId));
    MOCK_METHOD1(RemoveRequestId, ErrCode(int32_t requestId));
    MOCK_METHOD3(GetRootParentId, ErrCode(int32_t windowId, int32_t treeId, int64_t &parentId));
    MOCK_METHOD2(GetAllTreeId, RetError(int32_t windowId, std::vector<int32_t> &treeIds));
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_MANAGER_SERVICE_H