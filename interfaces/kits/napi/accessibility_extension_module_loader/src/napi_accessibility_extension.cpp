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

#include "napi_accessibility_extension.h"

#include <uv.h>
#include "accessible_ability_client.h"
#include "ability_info.h"
#include "hilog_wrapper.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_extension_context.h"
#include "napi_accessibility_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace Accessibility {
NAccessibilityExtension* NAccessibilityExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    HILOG_INFO("NAccessibilityExtension::Create runtime");
    return new NAccessibilityExtension(static_cast<AbilityRuntime::JsRuntime&>(*runtime));
}

NAccessibilityExtension::NAccessibilityExtension(AbilityRuntime::JsRuntime& jsRuntime) : jsRuntime_(jsRuntime)
{
    listener_ = std::make_shared<AbilityListener>(*this);

    HandleScope handleScope(jsRuntime_);
    engine_ = &jsRuntime_.GetNativeEngine();
}

NAccessibilityExtension::~NAccessibilityExtension() = default;

void NAccessibilityExtension::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
    std::shared_ptr<AppExecFwk::AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO("NAccessibilityExtension::Init begin.");
    AccessibilityExtension::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        HILOG_ERROR("Failed to get srcPath");
        return;
    }

    if (!abilityInfo_) {
        HILOG_ERROR("abilityInfo_ is nullptr.");
        return;
    }
    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("moduleName:%{public}s, srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath);
    if (!jsObj_) {
        HILOG_ERROR("Failed to get jsObj_");
        return;
    }
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(jsObj_->Get());
    if (!obj) {
        HILOG_ERROR("Failed to get NAccessibilityExtension object");
        return;
    }

    auto context = GetContext();
    if (!context) {
        HILOG_ERROR("Failed to get context");
        return;
    }
    NativeValue* contextObj = CreateJsAccessibilityExtensionContext(*engine_, context);
    auto shellContextRef = jsRuntime_.LoadSystemModule("application.AccessibilityExtensionContext", &contextObj, 1);
    if (!shellContextRef) {
        HILOG_ERROR("shellContextRef is nullptr.");
        return;
    }
    contextObj = shellContextRef->Get();
    context->Bind(jsRuntime_, shellContextRef.release());
    obj->SetProperty("context", contextObj);

    auto nativeObj = ConvertNativeValueTo<NativeObject>(contextObj);
    if (!nativeObj) {
        HILOG_ERROR("Failed to get accessibility extension native object");
        return;
    }
    nativeObj->SetNativePointer(new std::weak_ptr<AbilityRuntime::Context>(context),
        [](NativeEngine*, void* data, void*) {
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr);
}

sptr<IRemoteObject> NAccessibilityExtension::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO("called.");
    Extension::OnConnect(want);
    sptr<AccessibleAbilityClient> instance = AccessibleAbilityClient::GetInstance();
    if (!instance) {
        HILOG_ERROR("instance is nullptr");
        return nullptr;
    }
    instance->RegisterAbilityListener(listener_);
    return instance->GetRemoteObject();
}

void NAccessibilityExtension::OnAbilityConnected()
{
    HILOG_INFO("called.");
    uv_loop_t *loop = engine_->GetUVLoop();
    uv_work_t *work = new uv_work_t;
    ExtensionCallbackInfo *callbackInfo = new ExtensionCallbackInfo();
    callbackInfo->extension_ = this;
    work->data = callbackInfo;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ExtensionCallbackInfo *data = (ExtensionCallbackInfo *)work->data;
            data->extension_->CallObjectMethod("onConnect");
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    HILOG_INFO("end.");
}

void NAccessibilityExtension::OnAbilityDisconnected()
{
    HILOG_INFO("called.");
    uv_loop_t *loop = engine_->GetUVLoop();
    uv_work_t *work = new uv_work_t;
    ExtensionCallbackInfo *callbackInfo = new ExtensionCallbackInfo();
    callbackInfo->extension_ = this;
    work->data = callbackInfo;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ExtensionCallbackInfo *data = (ExtensionCallbackInfo *)work->data;
            data->extension_->CallObjectMethod("onDisconnect");
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    HILOG_INFO("end.");
}

void NAccessibilityExtension::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_INFO("called.");
    uv_loop_t *loop = engine_->GetUVLoop();
    uv_work_t *work = new uv_work_t;
    AccessibilityEventInfoCallbackInfo *callbackInfo = new AccessibilityEventInfoCallbackInfo();
    callbackInfo->engine_ = engine_;
    callbackInfo->extension_ = this;
    callbackInfo->eventInfo_ = eventInfo;
    work->data = callbackInfo;

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AccessibilityEventInfoCallbackInfo *data = (AccessibilityEventInfoCallbackInfo *)work->data;
            napi_value cons = nullptr;
            napi_get_reference_value(reinterpret_cast<napi_env>(data->engine_), NAccessibilityEventInfo::consRef_,
                &cons);
            napi_value napiEventInfo = nullptr;
            napi_new_instance(
                reinterpret_cast<napi_env>(data->engine_), cons, 0, nullptr, &napiEventInfo);
            ConvertAccessibilityEventInfoToJS(reinterpret_cast<napi_env>(data->engine_), napiEventInfo,
                data->eventInfo_);
            NativeValue* nativeEventInfo = reinterpret_cast<NativeValue*>(napiEventInfo);
            NativeValue* argv[] = {nativeEventInfo};
            data->extension_->CallObjectMethod("onAccessibilityEvent", argv, 1);
            
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    HILOG_INFO("end.");
}

bool NAccessibilityExtension::OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    HILOG_INFO("called.");
    uv_loop_t *loop = engine_->GetUVLoop();
    uv_work_t *work = new uv_work_t;
    KeyEventCallbackInfo *callbackInfo = new KeyEventCallbackInfo();
    callbackInfo->engine_ = engine_;
    callbackInfo->keyEvent_ = MMI::KeyEvent::Clone(keyEvent);
    callbackInfo->extension_ = this;
    work->data = callbackInfo;
    std::future syncFuture = callbackInfo->syncPromise_.get_future();

    uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            KeyEventCallbackInfo *data = (KeyEventCallbackInfo *)work->data;
            napi_value napiEventInfo = nullptr;
            if (napi_create_object(reinterpret_cast<napi_env>(data->engine_), &napiEventInfo) != napi_ok) {
                HILOG_ERROR("Create keyEvent object failed.");
                delete data;
                data = nullptr;
                delete work;
                work = nullptr;
                data->syncPromise_.set_value(false);
                return;
            }
            ConvertKeyEventToJS(reinterpret_cast<napi_env>(data->engine_), napiEventInfo, data->keyEvent_);
            NativeValue* nativeEventInfo = reinterpret_cast<NativeValue*>(napiEventInfo);
            NativeValue* argv[] = {nativeEventInfo};
            NativeValue* nativeResult = data->extension_->CallObjectMethod("onKeyEvent", argv, 1);

            // Unwrap result
            bool result = false;
            if (!ConvertFromJsValue(*data->engine_, nativeResult, result)) {
                HILOG_ERROR("ConvertFromJsValue failed");
                delete data;
                data = nullptr;
                delete work;
                work = nullptr;
                data->syncPromise_.set_value(false);
                return;
            }
            HILOG_INFO("OnKeyPressEvent result = %{public}d", result);
            data->syncPromise_.set_value(result);

            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    bool callbackResult = syncFuture.get();
    HILOG_INFO("OnKeyPressEvent callbackResult = %{public}d", callbackResult);
    return callbackResult;
}

NativeValue* NAccessibilityExtension::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    HILOG_INFO("NAccessibilityExtension::CallObjectMethod(%{public}s), begin", name);
    if (!jsObj_) {
        HILOG_ERROR("jsObj_ is nullptr");
        return nullptr;
    }

    NativeValue* value = jsObj_->Get();
    NativeObject* obj = ConvertNativeValueTo<NativeObject>(value);
    if (!obj) {
        HILOG_ERROR("Failed to get AccessibilityExtension object");
        return nullptr;
    }

    NativeValue* method = obj->GetProperty(name);
    if (!method) {
        HILOG_ERROR("Failed to get '%{public}s' from AccessibilityExtension object", name);
        return nullptr;
    }
    HILOG_INFO("NAccessibilityExtension::CallFunction(%{public}s), success", name);
    return engine_->CallFunction(value, method, argv, argc);
}

void NAccessibilityExtension::GetSrcPath(std::string &srcPath)
{
    if (!Extension::abilityInfo_) {
        HILOG_ERROR("abilityInfo_ is nullptr");
        return;
    }
    if (!Extension::abilityInfo_->isModuleJson) {
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