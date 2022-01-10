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

declare namespace tts {

  function getInstance(): TtsClient;

  interface TtsClient {

    create(context, ttsListener): void;

    release(): void;  //不用实现，有接口就可以

    init(ttsParams): boolean; //不用实现，有接口就可以，返回true

    setParams(ttsParams):boolean; //不用实现，有接口就可以，返回true

    setAudioType(audioType): boolean; //不用实现，有接口就可以，返回true

    speakText(text, utteranceId): void;

    isSpeaking(): boolean;

    stopSpeak(): void;

    destroy(): void;
  }
}

export default media;