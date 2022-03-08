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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_PROXY_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_CALLBACK_PROXY_H

#include <cstdint>
#include <list>
#include <string>

#include "accessibility_element_operator_callback_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the callback to feedback the result from UI to AA.
*/
class AccessibilityElementOperatorCallbackProxy : public IRemoteProxy<IAccessibilityElementOperatorCallback> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    explicit AccessibilityElementOperatorCallbackProxy(const sptr<IRemoteObject> &object);

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibilityElementOperatorCallbackProxy() override;

    /**
     * @brief Set the element information by accessibility id to AA.
     * @param infos The element info searched by accessibility id.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetSearchElementInfoByAccessibilityIdResult(const std::vector<AccessibilityElementInfo> &infos,
        const int requestId) override;

    /**
     * @brief Set the element information matched with text to AA.
     * @param infos The element information searched matched with text.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetSearchElementInfoByTextResult(const std::vector<AccessibilityElementInfo> &infos,
        const int requestId) override;

    /**
     * @brief Set the element information matched with focus type to AA.
     * @param info The element information searched matched with focus type.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetFindFocusedElementInfoResult(const AccessibilityElementInfo &info, const int requestId) override;

    /**
     * @brief Set the element information by focus direction to AA.
     * @param info The element information searched by focus direction.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetFocusMoveSearchResult(const AccessibilityElementInfo &info, const int requestId) override;

    /**
     * @brief Set the result of action executed to AA.
     * @param succeeded True: The action is executed successfully; otherwise is false.
     * @param requestId The request id from AA, it is used to match with request and response.
     * @return
     */
    void SetExecuteActionResult(const bool succeeded, const int requestId) override;

private:
    /**
     * @brief Write the descriptor of IPC.
     * @param data It is include the descriptor of IPC.
     * @return true: Write the descriptor successfully; otherwise is not.
     */
    bool WriteInterfaceToken(MessageParcel &data);

    /**
     * @brief Write the serializable data
     * @param parcelableVector The communication data of IPC
     * @param reply Serializable data
     * @return true: Write the serializable data successfully; otherwise is not.
     */
    template<typename T>
    bool WriteParcelableVector(const std::vector<T> &parcelableVector, Parcel &reply);
    static inline BrokerDelegator<AccessibilityElementOperatorCallbackProxy> delegator;
};
} // namespace Accessibility
} // namespace OHOS
#endif