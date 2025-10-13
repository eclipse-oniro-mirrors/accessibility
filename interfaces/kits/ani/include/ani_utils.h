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
#include "accessibility_base_utils.h"
#include "event_handler.h"
#include "event_runner.h"
#include "hilog_wrapper.h"

static thread_local std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler;

class ANIUtils {
public:
    static std::string ANIStringToStdString(ani_env *env, ani_string ani_str);
    static ani_string StdStringToAniString(ani_env *env, std::string str);
    static bool GetStringField(ani_env *env, std::string fieldName, ani_object object, std::string &fieldValue,
         bool isProperty = false);
    static bool GetIntField(ani_env *env, std::string fieldName, ani_object object, int32_t &fieldValue);
    static bool GetLongProperty(ani_env *env, std::string fieldName, ani_object object, int64_t &fieldValue);
    static bool GetArrayStringField(ani_env *env, std::string fieldName, ani_object object,
        std::vector<std::string> &fieldValue);
    static bool SetStringProperty(ani_env *env, ani_object &param, const std::string &name, const std::string &value);
    static bool SetStringArrayProperty(ani_env *env, ani_object &object, const std::string &fieldName,
        const std::vector<std::string> &values);
    static ani_array CreateEmptyAniArray(ani_env *env, uint32_t size);
    static bool GetEnumInt(ani_env *env, const char *enum_descriptor, ani_int enumIndex, int32_t& fieldValue);
    static bool CheckObserverEqual(ani_env *env, ani_ref fnRef, ani_env *iterEnv, ani_ref iterFn);
    static OHOS::Accessibility::EventType ConvertStringToEventInfoTypes(const std::string &type);
    static OHOS::Accessibility::ActionType ConvertStringToAccessibleOperationType(const std::string &type);
    static OHOS::Accessibility::WindowUpdateType ConvertStringToWindowUpdateTypes(const std::string &type);
    static OHOS::Accessibility::TextMoveUnit ConvertStringToTextMoveUnit(const std::string &type);
    static NAccessibilityErrMsg QueryRetMsg(OHOS::Accessibility::RetError errorCode);
    static void ThrowBusinessError(ani_env *env, NAccessibilityErrMsg errMsg);
    static ani_object CreateBoolObject(ani_env *env, ani_boolean value);
    static bool ConvertEventInfoMandatoryFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static void ConvertEventInfoStringFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static void ConvertEventInfoIntFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static void ConvertEventInfoRefFields(ani_env *env, ani_object eventObject,
        OHOS::Accessibility::AccessibilityEventInfo &eventInfo);
    static bool SendEventToMainThread(const std::function<void()> func);
    static bool ParseStringArray(ani_env *env, ani_object array, std::vector<std::string>& outValue);
    static ani_object CreateArray(ani_env *env, const std::vector<std::string> strs);
    static bool ParseString(ani_env *env, ani_ref ref, std::string& outValue);
    static bool GetStringMember(ani_env *env, ani_object options, const std::string &name, std::string &value);
    static std::string AniStrToString(ani_env *env, ani_ref aniStr);
    static ani_string CreateAniString(ani_env *env, const std::string &str);
    static bool GetNumberMember(ani_env *env, ani_object options, const std::string &name, uint32_t& value);
    static bool SetNumberMember(ani_env *env, ani_object obj, const std::string &name, const ani_int value);
    static ani_object CreateDouble(ani_env *env, float value);
    static ani_status CreateAniBoolean(ani_env* env, bool value, ani_object& result);
    static ani_status CreateAniInt(ani_env* env, int32_t value, ani_object& result);
    static ani_status CreateAniFloat(ani_env* env, float value, ani_object& result);
    static ani_status CreateAniLong(ani_env* env, int64_t value, ani_object& result);
    static bool SetIntField(ani_env *env, ani_object &object, const std::string &fieldName,
        int32_t fieldValue, bool isProperty = true);
    static bool SetLongPropertyRef(ani_env *env, ani_object &object, const std::string &propName, int64_t propValue);
    static ani_object CreateObject(ani_env *env, ani_class cls, ...);
    static bool ConvertStringToInt64(std::string &str, int64_t &value);
    static void CheckNumber(ani_env *env, std::string value);
    static void SetScrollTypeParam(ani_env *env, ani_object obj, std::map<std::string, std::string>& args);
    static void SetSelectionParam(ani_env *env, ani_object obj, std::map<std::string, std::string>& args);

    template <typename T>
    static ani_status Wrap(ani_env *env, ani_object object, T *nativePtr, const char *propName = "nativePtr")
    {
        return env->Object_SetFieldByName_Long(object, propName, reinterpret_cast<ani_long>(nativePtr));
    }

    template <typename T>
    static T *Unwrap(ani_env *env, ani_object object, const char *propName = "nativePtr")
    {
        ani_long nativePtr;
        if (ANI_OK != env->Object_GetFieldByName_Long(object, propName, &nativePtr)) {
            return nullptr;
        }
        return reinterpret_cast<T *>(nativePtr);
    }

    template <class T>
    static bool EnumConvert_EtsToNative(ani_env *env, ani_enum_item enumItem, T &result)
    {
        ani_status status = ANI_ERROR;
        if constexpr (std::is_enum<T>::value || std::is_integral<T>::value) {
            ani_int intValue{};
            status = env->EnumItem_GetValue_Int(enumItem, &intValue);
            if (ANI_OK != status) {
                HILOG_ERROR("EnumConvert_EtsToNative failed, status : %{public}d", status);
                return false;
            }
            result = static_cast<T>(intValue);
            return true;
        } else if constexpr (std::is_same<T, std::string>::value) {
            ani_string strValue{};
            status = env->EnumItem_GetValue_String(enumItem, &strValue);
            if (ANI_OK != status) {
                HILOG_ERROR("EnumItem_GetValue_String failed, status : %{public}d", status);
                return false;
            }
            result = ANIStringToStdString(env, strValue);
            return true;
        } else {
            HILOG_ERROR("Enum convert failed: type not supported");
            return false;
        }
    }

    template<class T>
    static bool EnumConvert_EtsToNative(ani_env *env, ani_object enumItem, T &result)
    {
        return EnumConvert_EtsToNative<T>(env, static_cast<ani_enum_item>(enumItem), result);
    }

    template <class T>
    static bool EnumConvert_NativeToEts(ani_env *env, const char *enumName, const T enumValue, ani_enum_item &result)
    {
        ani_enum aniEnum{};
        ani_status status = env->FindEnum(enumName, &aniEnum);
        if (ANI_OK != status) {
            HILOG_ERROR("Enum convert FindEnum failed: %{public}s status: %{public}d", enumName, status);
            return false;
        }
        constexpr int32_t loopMaxNum = 1000;
        for (int32_t index = 0U; index < loopMaxNum; index++) {
            ani_enum_item enumItem{};
            status = env->Enum_GetEnumItemByIndex(aniEnum, index, &enumItem);
            if (ANI_OK != status) {
                HILOG_ERROR(
                    "Enum_GetEnumItemByIndex failed: enumName:%{public}s index:%{public}d status:%{public}d",
                    enumName, index, status);
                return false;
            }
            T tmpValue{};
            if (EnumConvert_EtsToNative<T>(env, enumItem, tmpValue) && tmpValue == enumValue) {
                result = enumItem;
                return true;
            }
        }
        HILOG_ERROR("EnumConvert_NativeToEts failed enumName: %{public}s", enumName);
        return false;
    }

    template <class T>
    static bool SetEnumField(ani_env *env, ani_object &object, const char *enumName,
        const std::string &fieldName, const T enumValue)
    {
        if (env == nullptr) {
            HILOG_ERROR("SetEnumField null env");
            return false;
        }
        ani_enum_item modeItem = nullptr;
        if (!EnumConvert_NativeToEts(env, enumName, enumValue, modeItem)) {
            HILOG_ERROR("EnumConvert_NativeToEts failed");
            return false;
        }
        ani_status status = env->Object_SetFieldByName_Ref(object, fieldName.c_str(), modeItem);
        if (status != ANI_OK) {
            HILOG_ERROR("mode failed status:%{public}d", status);
            return false;
        }
        return true;
    }

    template <class T>
    static bool SetEnumProperty(ani_env *env, ani_object &object, const char *enumName,
        const std::string &fieldName, const T enumValue)
    {
        if (env == nullptr) {
            HILOG_ERROR("SetEnumField null env");
            return false;
        }
        ani_enum_item modeItem = nullptr;
        if (!EnumConvert_NativeToEts(env, enumName, enumValue, modeItem)) {
            HILOG_ERROR("EnumConvert_NativeToEts failed");
            return false;
        }
        ani_status status = env->Object_SetPropertyByName_Ref(object, fieldName.c_str(), modeItem);
        if (status != ANI_OK) {
            HILOG_ERROR("mode failed status:%{public}d", status);
            return false;
        }
        return true;
    }

public:
    constexpr static const char *CLASS_NAME_ARRAY = "escompat.Array";
    constexpr static int32_t INVALID_ANI_VERSION = 1;
    constexpr static int32_t MODULE_NOT_FOUND = 2;
    constexpr static int32_t NAMESPACE_NOT_FOUND = 3;
    constexpr static int32_t BIND_METHOD_FAILED = 4;
    constexpr static int32_t CLASS_NOT_FOUND = 5;
private:
    ANIUtils() = default;
    ~ANIUtils() = default;
};

#endif // ANI_UITLS_H