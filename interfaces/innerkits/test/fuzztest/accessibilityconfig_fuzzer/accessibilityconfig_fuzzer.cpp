/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "accessibilityconfig_fuzzer.h"
#include "accessibility_config.h"
#include "securec.h"

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 98;
    constexpr char END_CHAR = '\0';
    constexpr size_t LEN = 10;
} // namespace

template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    (void)memcpy_s(&object, objectSize, data, size);
    return objectSize;
}

static size_t GenerateCaptionProperty(
    OHOS::AccessibilityConfig::CaptionProperty &property, const uint8_t* data, size_t size)
{
    size_t position = 0;
    uint32_t temp = 0;
    position += GetObject<uint32_t>(temp, &data[position], size - position);
    property.SetFontColor(temp);

    position += GetObject<uint32_t>(temp, &data[position], size - position);
    property.SetWindowColor(temp);

    position += GetObject<uint32_t>(temp, &data[position], size - position);
    property.SetBackgroundColor(temp);

    position += GetObject<uint32_t>(temp, &data[position], size - position);
    property.SetFontScale(static_cast<int32_t>(temp));

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string family(name);
    property.SetFontFamily(family);
    position += LEN;

    (void)memcpy_s(&name, LEN, &data[position], LEN);
    std::string type(name);
    property.SetFontFamily(type);
    position += LEN;

    return position;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    auto &abConfig = Singleton<OHOS::AccessibilityConfig::AccessibilityConfig>::GetInstance();

    size_t startPos = 0;
    abConfig.SetScreenMagnificationState(data[startPos++] & 0x01);
    abConfig.SetShortKeyState(data[startPos++] & 0x01);
    abConfig.SetMouseKeyState(data[startPos++] & 0x01);
    abConfig.SetCaptionState(data[startPos++] & 0x01);
    abConfig.SetHighContrastTextState(data[startPos++] & 0x01);
    abConfig.SetInvertColorState(data[startPos++] & 0x01);
    abConfig.SetAnimationOffState(data[startPos++] & 0x01);
    abConfig.SetAudioMonoState(data[startPos++] & 0x01);

    uint32_t temp = 0;
    startPos += GetObject<uint32_t>(temp, &data[startPos], size - startPos);
    abConfig.SetContentTimeout(temp);

    startPos += GetObject<uint32_t>(temp, &data[startPos], size - startPos);
    abConfig.SetDaltonizationColorFilter(static_cast<OHOS::AccessibilityConfig::DALTONIZATION_TYPE>(temp));

    startPos += GetObject<uint32_t>(temp, &data[startPos], size - startPos);
    abConfig.SetMouseAutoClick(static_cast<int32_t>(temp));

    float tempFloat = 0;
    startPos += GetObject<float>(tempFloat, &data[startPos], size - startPos);
    abConfig.SetBrightnessDiscount(tempFloat);

    startPos += GetObject<float>(tempFloat, &data[startPos], size - startPos);
    abConfig.SetAudioBalance(tempFloat);

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    (void)memcpy_s(&name, LEN, &data[startPos], LEN);
    std::string nameStr(name);
    abConfig.SetShortkeyTarget(nameStr);
    startPos += LEN;

    OHOS::AccessibilityConfig::CaptionProperty property;
    startPos += GenerateCaptionProperty(property, &data[startPos], size - startPos);
    abConfig.SetCaptionProperty(property);

    (void)memcpy_s(&name, LEN, &data[startPos], LEN);
    std::string abilityName1(name);
    startPos += LEN;
    startPos += GetObject<uint32_t>(temp, &data[startPos], size - startPos);
    abConfig.EnableAbility(abilityName1, temp);

    (void)memcpy_s(&name, LEN, &data[startPos], LEN);
    std::string abilityName2(name);
    startPos += LEN;
    abConfig.DisableAbility(abilityName2);

    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
