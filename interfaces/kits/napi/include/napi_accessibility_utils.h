/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NAPI_ACCESSIBILITY_UTILS_H
#define NAPI_ACCESSIBILITY_UTILS_H

#include <string>
#include <stdint.h>
#include "accessibility_ability_info.h"
#include "accessibility_event_info.h"
#include "accessibility_element_info.h"
#include "accessible_ability.h"
#include "napi_accessibility_system_ability_client.h"
#include "accessibility_window_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;

std::string GetStringFromNAPI(napi_env env, napi_value value);
bool ParseString(napi_env env, std::string &param, napi_value args);
bool ParseUint32(napi_env env, uint32_t &param, napi_value args);
napi_value GetErrorValue(napi_env env, int errCode);


void ConvertRectToJS(napi_env env, napi_value result, OHOS::Accessibility::Rect& rect);
void ConvertAccessibilityWindowInfoToJS(napi_env env, napi_value result,
                                    OHOS::Accessibility::AccessibilityWindowInfo& accessibilityWindowInfo);
void ConvertAccessibilityWindowInfosToJS(napi_env env, napi_value result,
                                     std::vector<OHOS::Accessibility::AccessibilityWindowInfo>& accessibilityWindowInfos);
void ConvertAccessibilityEventInfoToJS(napi_env env, napi_value objEventInfo,
                                       const OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
void ConvertAccessibleAbilityInfoToJS(napi_env env, napi_value result,
    OHOS::Accessibility::AccessibilityAbilityInfo& info);
void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);

void ConvertAbilityInfoToJS(napi_env env, napi_value result, OHOS::AppExecFwk::AbilityInfo& info);

void ConvertAbilityApplicationInfoToJS(napi_env env, napi_value result, OHOS::AppExecFwk::ApplicationInfo& info);

void ConvertElementInfoToJS(napi_env env, napi_value result,
                                  OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
void ConvertOperationToJS(napi_env env, napi_value result, OHOS::Accessibility::AccessibleAction& operation);
void ConvertRangeInfoToJS(napi_env env, napi_value nRangeInfo, OHOS::Accessibility::RangeInfo& rangeInfo);
void ConvertGridInfoToJS(napi_env env, napi_value nGrid, OHOS::Accessibility::GridInfo& grid);
void ConvertGridItemToJS(napi_env env, napi_value nGridItem,
                               OHOS::Accessibility::GridItemInfo& gridItem);
void ConvertEventInfoJSToNAPI(napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
void ConvertElementInfosToJS(napi_env env, napi_value result,
    std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);
OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(std::string type);
OHOS::Accessibility::AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(std::string type);
OHOS::Accessibility::AbilityStateType ConvertStringToAbilityStateTypes(std::string type);
OHOS::Accessibility::GlobalAction ConvertStringToGlobalAction(std::string type);
void ConvertActionArgsJSToNAPI(napi_env env, napi_value object, std::map<std::string, std::string> &args, OHOS::Accessibility::ActionType action);
std::vector<std::string>  ParseAbilityTypesToVec(uint32_t abilityTypesValue);
std::vector<std::string>  ParseCapabilitiesToVec(uint32_t capabilitiesValue);
std::vector<std::string>  ParseEventTypesToVec(uint32_t eventTypesValue);
#endif // NAPI_ACCESSIBILITY_UTILS_H