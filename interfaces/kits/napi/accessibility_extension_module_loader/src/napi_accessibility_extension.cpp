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
#include "napi_accessibility_element.h"

using namespace OHOS::AbilityRuntime;
using namespace OHOS::AccessibilityNapi;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t VIRTUAL_COMPONENT_ID = -1;
}
NAccessibilityExtension* NAccessibilityExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    HILOG_INFO();
    return new(std::nothrow) NAccessibilityExtension(static_cast<AbilityRuntime::JsRuntime&>(*runtime));
}

NAccessibilityExtension::NAccessibilityExtension(AbilityRuntime::JsRuntime& jsRuntime) : jsRuntime_(jsRuntime)
{
    listener_ = std::make_shared<AbilityListener>(*this);

    HandleScope handleScope(jsRuntime_);
    engine_ = &jsRuntime_.GetNativeEngine();
}

NAccessibilityExtension::~NAccessibilityExtension()
{
    jsRuntime_.FreeNativeReference(std::move(jsObj_));
}

void NAccessibilityExtension::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
    std::shared_ptr<AppExecFwk::AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO();
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

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath);
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
    NAccessibilityElement::DefineJSAccessibilityElement(reinterpret_cast<napi_env>(engine_));
}

sptr<IRemoteObject> NAccessibilityExtension::OnConnect(const AAFwk::Want &want)
{
    HILOG_INFO();
    Extension::OnConnect(want);
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return nullptr;
    }
    aaClient->RegisterAbilityListener(listener_);
    return aaClient->GetRemoteObject();
}

void NAccessibilityExtension::OnAbilityConnected()
{
    HILOG_INFO();
    uv_loop_t *loop = engine_->GetUVLoop();
    ExtensionCallbackInfo *callbackInfo = new(std::nothrow) ExtensionCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->extension_ = this;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create data.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    int ret = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ExtensionCallbackInfo *data = static_cast<ExtensionCallbackInfo*>(work->data);
            data->extension_->CallObjectMethod("onConnect");
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    if (ret) {
        HILOG_ERROR("Failed to execute OnAbilityConnected work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
    HILOG_INFO("end.");
}

void NAccessibilityExtension::OnAbilityDisconnected()
{
    HILOG_INFO();
    uv_loop_t *loop = engine_->GetUVLoop();
    ExtensionCallbackInfo *callbackInfo = new(std::nothrow) ExtensionCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->extension_ = this;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create data.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    int ret = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ExtensionCallbackInfo *data = static_cast<ExtensionCallbackInfo*>(work->data);
            data->extension_->CallObjectMethod("onDisconnect");
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    if (ret) {
        HILOG_ERROR("Failed to execute OnAbilityDisconnected work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
    HILOG_INFO("end.");
}

std::shared_ptr<AccessibilityElement> NAccessibilityExtension::GetElement(const AccessibilityEventInfo& eventInfo)
{
    HILOG_DEBUG();

    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        return nullptr;
    }
    int32_t componentId = eventInfo.GetAccessibilityId();
    int32_t windowId = eventInfo.GetWindowId();
    std::shared_ptr<AccessibilityElement> element = nullptr;
    if (componentId > 0) {
        std::shared_ptr<AccessibilityElementInfo> elementInfo = std::make_shared<AccessibilityElementInfo>();
        if (aaClient->GetSource(eventInfo, *elementInfo) == RET_OK) {
            element = std::make_shared<AccessibilityElement>(elementInfo);
        }
    } else if (windowId > 0) {
        std::shared_ptr<AccessibilityWindowInfo> windowInfo = std::make_shared<AccessibilityWindowInfo>();
        if (aaClient->GetWindow(windowId, *windowInfo) == RET_OK) {
            element = std::make_shared<AccessibilityElement>(windowInfo);
        }
    } else {
        std::shared_ptr<AccessibilityElementInfo> elementInfo = std::make_shared<AccessibilityElementInfo>();
        CreateElementInfoByEventInfo(eventInfo, elementInfo);
        element = std::make_shared<AccessibilityElement>(elementInfo);
    }
    return element;
}

void NAccessibilityExtension::CreateElementInfoByEventInfo(const AccessibilityEventInfo& eventInfo,
    const std::shared_ptr<AccessibilityElementInfo> &elementInfo)
{
    HILOG_DEBUG();
    if (!elementInfo) {
        return;
    }
    elementInfo->SetComponentId(VIRTUAL_COMPONENT_ID);
    elementInfo->SetBundleName(eventInfo.GetBundleName());
    elementInfo->SetComponentType(eventInfo.GetComponentType());
    elementInfo->SetPageId(eventInfo.GetPageId());
    elementInfo->SetDescriptionInfo(eventInfo.GetDescription());
    elementInfo->SetTriggerAction(eventInfo.GetTriggerAction());
    elementInfo->SetTextMovementStep(eventInfo.GetTextMovementStep());
    elementInfo->SetContentList(eventInfo.GetContentList());
    elementInfo->SetLatestContent(eventInfo.GetLatestContent());
    elementInfo->SetBeginIndex(eventInfo.GetBeginIndex());
    elementInfo->SetCurrentIndex(eventInfo.GetCurrentIndex());
    elementInfo->SetEndIndex(eventInfo.GetEndIndex());
    elementInfo->SetItemCounts(eventInfo.GetItemCounts());
}

void ConvertAccessibilityElementToJS(napi_env env, napi_value objEventInfo,
    const std::shared_ptr<AccessibilityElement>& element)
{
    HILOG_DEBUG();
    if (!element) {
        HILOG_DEBUG("No element information.");
        return;
    }
    AccessibilityElement* pAccessibilityElement = new(std::nothrow) AccessibilityElement(*element);
    if (!pAccessibilityElement) {
        HILOG_ERROR("Failed to create AccessibilityElement.");
        return;
    }
    napi_value nTargetObject = nullptr;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, NAccessibilityElement::consRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &nTargetObject);
    // Bind js object to a Native object
    napi_status sts = napi_wrap(
        env,
        nTargetObject,
        pAccessibilityElement,
        [](napi_env env, void* data, void* hint) {
            AccessibilityElement* info = static_cast<AccessibilityElement*>(data);
            delete info;
            info = nullptr;
        },
        nullptr,
        nullptr);
    HILOG_DEBUG("napi_wrap status: %{public}d", (int)sts);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objEventInfo, "target", nTargetObject));
}

void NAccessibilityExtension::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_INFO();
    std::string strType = "";
    ConvertEventTypeToString(eventInfo, strType);
    if (strType.empty()) {
        HILOG_DEBUG("eventType is invalid.");
        return;
    }
    uv_loop_t *loop = engine_->GetUVLoop();
    AccessibilityEventInfoCallbackInfo *callbackInfo = new(std::nothrow) AccessibilityEventInfoCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    callbackInfo->engine_ = engine_;
    callbackInfo->extension_ = this;
    callbackInfo->eventType_ = strType;
    callbackInfo->timeStamp_ = eventInfo.GetTimeStamp();
    callbackInfo->element_ = GetElement(eventInfo);
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create data.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);
    int ret = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AccessibilityEventInfoCallbackInfo *data = static_cast<AccessibilityEventInfoCallbackInfo*>(work->data);
            napi_value napiEventInfo = nullptr;
            napi_create_object(reinterpret_cast<napi_env>(data->engine_), &napiEventInfo);

            napi_value nType;
            NAPI_CALL_RETURN_VOID(reinterpret_cast<napi_env>(data->engine_),
                napi_create_string_utf8(reinterpret_cast<napi_env>(data->engine_), data->eventType_.c_str(),
                NAPI_AUTO_LENGTH, &nType));
            NAPI_CALL_RETURN_VOID(reinterpret_cast<napi_env>(data->engine_),
                napi_set_named_property(reinterpret_cast<napi_env>(data->engine_), napiEventInfo, "eventType", nType));
            HILOG_DEBUG("eventType[%{public}s]", data->eventType_.c_str());

            napi_value nTimeStamp;
            NAPI_CALL_RETURN_VOID(reinterpret_cast<napi_env>(data->engine_),
                napi_create_int64(reinterpret_cast<napi_env>(data->engine_), data->timeStamp_, &nTimeStamp));
            NAPI_CALL_RETURN_VOID(reinterpret_cast<napi_env>(data->engine_),
                napi_set_named_property(reinterpret_cast<napi_env>(data->engine_),
                napiEventInfo, "timeStamp", nTimeStamp));

            ConvertAccessibilityElementToJS(reinterpret_cast<napi_env>(data->engine_), napiEventInfo, data->element_);
            NativeValue* nativeEventInfo = reinterpret_cast<NativeValue*>(napiEventInfo);
            NativeValue* argv[] = {nativeEventInfo};
            data->extension_->CallObjectMethod("onAccessibilityEvent", argv, 1);
            
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    if (ret) {
        HILOG_ERROR("Failed to execute OnAccessibilityEvent work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

bool NAccessibilityExtension::OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    HILOG_INFO();
    uv_loop_t *loop = engine_->GetUVLoop();
    KeyEventCallbackInfo *callbackInfo = new(std::nothrow) KeyEventCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return false;
    }
    callbackInfo->engine_ = engine_;
    callbackInfo->keyEvent_ = MMI::KeyEvent::Clone(keyEvent);
    callbackInfo->extension_ = this;
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create data.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return false;
    }
    work->data = static_cast<void*>(callbackInfo);
    std::future syncFuture = callbackInfo->syncPromise_.get_future();

    int ret = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            KeyEventCallbackInfo *data = static_cast<KeyEventCallbackInfo*>(work->data);
            napi_value napiEventInfo = nullptr;
            if (napi_create_object(reinterpret_cast<napi_env>(data->engine_), &napiEventInfo) != napi_ok) {
                HILOG_ERROR("Create keyEvent object failed.");
                data->syncPromise_.set_value(false);
                delete data;
                data = nullptr;
                delete work;
                work = nullptr;
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
                data->syncPromise_.set_value(false);
                delete data;
                data = nullptr;
                delete work;
                work = nullptr;
                return;
            }
            HILOG_INFO("OnKeyPressEvent result = %{public}d", result);
            data->syncPromise_.set_value(result);

            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        });
    if (ret) {
        HILOG_ERROR("Failed to execute OnKeyPressEvent work queue");
        callbackInfo->syncPromise_.set_value(false);
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
    bool callbackResult = syncFuture.get();
    HILOG_INFO("OnKeyPressEvent callbackResult = %{public}d", callbackResult);
    return callbackResult;
}

NativeValue* NAccessibilityExtension::CallObjectMethod(const char* name, NativeValue* const* argv, size_t argc)
{
    HILOG_INFO("name:%{public}s", name);
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
    HILOG_INFO("CallFunction(%{public}s), success", name);
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
