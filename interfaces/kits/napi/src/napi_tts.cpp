/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <string>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "napi_tts.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
napi_value NTTSClient::ttsClientCons_ = nullptr;
napi_env NTTSClient::env_ = nullptr;
napi_ref NTTSClient::onEventRef_ = nullptr;
napi_ref NTTSClient::onSpeechFinishedRef_ = nullptr;
sptr<TtsFwk::TtsListener> NTTSClient::listener_ = nullptr;

void TtsObserver::OnEvent(int eventType) {
    HILOG_INFO("%{public}s start, eventType[%{public}d]", __func__, eventType);
    int argc = 2;
    napi_value argv[2] = {0};
    napi_value fn;
    napi_value undefined;
    napi_value callResult;

    napi_create_int32(NTTSClient::env_, eventType, &argv[0]);
    napi_create_int32(NTTSClient::env_, 1, &argv[1]);
    napi_get_reference_value(NTTSClient::env_, NTTSClient::onEventRef_, &fn);
    napi_get_undefined(NTTSClient::env_, &undefined);
    napi_call_function(NTTSClient::env_, undefined, fn, argc, argv, &callResult);
}

void TtsObserver::OnSpeechFinish(int utteranceId) {
    HILOG_INFO("%{public}s start, utteranceId[%{public}d]", __func__, utteranceId);
    int argc = 1;
    napi_value argv;
    napi_value fn;
    napi_value undefined;
    napi_value callResult;

    napi_create_int32(NTTSClient::env_, utteranceId, &argv);
    napi_get_reference_value(NTTSClient::env_, NTTSClient::onSpeechFinishedRef_, &fn);
    napi_get_undefined(NTTSClient::env_, &undefined);
    napi_call_function(NTTSClient::env_, undefined, fn, argc, &argv, &callResult);
}

void TtsObserver::OnSpeechStart(int utteranceId) {
}

void TtsObserver::OnSpeechProgress(int utteranceId, int progress) {

}

static std::string GetStringFromNAPI(napi_env env, napi_value value) {
    std::string result;
    size_t size = 0;

    if (napi_get_value_string_utf8(env, value, nullptr, 0, &size) != napi_ok) {
        HILOG_ERROR("can not get string size");
        return "";
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
        HILOG_ERROR("can not get string value");
        return "";
    }
    return result;
}

static bool ParseString(napi_env env, std::string &param, napi_value args) {
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_INFO("napi_typeof error and status is %{public}d", status);
        return false;
    }
    NAPI_ASSERT(env, valuetype == napi_string, "Wrong argument type. String expected.");
    param = GetStringFromNAPI(env, args);
    HILOG_INFO("param=%{public}s.", param.c_str());
    return true;
}

static bool ParseInt32(napi_env env, int32_t &param, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    status = napi_typeof(env, args, &valuetype);
    if (status != napi_ok) {
        HILOG_INFO("napi_typeof error and status is %{public}d", status);
        return false;
    }
    HILOG_INFO("param=%{public}d.", valuetype);
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. int32 expected.");
    napi_get_value_int32(env, args, &param);
    return true;
}

napi_value NTTSClient::Create(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value fnOnEvent;
    napi_value fnOnSpeechFinished;
    napi_get_named_property(env, argv[1], "onEvent", &fnOnEvent);
    napi_create_reference(env, fnOnEvent, 1, &NTTSClient::onEventRef_);
    napi_get_named_property(env, argv[1], "onSpeechFinish", &fnOnSpeechFinished);
    napi_create_reference(env, fnOnSpeechFinished, 1, &NTTSClient::onSpeechFinishedRef_);
    NTTSClient::listener_ = new TtsObserver();
    TtsFwk::TtsEngine::GetInstance().Create(NTTSClient::listener_);
    return ret;
}

napi_value NTTSClient::Release(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    return ret;
}

napi_value NTTSClient::InitTTS(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    napi_get_boolean(env, true, &ret);
    return ret;
}

napi_value NTTSClient::SetParams(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    napi_get_boolean(env, true, &ret);
    return ret;
}

napi_value NTTSClient::SetAudioType(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    napi_get_boolean(env, true, &ret);
    return ret;
}

napi_value NTTSClient::SpeakText(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    std::string text;
    int utteranceId = 0; 
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    ParseString(env, text, argv[0]);
    ParseInt32(env, utteranceId, argv[1]);
    HILOG_INFO("%{public}s start, test[%{public}s], utteranceId[%{public}d]", __func__, text.c_str(), utteranceId);
    TtsFwk::TtsEngine::GetInstance().SpeakText(text.c_str(), text.size(), utteranceId);
    return ret;
}

napi_value NTTSClient::StopSpeak(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    TtsFwk::TtsEngine::GetInstance().StopSpeak();
    return ret;
}

napi_value NTTSClient::Destroy(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value ret = nullptr;
    TtsFwk::TtsEngine::GetInstance().Destroy();
    return ret;
}

napi_value TtsClientConstructor(napi_env env, napi_callback_info info) {
    napi_value jsthis = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
    return jsthis;
}

void DefineJsTTSClient(napi_env env) {
    napi_property_descriptor ttsDesc[] = {
        DECLARE_NAPI_FUNCTION("create", NTTSClient::Create),
        DECLARE_NAPI_FUNCTION("release", NTTSClient::Release),
        DECLARE_NAPI_FUNCTION("init", NTTSClient::InitTTS),
        DECLARE_NAPI_FUNCTION("setParams", NTTSClient::SetParams),
        DECLARE_NAPI_FUNCTION("setAudioType", NTTSClient::SetAudioType),
        DECLARE_NAPI_FUNCTION("speakText", NTTSClient::SpeakText),
        DECLARE_NAPI_FUNCTION("stopSpeak", NTTSClient::StopSpeak),
        DECLARE_NAPI_FUNCTION("destroy", NTTSClient::Destroy),
    };

    NAPI_CALL_RETURN_VOID(env,
        napi_define_class(env,
            "TtsClient",
            NAPI_AUTO_LENGTH,
            TtsClientConstructor,
            nullptr,
            sizeof(ttsDesc) / sizeof(ttsDesc[0]),
            ttsDesc,
            &NTTSClient::ttsClientCons_));
}

napi_value GetTTSClient(napi_env env, napi_callback_info info) {
    HILOG_INFO("%{public}s start", __func__);
    napi_value result;
    napi_new_instance(env, NTTSClient::ttsClientCons_, 0, nullptr, &result);
    return result;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    HILOG_INFO("%{public}s start", __func__);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getInstance", GetTTSClient),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    DefineJsTTSClient(env);
    NTTSClient::env_ = env;

    HILOG_INFO("-----Init end------");
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "media",
    .nm_priv = ((void *)0),
    .reserved = {0}
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
} // namespace OHOS