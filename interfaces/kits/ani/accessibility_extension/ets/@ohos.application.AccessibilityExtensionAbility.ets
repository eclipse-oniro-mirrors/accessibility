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
import AccessibilityExtensionContext from 'application.AccessibilityExtensionContext';
import AccessibilityElement from 'application.AccessibilityExtensionContext';
import { KeyEvent } from '@ohos.multimodalInput.keyEvent';
import { Key } from '@ohos.multimodalInput.keyEvent';
import { Action } from '@ohos.multimodalInput.keyEvent';
import { AccessibilityEventType } from '@ohos.accessibility';
import { KeyCode } from '@ohos.multimodalInput.keyCode';

export class AccessibilityExtensionAbility {
    context: AccessibilityExtensionContext = new AccessibilityExtensionContext();
    onAccessibilityConnect() {
        console.log('onAccessibilityConnect');
    }

    onAccessibilityDisconnect() {
        console.log('onAccessibilityDisconnect');
    }

    onAccessibilityEventInfo(event: AccessibilityEventInfoImpl): void {
        console.log('onAccessibilityEventInfo');
    }
}

 interface AccessibilityEventInfo {
    eventType: AccessibilityEventType;
    target?: AccessibilityElement;
    timestamp?: long;
    extraInfo?: string;
}

export class AccessibilityEventInfoImpl implements AccessibilityEventInfo {
    eventType: AccessibilityEventType = AccessibilityEventType.TYPE_ACCESSIBILITY_FOCUS;
    target?: AccessibilityElement = new AccessibilityElement();
    timestamp?: long = 0;
    extraInfo?: string = '';
}

export class KeyEventImpl implements KeyEvent {
    action: Action = Action.CANCEL;
    key: Key = new KeyImpl();
    unicodeChar: int = 0;
    keys: Key[] = {};
    ctrlKey: boolean = true;
    altKey: boolean = true;
    shiftKey: boolean = true;
    logoKey: boolean = true;
    fnKey: boolean = true;
    capsLock: boolean = true;
    numLock: boolean = true;
    scrollLock: boolean = true;
    id: int = 0;
    deviceId: int = 0;
    actionTime: long = 0;
    screenId: int = 0;
    windowId: int = 0;
}

export class KeyImpl implements Key {
    code: KeyCode = KeyCode.KEYCODE_FN;
    pressedTime: long = 0;
    deviceId: int = 0;
}
