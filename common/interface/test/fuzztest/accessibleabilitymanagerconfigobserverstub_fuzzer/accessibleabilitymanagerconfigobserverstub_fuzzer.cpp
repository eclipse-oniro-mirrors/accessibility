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

#include "accessibleabilitymanagerconfigobserverstub_fuzzer.h"
#include "accessible_ability_manager_config_observer_stub.h"
#include "securec.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr size_t DATA_MIN_SIZE = 200;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
}

class ConfigObserverImplFuzzTest : public AccessibleAbilityManagerConfigObserverStub {
public:
    ConfigObserverImplFuzzTest() = default;
    ~ConfigObserverImplFuzzTest() = default;
    void OnConfigStateChanged(const uint32_t stateType) override {};
    void OnAudioBalanceChanged(const float audioBalance) override {};
    void OnBrightnessDiscountChanged(const float brightnessDiscount) override {};
    void OnContentTimeoutChanged(const uint32_t contentTimeout) override {};
    void OnDaltonizationColorFilterChanged(const uint32_t filterType) override {};
    void OnMouseAutoClickChanged(const int32_t mouseAutoClick) override {};
    void OnShortkeyTargetChanged(const std::string &shortkeyTarget) override {};
    void OnShortkeyMultiTargetChanged(const std::vector<std::string> &shortkeyMultiTarget) override {};
    void OnClickResponseTimeChanged(const uint32_t clickResponseTime) override {};
    void OnIgnoreRepeatClickTimeChanged(const uint32_t ignoreRepeatClickTime) override {};
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

bool OnRemoteRequestFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    uint32_t configCode = 0;
    MessageParcel configData;
    MessageParcel configReply;
    MessageOption option(MessageOption::TF_SYNC);

    GetObject<uint32_t>(configCode, &data[position], size - position);
    ConfigObserverImplFuzzTest callBackImp;
    configData.WriteInterfaceToken(ConfigObserverImplFuzzTest::GetDescriptor());
    callBackImp.OnRemoteRequest(configCode, configData, configReply, option);
    return true;
}
} // namespace Accessibility
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Accessibility::OnRemoteRequestFuzzTest(data, size);
    return 0;
}