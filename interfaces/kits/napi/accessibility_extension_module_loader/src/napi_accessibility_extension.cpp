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
#include "accessibility_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_accessibility_element.h"

using namespace OHOS::AbilityRuntime;
using namespace OHOS::AccessibilityNapi;

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int64_t VIRTUAL_COMPONENT_ID = -1;
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
    env_ = jsRuntime_.GetNapiEnv();
}

NAccessibilityExtension::~NAccessibilityExtension()
{
    jsRuntime_.FreeNativeReference(std::move(jsObj_));
}

napi_handle_scope OpenScope(napi_env env)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL(env, napi_open_handle_scope(env, &scope));
    return scope;
}

void NAccessibilityExtension::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
    std::shared_ptr<AppExecFwk::AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO();
    AccessibilityExtension::Init(record, application, handler, token);
    std::string srcPath = "";
    std::string moduleName = "";
    if (!GetSrcPathAndModuleName(srcPath, moduleName)) {
        return;
    }
    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == CompileMode::ES_MODULE);
    if (!jsObj_) {
        HILOG_ERROR("Failed to get jsObj_");
        return;
    }
    napi_value obj = jsObj_->GetNapiValue();
    if (!obj) {
        HILOG_ERROR("Failed to get NAccessibilityExtension object");
        return;
    }

    auto context = GetContext();
    if (!context) {
        HILOG_ERROR("Failed to get context");
        return;
    }
    napi_value contextObj = CreateJsAccessibilityExtensionContext(env_, context);
    auto shellContextRef = jsRuntime_.LoadSystemModule("application.AccessibilityExtensionContext", &contextObj, 1);
    if (!shellContextRef) {
        HILOG_ERROR("shellContextRef is nullptr.");
        return;
    }
    contextObj = shellContextRef->GetNapiValue();
    context->Bind(jsRuntime_, shellContextRef.release());
    napi_set_named_property(env_, obj, "context", contextObj);

    if (!contextObj) {
        HILOG_ERROR("Failed to get accessibility extension native object");
        return;
    }
    napi_wrap(env_, contextObj, new std::weak_ptr<AbilityRuntime::Context>(context),
        [](napi_env env, void* data, void*) {
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr, nullptr);
    NAccessibilityElement::DefineJSAccessibilityElement(env_);
}

bool NAccessibilityExtension::GetSrcPathAndModuleName(std::string& srcPath, std::string& moduleName)
{
    if (!Extension::abilityInfo_) {
        HILOG_ERROR("abilityInfo_ is nullptr");
        return false;
    }
    if (!Extension::abilityInfo_->isModuleJson) {
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
    } else if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    } else {
        HILOG_ERROR("Failed to get srcPath");
        return false;
    }
    moduleName = Extension::abilityInfo_->moduleName;
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("moduleName:%{public}s, srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());
    return true;
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
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        HILOG_ERROR("loop is nullptr.");
        return;
    }
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

    int ret = uv_queue_work_with_qos(
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
        },
        uv_qos_user_initiated);
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
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        HILOG_ERROR("loop is nullptr.");
        return;
    }
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
    std::future syncFuture = callbackInfo->syncPromise_.get_future();

    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            ExtensionCallbackInfo *data = static_cast<ExtensionCallbackInfo*>(work->data);
            data->extension_->CallObjectMethod("onDisconnect");
            data->syncPromise_.set_value();
            delete data;
            data = nullptr;
            delete work;
            work = nullptr;
        },
        uv_qos_user_initiated);
    if (ret) {
        HILOG_ERROR("Failed to execute OnAbilityDisconnected work queue");
        callbackInfo->syncPromise_.set_value();
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
    syncFuture.get();
    HILOG_INFO("end.");
}

std::shared_ptr<AccessibilityElement> NAccessibilityExtension::GetElement(const AccessibilityEventInfo& eventInfo)
{
    HILOG_DEBUG();

    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        return nullptr;
    }
    int64_t componentId = eventInfo.GetAccessibilityId();
    int32_t windowId = eventInfo.GetWindowId();
    std::shared_ptr<AccessibilityElement> element = nullptr;
    if (componentId > 0) {
        std::shared_ptr<AccessibilityElementInfo> elementInfo =
            std::make_shared<AccessibilityElementInfo>(eventInfo.GetElementInfo());
        element = std::make_shared<AccessibilityElement>(elementInfo);
    } else if (windowId > 0) {
        std::shared_ptr<AccessibilityWindowInfo> windowInfo = std::make_shared<AccessibilityWindowInfo>();
        if (aaClient->GetWindow(windowId, *windowInfo) == RET_OK) {
            element = std::make_shared<AccessibilityElement>(windowInfo);
        }
    } else {
        std::shared_ptr<AccessibilityElementInfo> elementInfo = std::make_shared<AccessibilityElementInfo>();
        std::string inspectorKey = eventInfo.GetInspectorKey();
        if (eventInfo.GetEventType() == TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY && inspectorKey != "") {
            AccessibilityElementInfo accessibilityElementInfo;
            if (aaClient->SearchElementInfoByInspectorKey(inspectorKey, accessibilityElementInfo) == RET_OK) {
                elementInfo = std::make_shared<AccessibilityElementInfo>(accessibilityElementInfo);
            }
        }
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
    if (elementInfo->GetAccessibilityId() < 0) {
        elementInfo->SetComponentId(VIRTUAL_COMPONENT_ID);
    }
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
    auto closeScope = [env](napi_handle_scope scope) { napi_close_handle_scope(env, scope); };
    std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(OpenScope(env), closeScope);
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

napi_status SetNapiEventInfoIntProperty(napi_env env, const char *property, int64_t value, napi_value &napiEventInfo)
{
    if (property == nullptr) {
        HILOG_ERROR("property is null");
        return napi_invalid_arg;
    }
    napi_value nValue;
    napi_status status = napi_create_int64(env, value, &nValue);
    if (status != napi_ok) {
        return status;
    }
    return napi_set_named_property(env, napiEventInfo, property, nValue);
}

napi_status SetEventInfoStrProperty(napi_env env, const char *property, std::string &value, napi_value &napiEventInfo)
{
    if (property == nullptr) {
        HILOG_ERROR("property is null");
        return napi_invalid_arg;
    }
    napi_value nValue;
    napi_status status = napi_create_string_utf8(env, value.c_str(), NAPI_AUTO_LENGTH, &nValue);
    if (status != napi_ok) {
        HILOG_ERROR();
        return status;
    }
    return napi_set_named_property(env, napiEventInfo, property, nValue);
}

int NAccessibilityExtension::OnAccessibilityEventExec(uv_work_t *work, uv_loop_t *loop)
{
    if (loop == nullptr || work == nullptr) {
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            AccessibilityEventInfoCallbackInfo *data = static_cast<AccessibilityEventInfoCallbackInfo*>(work->data);
            napi_env env = data->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(OpenScope(data->env_), closeScope);
            napi_value napiEventInfo = nullptr;
            napi_create_object(data->env_, &napiEventInfo);

            do {
                if (SetEventInfoStrProperty(data->env_, "eventType", data->eventType_, napiEventInfo) != napi_ok) {
                    GET_AND_THROW_LAST_ERROR((data->env_));
                    break;
                }
                if (SetNapiEventInfoIntProperty(data->env_, "timeStamp", data->timeStamp_, napiEventInfo) != napi_ok) {
                    GET_AND_THROW_LAST_ERROR((data->env_));
                    break;
                }
                if (SetNapiEventInfoIntProperty(data->env_, "elementId", data->elementId_, napiEventInfo) != napi_ok) {
                    GET_AND_THROW_LAST_ERROR((data->env_));
                    break;
                }
                if (SetEventInfoStrProperty(data->env_, "textAnnouncedForAccessibility",
                    data->textAnnouncedForAccessibility_, napiEventInfo) != napi_ok) {
                    GET_AND_THROW_LAST_ERROR((data->env_));
                    break;
                }

                ConvertAccessibilityElementToJS(data->env_, napiEventInfo, data->element_);
                napi_value argv[] = {napiEventInfo};
                data->extension_->CallObjectMethod("onAccessibilityEvent", argv, 1);
            } while (0);
            if (data != nullptr) {
                delete data;
                data = nullptr;
            }
            if (work != nullptr) {
                delete work;
                work = nullptr;
            }
        },
        uv_qos_user_initiated);
    return ret;
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
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    AccessibilityEventInfoCallbackInfo *callbackInfo = new(std::nothrow) AccessibilityEventInfoCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return;
    }
    std::shared_ptr<AccessibilityElement> element = GetElement(eventInfo);
    callbackInfo->env_ = env_;
    callbackInfo->extension_ = this;
    callbackInfo->eventType_ = strType;
    callbackInfo->timeStamp_ = eventInfo.GetTimeStamp();
    callbackInfo->element_ = element;
    callbackInfo->elementId_ = eventInfo.GetRequestFocusElementId();
    callbackInfo->textAnnouncedForAccessibility_ = eventInfo.GetTextAnnouncedForAccessibility();
    uv_work_t *work = new(std::nothrow) uv_work_t;
    if (!work) {
        HILOG_ERROR("Failed to create data.");
        delete callbackInfo;
        callbackInfo = nullptr;
        return;
    }
    work->data = static_cast<void*>(callbackInfo);

    if (OnAccessibilityEventExec(work, loop)) {
        HILOG_ERROR("Failed to execute OnAccessibilityEvent work queue");
        delete callbackInfo;
        callbackInfo = nullptr;
        delete work;
        work = nullptr;
    }
}

void NAccessibilityExtension::OnAccessibilityEventCompleteCallback(uv_work_t* work, int status)
{
    AccessibilityEventInfoCallbackInfo *data = static_cast<AccessibilityEventInfoCallbackInfo*>(work->data);
    napi_env env = data->env_;
    auto closeScope = [env](napi_handle_scope scope) {
        napi_close_handle_scope(env, scope);
    };
    std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(OpenScope(data->env_), closeScope);
    napi_value napiEventInfo = nullptr;
    napi_create_object(data->env_, &napiEventInfo);

    napi_value nType;
    NAPI_CALL_RETURN_VOID(data->env_, napi_create_string_utf8(data->env_, data->eventType_.c_str(),
        NAPI_AUTO_LENGTH, &nType));
    NAPI_CALL_RETURN_VOID(data->env_, napi_set_named_property(data->env_, napiEventInfo, "eventType", nType));
    HILOG_DEBUG("eventType[%{public}s]", data->eventType_.c_str());

    napi_value nTimeStamp;
    NAPI_CALL_RETURN_VOID(data->env_, napi_create_int64(data->env_, data->timeStamp_, &nTimeStamp));
    NAPI_CALL_RETURN_VOID(data->env_, napi_set_named_property(data->env_, napiEventInfo, "timeStamp", nTimeStamp));

    ConvertAccessibilityElementToJS(data->env_, napiEventInfo, data->element_);
    napi_value argv[] = {napiEventInfo};
    data->extension_->CallObjectMethod("onAccessibilityEvent", argv, 1);
    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

int NAccessibilityExtension::OnKeyPressEventExec(uv_work_t *work, uv_loop_t *loop)
{
    if (loop == nullptr || work == nullptr) {
        HILOG_ERROR("loop or work is nullptr.");
        return RET_ERR_FAILED;
    }
    int ret = uv_queue_work_with_qos(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            KeyEventCallbackInfo *data = static_cast<KeyEventCallbackInfo*>(work->data);
            napi_env env = data->env_;
            auto closeScope = [env](napi_handle_scope scope) {
                napi_close_handle_scope(env, scope);
            };
            std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(
                OpenScope(data->env_), closeScope);
            napi_value napiEventInfo = nullptr;
            if (napi_create_object(data->env_, &napiEventInfo) != napi_ok) {
                HILOG_ERROR("Create keyEvent object failed.");
                data->syncPromise_.set_value(false);
                delete data;
                data = nullptr;
                delete work;
                work = nullptr;
                return;
            }
            ConvertKeyEventToJS(data->env_, napiEventInfo, data->keyEvent_);
            napi_value argv[] = {napiEventInfo};
            napi_value nativeResult = data->extension_->CallObjectMethod("onKeyEvent", argv, 1);

            // Unwrap result
            bool result = false;
            if (!ConvertFromJsValue(data->env_, nativeResult, result)) {
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
        },
        uv_qos_user_initiated);
    return ret;
}

bool NAccessibilityExtension::OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    HILOG_INFO();
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    KeyEventCallbackInfo *callbackInfo = new(std::nothrow) KeyEventCallbackInfo();
    if (!callbackInfo) {
        HILOG_ERROR("Failed to create callbackInfo.");
        return false;
    }
    callbackInfo->env_ = env_;
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

    if (OnKeyPressEventExec(work, loop)) {
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

void NAccessibilityExtension::OnKeyPressEventCompleteCallback(uv_work_t* work, int status)
{
    KeyEventCallbackInfo *data = static_cast<KeyEventCallbackInfo*>(work->data);
    napi_env env = data->env_;
    auto closeScope = [env](napi_handle_scope scope) {
        napi_close_handle_scope(env, scope);
    };
    std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scopes(OpenScope(data->env_), closeScope);
    napi_value napiEventInfo = nullptr;
    if (napi_create_object(data->env_, &napiEventInfo) != napi_ok) {
        HILOG_ERROR("Create keyEvent object failed.");
        data->syncPromise_.set_value(false);
        delete data;
        data = nullptr;
        delete work;
        work = nullptr;
        return;
    }
    ConvertKeyEventToJS(data->env_, napiEventInfo, data->keyEvent_);
    napi_value argv[] = {napiEventInfo};
    napi_value napiResult = data->extension_->CallObjectMethod("onKeyEvent", argv, 1);

    // Unwrap result
    bool result = false;
    if (!ConvertFromJsValue(data->env_, napiResult, result)) {
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
}

napi_value NAccessibilityExtension::CallObjectMethod(const char* name, napi_value* argv, size_t argc)
{
    HILOG_INFO("name:%{public}s", name);
    if (!jsObj_) {
        HILOG_ERROR("jsObj_ is nullptr");
        return nullptr;
    }

    napi_value obj = jsObj_->GetNapiValue();
    if (!obj) {
        HILOG_ERROR("Failed to get AccessibilityExtension object");
        return nullptr;
    }

    napi_value method = nullptr;
    if (napi_get_named_property(env_, obj, name, &method) != napi_ok) {
        HILOG_ERROR("Failed to get '%{public}s' from AccessibilityExtension object", name);
        return nullptr;
    }

    napi_value result = nullptr;
    if (napi_call_function(env_, obj, method, argc, argv, &result) != napi_ok) {
        HILOG_ERROR("call function failed");
        return nullptr;
    }

    return result;
}
} // namespace Accessibility
} // namespace OHOS