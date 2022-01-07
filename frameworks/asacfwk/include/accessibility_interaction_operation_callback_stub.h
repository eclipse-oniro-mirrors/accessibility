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

#ifndef ACCESSIBILITY_INTERACTION_OPERATION_CALLBACK_STUB_H
#define ACCESSIBILITY_INTERACTION_OPERATION_CALLBACK_STUB_H

#include <cstdint>
#include <list>
#include <string>

#include "accessibility_interaction_operation_callback_interface.h"
#include "accessibility_errorcode.h"
#include "iremote_stub.h"
#include "nocopyable.h"

namespace OHOS {
namespace Accessibility {
/*
* The class define the interface for UI to implement.
* It triggered by ABMS when AA to request the accessibility information.
*/
class AccessibilityInteractionOperationCallbackStub : public IRemoteStub<IAccessibilityInteractionOperationCallback> {
public:
    /**
     * @brief construct function
     * @param object The object of IPC
     * @return
     */
    AccessibilityInteractionOperationCallbackStub();

    /**
     * @brief destruct function
     * @param
     * @return
     */
    virtual ~AccessibilityInteractionOperationCallbackStub();

    /**
     * @brief Receive the event from proxy by IPC mechanism.
     * @param code The code is matched with the process function.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @param option The option paramter of IPC,such as: async,sync
     * @return
     */
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByAccessibilityIdResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSetSearchElementInfoByAccessibilityIdResult(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetSearchElementInfoByTextResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSetSearchElementInfoByTextResult(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFindFocusedElementInfoResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSetFindFocusedElementInfoResult(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetFocusMoveSearchResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSetFocusMoveSearchResult(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handle the IPC request for the function:SetPerformActionResult.
     * @param data The data of prcess communication
     * @param reply The response of IPC request
     * @return NO_ERROR: successful; otherwise is failed.
     */
    ErrCode HandleSetPerformActionResult(MessageParcel &data, MessageParcel &reply);

    using AccessibilityInteractionOperationCallbackFunc = ErrCode (AccessibilityInteractionOperationCallbackStub::*)
    (MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AccessibilityInteractionOperationCallbackFunc> memberFuncMap_;
};

} //namespace Accessibility
} //namespace OHOS
#endif