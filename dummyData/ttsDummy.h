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
 
#ifndef OHOS_FWK_TTS_ENGINE_H
#define OHOS_FWK_TTS_ENGINE_H

#include "refbase.h"

namespace OHOS {
namespace TtsFwk {
class TtsListener : public virtual RefBase {
public:
    enum TtsEvent {
        TTS_EVENT_CREATE_SUCCESS = 0,
        TTS_EVENT_CREATE_FAILED,
        TTS_EVENT_DESTROY_SUCCESS,
        TTS_EVENT_DESTROY_FAILED,
    };

    virtual ~TtsListener() = default;
    virtual void OnEvent(int eventType) {}
    virtual void OnSpeechStart(int utteranceId) {}
    virtual void OnSpeechProgress(int utteranceId, int progress) {}
    virtual void OnSpeechFinish(int utteranceId) {}
};

class TtsEngine {
public:
    TtsEngine() = default;
    ~TtsEngine() = default;

    static TtsEngine &GetInstance();
    virtual void Create(sptr<TtsListener> &ttsListener);
    virtual void Destroy();
    virtual void SpeakText(const char *text, int length, int utteranceId);
    virtual void StopSpeak();
};
} // TtsFwk
} // OHOS
#endif // OHOS_FWK_TTS_ENGINE_H