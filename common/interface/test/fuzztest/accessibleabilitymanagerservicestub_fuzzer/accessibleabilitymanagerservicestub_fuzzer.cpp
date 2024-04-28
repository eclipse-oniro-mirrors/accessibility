/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "accessibleabilitymanagerservicestub_fuzzer.h"
#include "accessible_ability_manager_service_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t FOO_MAX_LEN = 1024;
    constexpr size_t U32_AT_SIZE = 4;
    constexpr size_t BASE_CODE = 800;
    constexpr size_t MESSAGE_SIZE = 61;
    constexpr size_t FUZZ_NUM1 = 1;
    constexpr size_t FUZZ_NUM2 = 2;
    constexpr size_t FUZZ_NUM3 = 3;
    constexpr size_t FUZZ_NUM8 = 8;
    constexpr size_t FUZZ_NUM16 = 16;
    constexpr size_t FUZZ_NUM24 = 24;
}

class AccessibleAbilityManagerServiceStubFuzzTest : public AccessibleAbilityManagerServiceStub {
public:
    AccessibleAbilityManagerServiceStubFuzzTest() = default;
    ~AccessibleAbilityManagerServiceStubFuzzTest() = default;

    RetError SendEvent(const AccessibilityEventInfo &uiEvent) override {return RET_OK;}

    uint32_t RegisterStateObserver(const sptr<IAccessibleAbilityManagerStateObserver> &callback) override {return 0;}

    uint32_t RegisterCaptionObserver(const sptr<IAccessibleAbilityManagerCaptionObserver>
        &callback) override {return 0;}

    void RegisterEnableAbilityListsObserver(
        const sptr<IAccessibilityEnableAbilityListsObserver> &observer) override {}

    RetError GetAbilityList(const uint32_t abilityTypes, const int32_t stateType,
        std::vector<AccessibilityAbilityInfo> &infos) override {return RET_OK;}

    RetError RegisterElementOperator(const int32_t windowId,
        const sptr<IAccessibilityElementOperator> &operation, bool isApp = false) override {return RET_OK;}

    RetError DeregisterElementOperator(const int32_t windowId) override {return RET_OK;}

    RetError GetCaptionProperty(AccessibilityConfig::CaptionProperty &caption) override {return RET_OK;}
    bool GetEnabledState() override {return true;}
    RetError GetCaptionState(bool &state) override {return RET_OK;}
    bool GetTouchGuideState() override {return true;}
    bool GetGestureState() override {return true;}
    bool GetKeyEventObserverState() override {return true;}

    RetError SetCaptionProperty(const AccessibilityConfig::CaptionProperty &caption) override {return RET_OK;}
    RetError SetCaptionState(const bool state) override {return RET_OK;}

    RetError EnableAbility(const std::string &name, const uint32_t capabilities) override {return RET_OK;}
    RetError GetEnabledAbilities(std::vector<std::string> &enabledAbilities) override {return RET_OK;}
    RetError DisableAbility(const std::string &name) override {return RET_OK;}
    int32_t GetActiveWindow() override {return 0;}

    RetError EnableUITestAbility(const sptr<IRemoteObject> &obj) override {return RET_OK;}
    RetError DisableUITestAbility() override {return RET_OK;}

    RetError SetScreenMagnificationState(const bool state) override {return RET_OK;}
    RetError SetShortKeyState(const bool state) override {return RET_OK;}
    RetError SetMouseKeyState(const bool state) override {return RET_OK;}
    RetError SetMouseAutoClick(const int32_t time) override {return RET_OK;}
    RetError SetShortkeyTarget(const std::string &name) override {return RET_OK;}
    RetError SetShortkeyMultiTarget(const std::vector<std::string> &name) override {return RET_OK;}
    RetError SetHighContrastTextState(const bool state) override {return RET_OK;}
    RetError SetInvertColorState(const bool state) override {return RET_OK;}
    RetError SetAnimationOffState(const bool state) override {return RET_OK;}
    RetError SetAudioMonoState(const bool state) override {return RET_OK;}
    RetError SetDaltonizationState(const bool state) override {return RET_OK;}
    RetError SetDaltonizationColorFilter(const uint32_t filter) override {return RET_OK;}
    RetError SetContentTimeout(const uint32_t time) override {return RET_OK;}
    RetError SetBrightnessDiscount(const float discount) override {return RET_OK;}
    RetError SetAudioBalance(const float balance) override {return RET_OK;}
    RetError SetClickResponseTime(const uint32_t time) override {return RET_OK;}
    RetError SetIgnoreRepeatClickState(const bool state) override {return RET_OK;}
    RetError SetIgnoreRepeatClickTime(const uint32_t time) override {return RET_OK;}

    RetError GetScreenMagnificationState(bool &state) override {return RET_OK;}
    RetError GetShortKeyState(bool &state) override {return RET_OK;}
    RetError GetMouseKeyState(bool &state) override {return RET_OK;}
    RetError GetMouseAutoClick(int32_t &time) override {return RET_OK;}
    RetError GetShortkeyTarget(std::string &name) override {return RET_OK;}
    RetError GetShortkeyMultiTarget(std::vector<std::string> &name) override {return RET_OK;}
    RetError GetHighContrastTextState(bool &state) override {return RET_OK;}
    RetError GetInvertColorState(bool &state) override {return RET_OK;}
    RetError GetAnimationOffState(bool &state) override {return RET_OK;}
    RetError GetAudioMonoState(bool &state) override {return RET_OK;}
    RetError GetDaltonizationState(bool &state) override {return RET_OK;}
    RetError GetDaltonizationColorFilter(uint32_t &type) override {return RET_OK;}
    RetError GetContentTimeout(uint32_t &timer) override {return RET_OK;}
    RetError GetBrightnessDiscount(float &brightness) override {return RET_OK;}
    RetError GetAudioBalance(float &balance) override {return RET_OK;}
    RetError GetClickResponseTime(uint32_t& time) override {return RET_OK;}
    RetError GetIgnoreRepeatClickState(bool& state) override {return RET_OK;}
    RetError GetIgnoreRepeatClickTime(uint32_t& time) override {return RET_OK;}
    void GetAllConfigs(AccessibilityConfigData& configData) override {}
    void GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId) override {}
    void GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId, int32_t& innerWid) override {}
    RetError GetFocusedWindowId(int32_t &focusedWindowId) override {return RET_OK;}
    uint32_t RegisterConfigObserver(const sptr<IAccessibleAbilityManagerConfigObserver> &callback) override {return 0;}
    void PostDelayUnloadTask() override {}
};

uint32_t GetU32Data(const uint8_t* ptr)
{
    return (ptr[0] << FUZZ_NUM24) | (ptr[FUZZ_NUM1] << FUZZ_NUM16) |
        (ptr[FUZZ_NUM2] << FUZZ_NUM8) | (ptr[FUZZ_NUM3]);
}

bool OnRemoteRequestSvcFuzzTest(const uint8_t* data, size_t size)
{
    uint32_t code = (GetU32Data(data) % MESSAGE_SIZE) + BASE_CODE;
    MessageParcel datas;
    std::u16string descriptor = AccessibleAbilityManagerServiceStubFuzzTest::GetDescriptor();
    datas.WriteInterfaceToken(descriptor);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    AccessibleAbilityManagerServiceStubFuzzTest serviceStub;
    serviceStub.OnRemoteRequest(code, datas, reply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::Accessibility::U32_AT_SIZE) {
        return 0;
    }

    OHOS::Accessibility::OnRemoteRequestSvcFuzzTest(data, size);
    return 0;
}