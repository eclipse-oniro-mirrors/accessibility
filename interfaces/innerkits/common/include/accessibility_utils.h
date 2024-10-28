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

#ifndef ACCESSIBILITY_COMMON_UTILS_H
#define ACCESSIBILITY_COMMON_UTILS_H

#include <map>
#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_gesture_inject_path.h"
#include "accessibility_window_info.h"
#include "key_event.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "accessibility_def.h"

namespace OHOS {
namespace AccessibilityNapi {
// key action
enum KeyAction : int32_t {
    CANCEL = 0,
    DOWN = 1,
    UP = 2,
    UNKNOWN = 0xff,
};

std::string GetStringFromNAPI(napi_env env, napi_value value);
bool ParseBool(napi_env env, bool& param, napi_value args);
bool ParseString(napi_env env, std::string& param, napi_value args);
bool ParseNumber(napi_env env, napi_value args);
bool ParseInt32(napi_env env, int32_t& param, napi_value args);
bool ParseInt64(napi_env env, int64_t& param, napi_value args);
bool ParseDouble(napi_env env, double& param, napi_value args);
bool ParseBigInt(napi_env env, int64_t& param, napi_value args);
bool CheckJsFunction(napi_env env, napi_value args);
NAccessibilityErrMsg QueryRetMsg(OHOS::Accessibility::RetError errorCode);
napi_value CreateBusinessError(napi_env env, OHOS::Accessibility::RetError errCode);
napi_value GetErrorValue(napi_env env, int errCode);
bool CheckObserverEqual(napi_env env, napi_value observer, napi_env iterEnv, napi_ref iterRef);
std::string ConvertWindowTypeToString(OHOS::Accessibility::AccessibilityWindowType type);
std::string ConvertDaltonizationTypeToString(OHOS::AccessibilityConfig::DALTONIZATION_TYPE type);
std::string ConvertClickResponseTimeTypeToString(OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME type);
std::string ConvertIgnoreRepeatClickTimeTypeToString(OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME type);
void ConvertRectToJS(napi_env env, napi_value result, const OHOS::Accessibility::Rect& rect);
void ConvertGridItemToJS(napi_env env, napi_value result, const Accessibility::GridItemInfo& gridItem);
void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value& result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);
void ConvertAccessibleAbilityInfoToJS(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info);
void ConvertAccessibleAbilityInfoToJSPart1(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info);
void ConvertAccessibleAbilityInfoToJSPart2(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info);
void ConvertAccessibleAbilityInfoToJSPart3(
    napi_env env, napi_value& result, OHOS::Accessibility::AccessibilityAbilityInfo& info);
bool ConvertEventInfoJSToNAPI(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
bool ConvertEventInfoJSToNAPIPart1(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
bool ConvertEventInfoJSToNAPIPart2(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
bool ConvertEventInfoJSToNAPIPart3(
    napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
OHOS::AccessibilityConfig::DALTONIZATION_TYPE ConvertStringToDaltonizationTypes(std::string& type);
OHOS::AccessibilityConfig::CLICK_RESPONSE_TIME ConvertStringToClickResponseTimeTypes(std::string& type);
OHOS::AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME ConvertStringToIgnoreRepeatClickTimeTypes(std::string& type);
void ConvertActionArgsJSToNAPI(
    napi_env env, napi_value object, std::map<std::string, std::string>& args, OHOS::Accessibility::ActionType action);

KeyAction TransformKeyActionValue(int32_t keyAction);
bool HasKeyCode(const std::vector<int32_t>& pressedKeys, int32_t keyCode);
void GetKeyValue(napi_env env, napi_value keyObject, const OHOS::MMI::KeyEvent::KeyItem* keyItem);
void SetInputEventProperty(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent);
void SetKeyPropertyPart1(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent);
void SetKeyPropertyPart2(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent);
void ConvertKeyEventToJS(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent);
void ConvertCaptionPropertyToJS(napi_env env, napi_value& result,
    OHOS::AccessibilityConfig::CaptionProperty captionProperty);
bool ConvertObjToCaptionProperty(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty);
bool ConvertObjToCaptionPropertyPart1(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty);
bool ConvertObjToCaptionPropertyPart2(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty);
std::string ConvertCaptionPropertyJSToNAPI(
    napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty);
bool ConvertJSToStringVec(napi_env env, napi_value arrayValue, std::vector<std::string> &values);
void ConvertStringVecToJS(napi_env env, napi_value &result, std::vector<std::string> values);
void ConvertJSToEventTypes(napi_env env, napi_value arrayValue, uint32_t &eventTypes);
bool ConvertJSToCapabilities(napi_env env, napi_value arrayValue, uint32_t &capabilities);
uint32_t GetColorValue(napi_env env, napi_value object, napi_value propertyNameValue);
uint32_t GetColorValue(napi_env env, napi_value value);
uint32_t ConvertColorStringToNumer(std::string colorStr);
bool ColorRegexMatch(std::string colorStr, uint32_t &color);
std::string ConvertColorToString(uint32_t color);
std::string ConvertStringJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty);
int32_t ConvertIntJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty);
bool ConvertBoolJSToNAPI(napi_env env, napi_value object, napi_value propertyNameValue, bool &hasProperty);
void ConvertStringArrayJSToNAPI(napi_env env, napi_value object,
    napi_value propertyNameValue, bool &hasProperty, std::vector<std::string> &stringArray);
void ConvertStringArrayJSToNAPICommon(napi_env env, napi_value object, std::vector<std::string> &stringArray);
void ConvertSpanToJS(napi_env env, napi_value result, const Accessibility::SpanInfo& span);

OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(const std::string &type);
OHOS::Accessibility::AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(const std::string &type);
OHOS::Accessibility::AbilityStateType ConvertStringToAbilityStateType(const std::string &type);
OHOS::Accessibility::TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type);

std::string ConvertTextMoveUnitToString(OHOS::Accessibility::TextMoveUnit type);
std::string ConvertOperationTypeToString(OHOS::Accessibility::ActionType type);
std::string CoverGestureTypeToString(OHOS::Accessibility::GestureType type);
const std::string ConvertWindowUpdateTypeToString(OHOS::Accessibility::WindowUpdateType type);
const std::string ConvertAccessibilityEventTypeToString(OHOS::Accessibility::EventType type);
void ConvertEventTypeToString(const OHOS::Accessibility::AccessibilityEventInfo &eventInfo,
    std::string &eventTypeString);
bool ConvertGesturePathJSToNAPI(napi_env env, napi_value object,
    std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>& gesturePath);
bool ConvertGesturePathJSToNAPIPart1(napi_env env, napi_value object,
    std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>& gesturePath);
bool ConvertGesturePathJSToNAPIPart2(napi_env env, napi_value object,
    std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>& gesturePath);

struct AccessibilityCallbackInfo {
    napi_env env_;
    napi_ref ref_;
};

struct StateCallbackInfo: public AccessibilityCallbackInfo {
    bool state_;
    std::string stringValue_;
    uint32_t uint32Value_;
    int32_t int32Value_;
    float   floatValue_;
    std::vector<std::string> stringVector_;
};

struct CaptionCallbackInfo: public AccessibilityCallbackInfo {
    OHOS::AccessibilityConfig::CaptionProperty caption_;
};
} // namespace AccessibilityNapi
} // OHOS
#endif // ACCESSIBILITY_COMMON_UTILS_H