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

#ifndef FOUNDATION_ABILITYRUNTIME_OHOS_EXTENSION_H
#define FOUNDATION_ABILITYRUNTIME_OHOS_EXTENSION_H

#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AbilityRuntime {
class Extension : public std::enable_shared_from_this<Extension> {
public:
    Extension() = default;
    virtual ~Extension() = default;
    /**
     * @brief Called when this Service extension is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information about the Service extension.
     * @return Returns a pointer to the <b>sid</b> of the connected Service extension.
     */
    virtual sptr<IRemoteObject> OnConnect(const AAFwk::Want &want)
    {
        return nullptr;
    }
};
}
}
#endif  // FOUNDATION_ABILITYRUNTIME_OHOS_EXTENSION_H