/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import accessibility from './@ohos.accessibility'
import AccessibilityExtensionContext, { AccessibilityElement } from './application/AccessibilityExtensionContext'
import inputEventClient from './@ohos.multimodalInput.inputEventClient'

export default class AccessibilityExtensionAbility {
    context: AccessibilityExtensionContext;
    onConnect(): void;
    onDisconnect(): void;
    onAccessibilityEvent(event: AccessibilityEvent): void;
    onKeyEvent(keyEvent: inputEventClient.KeyEvent): boolean;
}

declare interface AccessibilityEvent {
    eventType: accessibility.EventType | accessibility.WindowUpdateType |
        TouchGuideType | GestureType | PageUpdateType;
    target?: AccessibilityElement;
    timeStamp?: number;
}

type GestureType = 'left' | 'leftThenRight' | 'leftThenUp' | 'leftThenDown' |
    'right' | 'rightThenLeft' | 'rightThenUp' | 'rightThenDown' |
    'up' | 'upThenLeft' | 'upThenRight' | 'upThenDown' |
    'down' | 'downThenLeft' | 'downThenRight' | 'downThenUp';

type PageUpdateType = 'pageContentUpdate' | 'pageStateUpdate';

type TouchGuideType = 'touchBegin' | 'touchEnd';