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
#include <map>
#include <string>
#include "accessibility_ability_info.h"
#include "accessibility_caption.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_info.h"
#include "accessibility_gesture_inject_path.h"
#include "element_name.h"
#include "key_event.h"
#include "napi_accessibility_element.h"
#include "accessibility_config.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

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
const uint32_t CONFIG_START_WORK_ARGS_SIZE = 2;

std::string GetStringFromNAPI(napi_env env, napi_value value);
bool ParseBool(napi_env env, bool& param, napi_value args);
bool ParseString(napi_env env, std::string& param, napi_value args);
bool ParseUint32(napi_env env, uint32_t& param, napi_value args);
bool ParseInt32(napi_env env, int32_t& param, napi_value args);
napi_value GetErrorValue(napi_env env, int errCode);

std::string ConvertWindowTypeToString(OHOS::Accessibility::AccessibilityWindowType type);
std::string ConvertDaltonizationTypeToString(OHOS::AccessibilityConfig::DALTONIZATION_TYPE type);
void ConvertOperationToJS(napi_env env, napi_value result, const OHOS::Accessibility::AccessibleAction& operation);
void ConvertGridInfoToJS(napi_env env, napi_value nGrid, const OHOS::Accessibility::GridInfo& grid);
void ConvertRangeInfoToJS(napi_env env, napi_value nRangeInfo, const OHOS::Accessibility::RangeInfo& rangeInfo);
void ConvertGridItemToJS(napi_env env, napi_value nGridItem, const OHOS::Accessibility::GridItemInfo& gridItem);
void ConvertRectToJS(napi_env env, napi_value result, const OHOS::Accessibility::Rect& rect);
void ConvertAccessibilityWindowInfoToJS(
    napi_env env, napi_value result, const OHOS::Accessibility::AccessibilityWindowInfo& accessibilityWindowInfo);
void ConvertAccessibilityWindowInfosToJS(napi_env env, napi_value result,
    const std::vector<OHOS::Accessibility::AccessibilityWindowInfo>& accessibilityWindowInfos);
void ConvertAccessibilityEventInfoToJS(napi_env env, napi_value objEventInfo,
    const OHOS::Accessibility::AccessibilityEventInfo& eventInfo,
    const std::shared_ptr<AccessibilityElement>& element);
void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value& result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos);

void ConvertElementInfoToJS(
    napi_env env, napi_value result, const OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
bool ConvertEventInfoJSToNAPI(napi_env env, napi_value object, OHOS::Accessibility::AccessibilityEventInfo& eventInfo);
void ConvertElementInfosToJS(
    napi_env env, napi_value result, const std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos);
OHOS::AccessibilityConfig::DALTONIZATION_TYPE ConvertStringToDaltonizationTypes(std::string& type);
void ConvertActionArgsJSToNAPI(
    napi_env env, napi_value object, std::map<std::string, std::string>& args, OHOS::Accessibility::ActionType action);

void ConvertGesturePathsJSToNAPI(napi_env env, napi_value object,
    std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>& gesturePath,
    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityGestureInjectPath>>& gesturePathArray,
    bool &isParameterArray);
void ConvertKeyEventToJS(napi_env env, napi_value result, const std::shared_ptr<OHOS::MMI::KeyEvent> &keyEvent);
void ConvertCaptionPropertyToJS(napi_env env, napi_value& result,
    OHOS::AccessibilityConfig::CaptionProperty captionProperty);
void ConvertObjToCaptionProperty(
    napi_env env, napi_value object, OHOS::AccessibilityConfig::CaptionProperty* ptrCaptionProperty);
void ConvertJSToStringVec(napi_env env, napi_value arrayValue, std::vector<std::string> &values);
void ConvertStringVecToJS(napi_env env, napi_value &result, std::vector<std::string> values);
void ConvertJSToEventTypes(napi_env env, napi_value arrayValue, uint32_t &eventTypes);
void ConvertJSToCapabilities(napi_env env, napi_value arrayValue, uint32_t &capabilities);
void ConvertEnabledToJS(napi_env env, napi_value& captionsManager, bool value);
void ConvertStyleToJS(napi_env env, napi_value& captionsManager,
    OHOS::AccessibilityConfig::CaptionProperty captionProperty_);
uint32_t GetColorValue(napi_env env, napi_value object, napi_value propertyNameValue);
uint32_t GetColorValue(napi_env env, napi_value value);
uint32_t ConvertColorStringToNumer(std::string colorStr);
std::string ConvertColorToString(uint32_t color);

OHOS::Accessibility::NotificationCategory ConvertStringToNotificationCategory(const std::string &type);
OHOS::Accessibility::GestureType ConvertStringToGestureType(const std::string &type);
OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(const std::string &type);
OHOS::Accessibility::AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(const std::string &type);
OHOS::Accessibility::AbilityStateType ConvertStringToAbilityStateType(const std::string &type);
OHOS::Accessibility::GlobalAction ConvertStringToGlobalAction(const std::string &type);
OHOS::Accessibility::TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type);

std::string ConvertTextMoveUnitToString(OHOS::Accessibility::TextMoveUnit type);
std::string ConvertOperationTypeToString(OHOS::Accessibility::ActionType type);

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
};

struct CaptionCallbackInfo: public AccessibilityCallbackInfo {
    OHOS::AccessibilityConfig::CaptionProperty caption_;
};

class ConfigListener : public OHOS::AccessibilityConfig::AccessibilityConfigObserver {
public:
    ConfigListener() = default;
    void NotifyStateChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, bool enabled);
    void NotifyStringChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, const std::string &value);
    void NotifyIntChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, int32_t value);
    void NotifyUintChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, uint32_t value);
    void NotifyFloatChanged2JS(const OHOS::AccessibilityConfig::CONFIG_ID id, float value);
    void NotifyPropertyChanged2JS(
        const OHOS::AccessibilityConfig::CONFIG_ID id, OHOS::AccessibilityConfig::CaptionProperty caption);

    napi_value StartWork(napi_env env, size_t functionIndex, napi_value (&args)[CONFIG_START_WORK_ARGS_SIZE]);
    virtual void OnConfigChanged(
        const OHOS::AccessibilityConfig::CONFIG_ID id, const OHOS::AccessibilityConfig::ConfigValue& value) override;

    void SetEnv(const napi_env env)
    {
        env_ = env;
    }

    napi_env GetEnv() const
    {
        return env_;
    }

    void SetHandler(const napi_ref handler)
    {
        handlerRef_ = handler;
    }

    napi_ref GetHandler() const
    {
        return handlerRef_;
    }

    void SetConfigId(const OHOS::AccessibilityConfig::CONFIG_ID id)
    {
        configId_ = id;
    }

    OHOS::AccessibilityConfig::CONFIG_ID GetConfigId() const
    {
        return configId_;
    }
private:
    static void NotifyStateChangedJS(napi_env env, bool enabled, const OHOS::AccessibilityConfig::CONFIG_ID id,
                                     napi_ref handlerRef);
    static void NotifyPropertyChangedJS(napi_env env, OHOS::AccessibilityConfig::CaptionProperty caption,
                                        const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyStringChanged2JSInner(napi_env env, const std::string& value,
                                            const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyIntChanged2JSInner(
        napi_env env, int32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyUintChanged2JSInner(
        napi_env env, uint32_t value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);
    static void NotifyFloatChanged2JSInner(
        napi_env env, float value, const OHOS::AccessibilityConfig::CONFIG_ID id, napi_ref handlerRef);

private:
    napi_ref handlerRef_ = nullptr;
    napi_env env_ = nullptr;
    OHOS::AccessibilityConfig::CONFIG_ID configId_ = OHOS::AccessibilityConfig::CONFIG_ID::CONFIG_ID_MAX;
};
#endif  // NAPI_ACCESSIBILITY_UTILS_H