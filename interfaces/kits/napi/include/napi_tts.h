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

#ifndef NAPI_TTS_H
#define NAPI_TTS_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ttsDummy.h"
// #include "tts_engine.h"

namespace OHOS {
class TtsObserver : public TtsFwk::TtsListener {
public:
    TtsObserver() = default;
    ~TtsObserver() = default;

    void OnEvent(int eventType) override;
    void OnSpeechStart(int utteranceId) override;
    void OnSpeechProgress(int utteranceId, int progress) override;
    void OnSpeechFinish(int utteranceId) override;
};

class NTTSClient {
public:
    static napi_value Create(napi_env env, napi_callback_info info);
    static napi_value Release(napi_env env, napi_callback_info info);
    static napi_value InitTTS(napi_env env, napi_callback_info info);
    static napi_value SetParams(napi_env env, napi_callback_info info);
    static napi_value SetAudioType(napi_env env, napi_callback_info info);
    static napi_value SpeakText(napi_env env, napi_callback_info info);
    static napi_value StopSpeak(napi_env env, napi_callback_info info);
    static napi_value Destroy(napi_env env, napi_callback_info info);

    static napi_value ttsClientCons_;
    static napi_env env_;
    static napi_ref onEventRef_;
    static napi_ref onSpeechFinishedRef_;
    static sptr<TtsFwk::TtsListener> listener_;
};
}// namespace OHOS


#endif // NAPI_TTS_H