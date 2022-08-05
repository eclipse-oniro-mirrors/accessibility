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

#ifndef ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H
#define ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H

#include "i_accessibility_element_operator.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface for UI to implement.
* It is triggered by ABMS when AA to request the accessibility information.
*/
class AccessibilityElementOperatorStub : public IRemoteStub<IAccessibilityElementOperator> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     */
    AccessibilityElementOperatorStub();

    /**
     * @brief destruct function
     */
    virtual ~AccessibilityElementOperatorStub();

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @param option The option parameter of IPC,such as: async,sync
     */
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByAccessibilityIdResult.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSearchElementInfoByAccessibilityId(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByTextResult.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSearchElementInfosByText(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFindFocusedElementInfoResult.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleFindFocusedElementInfo(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFocusMoveSearchResult.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleFocusFind(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetExecuteActionResult.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleExecuteAction(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:ClearFocus.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleClearFocus(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:OutsideTouch.
     * @param data The data of process communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleOutsideTouch(MessageParcel &data, MessageParcel &reply);

    using AccessibilityElementOperatorFunc =
        ErrCode (AccessibilityElementOperatorStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AccessibilityElementOperatorFunc> memberFuncMap_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_ELEMENT_OPERATOR_STUB_H