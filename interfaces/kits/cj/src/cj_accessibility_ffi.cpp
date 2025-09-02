/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "cj_accessibility_ffi.h"
#include <cstdint>
#include "cj_accessibility_utils.h"
#include "cj_accessibility_callback.h"
#include "cj_common_ffi.h"
#include "cj_lambda.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_def.h"
#include "accessibility_utils.h"
#include "hilog_wrapper.h"
#include "securec.h"
#include "native/ffi_remote_data.h"

namespace OHOS {
namespace Accessibility {
extern "C" {
bool FfiAccIsOpenAccessibility(int32_t *errorCode)
{
    bool status = false;
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("AccessibilitySystemAbilityClient instance is nullptr.");
        return status;
    }
    auto ret = asaClient->IsEnabled(status);
    if (ret != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("FfiAccIsOpenAccessibility called failed.");
    }
    return status;
}

bool FfiAccIsOpenTouchGuide(int32_t *errorCode)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient == nullptr) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("AccessibilitySystemAbilityClient instance is nullptr.");
        return false;
    }
    bool status = false;
    auto ret = asaClient->IsTouchExplorationEnabled(status);
    if (ret != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("FfiAccIsOpenTouchGuide called failed.");
    }
    return status;
}

bool FfiAccIsScreenReaderOpen(int32_t *errorCode)
{
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    bool status = false;
    auto ret = asaClient->IsScreenReaderEnabled(status);
    if (ret != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("FfiAccIsOpenTouchGuide called failed.");
    }
    return status;
}

CArrAccessibilityAbilityInfo FfiAccGetAccessibilityExtensionList(char *cAbilityType, char *cStateType,
    int32_t *errorCode)
{
    RetError errCode = RET_OK;
    AbilityStateType stateTypes = ABILITY_STATE_INVALID;
    uint32_t abilityTypes = 0;
    CArrAccessibilityAbilityInfo arrAccAbiliyInfo = {
        .size = 0
    };

    // parse inputs
    std::string abilityTypeStr(cAbilityType);
    std::string stateTypeStr(cStateType);
    HILOG_DEBUG("abilityTypeStr = %{private}s", abilityTypeStr.c_str());
    if (Utils::CheckAbilityType(abilityTypeStr)) {
        abilityTypes = ConvertStringToAccessibilityAbilityTypes(abilityTypeStr);
    } else {
        *errorCode = ERR_INPUT_INVALID;
        return arrAccAbiliyInfo;
    }
    HILOG_DEBUG("stateTypes = %{private}s", stateTypeStr.c_str());
    if (CheckStateType(stateTypeStr)) {
        stateTypes = ConvertStringToAbilityStateType(stateTypeStr);
    } else {
        errCode = RET_ERR_INVALID_PARAM;
    }
    std::vector<AccessibilityAbilityInfo> abilityList{};
    if (errCode == RET_OK) {
        auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
        if (asaClient) {
            errCode = asaClient->GetAbilityList(abilityTypes, stateTypes, abilityList);
        }
    }
    arrAccAbiliyInfo = Utils::ConvertArrAccAbilityInfo2CArr(abilityList, errCode);
    if (errCode != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("GetAbilityList failed or abilityList.size is 0.");
    }
    return arrAccAbiliyInfo;
}

CEventInfo FfiAccEventInfoInit(char *ctype, char *cBundleName, char *cTrigger, int32_t *errorCode)
{
    RetError errCode = RET_OK;
    CEventInfo cEventInfo;
    std::string eType(ctype);
    std::string bundleName(cBundleName);
    std::string action(cTrigger);
    AccessibilityEventInfo eventInfo(Utils::GetValueFromStr(Utils::rEventTypeMap, eType, errCode, TYPE_VIEW_INVALID));
    eventInfo.SetBundleName(bundleName);
    ActionType act = Utils::GetValueFromStr(Utils::rActionMap, action, errCode, ACCESSIBILITY_ACTION_INVALID);
    eventInfo.SetTriggerAction(act);
    cEventInfo = Utils::ConvertEventInfo2C(eventInfo, errCode);
    if (errCode != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("ConvertEventInfo2C failed.");
    }
    return cEventInfo;
}

void FfiAccSendAccessibilityEvent(CEventInfo event, int32_t *errorCode)
{
    RetError errCode = RET_OK;
    AccessibilityEventInfo eventInfo = Utils::ConvertEventInfo(event, errCode);
    auto asaClient = AccessibilitySystemAbilityClient::GetInstance();
    if (asaClient != nullptr) {
        errCode = asaClient->SendEvent(eventInfo);
    }
    if (asaClient == nullptr || errCode != RET_OK) {
        *errorCode = ERR_INPUT_INVALID;
        HILOG_ERROR("FfiAccSendAccessibilityEvent failed.");
    } else {
        HILOG_INFO("FfiAccSendAccessibilityEvent success.");
    }
}

void FfiAccOn(char *cbType, void (*callback)(), int32_t *errorCode)
{
    std::string cbTypeStr(cbType);
    auto cbFunc = CJLambda::Create(reinterpret_cast<void (*)(bool)>(callback));
    if (cbFunc == nullptr) {
        HILOG_ERROR("callback registered failed.");
        *errorCode = ERR_INPUT_INVALID;
        return;
    }
    if (cbTypeStr == "accessibilityStateChange") {
        cjAccessibilityStateListeners_->SubscribeObserver(cbFunc);
    } else if (cbTypeStr == "touchGuideStateChange") {
        cjTouchGuideStateListeners_->SubscribeObserver(cbFunc);
    } else {
        HILOG_ERROR("callback type should only be 'accessibilityStateChange' or 'touchGuideStateChange'");
        *errorCode = ERR_INPUT_INVALID;
        return;
    }
    HILOG_INFO("FfiAccOn success.");
}
}
} // namespace Accessibility
} // namespace OHOS
