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
#include "cj_accessibility_utils.h"
#include <cstdint>
#include "cj_common_ffi.h"
#include "cj_accessibility_ffi.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_def.h"
#include "accessibility_utils.h"
#include "securec.h"
#include "native/ffi_remote_data.h"

namespace OHOS {
namespace Accessibility {
namespace Utils {
char *MallocCString(const std::string &origin, RetError &errCode)
{
    if (origin.empty() || errCode == RET_ERR_FAILED) {
        return nullptr;
    }
    auto len = origin.length() + 1;
    char *res = static_cast<char *>(malloc(sizeof(char) * len));
    if (res == nullptr) {
        HILOG_ERROR("MallocCString malloc failed");
        errCode = RET_ERR_FAILED;
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), len);
}

CArrString VectorToCArrString(std::vector<std::string> &vec, RetError &errCode)
{
    if (vec.size() == 0 || errCode == RET_ERR_FAILED) {
        return { nullptr, 0 };
    }
    char **result = new char *[vec.size()];
    if (result == nullptr) {
        HILOG_ERROR("VectorToCArrString malloc failed");
        errCode = RET_ERR_NULLPTR;
        return { nullptr, 0 };
    }
    size_t temp = 0;
    for (size_t i = 0; i < vec.size(); i++) {
        result[i] = new char[vec[i].length() + 1];
        if (result[i] == nullptr) {
            break;
        }
        auto res = strcpy_s(result[i], vec[i].length() + 1, vec[i].c_str());
        if (res != EOK) {
            errCode = RET_ERR_FAILED;
            HILOG_ERROR("failed to strcpy_s.");
        }
        temp++;
    }

    if (temp != vec.size()) {
        for (size_t j = temp; j > 0; j--) {
            delete[] result[j - 1];
            result[j - 1] = nullptr;
        }
        delete[] result;
        errCode = RET_ERR_FAILED;
        return { nullptr, 0 };
    }
    return { result, vec.size() };
}

std::vector<std::string> CArrStringToVector(CArrString cArrStr)
{
    std::vector<std::string> vec;
    if (cArrStr.head == nullptr || cArrStr.size <= 0) {
        return vec;
    }
    for (size_t i = 0; i < static_cast<size_t>(cArrStr.size); i++) {
        if (cArrStr.head[i] != nullptr) {
            vec.emplace_back(cArrStr.head[i]);
        }
    }
    return vec;
}

CArrString GetAbilityTypesStr(uint32_t abilityTypes, RetError &errCode)
{
    std::vector<std::string> abilityTypesStr;
    for (auto [key, val] : abilityTypeMap) {
        if (abilityTypes & key) {
            abilityTypesStr.push_back(val);
        }
    }
    return VectorToCArrString(abilityTypesStr, errCode);
}

CArrString GetCapabilityStr(uint32_t capabilities, RetError &errCode)
{
    std::vector<std::string> capabilityStr;
    for (auto [key, val] : capabilityMap) {
        if (capabilities & key) {
            capabilityStr.push_back(val);
        }
    }
    return VectorToCArrString(capabilityStr, errCode);
}

CArrString GetEventTypeStr(uint32_t eventType, RetError &errCode)
{
    std::vector<std::string> eventTypeStr;
    if ((eventType < EventType::TYPE_VIEW_CLICKED_EVENT) ||
        ((eventType >= EventType::TYPE_MAX_NUM) && (eventType != EventType::TYPES_ALL_MASK))) {
        errCode = RET_ERR_FAILED;
        return VectorToCArrString(eventTypeStr, errCode);
    }
    for (auto [key, val] : eventTypeMap) {
        if (eventType & key) {
            eventTypeStr.push_back(val);
        }
    }
    return VectorToCArrString(eventTypeStr, errCode);
}

CAccessibilityAbilityInfo ConvertAccAbilityInfo2C(AccessibilityAbilityInfo &abilityInfo, RetError &errCode)
{
    CAccessibilityAbilityInfo cAbility;
    cAbility.id_ = MallocCString(abilityInfo.GetId(), errCode);
    cAbility.name_ = MallocCString(abilityInfo.GetName(), errCode);
    cAbility.bundleName_ = MallocCString(abilityInfo.GetPackageName(), errCode);
    std::vector<std::string> bundleNames(abilityInfo.GetFilterBundleNames());
    cAbility.targetBundleNames_ = VectorToCArrString(bundleNames, errCode);
    cAbility.abilityTypes_ = GetAbilityTypesStr(abilityInfo.GetAccessibilityAbilityType(), errCode);
    cAbility.capabilities_ = GetCapabilityStr(abilityInfo.GetCapabilityValues(), errCode);
    cAbility.description_ = MallocCString(abilityInfo.GetDescription(), errCode);
    cAbility.eventTypes_ = GetEventTypeStr(abilityInfo.GetEventTypes(), errCode);
    cAbility.needHide_ = abilityInfo.NeedHide();
    cAbility.label_ = MallocCString(abilityInfo.GetLabel(), errCode);
    return cAbility;
}

void FreecAbility(CAccessibilityAbilityInfo *cAbility)
{
    free(cAbility->id_);
    cAbility->id_ = nullptr;
    free(cAbility->name_);
    cAbility->name_ = nullptr;
    free(cAbility->bundleName_);
    cAbility->bundleName_ = nullptr;
    free(cAbility->description_);
    cAbility->description_ = nullptr;
    free(cAbility->label_);
    cAbility->label_ = nullptr;
    for (auto i = 0; i < cAbility->targetBundleNames_.size; i++) {
        free(cAbility->targetBundleNames_.head[i]);
    }
    free(cAbility->targetBundleNames_.head);
    cAbility->targetBundleNames_.head = nullptr;

    for (auto i = 0; i < cAbility->abilityTypes_.size; i++) {
        free(cAbility->abilityTypes_.head[i]);
    }
    free(cAbility->abilityTypes_.head);
    cAbility->abilityTypes_.head = nullptr;

    for (auto i = 0; i < cAbility->capabilities_.size; i++) {
        free(cAbility->capabilities_.head[i]);
    }
    free(cAbility->capabilities_.head);
    cAbility->capabilities_.head = nullptr;

    for (auto i = 0; i < cAbility->eventTypes_.size; i++) {
        free(cAbility->eventTypes_.head[i]);
    }
    free(cAbility->eventTypes_.head);
    cAbility->eventTypes_.head = nullptr;
}

CArrAccessibilityAbilityInfo ConvertArrAccAbilityInfo2CArr(std::vector<AccessibilityAbilityInfo> &abilityList,
    RetError &errCode)
{
    CArrAccessibilityAbilityInfo cArrAbility;
    cArrAbility.size = static_cast<int64_t>(abilityList.size());
    if (cArrAbility.size == 0) {
        return cArrAbility;
    }
    int64_t mallocSize = static_cast<int64_t>(sizeof(CAccessibilityAbilityInfo)) * cArrAbility.size;
    CAccessibilityAbilityInfo *cAbility = static_cast<CAccessibilityAbilityInfo *>(malloc(mallocSize));
    if (cAbility == nullptr) {
        errCode = RET_ERR_NULLPTR;
        return cArrAbility;
    }
    memset_s(cAbility, mallocSize, 0, mallocSize);
    for (auto i = 0; i < cArrAbility.size; ++i) {
        cAbility[i] = ConvertAccAbilityInfo2C(abilityList[i], errCode);
        if (errCode != RET_OK) {
            for (auto j = 0; j < i; j++) {
                FreecAbility(&cAbility[j]);         
            }
            free(cAbility);
            cAbility = nullptr;
            HILOG_ERROR("ConvertAccAbilityInfo2C failed.");
            return cArrAbility;
        }
    }
    cArrAbility.head = cAbility;
    return cArrAbility;
}

CEventInfo ConvertEventInfo2C(const AccessibilityEventInfo &eventInfo, RetError &errCode)
{
    CEventInfo cEventInfo;
    auto eventTypeStr = GetStrFromVal(Utils::eventTypeMap, eventInfo.GetEventType(), errCode, "accessibilityFocus");
    cEventInfo.type_ = MallocCString(eventTypeStr, errCode);
    auto windowTypeStr = GetStrFromVal(Utils::windowUpdateTypeMap, eventInfo.GetWindowChangeTypes(), errCode, "add");
    cEventInfo.windowUpdateType_ = MallocCString(windowTypeStr, errCode);
    cEventInfo.bundleName_ = MallocCString(eventInfo.GetBundleName(), errCode);
    cEventInfo.componentType_ = MallocCString(eventInfo.GetComponentType(), errCode);
    cEventInfo.pageId_ = eventInfo.GetPageId();
    cEventInfo.description_ = MallocCString(eventInfo.GetDescription(), errCode);
    auto actionStr = GetStrFromVal(Utils::actionMap, eventInfo.GetTriggerAction(), errCode, "click");
    cEventInfo.triggerAction_ = MallocCString(actionStr, errCode);
    auto textMoveStr = GetStrFromVal(Utils::textMoveUnitMap, eventInfo.GetTextMovementStep(), errCode, "char");
    cEventInfo.textMoveUnit_ = MallocCString(textMoveStr, errCode);
    auto contVec = eventInfo.GetContentList();
    cEventInfo.contents_ = VectorToCArrString(contVec, errCode);
    cEventInfo.lastContent_ = MallocCString(eventInfo.GetLatestContent(), errCode);
    cEventInfo.beginIndex_ = eventInfo.GetBeginIndex();
    cEventInfo.currentIndex_ = eventInfo.GetCurrentIndex();
    cEventInfo.endIndex_ = eventInfo.GetEndIndex();
    cEventInfo.itemCount_ = eventInfo.GetItemCounts();
    cEventInfo.elementId_ = eventInfo.GetAccessibilityId();
    cEventInfo.textAnnouncedForAccessibility_ = MallocCString(eventInfo.GetTextAnnouncedForAccessibility(), errCode);
    cEventInfo.customId_ = MallocCString(eventInfo.GetInspectorKey(), errCode);
    return cEventInfo;
}

AccessibilityEventInfo ConvertEventInfo(const CEventInfo &cEventInfo, RetError &errCode)
{
    AccessibilityEventInfo eventInfo;
    std::string typeStr(cEventInfo.type_);
    eventInfo.SetEventType(Utils::GetValueFromStr(rEventTypeMap, typeStr, errCode, TYPE_VIEW_INVALID));
    std::string windowTypeStr(cEventInfo.windowUpdateType_);
    eventInfo.SetWindowChangeTypes(
        Utils::GetValueFromStr(rWindowUpdateTypeMap, windowTypeStr, errCode, WINDOW_UPDATE_ADDED));
    std::string bundleStr(cEventInfo.bundleName_);
    eventInfo.SetBundleName(bundleStr);
    std::string compTypeStr(cEventInfo.componentType_);
    eventInfo.SetComponentType(compTypeStr);
    eventInfo.SetPageId(cEventInfo.pageId_);
    std::string descStr(cEventInfo.description_);
    eventInfo.SetDescription(descStr);
    std::string actStr(cEventInfo.triggerAction_);
    eventInfo.SetTriggerAction(Utils::GetValueFromStr(rActionMap, actStr, errCode, ACCESSIBILITY_ACTION_CLICK));
    std::string textStr(cEventInfo.textMoveUnit_);
    eventInfo.SetTextMovementStep(Utils::GetValueFromStr(rTextMoveUnitMap, textStr, errCode, STEP_CHARACTER));
    std::vector<std::string> contStr = CArrStringToVector(cEventInfo.contents_);
    for (const auto &cont : contStr) {
        eventInfo.AddContent(cont);
    }
    std::string lastStr(cEventInfo.lastContent_);
    eventInfo.SetLatestContent(lastStr);
    eventInfo.SetBeginIndex(cEventInfo.beginIndex_);
    eventInfo.SetCurrentIndex(cEventInfo.currentIndex_);
    eventInfo.SetEndIndex(cEventInfo.endIndex_);
    eventInfo.SetItemCounts(cEventInfo.itemCount_);
    eventInfo.SetSource(cEventInfo.elementId_);
    std::string textForStr(cEventInfo.textAnnouncedForAccessibility_);
    eventInfo.SetTextAnnouncedForAccessibility(textForStr);
    std::string custStr(cEventInfo.customId_);
    eventInfo.SetInspectorKey(custStr);
    return eventInfo;
}
} // namespace Utils
} // namespace Accessibility
} // namespace OHOS