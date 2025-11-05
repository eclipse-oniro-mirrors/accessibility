/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ABILIT_CHANNEL_IMPL_FUZZ_H
#define ABILIT_CHANNEL_IMPL_FUZZ_H

#include "accessible_ability_channel_stub.h"
#include "accessibility_ipc_interface_code.h"
#include "parcel/accessibility_gesture_inject_path_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
    constexpr int64_t DURATION_TIME = 300;
}

class AbilityChannelImplFuzzTest : public AccessibleAbilityChannelStub {
public:
    AbilityChannelImplFuzzTest() = default;
    ~AbilityChannelImplFuzzTest() = default;

    RetError SearchElementInfoByAccessibilityId(const ElementBasicInfo elementBasicInfo,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter, bool systemApi) override
    {
        return RET_OK;
    }
    RetError SearchDefaultFocusedByWindowId(const ElementBasicInfo elementBasicInfo,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int32_t mode, bool isFilter) override
    {
        return RET_OK;
    }
    RetError SearchElementInfosByText(const int32_t accessibilityWindowId, const int64_t elementId,
        const std::string &text, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi) override
    {
        return RET_OK;
    }
    RetError FindFocusedElementInfo(const int32_t accessibilityWindowId, const int64_t elementId,
        const int32_t focusType, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi) override
    {
        return RET_OK;
    }
    RetError FocusMoveSearch(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t direction,
        const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, bool systemApi) override
    {
        return RET_OK;
    }
    RetError ExecuteAction(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t action,
        const std::map<std::string, std::string> &actionArguments, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError EnableScreenCurtain(bool isEnable) override
    {
        return RET_OK;
    }
    RetError HoldRunningLock() override
    {
        return RET_OK;
    }
    RetError UnholdRunningLock() override
    {
        return RET_OK;
    }
    RetError GetCursorPosition(const int32_t accessibilityWindowId, const int64_t elementId, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override
    {
        return RET_OK;
    }
    RetError GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo) override
    {
        return RET_OK;
    }
    RetError GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi) override
    {
        return RET_OK;
    }
    RetError GetWindowsByDisplayId(
        const uint64_t displayId, std::vector<AccessibilityWindowInfo>& windows, bool systemApi) override
    {
        return RET_OK;
    }
    void SetOnKeyPressEventResult(const bool handled, const int32_t sequence) override {}
    RetError SendSimulateGesture(const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath) override
    {
        return RET_OK;
    }
    RetError SetTargetBundleName(const std::vector<std::string> &targetBundleNames) override
    {
        return RET_OK;
    }
    RetError SetIsRegisterDisconnectCallback(bool isRegister) override
    {
        return RET_OK;
    }
    RetError NotifyDisconnect() override
    {
        return RET_OK;
    }
    void SearchElementInfoBySpecificProperty(const ElementBasicInfo elementBasicInfo,
        const SpecificPropertyParam& param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback) override {}

    RetError ConfigureEvents(const std::vector<uint32_t> needEvents) override
    {
        return RET_OK;
    }

    void FocusMoveSearchWithCondition(const int64_t elementId,
        const AccessibilityFocusMoveParam& param, const int32_t requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t windowId) override {}

};

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}
} // namespace Accessibility
} // namespace OHOS
#endif // ABILIT_CHANNEL_IMPL_FUZZ_H