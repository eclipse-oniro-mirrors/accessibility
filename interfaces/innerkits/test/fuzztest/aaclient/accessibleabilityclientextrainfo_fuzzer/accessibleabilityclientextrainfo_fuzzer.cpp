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

#include "accessibleabilityclientextrainfo_fuzzer.h"
#include "accessible_ability_client_fuzz.h"

namespace OHOS {

static void FuzzWithExtraElementInfoFirstPart(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    OHOS::Accessibility::ExtraElementInfo extraElementInfo;
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string keyStr(name);
    std::string valueStr(name);
    extraElementInfo.SetExtraElementInfo(keyStr, valueStr);

    int32_t int32Data = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    extraElementInfo.SetExtraElementInfo(keyStr, int32Data);

    extraElementInfo.GetExtraElementInfoValueStr();
    extraElementInfo.GetExtraElementInfoValueInt();

    sourceElementInfo.SetExtraElement(extraElementInfo);
    sourceElementInfo.GetExtraElement();
    sourceElementInfo.GetAccessibilityLevel();

    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    sourceElementInfo.SetZIndex(int32Data);
    sourceElementInfo.GetZIndex();

    float floatData = 0.0;
    position += GetObject<float>(floatData, &data[position], size - position);
    sourceElementInfo.SetOpacity(floatData);
    sourceElementInfo.GetOpacity();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string backgroundColor(name);
    sourceElementInfo.SetBackgroundColor(backgroundColor);
    sourceElementInfo.GetBackgroundColor();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string backgroundImage(name);
    sourceElementInfo.SetBackgroundImage(backgroundImage);
    sourceElementInfo.GetBackgroundImage();
}

static void FuzzWithExtraElementInfoSecondPart(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string blur(name);
    sourceElementInfo.SetBlur(blur);
    sourceElementInfo.GetBlur();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string hitTestBehavior(name);
    sourceElementInfo.SetHitTestBehavior(hitTestBehavior);
    sourceElementInfo.GetHitTestBehavior();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string customComponentType(name);
    sourceElementInfo.SetCustomComponentType(customComponentType);
    sourceElementInfo.GetCustomComponentType();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string accessibilityNextFocusInspectorKey(name);
    sourceElementInfo.SetAccessibilityNextFocusInspectorKey(accessibilityNextFocusInspectorKey);
    sourceElementInfo.GetAccessibilityNextFocusInspectorKey();
}

static void FuzzWithExtraElementInfoThirdPart(OHOS::Accessibility::AccessibilityElementInfo &sourceElementInfo,
    const uint8_t* data, size_t size, size_t& position)
{
    int64_t int64Data = 0;
    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetNavDestinationId(int64Data);
    sourceElementInfo.GetNavDestinationId();

    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetAccessibilityNextFocusId(int64Data);
    sourceElementInfo.GetAccessibilityNextFocusId();

    position += GetObject<int64_t>(int64Data, &data[position], size - position);
    sourceElementInfo.SetAccessibilityPreviousFocusId(int64Data);
    sourceElementInfo.GetAccessibilityPreviousFocusId();

    bool boolData = false;
    position += GetObject<bool>(boolData, &data[position], size - position);
    sourceElementInfo.SetIsActive(boolData);
    sourceElementInfo.GetIsActive();

    position += GetObject<bool>(boolData, &data[position], size - position);
    sourceElementInfo.SetAccessibilityVisible(boolData);
    sourceElementInfo.GetAccessibilityVisible();

    position += GetObject<bool>(boolData, &data[position], size - position);
    sourceElementInfo.SetClip(boolData);
    sourceElementInfo.GetClip();

    position += GetObject<bool>(boolData, &data[position], size - position);
    sourceElementInfo.SetAccessibilityScrollable(boolData);
    sourceElementInfo.GetAccessibilityScrollable();
}

bool FuzzWithExtraElementInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    size_t position = 0;
    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    FuzzWithExtraElementInfoFirstPart(sourceElementInfo, data, size, position);
    FuzzWithExtraElementInfoSecondPart(sourceElementInfo, data, size, position);
    FuzzWithExtraElementInfoThirdPart(sourceElementInfo, data, size, position);

    return true;
}

bool FuzzWithSpanInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }

    OHOS::Accessibility::SpanInfo span;
    int32_t int32Data = 0;
    size_t position = 0;
    position += GetObject<int32_t>(int32Data, &data[position], size - position);
    span.SetSpanId(int32Data);
    span.GetSpanId();

    char name[LEN + 1];
    name[LEN] = END_CHAR;
    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string spanText(name);
    span.SetSpanText(spanText);
    span.GetSpanText();

    OHOS::Accessibility::AccessibilityElementInfo sourceElementInfo;
    std::vector<OHOS::Accessibility::SpanInfo> spanList;
    spanList.push_back(span);
    sourceElementInfo.SetSpanList(spanList);
    sourceElementInfo.GetSpanList();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string accessibilityText(name);
    span.SetAccessibilityText(accessibilityText);
    span.GetAccessibilityText();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string accessibilityDescription(name);
    span.SetAccessibilityDescription(accessibilityDescription);
    span.GetAccessibilityDescription();

    for (size_t i = 0; i < LEN; i++) {
        position += GetObject<char>(name[i], &data[position], size - position);
    }
    std::string accessibilityLevel(name);
    span.SetAccessibilityLevel(accessibilityLevel);
    span.GetAccessibilityLevel();

    sourceElementInfo.AddSpan(span);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzWithExtraElementInfo(data, size);
    OHOS::FuzzWithSpanInfo(data, size);
    return 0;
}