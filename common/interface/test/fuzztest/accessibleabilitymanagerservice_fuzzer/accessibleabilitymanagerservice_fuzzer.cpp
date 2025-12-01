/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "accessibleabilitymanagerservice_fuzzer.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "token_setproc.h"
#include "accessible_ability_manager_service.h"

namespace OHOS {
namespace Accessibility {

class StateObserver : public IAccessibleAbilityManagerStateObserver {
public:

    virtual void OnStateChanged(const uint32_t stateType) override {}

    virtual sptr<IRemoteObject> AsObject() override {
        return nullptr;
    }
};

class CaptionObserver : public IAccessibleAbilityManagerCaptionObserver {
public:

    virtual void OnPropertyChanged(const AccessibilityConfig::CaptionProperty &property) override {}

    virtual sptr<IRemoteObject> AsObject() override {
        return nullptr;
    }
};

class EnableAbilityListsObserver : public IAccessibilityEnableAbilityListsObserver {
public:

    virtual void OnAccessibilityEnableAbilityListsChanged() override {}

    virtual void OnAccessibilityInstallAbilityListsChanged() override {}

    virtual sptr<IRemoteObject> AsObject() override {
        return nullptr;
    }
};

class ConfigObserver : public IAccessibleAbilityManagerConfigObserver {

    virtual void OnConfigStateChanged(const uint32_t stateType) override {}
    virtual void OnAudioBalanceChanged(const float audioBalance) override {}
    virtual void OnBrightnessDiscountChanged(const float brightnessDiscount) override {}
    virtual void OnContentTimeoutChanged(const uint32_t contentTimeout) override {}
    virtual void OnDaltonizationColorFilterChanged(const uint32_t filterType) override {}
    virtual void OnMouseAutoClickChanged(const int32_t mouseAutoClick) override {}
    virtual void OnShortkeyTargetChanged(const std::string &shortkeyTarget) override {}
    virtual void OnShortkeyMultiTargetChanged(const std::vector<std::string> &shortkeyMultiTarget) override {}
    virtual void OnClickResponseTimeChanged(const uint32_t clickResponseTime) override {}
    virtual void OnIgnoreRepeatClickTimeChanged(const uint32_t ignoreRepeatClickTime) override {}

    virtual sptr<IRemoteObject> AsObject() override {
        return nullptr;
    }
};

class AccessibilityElementOperator : public IAccessibilityElementOperator {
public:

    virtual RetError SearchElementInfoByAccessibilityId(const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
        bool isFliter = false) override {return RET_OK;}

    virtual void SearchDefaultFocusedByWindowId(const int32_t windowId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
        bool isFliter = false) override {}

    virtual void SearchElementInfosByText(const int64_t elementId, const std::string &text,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void FindFocusedElementInfo(const int64_t elementId, const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void FocusMoveSearch(const int64_t elementId, const int32_t direction, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void ExecuteAction(const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void GetCursorPosition(const int64_t elementId,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void ClearFocus() override {}

    virtual void OutsideTouch() override {}

    virtual void SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId,
        const int32_t childWindowId) override {}

    virtual void SetBelongTreeId(const int32_t treeId) override {}

    virtual void SetParentWindowId(const int32_t iParentWindowId) override {}

    virtual void SearchElementInfoBySpecificProperty(const int64_t elementId,
        const SpecificPropertyParam& param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual void FocusMoveSearchWithCondition(const AccessibilityElementInfo &info,
        const AccessibilityFocusMoveParam &param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    virtual sptr<IRemoteObject> AsObject() override {
        return nullptr;
    }
};

} // namespace Accessibility
} // namespace OHOS

extern "C" int FuzzIAccessibleAbilityManagerService(FuzzedDataProvider &provider) {
    static const int ipccodes[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 101, 102, 104, 105
    };
    int code = provider.PickValueInArray(ipccodes);
    switch (code) {
        case 0: {
            OHOS::Accessibility::AccessibilityEventInfo event;
            OHOS::Accessibility::AccessibilityEventInfoParcel eventParcel(event);
            int32_t flag = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SendEvent(eventParcel, flag);
            break;
        }
        case 1: {
            OHOS::sptr<IAccessibleAbilityManagerStateObserver> callback = new OHOS::Accessibility::StateObserver();
            uint32_t state = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterStateObserver(callback, state);
            break;
        }
        case 2: {
            OHOS::sptr<IAccessibleAbilityManagerCaptionObserver> captionObserver =
                new OHOS::Accessibility::CaptionObserver();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterCaptionObserver(captionObserver);
            break;
        }
        case 3: {
            OHOS::sptr<IAccessibilityEnableAbilityListsObserver> observer =
                new OHOS::Accessibility::EnableAbilityListsObserver();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterEnableAbilityListsObserver(observer);
            break;
        }
        case 4: {
            uint32_t abilityTypes = provider.ConsumeIntegral<uint32_t>();
            int32_t stateType = provider.ConsumeIntegral<int32_t>();
            std::vector<OHOS::Accessibility::AccessibilityAbilityInfoParcel> infos;
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetAbilityList(abilityTypes, stateType, infos);
            break;
        }
        case 5: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            OHOS::sptr<IAccessibilityElementOperator> elementOperator =
                new OHOS::Accessibility::AccessibilityElementOperator();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterElementOperatorByWindowId(windowId, elementOperator);
            break;
        }
        case 6: {
            OHOS::Accessibility::RegistrationPara parameter {
                .windowId = provider.ConsumeIntegral<int32_t>(),
                .parentWindowId = provider.ConsumeIntegral<int32_t>(),
                .parentTreeId = provider.ConsumeIntegral<int32_t>(),
                .elementId = provider.ConsumeIntegral<int64_t>(),
            };
            OHOS::sptr<IAccessibilityElementOperator> elementOperator =
                new OHOS::Accessibility::AccessibilityElementOperator();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterElementOperatorByParameter(parameter, elementOperator);
            break;
        }
        case 7: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .DeregisterElementOperatorByWindowId(windowId);
            break;
        }
        case 8: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            int32_t treeId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .DeregisterElementOperatorByWindowIdAndTreeId(windowId, treeId);
            break;
        }
        case 9: {
            CaptionPropertyParcel caption;
            bool isPermissionRequired = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetCaptionProperty(caption, isPermissionRequired);
            break;
        }
        case 10: {
            bool state = provider.ConsumeBool();
            bool isPermissionRequired = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetCaptionState(state, isPermissionRequired);
            break;
        }
        case 11: {
            CaptionPropertyParcel caption;
            bool isPermissionRequired = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetCaptionProperty(caption, isPermissionRequired);
            break;
        }
        case 12: {
            bool state = provider.ConsumeBool();
            bool isPermissionRequired = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetCaptionState(state, isPermissionRequired);
            break;
        }
        case 13: {
            std::string name = provider.ConsumeRandomLengthString();
            uint32_t capabilities = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .EnableAbility(name, capabilities);
            break;
        }
        case 14: {
            std::vector<std::string> enabledAbilities;
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetEnabledAbilities(enabledAbilities);
            break;
        }
        case 15: {
            std::string name = provider.ConsumeRandomLengthString();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .DisableAbility(name);
            break;
        }
        case 16: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetMagnificationState(state);
            break;
        }
        case 17: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetActiveWindow(windowId);
            break;
        }
        case 18: {
            std::string name = provider.ConsumeRandomLengthString();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .CheckExtensionAbilityPermission(name);
            break;
        }
        case 19: {
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .DisableUITestAbility();
            break;
        }
        case 20: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetScreenMagnificationState(state);
            break;
        }
        case 21: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetShortKeyState(state);
            break;
        }
        case 22: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetMouseKeyState(state);
            break;
        }
        case 23: {
            int32_t time = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetMouseAutoClick(time);
            break;
        }
        case 24: {
            std::string name = provider.ConsumeRandomLengthString();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetShortkeyTarget(name);
            break;
        }
        case 25: {
            std::vector<std::string> nameList;
            nameList.push_back(provider.ConsumeRandomLengthString());
            nameList.push_back(provider.ConsumeRandomLengthString());
            nameList.push_back(provider.ConsumeRandomLengthString());
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetShortkeyMultiTarget(nameList);
            break;
        }
        case 26: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetHighContrastTextState(state);
            break;
        }
        case 27: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetInvertColorState(state);
            break;
        }
        case 28: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetAnimationOffState(state);
            break;
        }
        case 29: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetAudioMonoState(state);
            break;
        }
        case 30: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetDaltonizationState(state);
            break;
        }
        case 31: {
            uint32_t filter = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetDaltonizationColorFilter(filter);
            break;
        }
        case 32: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetContentTimeout(time);
            break;
        }
        case 33: {
            float discount = provider.ConsumeFloatingPoint<float>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetBrightnessDiscount(discount);
            break;
        }
        case 34: {
            float balance = provider.ConsumeFloatingPoint<float>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetAudioBalance(balance);
            break;
        }
        case 35: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetClickResponseTime(time);
            break;
        }
        case 36: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetIgnoreRepeatClickState(state);
            break;
        }
        case 37: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetIgnoreRepeatClickTime(time);
            break;
        }
        case 38: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetScreenMagnificationState(state);
            break;
        }
        case 39: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetShortKeyState(state);
            break;
        }
        case 40: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetMouseKeyState(state);
            break;
        }
        case 41: {
            int32_t time = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetMouseAutoClick(time);
            break;
        }
        case 42: {
            std::string name = provider.ConsumeRandomLengthString();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetShortkeyTarget(name);
            break;
        }
        case 43: {
            std::vector<std::string> nameList;
            nameList.push_back(provider.ConsumeRandomLengthString());
            nameList.push_back(provider.ConsumeRandomLengthString());
            nameList.push_back(provider.ConsumeRandomLengthString());
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetShortkeyMultiTarget(nameList);
            break;
        }
        case 44: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetHighContrastTextState(state);
            break;
        }
        case 45: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetInvertColorState(state);
            break;
        }
        case 46: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetAnimationOffState(state);
            break;
        }
        case 47: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetAudioMonoState(state);
            break;
        }
        case 48: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetDaltonizationState(state);
            break;
        }
        case 49: {
            uint32_t filter = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetDaltonizationColorFilter(filter);
            break;
        }
        case 50: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetContentTimeout(time);
            break;
        }
        case 51: {
            float brightness = provider.ConsumeFloatingPoint<float>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetBrightnessDiscount(brightness);
            break;
        }
        case 52: {
            float balance = provider.ConsumeFloatingPoint<float>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetAudioBalance(balance);
            break;
        }
        case 53: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetClickResponseTime(time);
            break;
        }
        case 54: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetIgnoreRepeatClickState(state);
            break;
        }
        case 55: {
            uint32_t time = provider.ConsumeIntegral<uint32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetIgnoreRepeatClickTime(time);
            break;
        }
        case 56: {
            OHOS::Accessibility::AccessibilityConfigData configData;
            OHOS::Accessibility::CaptionPropertyParcel caption;
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetAllConfigs(configData, caption);
            break;
        }
        case 57: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            int64_t elementId = provider.ConsumeIntegral<int64_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetRealWindowAndElementId(windowId, elementId);
            break;
        }
        case 58: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            int64_t elementId = provider.ConsumeIntegral<int64_t>();
            int32_t innerWindowId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetSceneBoardInnerWinId(windowId, elementId, innerWindowId);
            break;
        }
        case 59: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetFocusedWindowId(windowId);
            break;
        }
        case 60: {
            OHOS::sptr<IAccessibleAbilityManagerConfigObserver> observer = new OHOS::Accessibility::ConfigObserver();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RegisterConfigObserver(observer);
            break;
        }
        case 61: {
            int32_t requestId = provider.ConsumeIntegral<int32_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .RemoveRequestId(requestId);
            break;
        }
        case 62: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            int32_t treeId = provider.ConsumeIntegral<int32_t>();
            int64_t parentId = provider.ConsumeIntegral<int64_t>();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetRootParentId(windowId, treeId, parentId);
            break;
        }
        case 63: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            int32_t treeId = provider.ConsumeIntegral<int32_t>();
            int64_t parentId = provider.ConsumeIntegral<int64_t>();
            bool systemApi = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetRootParentId(windowId, treeId, parentId, systemApi);
            break;
        }
        case 64: {
            bool state = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetScreenReaderState(state);
            break;
        }
        case 65: {
            OHOS::Accessibility::AccessibilitySecCompRawdata rawData;
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SetEnhanceConfig(rawData);
            break;
        }
        case 66: {
            std::vector<uint32_t> needEvents;
            needEvents.push_back(provider.ConsumeIntegral<uint32_t>());
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .SearchNeedEvents(needEvents);
            break;
        }
        case 67: {
            std::string name = provider.ConsumeRandomLengthString();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetReadableRules(name);
            break;
        }
        case 68: {
            int32_t windowId = provider.ConsumeIntegral<int32_t>();
            bool systemApi = provider.ConsumeBool();
            OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance()
                .GetActiveWindow(windowId, systemApi);
            break;
        }
    }
    return 0;
}

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv) {
    if (SetSelfTokenID(718336240ull | (1ull << 32)) < 0) {
        return -1;
    }
    OHOS::Singleton<OHOS::Accessibility::AccessibleAbilityManagerService>::GetInstance().OnStart();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
    FuzzedDataProvider fdp(data,size);
    FuzzIAccessibleAbilityManagerService(fdp);
    return 0;
}
