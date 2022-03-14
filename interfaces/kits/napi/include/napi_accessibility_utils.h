/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <string>
#include "accessibility_ability_info.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_info.h"
#include "accessibility_extension_context.h"
#include "gesture_simulation.h"
#include "key_event.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessibility_system_ability_client.h"

constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ZERO = 0;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;
constexpr int32_t CHAE_BUFFER_MAX = 1024;

std::string GetStringFromNAPI(napi_env env, napi_value value);
bool ParseString(napi_env env, std::string& param, napi_value args);
bool ParseUint32(napi_env env, uint32_t& param, napi_value args);
napi_value GetErrorValue(napi_env env, int errCode);

void ConvertAccessibilityWindowInfoToJS(
    napi_env env, napi_value result, const OHOS::Accessibility::AccessibilityWindowInfo& accessibilityWindowInfo);
void ConvertAccessibilityWindowInfosToJS(napi_env env, napi_value result,
    const std::vector<OHOS::Accessibility::AccessibilityWindowInfo>& accessibilityWindowInfos);
void ConvertAccessibilityEventInfoToJS(
    napi_env env, napi_value objEventInfo, const OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value& result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);

void ConvertElementInfoToJS(
    napi_env env, napi_value result, const OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
bool ConvertEventInfoJSToNAPI(napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
void ConvertElementInfosToJS(
    napi_env env, napi_value result, const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);
OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(std::string type);
OHOS::Accessibility::AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(std::string type);
OHOS::Accessibility::AbilityStateType ConvertStringToAbilityStateTypes(std::string type);
OHOS::Accessibility::GlobalAction ConvertStringToGlobalAction(std::string type);
void ConvertActionArgsJSToNAPI(
    napi_env env, napi_value object, std::map<std::string, std::string>& args, OHOS::Accessibility::ActionType action);

void ConvertGesturePathsJSToNAPI(
    napi_env env, napi_value object, std::vector<OHOS::Accessibility::GesturePathDefine>& gesturePaths);
void ConvertKeyEventToJS(napi_env env, napi_value result, OHOS::MMI::KeyEvent& keyEvent);
void ConvertCaptionPropertyToJS(napi_env env, napi_value& result, OHOS::Accessibility::CaptionProperty captionProperty);
void ConvertObjToCaptionProperty(
    napi_env env, napi_value object, OHOS::Accessibility::CaptionProperty* ptrCaptionProperty);
void ConvertJSToAccessibleAbilityInfos(napi_env env, napi_value arrayValue,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);
void ConvertJSToAccessibleAbilityInfo(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityAbilityInfo& abilityInfo);
void ConvertEnabledAbilitiesToJS(
    napi_env env, napi_value result, std::map<std::string, OHOS::AppExecFwk::ElementName>& enabledAbilities);
void ConvertJSToEnabledAbilities(
    napi_env env, napi_value arrayValue, std::map<std::string, OHOS::AppExecFwk::ElementName>& enabledAbilities);

void ConvertEnabledToJS(napi_env env, napi_value& captionsManager, bool value);
void ConvertStyleToJS(napi_env env, napi_value& captionsManager, OHOS::Accessibility::CaptionProperty captionProperty_);
uint32_t GetColorValue(napi_env env, napi_value object, napi_value propertyNameValue);
uint32_t GetColorValue(napi_env env, napi_value value);
uint32_t ConvertColorStringToNumer(std::string colorStr);

#endif  // NAPI_ACCESSIBILITY_UTILS_H