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

#include <vector>
#include "hilog_wrapper.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_info.h"
#include "napi_accessibility_utils.h"
#include "napi_accessibility_window_info.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessible_ability.h"

using namespace OHOS;
using namespace OHOS::Accessibility;

std::string GetStringFromNAPI(napi_env env, napi_value value) {
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
        HILOG_ERROR("can not get string size");
        return "";
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
        HILOG_ERROR("can not get string value");
        return "";
    }
    return result;
}

bool ParseString(napi_env env, std::string &param, napi_value args) {
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_INFO("napi_typeof error and status is %{public}d", status);
        return false;
    }
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    param = GetStringFromNAPI(env, args);
    HILOG_INFO("param=%{public}s.", param.c_str());
    return true;
}

bool ParseUint32(napi_env env, uint32_t &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_INFO("napi_typeof error and status is %{public}d", status);
        return false;
    }
    HILOG_INFO("param=%{public}d.", valuetype);
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. uint32 expected.");
    napi_get_value_uint32(env, args, &param);
    return true;
}

napi_value GetErrorValue(napi_env env, int errCode) {
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

/**********************************************************
 * Convert native object to js object
 *********************************************************/
void ConvertRectToJS(napi_env env, napi_value result, Accessibility::Rect& rect) {
    napi_value nLeftTopX;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopXScreenPostion(), &nLeftTopX));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "leftTopX", nLeftTopX));

    napi_value nLeftTopY;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetLeftTopYScreenPostion(), &nLeftTopY));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "leftTopY", nLeftTopY));

    napi_value nRightBottomX;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetRightBottomXScreenPostion(), &nRightBottomX));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "rightBottomX", nRightBottomX));

    napi_value nRightBottomY;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rect.GetRightBottomYScreenPostion(), &nRightBottomY));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "rightBottomY", nRightBottomY));
}

static std::string ConvertWindowTypeToString(WindowType type) {
    static const std::map<WindowType, const std::string> windowTypeTable = {
        {WindowType::TYPE_ACCESSIBILITY_OVERLAY, "accessibilityOverlay"},
        {WindowType::TYPE_APPLICATION, "application"},
        {WindowType::TYPE_INPUT_METHOD, "inputMethod"},
        {WindowType::TYPE_SPLIT_SCREEN_DIVIDER, "screenDivider"},
        {WindowType::TYPE_SYSTEM, "system"}
    };

    if (windowTypeTable.find(type) == windowTypeTable.end()) {
        return "";
    }

    return windowTypeTable.at(type);
}

void ConvertAccessibleAbilityInfoToJS(napi_env env, napi_value result, AccessibilityAbilityInfo& info)
{
    napi_value nId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetId().c_str(), NAPI_AUTO_LENGTH, &nId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "id", nId));

    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetName().c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", nName));

    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetPackageName().c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", nBundleName));

    napi_value ability;
    napi_create_object(env, &ability);
    OHOS::AppExecFwk::AbilityInfo abilityInfo = info.GetAbilityInfo();
    ConvertAbilityInfoToJS(env, ability, abilityInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "abilityInfo", ability));

    napi_value nAbilityType;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityType));
    uint32_t abilityTypesValue = info.GetAccessibilityAbilityType();
    std::vector<std::string> abilityTypes = ParseAbilityTypesToVec(abilityTypesValue);
    for (size_t idx = 0; idx < abilityTypes.size(); idx++) {
        napi_value nType;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityTypes[idx].c_str(), NAPI_AUTO_LENGTH, &nType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityType, idx, nType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "abilityTypes", nAbilityType));

    napi_value nCapabilities;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nCapabilities));
    uint32_t capabilitiesValue = info.GetCapabilityValues();
    std::vector<std::string> capabilities = ParseCapabilitiesToVec(capabilitiesValue);
    for (size_t idx = 0; idx < capabilities.size(); idx++) {
        napi_value nType;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, capabilities[idx].c_str(), NAPI_AUTO_LENGTH, &nType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nCapabilities, idx, nType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "capabilities", nCapabilities));

    napi_value description;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.GetDescription().c_str(), NAPI_AUTO_LENGTH, &description));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", description));

    napi_value nEventTypes;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nEventTypes));
    uint32_t eventTypesValue = info.GetEventTypes();
    std::vector<std::string> eventTypes = ParseEventTypesToVec(eventTypesValue);
    for (size_t idx = 0; idx < eventTypes.size(); idx++) {
        napi_value nType;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, eventTypes[idx].c_str(), NAPI_AUTO_LENGTH, &nType));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nEventTypes, idx, nType));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "eventTypes", nEventTypes));

    // napi_value fingerprintGestureInterception;
    // NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, info.GetFingerprintGestureInterception(),
    //     &fingerprintGestureInterception));
    // NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "fingerprintGestureInterception",
    //     fingerprintGestureInterception));

    // napi_value keyEventInterception;
    // NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, info.GetKeyEventType(), &keyEventInterception));
    // NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "keyEventInterception", keyEventInterception));

    // napi_value uiInteractiveTime;
    // NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, info.GetUiInteractiveTime(), &uiInteractiveTime));
    // NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "uiInteractiveTime", uiInteractiveTime));

    // napi_value uiNoninteractiveTime;
    // NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, info.GetUiNoninteractiveTime(), &uiNoninteractiveTime));
    // NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "uiNoninteractiveTime", uiNoninteractiveTime));

    napi_value filterBundleNames;
    size_t idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &filterBundleNames));
    std::vector<std::string> strFilterBundleNames = info.GetFilterBundleNames();
    for (auto filterBundleName : strFilterBundleNames) {
        napi_value bundleName;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, filterBundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, filterBundleNames, idx, bundleName));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "filterBundleNames", filterBundleNames));
}

void ConvertAbilityApplicationInfoToJS(napi_env env, napi_value result, OHOS::AppExecFwk::ApplicationInfo& info)
{
    napi_value name;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.name.c_str(), NAPI_AUTO_LENGTH, &name));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", name));

    napi_value bundleName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", bundleName));

    napi_value description;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.description.c_str(), NAPI_AUTO_LENGTH, &description));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", description));

    napi_value iconPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.iconPath.c_str(), NAPI_AUTO_LENGTH, &iconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "iconPath", iconPath));

    napi_value label;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.label.c_str(), NAPI_AUTO_LENGTH, &label));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "label", label));

    napi_value labelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, int32_t(info.labelId), &labelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "labelId", labelId));

    napi_value iconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, int32_t(info.iconId), &iconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "iconId", iconId));

    napi_value descriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, int32_t(info.descriptionId), &descriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "descriptionId", descriptionId));

    napi_value deviceId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.deviceId.c_str(), NAPI_AUTO_LENGTH, &deviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deviceId", deviceId));

    napi_value signatureKey;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.signatureKey.c_str(), NAPI_AUTO_LENGTH, &signatureKey));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "signatureKey", signatureKey));

    napi_value isSystemApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.isSystemApp, &isSystemApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "systemApp", isSystemApp));

    napi_value isLauncherApp;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.isLauncherApp, &isLauncherApp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isLauncherApp", isLauncherApp));

    napi_value supportedModes;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, int32_t(info.supportedModes), &supportedModes));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "supportedModes", supportedModes));

    napi_value process;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.process.c_str(), NAPI_AUTO_LENGTH, &process));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "process", process));

    napi_value permissions;
    size_t idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &permissions));
    for (auto permission : info.permissions) {
        napi_value per;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, permission.c_str(), NAPI_AUTO_LENGTH, &per));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, permissions, idx, per));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "permissions", permissions));

    napi_value moduleSourceDirs;
    idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &moduleSourceDirs));
    for (auto moduleSourceDir : info.moduleSourceDirs) {
        napi_value moduleSource;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleSourceDir.c_str(), NAPI_AUTO_LENGTH, &moduleSource));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, moduleSourceDirs, idx, moduleSource));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "moduleSourceDirs", moduleSourceDirs));

    napi_value moduleInfos;
    idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &moduleInfos));
    for (auto moduleInfo : info.moduleInfos) {
        napi_value module = 0;
        napi_value moduleName;
        napi_value moduleSourceDir;
        napi_create_object(env, &module);
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &moduleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, module, "moduleName", moduleName));
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleInfo.moduleSourceDir.c_str(), NAPI_AUTO_LENGTH, &moduleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, module, "moduleSourceDir", moduleSourceDir));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, moduleInfos, idx, module));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "moduleInfos", moduleInfos));

    napi_value entryDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.entryDir.c_str(), NAPI_AUTO_LENGTH, &entryDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "entryDir", entryDir));

    napi_value codePath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.codePath.c_str(), NAPI_AUTO_LENGTH, &codePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "codePath", codePath));

    napi_value dataDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.dataDir.c_str(), NAPI_AUTO_LENGTH, &dataDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "dataDir", dataDir));

    napi_value dataBaseDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.dataBaseDir.c_str(), NAPI_AUTO_LENGTH, &dataBaseDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "dataBaseDir", dataBaseDir));

    napi_value cacheDir;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.cacheDir.c_str(), NAPI_AUTO_LENGTH, &cacheDir));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "cacheDir", cacheDir));

    napi_value flags;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, int32_t(info.flags), &flags));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "flags", flags));

    napi_value enabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.enabled, &enabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "enabled", enabled));
}

void ConvertAbilityInfoToJS(napi_env env, napi_value result, OHOS::AppExecFwk::AbilityInfo& info)
{
    napi_value name;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.name.c_str(), NAPI_AUTO_LENGTH, &name));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "name", name));

    napi_value label;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.label.c_str(), NAPI_AUTO_LENGTH, &label));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "label", label));

    napi_value description;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.description.c_str(), NAPI_AUTO_LENGTH, &description));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", description));

    napi_value iconPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.iconPath.c_str(), NAPI_AUTO_LENGTH, &iconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "iconPath", iconPath));

    napi_value visible;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.visible, &visible));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "visible", visible));

    napi_value kind;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.kind.c_str(), NAPI_AUTO_LENGTH, &kind));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "kind", kind));

    napi_value type;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, uint32_t(info.type), &type));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "type", type));

    napi_value orientation;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, uint32_t(info.orientation), &orientation));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "orientation", orientation));

    napi_value launchMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, uint32_t(info.launchMode), &launchMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "launchMode", launchMode));

    napi_value permissions;
    size_t idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &permissions));
    for (auto permission : info.permissions) {
        napi_value per;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, permission.c_str(), NAPI_AUTO_LENGTH, &per));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, permissions, idx, per));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "permissions", permissions));

    napi_value process;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.process.c_str(), NAPI_AUTO_LENGTH, &process));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "process", process));

    napi_value deviceTypes;
    idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &deviceTypes));
    for (auto deviceType : info.deviceTypes) {
        napi_value device;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, deviceType.c_str(), NAPI_AUTO_LENGTH, &device));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, deviceTypes, idx, device));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deviceTypes", deviceTypes));

    napi_value deviceCapabilities;
    idx = 0;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &deviceCapabilities));
    for (auto deviceCap : info.deviceCapabilities) {
        napi_value cap;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, deviceCap.c_str(), NAPI_AUTO_LENGTH, &cap));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, deviceCapabilities, idx, cap));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deviceCapabilities", deviceCapabilities));

    napi_value uri;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.uri.c_str(), NAPI_AUTO_LENGTH, &uri));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "uri", uri));

    napi_value targetAbility;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.targetAbility.c_str(), NAPI_AUTO_LENGTH, &targetAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "targetAbility", targetAbility));

    napi_value applicationInfo = 0;
    napi_create_object(env, &applicationInfo);
    ConvertAbilityApplicationInfoToJS(env, applicationInfo, info.applicationInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "applicationInfo", applicationInfo));

    napi_value isLauncherAbility;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.isLauncherAbility, &isLauncherAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isLauncherAbility", isLauncherAbility));

    napi_value isNativeAbility;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.isNativeAbility, &isNativeAbility));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isNativeAbility", isNativeAbility));

    napi_value enabled;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, info.enabled, &enabled));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "enabled", enabled));

    napi_value package;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.package.c_str(), NAPI_AUTO_LENGTH, &package));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "package", package));

    napi_value bundleName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", bundleName));

    napi_value moduleName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.moduleName.c_str(), NAPI_AUTO_LENGTH, &moduleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "moduleName", moduleName));

    napi_value applicationName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.applicationName.c_str(), NAPI_AUTO_LENGTH, &applicationName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "applicationName", applicationName));

    napi_value deviceId;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.deviceId.c_str(), NAPI_AUTO_LENGTH, &deviceId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deviceId", deviceId));

    napi_value codePath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.codePath.c_str(), NAPI_AUTO_LENGTH, &codePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "codePath", codePath));

    napi_value resourcePath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.resourcePath.c_str(), NAPI_AUTO_LENGTH, &resourcePath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "resourcePath", resourcePath));

    napi_value libPath;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, info.libPath.c_str(), NAPI_AUTO_LENGTH, &libPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "libPath", libPath));

}

void ConvertAccessibilityWindowInfoToJS(napi_env env, napi_value result, AccessibilityWindowInfo& accessibilityWindowInfo) {
    napi_value nRect;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nRect));
    Accessibility::Rect rect = accessibilityWindowInfo.GetRectInScreen();
    ConvertRectToJS(env, nRect, rect);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "screenRect", nRect));

    napi_value nId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, accessibilityWindowInfo.GetWindowId(), &nId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "id", nId));

    napi_value nLayer;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, accessibilityWindowInfo.GetWindowLayer(), &nLayer));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "layer", nLayer));

    napi_value nTitle;
    std::string strTitle = accessibilityWindowInfo.GetWindowTitle();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strTitle.c_str(), NAPI_AUTO_LENGTH, &nTitle));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "title", nTitle));

    napi_value nWindowType;
    WindowType windowType = accessibilityWindowInfo.GetWindowType();
    std::string strWindowType = ConvertWindowTypeToString(windowType);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strWindowType.c_str(), NAPI_AUTO_LENGTH, &nWindowType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "type", nWindowType));

    napi_value nChildIds;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nChildIds));
    std::vector<int> childIds = accessibilityWindowInfo.GetChildIds();
    for (auto childId : childIds) {
        size_t idx = 0;
        napi_value nChildId;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, childId, &nChildId));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nChildIds, idx, nChildId));
        idx++;
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "childIds", nChildIds));

    napi_value nParentId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, accessibilityWindowInfo.GetParentId(), &nParentId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "parentId", nParentId));

    napi_value nAccessibilityFocused;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env,
        accessibilityWindowInfo.IsAccessibilityFocused(), &nAccessibilityFocused));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isAccessibilityFocused", nAccessibilityFocused));

    napi_value nIsActive;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env,
        accessibilityWindowInfo.IsActive(), &nIsActive));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isActive", nIsActive));

    napi_value nIsFocused;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env,
        accessibilityWindowInfo.IsFocused(), &nIsFocused));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isFocused", nIsFocused));

    // Bind js object to a Native object
    AccessibilityWindowInfo* pAccessibilityWindowInfo = new AccessibilityWindowInfo();
    *pAccessibilityWindowInfo = accessibilityWindowInfo;
    napi_status sts = napi_wrap(env, result, pAccessibilityWindowInfo,
                                [](napi_env env, void* data, void* hint) {
                                    AccessibilityWindowInfo* info = (AccessibilityWindowInfo*)data;
                                    delete info;
                                },
                                nullptr, nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
}

void ConvertAccessibilityWindowInfosToJS(napi_env env, napi_value result,
    std::vector<AccessibilityWindowInfo>& accessibilityWindowInfos) {
    size_t idx = 0;

    if (accessibilityWindowInfos.empty()) {
        return;
    }

    for (auto& windowInfo : accessibilityWindowInfos) {
        napi_value obj = nullptr;
        napi_new_instance(env, NAccessibilityWindowInfo::cons_, 0, nullptr, &obj);
        ConvertAccessibilityWindowInfoToJS(env, obj, windowInfo);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertAccessibleAbilityInfosToJS(napi_env env, napi_value result,
    std::vector<OHOS::Accessibility::AccessibilityAbilityInfo>& accessibleAbilityInfos)
{
    size_t index = 0;

    if (accessibleAbilityInfos.empty()) {
        return;
    }

    for (auto& abilityInfo : accessibleAbilityInfos) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertAccessibleAbilityInfoToJS(env, obj, abilityInfo);
        napi_set_element(env, result, index, obj);
        index++;
    }
}

static const std::string ConvertAccessibilityEventTypeToString(EventType type) {
    static const std::map<EventType, const std::string> a11yEvtTypeTable = {
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT, "textMoveUnit"},
        {EventType::TYPE_WINDOW_UPDATE, "windowUpdate"},
        {EventType::TYPE_PAGE_CONTENT_UPDATE, "pageContentUpdate"},
        {EventType::TYPE_PAGE_STATE_UPDATE, "pageStateUpdate"},
        {EventType::TYPE_TOUCH_BEGIN, "touchBegin"},
        {EventType::TYPE_TOUCH_END, "touchEnd"},
        {EventType::TYPE_TOUCH_GUIDE_BEGIN, "touchGuideBegin"},
        {EventType::TYPE_TOUCH_GUIDE_END, "touchGuideEnd"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN, "touchGuideGestureBegin"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_END, "touchGuideGestureEnd"},
        {EventType::TYPE_PUBLIC_NOTICE_EVENT, "publicNotice"},
        {EventType::TYPE_NOTIFICATION_UPDATE_EVENT, "notificationUpdate"},
        // {EventType::TYPE_VIEW_CONTEXT_CLICKED_EVENT, ""}
    };

    if (a11yEvtTypeTable.find(type) == a11yEvtTypeTable.end()) {
        return "";
    }

    return a11yEvtTypeTable.at(type);
}

static const std::string ConvertWindowUpdateTypeToString(WindowUpdateType type) {
    static const std::map<WindowUpdateType, const std::string> WindowUpdateTypeTable = {
        {WindowUpdateType::WINDOW_UPDATE_ACCESSIBILITY_FOCUSED, "accessibilityFocus"},
        {WindowUpdateType::WINDOW_UPDATE_FOCUSED, "focus"},
        {WindowUpdateType::WINDOW_UPDATE_ACTIVE, "active"},
        {WindowUpdateType::WINDOW_UPDATE_ADDED, "add"},
        {WindowUpdateType::WINDOW_UPDATE_REMOVED, "remove"},
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, "bounds"},
        {WindowUpdateType::WINDOW_UPDATE_TITLE, "title"},
        {WindowUpdateType::WINDOW_UPDATE_LAYER, "layer"},
        {WindowUpdateType::WINDOW_UPDATE_PARENT, "parent"},
        {WindowUpdateType::WINDOW_UPDATE_CHILDREN, "children"},
        {WindowUpdateType::WINDOW_UPDATE_PIP, "pip"}
    };

    if (WindowUpdateTypeTable.find(type) == WindowUpdateTypeTable.end()) {
        return "";
    }

    return WindowUpdateTypeTable.at(type);
}

static const std::string ConvertOperationTypeToString(ActionType type) {
    static const std::map<ActionType, const std::string> triggerActionTable = {
        {ActionType::ACCESSIBILITY_ACTION_FOCUS, "focus"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS, "clearFocus"},
        {ActionType::ACCESSIBILITY_ACTION_SELECT, "select"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION, "clearSelection"},
        {ActionType::ACCESSIBILITY_ACTION_CLICK, "click"},
        {ActionType::ACCESSIBILITY_ACTION_LONG_CLICK, "longClick"},
        {ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS, "accessibilityFocus"},
        {ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, "clearAccessibilityFocus"},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD, "scrollForward"},
        {ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD, "scrollBackward"},
        {ActionType::ACCESSIBILITY_ACTION_COPY, "copy"},
        {ActionType::ACCESSIBILITY_ACTION_PASTE, "paste"},
        {ActionType::ACCESSIBILITY_ACTION_CUT, "cut"},
        {ActionType::ACCESSIBILITY_ACTION_SET_SELECTION, "setSelection"},
        {ActionType::ACCESSIBILITY_ACTION_SET_TEXT, "setText"},
        {ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT, "nextText"},
        {ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT, "previousText"},
        {ActionType::ACCESSIBILITY_ACTION_UNFOLD, "unfold"},
        {ActionType::ACCESSIBILITY_ACTION_FOLD, "fold"},
        {ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM, "nextHtmlItem"},
        {ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM, "previousHtmlItem"},
        {ActionType::ACCESSIBILITY_ACTION_DELETED, "delete"},
    };

    if (triggerActionTable.find(type) == triggerActionTable.end()) {
        return "";
    }

    return triggerActionTable.at(type);
}

static const std::string CovertTextMoveStepToString(TextMoveUnit step) {
    static const std::map<TextMoveUnit, const std::string> textMoveStepTable = {
        {TextMoveUnit::STEP_CHARACTER, "char"},
        {TextMoveUnit::STEP_WORD, "word"},
        {TextMoveUnit::STEP_LINE, "line"},
        {TextMoveUnit::STEP_PAGE, "page"},
        {TextMoveUnit::STEP_PARAGRAPH, "paragraph"}
    };

    if (textMoveStepTable.find(step) == textMoveStepTable.end()) {
        return "";
    }

    return textMoveStepTable.at(step);
}

static const std::string ConvertCategoryNotificationToString(NotificationCategory category) {
    static const std::map<NotificationCategory, const std::string> categoryTable = {
        {NotificationCategory::CATEGORY_CALL, "call"},
        {NotificationCategory::CATEGORY_MSG, "msg"},
        {NotificationCategory::CATEGORY_EMAIL, "email"},
        {NotificationCategory::CATEGORY_EVENT, "event"},
        {NotificationCategory::CATEGORY_PROMO, "promo"},
        {NotificationCategory::CATEGORY_ALARM, "alarm"},
        {NotificationCategory::CATEGORY_PROGRESS, "progress"},
        {NotificationCategory::CATEGORY_SOCIAL, "social"},
        {NotificationCategory::CATEGORY_ERR, "err"},
        {NotificationCategory::CATEGORY_TRANSPORT, "transport"},
        {NotificationCategory::CATEGORY_SYS, "sys"},
        {NotificationCategory::CATEGORY_SERVICE, "service"},
        {NotificationCategory::CATEGORY_OTHERS, ""},
    };

    if (categoryTable.find(category) == categoryTable.end()) {
        return "";
    }

    return categoryTable.at(category);
}

void ConvertAccessibilityEventInfoToJS(napi_env env,napi_value objEventInfo, const AccessibilityEventInfo &eventInfo) {

    napi_value nType;
    EventType type = eventInfo.GetEventType();
    std::string strType = ConvertAccessibilityEventTypeToString(type);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strType.c_str(), NAPI_AUTO_LENGTH, &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "type", nType));
    HILOG_DEBUG("%{public}s: type[%{public}s]", __func__, strType.c_str());

    napi_value nWindowUpdateType;
    WindowUpdateType windowUpdateType = eventInfo.GetWindowChangeTypes();
    std::string strWindowUpdateType = ConvertWindowUpdateTypeToString(windowUpdateType);
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, strWindowUpdateType.c_str(), NAPI_AUTO_LENGTH, &nWindowUpdateType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "windowUpdateType", nWindowUpdateType));
    HILOG_DEBUG("%{public}s: windowUpdateType[%{public}s]", __func__, strWindowUpdateType.c_str());

    napi_value nBundleName;
    std::string bundleName = eventInfo.GetBundleName();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "bundleName", nBundleName));

    napi_value nComponentType;
    std::string componentType = eventInfo.GetComponentType();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, componentType.c_str(), NAPI_AUTO_LENGTH, &nComponentType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "componentType", nComponentType));
    HILOG_DEBUG("%{public}s: componentType[%{public}s]", __func__, componentType.c_str());

    napi_value nTimeStamp;
    int64_t timeStamp = eventInfo.GetTimeStamp();
    NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, timeStamp, &nTimeStamp));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "timeStamp", nTimeStamp));

    napi_value nWindowId;
    int windowId = eventInfo.GetWindowId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, windowId, &nWindowId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "windowId", nWindowId));
    HILOG_DEBUG("%{public}s: windowId[%{public}d]", __func__, windowId);

    napi_value nPageId;
    int pageId = eventInfo.GetPageId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, pageId, &nPageId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "pageId", nPageId));
    HILOG_DEBUG("%{public}s: pageId[%{public}d]", __func__, pageId);

    napi_value nComponentId;
    int componentId = eventInfo.GetViewId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, componentId, &nComponentId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "componentId", nComponentId));
    HILOG_DEBUG("%{public}s: componentId[%{public}d]", __func__, componentId);

    napi_value nDescription;
    std::string description = eventInfo.GetDescription();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "description", nDescription));

    napi_value nTriggerAction;
    ActionType triggerAction = eventInfo.GetTriggerAction();
    std::string strTriggerAction = ConvertOperationTypeToString(triggerAction);
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, strTriggerAction.c_str(), NAPI_AUTO_LENGTH, &nTriggerAction));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "triggerAction", nTriggerAction));

    napi_value nTextMoveUnit;
    TextMoveUnit textMoveUnit = eventInfo.GetTextMovementStep();
    std::string strTextMoveUnit = CovertTextMoveStepToString(textMoveUnit);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strTextMoveUnit.c_str(), NAPI_AUTO_LENGTH, &nTextMoveUnit));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "textMoveUnit", nTextMoveUnit));

    napi_value nContents;
    std::vector<std::string> contents = eventInfo.GetContentList();
    size_t contentCount = contents.size();
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nContents));
    for (size_t idx = 0; idx < contentCount; idx++) {
        napi_value nContent;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, contents[idx].c_str(), NAPI_AUTO_LENGTH, &nContent));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nContents, idx, nContent));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "contents", nContents));

    napi_value nLastContent;
    std::string lastContent = eventInfo.GetLatestContent();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, lastContent.c_str(), NAPI_AUTO_LENGTH, &nLastContent));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "lastContent", nLastContent));

    napi_value nBeginIndex;
    int beginIndex = eventInfo.GetBeginIndex();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, beginIndex, &nBeginIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "beginIndex", nBeginIndex));

    napi_value nCurrentIndex;
    int currentIndex = eventInfo.GetCurrentIndex();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, currentIndex, &nCurrentIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "currentIndex", nCurrentIndex));

    napi_value nEndIndex;
    int endIndex = eventInfo.GetEndIndex();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, endIndex, &nEndIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "endIndex", nEndIndex));

    napi_value nItemCount;
    int itemCount = eventInfo.GetItemCounts();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, itemCount, &nItemCount));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "itemCount", nItemCount));

    // napi_value nBeforeText;
    // std::string beforeText = eventInfo.GetBeforeText();
    // NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, beforeText.c_str(), NAPI_AUTO_LENGTH, &nBeforeText));
    // NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "beforeText", nBeforeText));

    napi_value nCategoryNotification;
    NotificationCategory categoryNotification = eventInfo.GetNotificationInfo();
    std::string strCategoryNotification = ConvertCategoryNotificationToString(categoryNotification);
    NAPI_CALL_RETURN_VOID(env,
        napi_create_string_utf8(env, strCategoryNotification.c_str(), NAPI_AUTO_LENGTH, &nCategoryNotification));
    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, objEventInfo, "categoryNotification", nCategoryNotification));

    // Bind js object to a Native object
    AccessibilityEventInfo* pAccessibilityEventInfo = new AccessibilityEventInfo();
    *pAccessibilityEventInfo = eventInfo;
    napi_status sts = napi_wrap(env, objEventInfo, pAccessibilityEventInfo,
                                [](napi_env env, void* data, void* hint) {
                                    AccessibilityEventInfo* info = (AccessibilityEventInfo*)data;
                                    delete info;
                                },
                                nullptr, nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
}

void ConvertOperationToJS(napi_env env, napi_value result, AccessibleAction& operation) {
    napi_value nType;
    ActionType type = operation.GetActionType();
    std::string strType = ConvertOperationTypeToString(type);
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strType.c_str(), NAPI_AUTO_LENGTH, &nType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "type", nType));
    HILOG_DEBUG("%{public}s: operationType[%{public}s]", __func__, strType.c_str());

    napi_value nDescription;
    std::string strDescription = operation.GetDescriptionInfo();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strDescription.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", nDescription));
}

void ConvertRangeInfoToJS(napi_env env, napi_value nRangeInfo, OHOS::Accessibility::RangeInfo& rangeInfo) {
    napi_value nMin;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rangeInfo.GetMin(), &nMin));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nRangeInfo, "min", nMin));

    napi_value nMax;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rangeInfo.GetMax(), &nMax));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nRangeInfo, "max", nMax));

    napi_value nCurrent;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, rangeInfo.GetCurrent(), &nCurrent));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nRangeInfo, "current", nCurrent));
}

void ConvertGridInfoToJS(napi_env env, napi_value nGrid, OHOS::Accessibility::GridInfo& grid) {
    napi_value nRowCount;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetRowCount(), &nRowCount));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "rowCount", nRowCount));

    napi_value nColumnCount;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetColumnCount(), &nColumnCount));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "columnCount", nColumnCount));

    napi_value nSelectionMode;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, grid.GetSelectionMode(), &nSelectionMode));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGrid, "selectionMode", nSelectionMode));
}

void ConvertGridItemToJS(napi_env env, napi_value nGridItem, GridItemInfo& gridItem) {
    napi_value nHeading;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, gridItem.IsHeading(), &nHeading));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "heading", nHeading));

    napi_value nColumnIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetColumnIndex(), &nColumnIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "columnIndex", nColumnIndex));

    napi_value nRowIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetRowIndex(), &nRowIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "rowIndex", nRowIndex));

    napi_value nColumnSpan;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetColumnSpan(), &nColumnSpan));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "columnSpan", nColumnSpan));

    napi_value nRowSpan;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, gridItem.GetRowSpan(), &nRowSpan));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "rowSpan", nRowSpan));

    napi_value nSelected;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, gridItem.IsSelected(), &nSelected));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nGridItem, "selected", nSelected));
}

void ConvertElementInfosToJS(napi_env env, napi_value result,
    std::vector<OHOS::Accessibility::AccessibilityElementInfo>& elementInfos)
{
    size_t index = 0;

    for (auto& elementInfo : elementInfos) {
        napi_value obj = nullptr;
        napi_new_instance(env, NElementInfo::cons_, 0, nullptr, &obj);
        ConvertElementInfoToJS(env, obj, elementInfo);
        napi_set_element(env, result, index, obj);
        index++;
    }
}

void ConvertElementInfoToJS(napi_env env, napi_value result, AccessibilityElementInfo& elementInfo) {

    napi_value nWindowsId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetWindowId(), &nWindowsId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "windowId", nWindowsId));
    HILOG_DEBUG("%{public}s: windowId[%{public}d]", __func__, elementInfo.GetWindowId());

    napi_value nAccessibilityId;
    int accessibilityId = elementInfo.GetAccessibilityId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, accessibilityId, &nAccessibilityId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "accessibilityId", nAccessibilityId));
    HILOG_DEBUG("%{public}s: accessibilityId[%{public}d]", __func__, accessibilityId);

    napi_value nComponentId;
    int componentId = elementInfo.GetAccessibilityId();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, componentId, &nComponentId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "componentId", nComponentId));

    napi_value nBundleName;
    std::string strBundleName = elementInfo.GetBundleName();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strBundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "bundleName", nBundleName));

    napi_value nComponentType;
    std::string strComponentType = elementInfo.GetComponentType();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strComponentType.c_str(), NAPI_AUTO_LENGTH,
                          &nComponentType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "componentType", nComponentType));
    HILOG_DEBUG("%{public}s: componentType[%{public}s]", __func__, strComponentType.c_str());

    napi_value nInputType;
    int inputType = elementInfo.GetInputType();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, inputType, &nInputType));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "inputType", nInputType));

    napi_value nText;
    std::string strText = elementInfo.GetContent();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strText.c_str(), NAPI_AUTO_LENGTH, &nText));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "text", nText));
    HILOG_DEBUG("%{public}s: text[%{public}s]", __func__, strText.c_str());

    napi_value nHintText;
    std::string strHintText = elementInfo.GetHint();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strHintText.c_str(), NAPI_AUTO_LENGTH, &nHintText));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "hintText", nHintText));

    napi_value nDescription;
    std::string strDescription = elementInfo.GetDescriptionInfo();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strDescription.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "description", nDescription));

    napi_value nResourceName;
    std::string strResourceName = elementInfo.GetComponentResourceId();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strResourceName.c_str(), NAPI_AUTO_LENGTH, &nResourceName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "resourceName", nResourceName));

    napi_value nChildNodeIds;
    std::vector<int> childIds = elementInfo.GetChildIds();
    size_t childCount = childIds.size();
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nChildNodeIds));

    for (size_t idx = 0; idx < childCount; idx++) {
        napi_value nChildId;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, childIds[idx], &nChildId));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nChildNodeIds, idx, nChildId));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "childNodeIds", nChildNodeIds));

    napi_value nOperations;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nOperations));
    std::vector<AccessibleAction> operations = elementInfo.GetActionList();
    size_t operationCount = operations.size();
    for (size_t idx = 0; idx < operationCount; idx++) {
        napi_value nOp;
        napi_create_object(env, &nOp);
        ConvertOperationToJS(env, nOp, operations[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nOperations, idx, nOp));
    }
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "operations", nOperations));

    napi_value nTextLengthLimit;
    int textLengthLimit = elementInfo.GetTextLengthLimit();
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, textLengthLimit, &nTextLengthLimit));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "textLengthLimit", nTextLengthLimit));

    napi_value nRect;
    napi_create_object(env, &nRect);
    Accessibility::Rect rect = elementInfo.GetRectInScreen();
    ConvertRectToJS(env, nRect, rect);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "rect", nRect));

    napi_value nCheckable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsCheckable(), &nCheckable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "checkable", nCheckable));

    napi_value nChecked;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsChecked(), &nChecked));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "checked", nChecked));

    napi_value nFocusable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsFocusable(), &nFocusable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "focusable", nFocusable));
    HILOG_DEBUG("%{public}s: focusable[%{public}d]", __func__, elementInfo.IsFocusable());

    napi_value nFocused;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsFocused(), &nFocused));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "focused", nFocused));
    HILOG_DEBUG("%{public}s: focused[%{public}d]", __func__, elementInfo.IsFocused());

    napi_value nVisable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsVisible(), &nVisable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isVisible", nVisable));
    HILOG_DEBUG("%{public}s: isVisible[%{public}d]", __func__, elementInfo.IsVisible());

    napi_value nAccessibilityFocused;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.HasAccessibilityFocus(), &nAccessibilityFocused));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "accessibilityFocused", nAccessibilityFocused));
    HILOG_DEBUG("%{public}s: accessibilityFocused[%{public}d]", __func__, elementInfo.HasAccessibilityFocus());

    napi_value nSelected;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsSelected(), &nSelected));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "selected", nSelected));

    napi_value nClickable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsClickable(), &nClickable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "clickable", nClickable));

    napi_value nLongClickable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsLongClickable(), &nLongClickable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "longClickable", nLongClickable));

    napi_value nIsEnable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsEnabled(), &nIsEnable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isEnable", nIsEnable));

    napi_value nIsPassword;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsPassword(), &nIsPassword));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isPassword", nIsPassword));

    napi_value nScrollable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsScrollable(), &nScrollable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "scrollable", nScrollable));

    napi_value nEditable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsEditable(), &nEditable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "editable", nEditable));

    napi_value nPopupSupported;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsPopupSupported(), &nPopupSupported));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "popupSupported", nPopupSupported));

    napi_value nPluraLineSupported;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsPluraLineSupported(), &nPluraLineSupported));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "pluraLineSupported", nPluraLineSupported));

    napi_value nDeleteable;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsDeletable(), &nDeleteable));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "deleteable", nDeleteable));

    napi_value nIsHint;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsGivingHint(), &nIsHint));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isHint", nIsHint));

    napi_value nIsEssential;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.IsEssential(), &nIsEssential));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isEssential", nIsEssential));
    HILOG_DEBUG("%{public}s: isEssential[%{public}d]", __func__, elementInfo.IsEssential());

    napi_value nCurrentIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetCurrentIndex(), &nCurrentIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "currentIndex", nCurrentIndex));

    napi_value nStartIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetBeginIndex(), &nStartIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "startIndex", nStartIndex));

    napi_value nEndIndex;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetEndIndex(), &nEndIndex));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "endIndex", nEndIndex));

    napi_value nRangeInfo;
    napi_create_object(env, &nRangeInfo);
    RangeInfo rangeInfo = elementInfo.GetRange();
    ConvertRangeInfoToJS(env, nRangeInfo, rangeInfo);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "rangeInfo", nRangeInfo));

    napi_value nGrid;
    napi_create_object(env, &nGrid);
    GridInfo grid = elementInfo.GetGrid();
    ConvertGridInfoToJS(env, nGrid, grid);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "grid", nGrid));

    napi_value nGridItem;
    napi_create_object(env, &nGridItem);
    GridItemInfo gridItem = elementInfo.GetGridItem();
    ConvertGridItemToJS(env, nGridItem, gridItem);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "gridItem", nGridItem));

    napi_value nActiveRegion;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetLiveRegion(), &nActiveRegion));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "activeRegion", nActiveRegion));

    napi_value nIsContentInvalid;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, elementInfo.GetContentInvalid(), &nIsContentInvalid));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "isContentInvalid", nIsContentInvalid));

    napi_value nError;
    std::string strError = elementInfo.GetError();
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, strError.c_str(), NAPI_AUTO_LENGTH, &nError));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "error", nError));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetLabeledAccessibilityId(), &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "label", nLabel));

    napi_value nBeginSelected;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetSelectedBegin(), &nBeginSelected));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "beginSelected", nBeginSelected));

    napi_value nEndSelected;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, elementInfo.GetSelectedEnd(), &nEndSelected));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, "endSelected", nEndSelected));
    // Bind js object to a Native object
    AccessibilityElementInfo* pElementInfo = new AccessibilityElementInfo(elementInfo);
    napi_status sts = napi_wrap(env, result, pElementInfo,
                                [](napi_env env, void* data, void* hint) {
                                    AccessibilityElementInfo* info = (AccessibilityElementInfo*)data;
                                    delete info;
                                },
                                nullptr, nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
}

static WindowUpdateType ConvertStringToWindowUpdateTypes(std::string type) {
    static const std::map<const std::string, WindowUpdateType> windowsUpdateTypesTable = {
        {"accessibilityFocus", WindowUpdateType::WINDOW_UPDATE_ACCESSIBILITY_FOCUSED},
        {"focus", WindowUpdateType::WINDOW_UPDATE_FOCUSED},
        {"active", WindowUpdateType::WINDOW_UPDATE_ACTIVE},
        {"add", WindowUpdateType::WINDOW_UPDATE_ADDED},
        {"remove", WindowUpdateType::WINDOW_UPDATE_REMOVED},
        {"bounds", WindowUpdateType::WINDOW_UPDATE_BOUNDS},
        {"title", WindowUpdateType::WINDOW_UPDATE_TITLE},
        {"layer", WindowUpdateType::WINDOW_UPDATE_LAYER},
        {"parent", WindowUpdateType::WINDOW_UPDATE_PARENT},
        {"children", WindowUpdateType::WINDOW_UPDATE_CHILDREN},
        {"pip", WindowUpdateType::WINDOW_UPDATE_PIP}
    };

    if (windowsUpdateTypesTable.find(type) == windowsUpdateTypesTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return WINDOW_UPDATE_INVALID;
    }

    return windowsUpdateTypesTable.at(type);
}

static EventType ConvertStringToEventInfoTypes(std::string type) {
    static const std::map<const std::string, EventType> eventInfoTypesTable = {
        {"click", EventType::TYPE_VIEW_CLICKED_EVENT},
        {"longClick", EventType::TYPE_VIEW_LONG_CLICKED_EVENT},
        {"select", EventType::TYPE_VIEW_SELECTED_EVENT},
        {"focus", EventType::TYPE_VIEW_FOCUSED_EVENT},
        {"textUpdate", EventType::TYPE_VIEW_TEXT_UPDATE_EVENT},
        {"pageStateUpdate", EventType::TYPE_PAGE_STATE_UPDATE},
        {"notificationUpdate", EventType::TYPE_NOTIFICATION_UPDATE_EVENT},
        {"hoverEnter", EventType::TYPE_VIEW_HOVER_ENTER_EVENT},
        {"hoverExit", EventType::TYPE_VIEW_HOVER_EXIT_EVENT},
        {"touchGuideGestureBegin", EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN},
        {"touchGuideGestureEnd", EventType::TYPE_TOUCH_GUIDE_GESTURE_END},
        {"pageContentUpdate", EventType::TYPE_PAGE_CONTENT_UPDATE},
        {"scroll", EventType::TYPE_VIEW_SCROLLED_EVENT},
        {"textSelectionUpdate", EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT},
        {"publicNotice", EventType::TYPE_PUBLIC_NOTICE_EVENT},
        {"accessibilityFocus", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT},
        {"accessibilityFocusClear", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT},
        {"textMoveUnit", EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT},
        {"touchGuideBegin", EventType::TYPE_TOUCH_GUIDE_BEGIN},
        {"touchGuideEnd", EventType::TYPE_TOUCH_GUIDE_END},
        {"touchBegin", EventType::TYPE_TOUCH_BEGIN},
        {"touchEnd", EventType::TYPE_TOUCH_END},
        {"windowUpdate", EventType::TYPE_WINDOW_UPDATE}
    };

    if (eventInfoTypesTable.find(type) == eventInfoTypesTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return TYPE_VIEW_INVALID;
    }

    return eventInfoTypesTable.at(type);
}

ActionType ConvertStringToAccessibleOperationType(std::string type) {
    std::map<const std::string, ActionType> accessibleOperationTypeTable = {
        {"focus", ActionType::ACCESSIBILITY_ACTION_FOCUS},
        {"clearFocus", ActionType::ACCESSIBILITY_ACTION_CLEAR_FOCUS},
        {"select", ActionType::ACCESSIBILITY_ACTION_SELECT},
        {"clearSelection", ActionType::ACCESSIBILITY_ACTION_CLEAR_SELECTION},
        {"click", ActionType::ACCESSIBILITY_ACTION_CLICK},
        {"longClick", ActionType::ACCESSIBILITY_ACTION_LONG_CLICK},
        {"accessibilityFocus", ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS},
        {"clearAccessibilityFocus", ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS},
        {"scrollForward", ActionType::ACCESSIBILITY_ACTION_SCROLL_FORWARD},
        {"scrollBackward", ActionType::ACCESSIBILITY_ACTION_SCROLL_BACKWARD},
        {"copy", ActionType::ACCESSIBILITY_ACTION_COPY},
        {"paste", ActionType::ACCESSIBILITY_ACTION_PASTE},
        {"cut", ActionType::ACCESSIBILITY_ACTION_CUT},
        {"setSelection", ActionType::ACCESSIBILITY_ACTION_SET_SELECTION},
        {"setText", ActionType::ACCESSIBILITY_ACTION_SET_TEXT},
        {"nextText", ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT},
        {"previousText", ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT},
        {"unfold", ActionType::ACCESSIBILITY_ACTION_UNFOLD},
        {"fold", ActionType::ACCESSIBILITY_ACTION_FOLD},
        {"nextHtmlItem", ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM},
        {"previousHtmlItem", ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM},
        {"delete", ActionType::ACCESSIBILITY_ACTION_DELETED}
    };

    if (accessibleOperationTypeTable.find(type) == accessibleOperationTypeTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return ACCESSIBILITY_ACTION_INVALID;
    }

    return accessibleOperationTypeTable.at(type);
}

AccessibilityAbilityTypes ConvertStringToAccessibilityAbilityTypes(std::string type)
{
    std::map<const std::string, AccessibilityAbilityTypes> accessibilityAbilityTypesTable = {
        {"spoken", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN},
        {"haptic", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC},
        {"audible", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE},
        {"visual", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL},
        {"generic", AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC},
    };

    if (accessibilityAbilityTypesTable.find(type) == accessibilityAbilityTypesTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_INVALID;
    }

    return accessibilityAbilityTypesTable.at(type);
}

AbilityStateType ConvertStringToAbilityStateTypes(std::string type)
{
    std::map<const std::string, AbilityStateType> abilityStateTypeTable = {
        {"enable", AbilityStateType::ABILITY_STATE_ENABLE},
        {"disable", AbilityStateType::ABILITY_STATE_DISABLE},
        {"install", AbilityStateType::ABILITY_STATE_INSTALLED}
    };

    if (abilityStateTypeTable.find(type) == abilityStateTypeTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return ABILITY_STATE_INVALID;
    }

    return abilityStateTypeTable.at(type);
}

GlobalAction ConvertStringToGlobalAction(std::string type)
{
    std::map<const std::string, GlobalAction> globalActionTable = {
        {"back", GlobalAction::GLOBAL_ACTION_BACK},
        {"home", GlobalAction::GLOBAL_ACTION_HOME},
        {"recent", GlobalAction::GLOBAL_ACTION_RECENT},
        {"notification", GlobalAction::GLOBAL_ACTION_NOTIFICATION},
        {"popUpPowerDialog", GlobalAction::GLOBAL_ACTION_POP_UP_POWER_DIALOG},
        {"divideScreen", GlobalAction::GLOBAL_ACTION_DIVIDE_SCREEN},
        {"lockScreen", GlobalAction::GLOBAL_ACTION_LOCK_SCREEN},
        {"captureScreen", GlobalAction::GLOBAL_ACTION_CAPTURE_SCREEN}
    };

    if (globalActionTable.find(type) == globalActionTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return GLOBAL_ACTION_INVALID;
    }

    return globalActionTable.at(type);
}

static TextMoveUnit ConvertStringToTextMoveStep(std::string type) {
    static const std::map<const std::string, TextMoveUnit> textMoveStepTable = {
        {"char", TextMoveUnit::STEP_CHARACTER},
        {"word", TextMoveUnit::STEP_WORD},
        {"line", TextMoveUnit::STEP_LINE},
        {"page", TextMoveUnit::STEP_PAGE},
        {"paragraph", TextMoveUnit::STEP_PARAGRAPH}
    };

    if (textMoveStepTable.find(type) == textMoveStepTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return STEP_INVALID;
    }

    return textMoveStepTable.at(type);
}

static NotificationCategory ConvertStringToNotificationCategory(std::string type) {
    static const std::map<const std::string, NotificationCategory> notificationCategoryTable = {
        {"call", NotificationCategory::CATEGORY_CALL},
        {"msg", NotificationCategory::CATEGORY_MSG},
        {"email", NotificationCategory::CATEGORY_EMAIL},
        {"event", NotificationCategory::CATEGORY_EVENT},
        {"promo", NotificationCategory::CATEGORY_PROMO},
        {"alarm", NotificationCategory::CATEGORY_ALARM},
        {"progress", NotificationCategory::CATEGORY_PROGRESS},
        {"social", NotificationCategory::CATEGORY_SOCIAL},
        {"err", NotificationCategory::CATEGORY_ERR},
        {"transport", NotificationCategory::CATEGORY_TRANSPORT},
        {"sys", NotificationCategory::CATEGORY_SYS},
        {"service", NotificationCategory::CATEGORY_SERVICE}
    };

    if (notificationCategoryTable.find(type) == notificationCategoryTable.end()) {
        HILOG_WARN("%{public}s: invalid key[%{public}s]", __func__, type.c_str());
        return CATEGORY_INVALID;
    }

    return notificationCategoryTable.at(type);
}

void ConvertActionArgsJSToNAPI(napi_env env, napi_value object, std::map<std::string, std::string> &args,
    OHOS::Accessibility::ActionType action)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    std::string str = "";
    switch(action) {
        case ActionType::ACCESSIBILITY_ACTION_NEXT_HTML_ITEM:
            napi_create_string_utf8(env, "htmlItem", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("htmlItem", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM:
            napi_create_string_utf8(env, "htmlItem", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("htmlItem", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_NEXT_TEXT:
            napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("textMoveUnit", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_PREVIOUS_TEXT:
            napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("textMoveUnit", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_SELECTION:
            napi_create_string_utf8(env, "selectTextBegin", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("selectTextBegin", str.c_str()));
            }
            napi_create_string_utf8(env, "selectTextEnd", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("selectTextEnd", str.c_str()));
            }
            break;
        case ActionType::ACCESSIBILITY_ACTION_SET_TEXT:
            napi_create_string_utf8(env, "setText", NAPI_AUTO_LENGTH, &propertyNameValue);
            napi_has_property(env, object, propertyNameValue, &hasProperty);
            if (hasProperty) {
                napi_value value = nullptr;
                napi_get_property(env, object, propertyNameValue, &value);
                str = GetStringFromNAPI(env, value);
                args.insert(std::pair<std::string, std::string>("setText", str.c_str()));
            }
            break;
        default:
            break;
    }
}

void ConvertEventInfoJSToNAPI(napi_env env, napi_value object, AccessibilityEventInfo &eventInfo)
{
    napi_value propertyNameValue = nullptr;
    bool hasProperty = false;
    int32_t dataValue = 0;
    std::string str = "";
    napi_create_string_utf8(env, "type", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetEventType(ConvertStringToEventInfoTypes(str));
    }

    napi_create_string_utf8(env, "windowUpdateType", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetWindowChangeTypes(ConvertStringToWindowUpdateTypes(str));
    }

    napi_create_string_utf8(env, "bundleName", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetBundleName(str);
    }

    napi_create_string_utf8(env, "componentType", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetComponentType(str);
    }

    int64_t timestamp = 0;
    napi_create_string_utf8(env, "timeStamp", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int64(env, value, &timestamp);
        eventInfo.SetTimeStamp(timestamp);
    }

    napi_create_string_utf8(env, "windowId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetWindowId(dataValue);
    }

    napi_create_string_utf8(env, "pageId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetPageId(dataValue);
    }

    napi_create_string_utf8(env, "componentId", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetSource(dataValue);
    }

    napi_create_string_utf8(env, "description", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetComponentType(str);
    }

    napi_create_string_utf8(env, "triggerAction", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetTriggerAction(ConvertStringToAccessibleOperationType(str));
    }

    napi_create_string_utf8(env, "textMoveUnit", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetTextMovementStep(ConvertStringToTextMoveStep(str));
    }

    napi_create_string_utf8(env, "contents", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        void *datas = nullptr;
        size_t dataLen = 0;
        napi_get_arraybuffer_info(env, value, &datas, &dataLen);
        auto rawData = (std::string *)datas;
        for (int i = 0; i < int(dataLen); i++) {
            eventInfo.AddContent(*(rawData+i));
        }
    }

    napi_create_string_utf8(env, "lastContent", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetLatestContent(str);
    }

    napi_create_string_utf8(env, "beginIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetBeginIndex(dataValue);
    }

    napi_create_string_utf8(env, "currentIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetCurrentIndex(dataValue);
    }

    napi_create_string_utf8(env, "endIndex", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetEndIndex(dataValue);
    }

    napi_create_string_utf8(env, "itemCount", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        napi_get_value_int32(env, value, &dataValue);
        eventInfo.SetItemCounts(dataValue);
    }

    // napi_create_string_utf8(env, "beforeText", NAPI_AUTO_LENGTH, &propertyNameValue);
    // napi_has_property(env, object, propertyNameValue, &hasProperty);
    // if (hasProperty) {
    //     napi_value value = nullptr;
    //     napi_get_property(env, object, propertyNameValue, &value);
    //     str = GetStringFromNAPI(env, value);
    //     eventInfo.SetBeforeText(str);
    // }


    napi_create_string_utf8(env, "categoryNotification", NAPI_AUTO_LENGTH, &propertyNameValue);
    napi_has_property(env, object, propertyNameValue, &hasProperty);
    if (hasProperty) {
        napi_value value = nullptr;
        napi_get_property(env, object, propertyNameValue, &value);
        str = GetStringFromNAPI(env, value);
        eventInfo.SetNotificationInfo(ConvertStringToNotificationCategory(str));
    }
}

std::vector<std::string>  ParseAbilityTypesToVec(uint32_t abilityTypesValue)
{
    std::vector<std::string> result;

    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN) {
        result.push_back("spoken");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC) {
        result.push_back("haptic");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE) {
        result.push_back("audible");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL) {
        result.push_back("visual");
    }
    if (abilityTypesValue & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC) {
        result.push_back("generic");
    }

    return result;
}

std::vector<std::string>  ParseCapabilitiesToVec(uint32_t capabilitiesValue)
{
    std::vector<std::string> result;

    if (capabilitiesValue & Capability::CAPABILITY_RETRIEVE) {
        result.push_back("retrieve");
    }
    if (capabilitiesValue & Capability::CAPABILITY_TOUCH_GUIDE) {
        result.push_back("touchGuide");
    }
    if (capabilitiesValue & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
        result.push_back("keyEventObserver");
    }
    if (capabilitiesValue & Capability::CAPABILITY_ZOOM) {
        result.push_back("zoom");
    }
    if (capabilitiesValue & Capability::CAPABILITY_GESTURE) {
        result.push_back("gesture");
    }

    return result;
}

std::vector<std::string>  ParseEventTypesToVec(uint32_t eventTypesValue)
{
    std::vector<std::string> result;

    if (eventTypesValue & EventType::TYPE_VIEW_CLICKED_EVENT) {
        result.push_back("click");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_LONG_CLICKED_EVENT) {
        result.push_back("longClick");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_SELECTED_EVENT) {
        result.push_back("select");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_FOCUSED_EVENT) {
        result.push_back("focus");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_TEXT_UPDATE_EVENT) {
        result.push_back("textUpdate");
    }
    if (eventTypesValue & EventType::TYPE_PAGE_STATE_UPDATE) {
        result.push_back("pageStateUpdate");
    }
    if (eventTypesValue & EventType::TYPE_NOTIFICATION_UPDATE_EVENT) {
        result.push_back("notificationUpdate");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_HOVER_ENTER_EVENT) {
        result.push_back("hoverEnter");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_HOVER_EXIT_EVENT) {
        result.push_back("hoverExit");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN) {
        result.push_back("touchGuideGestureBegin");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_GUIDE_GESTURE_END) {
        result.push_back("touchGuideGestureEnd");
    }
    if (eventTypesValue & EventType::TYPE_PAGE_CONTENT_UPDATE) {
        result.push_back("pageContentUpdate");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_SCROLLED_EVENT) {
        result.push_back("scroll");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT) {
        result.push_back("textSelectionUpdate");
    }
    if (eventTypesValue & EventType::TYPE_PUBLIC_NOTICE_EVENT) {
        result.push_back("publicNotice");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT) {
        result.push_back("accessibilityFocus");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT) {
        result.push_back("accessibilityFocusClear");
    }
    if (eventTypesValue & EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT) {
        result.push_back("textMoveUnit");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_GUIDE_BEGIN) {
        result.push_back("touchGuideBegin");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_GUIDE_END) {
        result.push_back("touchGuideEnd");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_BEGIN) {
        result.push_back("touchBegin");
    }
    if (eventTypesValue & EventType::TYPE_TOUCH_END) {
        result.push_back("touchEnd");
    }
    if (eventTypesValue & EventType::TYPE_WINDOW_UPDATE) {
        result.push_back("windowUpdate");
    }

    return result;
}