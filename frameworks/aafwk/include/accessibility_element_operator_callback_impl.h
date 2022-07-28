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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_IMPL_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_IMPL_H

#include <future>
#include "accessibility_element_operator_callback_stub.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the api called by AA to get the accessibility info of UI.
* It call the API of AAMS and the request will be processed by
* UI and then set the result by callback(AA-->this class(with Callback)--> AAMS
* --> UI process and set Callback).
* It supply one instance for each thread to use this class.
*/
class AccessibilityElementOperatorCallbackImpl : public AccessibilityElementOperatorCallbackStub {
public:
    /**
     * @brief construct function
     */
    AccessibilityElementOperatorCallbackImpl() = default;

    /**
     * @brief deconstruct function
     */
    ~AccessibilityElementOperatorCallbackImpl() = default;

    /**
     * @brief Save the elements information searched in ACE side
     * @param infos The elements info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override;

    /**
     * @brief Save the elements information searched in ACE side
     * @param infos The elements info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int32_t requestId) override;

    /**
     * @brief Save the element information searched in ACE side
     * @param info The element info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info,
        const int32_t requestId) override;

    /**
     * @brief Save the element information searched in ACE side
     * @param info The element info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int32_t requestId) override;

    /**
     * @brief Save the result of action executed in ACE.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param requestId The request id from AA, it is used to match with request and response.
     */
    virtual void SetExecuteActionResult(const bool succeeded, const int32_t requestId) override;

private:
    std::promise<void> promise_;
    bool executeActionResult_ = false;
    AccessibilityElementInfo accessibilityInfoResult_ = {};
    std::vector<AccessibilityElementInfo> elementInfosResult_;

    friend class AccessibleAbilityChannelClient;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_IMPL_H