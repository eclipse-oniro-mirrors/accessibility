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

#ifndef ANI_ACCESSIBILITY_ELEMENT_H
#define ANI_ACCESSIBILITY_ELEMENT_H

#include "accessibility_def.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_window_info.h"
#include "ani.h"

namespace OHOS {
namespace Accessibility {
    struct AccessibilityElement {
    AccessibilityElement() = default;
    AccessibilityElement(std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> elementInfo)
        : elementInfo_(elementInfo), isElementInfo_(true)
    {}
    AccessibilityElement(std::shared_ptr<OHOS::Accessibility::AccessibilityWindowInfo> windowInfo)
        : windowInfo_(windowInfo), isElementInfo_(false)
    {}

    std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> elementInfo_ = nullptr;
    std::shared_ptr<OHOS::Accessibility::AccessibilityWindowInfo> windowInfo_ = nullptr;
    bool isElementInfo_ = true;
};

enum FindElementCondition {
    FIND_ELEMENT_CONDITION_CONTENT,
    FIND_ELEMENT_CONDITION_TEXT_TYPE,
    FIND_ELEMENT_CONDITION_FOCUS_DIRECTION,
    FIND_ELEMENT_CONDITION_ELEMENT_ID,
    FIND_ELEMENT_CONDITION_INVALID,
    FIND_ELEMENT_BY_CONDITION
};

struct FindElementParams {
    FindElementCondition conditionId_ = FIND_ELEMENT_CONDITION_INVALID;
    std::string condition_ = "";
    AccessibilityElement accessibilityElement_ = {};
    std::vector<OHOS::Accessibility::AccessibilityElementInfo> nodeInfos_ {};
    AccessibilityElementInfo nodeInfo_ = {};
    bool systemApi_ = false;
    std::string attribute_ = "";
    OHOS::Accessibility::RetError ret_ = OHOS::Accessibility::RetError::RET_ERR_FAILED;
    std::map<std::string, std::string> actionArguments_;
    std::string actionName_ = "";
};

// AccessibilityElement initialization and creation
bool InitializeAccessibilityElementClass(ani_env *env);
void Clean(ani_env *env, ani_object thisObj);
ani_object CreateAniAccessibilityElement(ani_env *env, const AccessibilityWindowInfo& windowInfo);
ani_object CreateAniAccessibilityElement(ani_env *env, const AccessibilityElementInfo& elementInfo);
ani_int GetCursorPosition(ani_env *env, ani_object thisObj);
void EnableScreenCurtain(ani_env *env, ani_object thisObj, ani_boolean isEnable);
ani_object FindElement(ani_env *env, ani_object thisObj, ani_string type, ani_double condition);
ani_object FindElements(ani_env *env, ani_object thisObj, ani_string type, ani_string condition);
ani_object FindElementsByCondition(ani_env *env, ani_object thisObj, ani_string rule, ani_string condition);
void FindElementExecute(FindElementParams* data);
ani_object ConvertElementInfosToJs(ani_env *env, const std::vector<AccessibilityWindowInfo>& windowInfos);
ani_object ConvertElementInfosToJs(ani_env *env, const std::vector<AccessibilityElementInfo>& elementInfos);
ani_object GetRootElement(ani_env *env, ani_object thisObj);
ani_object GetChildren(ani_env *env, ani_object thisObj);
ani_object GetParent(ani_env *env, ani_object thisObj);
ani_object FindElementByContent(ani_env *env, ani_object thisObj, ani_string content);
ani_object FindElementByFocusDirection(ani_env *env, ani_object thisObj, ani_string direction);
ani_object FindElementsByAccessibilityHintText(ani_env *env, ani_object thisObj, ani_string hintText);
ani_object FindElementById(ani_env *env, ani_object thisObj, ani_long elementId);
void FindElementByText(FindElementParams *data);
void ExecuteACtion(ani_env *env, ani_object thisObj, ani_enum_item action, ani_object obj);
} // namespace Accessibility
} // namespace OHOS
#endif // ANI_ACCESSIBILITY_ELEMENT_H