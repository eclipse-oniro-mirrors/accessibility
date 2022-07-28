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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_H

#include <list>
#include "accessibility_element_info.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the callback to feedback the result from UI to AA.
*/
class AccessibilityElementOperatorCallback {
public:
    /**
     * @brief Destruct
     */
    virtual ~AccessibilityElementOperatorCallback() = default;

    /**
     * @brief Set the element information by accessibility id to AA.
     * @param infos The element information searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetSearchElementInfoByAccessibilityIdResult(const std::list<AccessibilityElementInfo> &infos,
        const int32_t requestId) = 0;

    /**
     * @brief Set the element information matched with text to AA.
     * @param infos The element information searched matched with text.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetSearchElementInfoByTextResult(const std::list<AccessibilityElementInfo> &infos,
        const int32_t requestId) = 0;

    /**
     * @brief Set the element information matched with focus type to AA.
     * @param info The element information searched matched with focus type.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int32_t requestId) = 0;

    /**
     * @brief Set the element information by focus direction to AA.
     * @param info The element information searched by focus direction.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) = 0;

    /**
     * @brief Set the result of action executed to AA.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetExecuteActionResult(const bool succeeded, const int32_t requestId) = 0;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_H