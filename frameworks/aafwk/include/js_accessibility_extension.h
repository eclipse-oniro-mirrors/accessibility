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

#ifndef JS_ACCESSIBILITY_EXTENSION_H
#define JS_ACCESSIBILITY_EXTENSION_H

#include "accessibility_extension.h"
#include "js_runtime.h"

class NativeReference;
class NativeValue;

namespace OHOS {
namespace Accessibility {
class AccessibilityExtension;

class JsAccessibilityExtension : public AccessibilityExtension {
public:
    JsAccessibilityExtension(AbilityRuntime::JsRuntime& jsRuntime);
    virtual ~JsAccessibilityExtension() override;

    /**
     * @brief Create JsAccessibilityExtension.
     *
     * @param runtime The runtime.
     * @return The JsAccessibilityExtension instance.
     */
    static JsAccessibilityExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;

    /**
     * @brief Called when this Accessibility extension is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information
     * about the Accessibility extension.
     * @return Returns a pointer to the <b>sid</b> of the connected Accessibility extension.
     */
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;

    /**
     * @brief Called when your accessibility service is successfully connected to the OS.
     * @param
     * @return
     */
    void OnAbilityConnected() override;

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The infomation of accessible event.
     * @return
     */
    void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override;

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @return Returns true if the event has been consumed; returns false otherwise.
     *         The event that has been consumed will not be sent to the application.
     */
    bool OnKeyPressEvent(const MMI::KeyEvent& keyEvent) override;

private:
    NativeValue* CallObjectMethod(const char* name, NativeValue * const * argv = nullptr, size_t argc = 0);

    void GetSrcPath(std::string &srcPath);

    AbilityRuntime::JsRuntime& jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
};
} // namespace Accessibility
} // namespace OHOS

#endif  // JS_ACCESSIBILITY_EXTENSION_H