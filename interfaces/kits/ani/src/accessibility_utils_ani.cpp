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

#include <ani.h>
#include <array>
#include <iostream>
#include <vector>
#include "hilog_wrapper.h"
#include "accessibility_utils_ani.h"
#include <ani_signature_builder.h>

namespace OHOS {
namespace AccessibilityAni {
using namespace OHOS::Accessibility;
using namespace arkts::ani_signature;

NAccessibilityErrMsg QueryRetMsg(RetError errorCode)
{
    switch (errorCode) {
        case RetError::RET_OK:
            return { NAccessibilityErrorCode::ACCESSIBILITY_OK, "" };
        case RetError::RET_ERR_FAILED:
        case RetError::RET_ERR_NULLPTR:
        case RetError::RET_ERR_IPC_FAILED:
        case RetError::RET_ERR_SAMGR:
        case RetError::RET_ERR_TIME_OUT:
        case RetError::RET_ERR_REGISTER_EXIST:
        case RetError::RET_ERR_NO_REGISTER:
        case RetError::RET_ERR_NO_CONNECTION:
        case RetError::RET_ERR_NO_WINDOW_CONNECTION:
        case RetError::RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE:
        case RetError::RET_ERR_PERFORM_ACTION_FAILED_BY_ACE:
        case RetError::RET_ERR_NO_INJECTOR:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
        case RetError::RET_ERR_INVALID_PARAM:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_INVALID_PARAM, ERROR_MESSAGE_PARAMETER_ERROR };
        case RetError::RET_ERR_NO_PERMISSION:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_PERMISSION, ERROR_MESSAGE_NO_PERMISSION };
        case RetError::RET_ERR_CONNECTION_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_TARGET_ABILITY_ALREADY_ENABLED,
                     ERROR_MESSAGE_TARGET_ABILITY_ALREADY_ENABLED };
        case RetError::RET_ERR_NO_CAPABILITY:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NO_RIGHT, ERROR_MESSAGE_NO_RIGHT };
        case RetError::RET_ERR_NOT_INSTALLED:
        case RetError::RET_ERR_NOT_ENABLED:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ERROR_EXTENSION_NAME,
                     ERROR_MESSAGE_INVALID_BUNDLE_NAME_OR_ABILITY_NAME};
        case RetError::RET_ERR_PROPERTY_NOT_EXIST:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_PROPERTY_NOT_EXIST,
                     ERROR_MESSAGE_PROPERTY_NOT_EXIST };
        case RetError::RET_ERR_ACTION_NOT_SUPPORT:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_ACTION_NOT_SUPPORT,
                     ERROR_MESSAGE_ACTION_NOT_SUPPORT };
        case RetError::RET_ERR_NOT_SYSTEM_APP:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_NOT_SYSTEM_APP,
                     ERROR_MESSAGE_NOT_SYSTEM_APP };
        default:
            return { NAccessibilityErrorCode::ACCESSIBILITY_ERROR_SYSTEM_ABNORMALITY,
                     ERROR_MESSAGE_SYSTEM_ABNORMALITY };
    }
}

void ThrowBusinessError(ani_env *env, NAccessibilityErrMsg errMsg)
{
    Type errorClass = Builder::BuildClass("@ohos.base.BusinessError");
    ani_class cls {};
    if (env->FindClass(errorClass.Descriptor().c_str(), &cls) != ANI_OK) {
        HILOG_ERROR("find class BusinessError failed");
        return;
    }
    ani_method ctor;
    std::string ctorName = Builder::BuildConstructorName();
    SignatureBuilder sb{};
    if (env->Class_FindMethod(cls, ctorName.c_str(), sb.BuildSignatureDescriptor().c_str(), &ctor) != ANI_OK) {
        HILOG_ERROR("find method BusinessError.constructor failed");
        return;
    }
    ani_double errCode = static_cast<ani_double>(errMsg.errCode);
    ani_string errMsgStr;
    env->String_NewUTF8(errMsg.message.c_str(), errMsg.message.length(), &errMsgStr);
    ani_object errorObject;
    if (env->Object_New(cls, ctor, &errorObject) != ANI_OK) {
        HILOG_ERROR("create BusinessError object failed");
        return;
    }
    if (env->Object_SetPropertyByName_Double(errorObject, "code", errCode) != ANI_OK) {
        HILOG_ERROR("set property BusinessError.code failed!");
        return;
    }
    if (env->Object_SetPropertyByName_Ref(errorObject, "message", static_cast<ani_ref>(errMsgStr)) != ANI_OK) {
        HILOG_ERROR("set property BusinessError.message failed!");
        return;
    }
    if (env->ThrowError(static_cast<ani_error>(errorObject)) != ANI_OK) {
        HILOG_ERROR("throw BusinessError failed!");
        return;
    }
    HILOG_INFO("throw BusinessError success!");
    return;
}
} // namespace AccessibilityAni
} // namespace OHOS