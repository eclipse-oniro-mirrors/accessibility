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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_INTERFACE_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_INTERFACE_H

#include <cstdint>
#include <list>
#include <string>

#include "accessibility_element_info.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the callback to feedback the result from UI to AA.
*/
class IAccessibilityElementOperatorCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.accessibility.IAccessibilityElementOperatorCallback");

    /**
     * @brief Set the element information by accessibility id to AA.
     * @param infos The element info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    virtual void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int requestId) = 0;

    /**
     * @brief Set the element informations matched with text to AA.
     * @param infos The element informations searched matched with text.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    virtual void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int requestId) = 0;

    /**
     * @brief Set the element informations matched with focus type to AA.
     * @param info The element informations searched matched with focus type.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId) = 0;

    /**
     * @brief Set the element informations by focus direction to AA.
     * @param info The element informations searched by focus direction.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId) = 0;

    /**
     * @brief Set the result of action executed to AA.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    virtual void SetExecuteActionResult(const bool succeeded, const int requestId) = 0;

    enum class Message {
        SET_RESULT_BY_ACCESSIBILITY_ID = 0,
        SET_RESULT_BY_TEXT,
        SET_RESULT_FOCUSED_INFO,
        SET_RESULT_FOCUS_MOVE,
        SET_RESULT_PERFORM_ACTION,
    };
};
} // namespace Accessibility
} // namespace OHOS
#endif