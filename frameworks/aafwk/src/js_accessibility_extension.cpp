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

#include "js_accessibility_extension.h"

#include "accessible_ability_client_stub_impl.h"
#include "ability_info.h"
#include "hilog_wrapper.h"
#include "js_accessibility_extension_context.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace Accessibility {
JsAccessibilityExtension* JsAccessibilityExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    HILOG_INFO("JsAccessibilityExtension::Create runtime");
    return new JsAccessibilityExtension(static_cast<AbilityRuntime::JsRuntime&>(*runtime));
}

JsAccessibilityExtension::JsAccessibilityExtension(AbilityRuntime::JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}
JsAccessibilityExtension::~JsAccessibilityExtension() = default;

void JsAccessibilityExtension::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
    std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    HILOG_INFO("JsAccessibilityExtension::Init begin.");
    AccessibilityExtension::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("JsAccessibilityExtension::Init moduleName:%{public}s, srcPath:%{public}s.",
        moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);
    auto& engine = jsRuntime_.GetNativeEngine();

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (jsObj_ == nullptr) {
        HILOG_ERROR("Failed to get jsObj_");
        return;
    }
    HILOG_INFO("JsAccessibilityExtension::Init ConvertNativeValueTo.");
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (obj == nullptr) {
        HILOG_ERROR("Failed to get JsAccessibilityExtension object");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        HILOG_ERROR("Failed to get context");
        return;
    }
    HILOG_INFO("JsAccessibilityExtension::Init CreateJsAccessibilityExtensionContext.");
    NativeValue* contextObj = CreateJsAccessibilityExtensionContext(engine, context);
    auto shellContextRef = jsRuntime_.LoadSystemModule("application.AccessibilityExtensionContext", &contextObj, 1);
    contextObj = shellContextRef->Get();
    HILOG_INFO("JsAccessibilityExtension::Init Bind.");
    context->Bind(jsRuntime_, shellContextRef.release());
    HILOG_INFO("JsAccessibilityExtension::SetProperty.");
    obj->SetProperty("context", contextObj);

    auto nativeObj = ConvertNativeValueTo<NativeObject>(contextObj);
    if (nativeObj == nullptr) {
        HILOG_ERROR("Failed to get accessibility extension native object");
        return;
    }

    HILOG_INFO("Set accessibility extension context pointer: %{public}p", context.get());

    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context),
        [](NativeEngine*, void* data, void*) {
            HILOG_INFO("Finalizer for weak_ptr accessibility extension context is called");
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr);

    HILOG_INFO("JsAccessibilityExtension::Init end.");
}

sptr<IRemoteObject> JsAccessibilityExtension::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO("called.");
    Extension::OnConnect(want);

    sptr<AccessibleAbilityClientStubImpl> stub = new AccessibleAbilityClientStubImpl();
    if (!stub) {
        HILOG_ERROR("stub is nullptr.");
        return nullptr;
    }
    HILOG_DEBUG("Create stub successfully");
    std::shared_ptr<AccessibilityExtension> accessibilityExtension =
        std::static_pointer_cast<JsAccessibilityExtension>(shared_from_this());
    stub->RegisterListenerImpl(accessibilityExtension);

    return stub->AsObject();
}

void JsAccessibilityExtension::OnAbilityConnected()
{
    HILOG_INFO("called.");
    AccessibilityExtension::OnAbilityConnected();
    CallObjectMethod("onAbilityConnected");
    HILOG_INFO("end.");
}

void JsAccessibilityExtension::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_INFO("called.");
    AccessibilityExtension::OnAccessibilityEvent(eventInfo);

    HandleScope handleScope(jsRuntime_);
    NativeEngine* nativeEngine = &jsRuntime_.GetNativeEngine();

    napi_get_reference_value(reinterpret_cast<napi_env>(&nativeEngine), NAccessibilityEventInfo::consRef_, 
        &NAccessibilityEventInfo::cons_);
    napi_value napiEventInfo = nullptr;
    napi_new_instance(
        reinterpret_cast<napi_env>(nativeEngine), NAccessibilityEventInfo::cons_, 0, nullptr, &napiEventInfo);
    ConvertAccessibilityEventInfoToJS(reinterpret_cast<napi_env>(nativeEngine), napiEventInfo, eventInfo);
    NativeValue* nativeEventInfo = reinterpret_cast<NativeValue*>(napiEventInfo);
    NativeValue* argv[] = {nativeEventInfo};
    CallObjectMethod("onAccessibilityEvent", argv, 1);
    HILOG_INFO("end.");
}

bool JsAccessibilityExtension::OnKeyPressEvent(const MMI::KeyEvent& keyEvent)
{
    HILOG_INFO("called.");
    AccessibilityExtension::OnKeyPressEvent(keyEvent);

    HandleScope handleScope(jsRuntime_);
    NativeEngine* nativeEngine = &jsRuntime_.GetNativeEngine();
    // wrap keyEvent
    napi_value napiEventInfo = nullptr;
    if (napi_create_object(reinterpret_cast<napi_env>(nativeEngine), &napiEventInfo) != napi_ok) {
        HILOG_ERROR("Create keyEvent object failed.");
        return false;
    }
    ConvertKeyEventToJS(reinterpret_cast<napi_env>(nativeEngine), napiEventInfo, const_cast<MMI::KeyEvent&>(keyEvent));
    NativeValue* nativeEventInfo = reinterpret_cast<NativeValue*>(napiEventInfo);
    NativeValue* argv[] = {nativeEventInfo};
    NativeValue* nativeResult = CallObjectMethod("onKeyPressEvent", argv, 1);

    // unwrap result
    bool result = false;
    if (!ConvertFromJsValue(*nativeEngine, nativeResult, result)) {
        HILOG_ERROR("ConvertFromJsValue failed");
        return false;
    }
    HILOG_INFO("OnKeyPressEvent result = %{public}d", result);

    return result;
}

NativeValue* JsAccessibilityExtension::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    HILOG_INFO("JsAccessibilityExtension::CallObjectMethod(%{public}s), begin", name);
    if (!jsObj_) {
        HILOG_WARN("jsObj_ is nullptr");
        return nullptr;
    }

    HandleScope handleScope(jsRuntime_);
    auto& nativeEngine = jsRuntime_.GetNativeEngine();

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (obj == nullptr) {
        HILOG_ERROR("Failed to get AccessibilityExtension object");
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (method == nullptr) {
        HILOG_ERROR("Failed to get '%{public}s' from AccessibilityExtension object", name);
        return nullptr;
    }
    HILOG_INFO("JsAccessibilityExtension::CallFunction(%{public}s), success", name);
    return handleScope.Escape(nativeEngine.CallFunction(value, method, argv, argc));
}

void JsAccessibilityExtension::GetSrcPath(std::string &srcPath)
{
    if (!Extension::abilityInfo_->isModuleJson) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}
} // namespace Accessibility
} // namespace OHOS