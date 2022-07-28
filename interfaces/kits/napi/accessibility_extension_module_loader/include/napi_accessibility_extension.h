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

#ifndef NAPI_ACCESSIBILITY_EXTENSION_H
#define NAPI_ACCESSIBILITY_EXTENSION_H

#include <future>
#include "accessibility_extension.h"
#include "accessible_ability_listener.h"
#include "js_runtime.h"
#include "napi_accessibility_element.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"

namespace OHOS {
namespace Accessibility {
class NAccessibilityExtension : public AccessibilityExtension {
public:
    NAccessibilityExtension(AbilityRuntime::JsRuntime& jsRuntime);
    virtual ~NAccessibilityExtension() override;

    /**
     * @brief Create NAccessibilityExtension.
     *
     * @param runtime The runtime.
     * @return The NAccessibilityExtension instance.
     */
    static NAccessibilityExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);

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
     */
    void OnAbilityConnected();

    /**
     * @brief Called when your accessibility service is successfully disconnected to the OS.
     */
    void OnAbilityDisconnected();

    /**
     * @brief Called when an accessibility event occurs.
     * @param eventInfo The information of accessible event.
     */
    void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo);

    /**
     * @brief Called when a key event occurs.
     * @param keyEvent Indicates the key event to send.
     * @return Returns true if the event has been consumed; returns false otherwise.
     *         The event that has been consumed will not be sent to the application.
     */
    bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

private:
    class AbilityListener : public AccessibleAbilityListener {
    public:
        AbilityListener(NAccessibilityExtension &extension) : extension_(extension) {}
        ~AbilityListener() = default;
        /**
         * @brief Called when your accessibility service is successfully connected to the OS.
         */
        void OnAbilityConnected() override
        {
            extension_.OnAbilityConnected();
        }

        /**
         * @brief Called when an accessibility is disconnected.
         */
        void OnAbilityDisconnected() override
        {
            extension_.OnAbilityDisconnected();
        }

        /**
         * @brief Called when an accessibility event occurs.
         * @param eventInfo The information of accessible event.
         */
        void OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo) override
        {
            extension_.OnAccessibilityEvent(eventInfo);
        }

        /**
         * @brief Called when a key event occurs.
         * @param keyEvent Indicates the key event to send.
         * @return Returns true if the event has been consumed; returns false otherwise.
         *         The event that has been consumed will not be sent to the application.
         */
        bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent) override
        {
            return extension_.OnKeyPressEvent(keyEvent);
        }

    private:
        NAccessibilityExtension &extension_;
    };

    NativeValue* CallObjectMethod(const char* name, NativeValue * const * argv = nullptr, size_t argc = 0);

    std::shared_ptr<AccessibilityElement> GetElement(const AccessibilityEventInfo& eventInfo);
    void CreateElementInfoByEventInfo(const AccessibilityEventInfo& eventInfo,
        const std::shared_ptr<AccessibilityElementInfo> &elementInfo);

    void GetSrcPath(std::string &srcPath);

    NativeEngine *engine_ = nullptr;
    AbilityRuntime::JsRuntime& jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    std::shared_ptr<AbilityListener> listener_ = nullptr;
};

struct ExtensionCallbackInfo {
    NativeEngine *engine_;
    NAccessibilityExtension *extension_;
};

struct KeyEventCallbackInfo : public ExtensionCallbackInfo {
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
    std::promise<bool> syncPromise_;
};

struct AccessibilityEventInfoCallbackInfo : public ExtensionCallbackInfo {
    std::string eventType_ = "";
    int64_t timeStamp_ = 0;
    std::shared_ptr<AccessibilityElement> element_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS

#endif  // NAPI_ACCESSIBILITY_EXTENSION_H