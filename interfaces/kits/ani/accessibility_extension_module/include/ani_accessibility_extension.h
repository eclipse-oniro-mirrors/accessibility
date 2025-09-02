/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef ANI_ACCESSIBILITY_EXTENSION_H
#define ANI_ACCESSIBILITY_EXTENSION_H

#include <uv.h>
#include "ani.h"
#include "accessibility_extension.h"
#include "accessible_ability_listener.h"
#include "ffrt_inner.h"
#include "ets_runtime.h"
#include "runtime.h"
#include "native_engine/native_reference.h"
#include "ohos_application.h"
#include "want.h"
#include "ani_accessibility_extension_context.h"
#include "key_event.h"

namespace OHOS {
namespace Accessibility {
class AniAccessibilityExtension : public AccessibilityExtension {
public:
    AniAccessibilityExtension(AbilityRuntime::ETSRuntime& stsRuntime);
    virtual ~AniAccessibilityExtension() override;

    /**
     * @brief Create NAccessibilityExtension.
     *
     * @param runtime The runtime.
     * @return The NAccessibilityExtension instance.
     */
    static AniAccessibilityExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);

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

    bool GetSrcPathAndModuleName(std::string &srcPath, std::string &moduleName);

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

    void onAccessibilityConnect(ani_env *env, ani_object object);

    void onAccessibilityDisconnect(ani_env *env, ani_object object);

private:
class AbilityListener : public AccessibleAbilityListener {
    public:
        AbilityListener(AniAccessibilityExtension &extension) : extension_(extension) {}

        AbilityListener() = delete;
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
            (void)keyEvent;
            return false;
        }

    private:
        AniAccessibilityExtension &extension_;
    };
private:
    void BindContext(ani_env *env, std::shared_ptr<AAFwk::Want> want,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application);

    std::unique_ptr<AppExecFwk::ETSNativeReference> jsObj_;
    AbilityRuntime::ETSRuntime& stsRuntime_;
    std::shared_ptr<AbilityListener> listener_ = nullptr;
    ani_env* env_ = nullptr;
};

ani_class GetAniAccessibilityElementClass();
} // namespace Accessibility
} // namespace OHOS

#endif  // ANI_ACCESSIBILITY_EXTENSION_H