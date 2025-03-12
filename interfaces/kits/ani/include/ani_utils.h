/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ANI_UITLS_H
#define ANI_UITLS_H

#include <ani.h>
#include <vector>
#include <functional>
#include <map>
#include "accessibility_def.h"
#include "accessibility_event_info.h"
#include "event_handler.h"
#include "event_runner.h"

static thread_local std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler;

class ANIUtils {
public:
    static std::string ANIStringToStdString(ani_env *env, ani_string ani_str);
    static bool GetStringField(ani_env *env, std::string fieldName, ani_object object, std::string &fieldValue);
    static bool GetIntField(ani_env *env, std::string fieldName, ani_object object, int32_t &fieldValue);
    static bool GetArrayStringField(ani_env *env, std::string fieldName, ani_object object,
        std::vector<std::string> &fieldValue);
    static bool CheckObserverEqual(ani_env *env, ani_ref fnRef, ani_env *iterEnv, ani_ref iterFn);
    static OHOS::Accessibility::EventType ConvertStringToEventInfoTypes(const std::string &type);
    static OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(const std::string &type);
    static OHOS::Accessibility::WindowUpdateType ConvertStringToWindowUpdateTypes(const std::string &type);
    static OHOS::Accessibility::TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type);
    static NAccessibilityErrMsg QueryRetMsg(OHOS::Accessibility::RetError errorCode);
    static void ThrowBusinessError(ani_env *env, NAccessibilityErrMsg errMsg);
    static ani_int ConvertEventInfoMandatoryFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static void ConvertEventInfoStringFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static void ConvertEventInfoIntFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static bool SendEventToMainThread(const std::function<void()> func);

private:
    ANIUtils() = default;
    ~ANIUtils() = default;
};

#endif // ANI_UITLS_H