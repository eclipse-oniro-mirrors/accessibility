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

#ifndef ACCESSIBLE_ABILITY_CLIENT_STUB_H
#define ACCESSIBLE_ABILITY_CLIENT_STUB_H

#include <map>
#include <stdint.h>
#include "accessible_ability_client_interface.h"
#include "accessibility_errorcode.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "message_option.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityClientStub : public IRemoteStub<IAccessibleAbilityClient> {
public:
    AccessibleAbilityClientStub();
    virtual ~AccessibleAbilityClientStub();

    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                    MessageOption &option) override;

private:
    ErrCode HandleInit(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleDisconnect(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleOnAccessibilityEvent(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleOnKeyPressEvent(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleOnDisplayResized(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleOnGestureSimulateResult(MessageParcel &data, MessageParcel &reply);

    using AccessibleAbilityClientFunc =
            ErrCode (AccessibleAbilityClientStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AccessibleAbilityClientFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AccessibleAbilityClientStub);
};

} // namespace Accessibility
} // namespace OHOS

#endif // ACCESSIBLE_ABILITY_CLIENT_STUB_H