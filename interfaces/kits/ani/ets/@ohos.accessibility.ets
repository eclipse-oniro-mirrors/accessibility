/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export class AccessibilityError extends Error {
    constructor(code: int, message: string) {
        super(message)
        this.code = code;
    }
    code: int;
}

type AccessibilityCallback<T> = (err: AccessibilityError, data?: T) => void;

native function sendAccessibilityEventSync(event: accessibility.EventInfo): int;

let errMap: Map<int, string> = new Map<int, string>();
errMap.set(9300001, 'Invalid bundle name or ability name.');
errMap.set(9300002, 'Target ability already enabled.');
errMap.set(9300003, 'No accessibility permission to perform the operation.');
errMap.set(9300004, 'This property does not exist.');
errMap.set(9300005, 'This action is not supported.');

export namespace accessibility {
    loadLibrary("ACCESSIBILITYNativeModule");

    export type EventType = 'accessibilityFocus' | 'accessibilityFocusClear' |
        'click' | 'longClick' | 'focus' | 'select' | 'hoverEnter' | 'hoverExit' |
        'textUpdate' | 'textSelectionUpdate' | 'scroll' | 'requestFocusForAccessibility' |
        'announceForAccessibility';

    export type Action = 'accessibilityFocus' | 'clearAccessibilityFocus' | 'focus' | 'clearFocus' | 'clearSelection' |
        'click' | 'longClick' | 'cut' | 'copy' | 'paste' | 'select' | 'setText' | 'delete' |
        'scrollForward' | 'scrollBackward' | 'setSelection' | 'setCursorPosition' | 'home' |
        'back' | 'recentTask' | 'notificationCenter' | 'controlCenter' | 'common';

    export type WindowUpdateType = 'add' | 'remove' | 'bounds' | 'active' | 'focus';

    export type TextMoveUnit = 'char' | 'word' | 'line' | 'page' | 'paragraph';

    export type ObserverEventType = 'accessibilityStateChange' | 'touchGuideStateChange' | 'screenReaderStateChange';

    export native function isOpenTouchGuideSync(): boolean;

    export native function isOpenAccessibilitySync(): boolean;

    export native function on(type: ObserverEventType, callback: ((parameter: boolean) => void)): void;

    native function offObserver(type: ObserverEventType, callback: ((parameter: boolean) => void)): void;

    native function offAll(type: ObserverEventType): void;

    export function off (type: ObserverEventType, callback?: ((parameter: boolean) => void)): void {
        if (callback === undefined) {
            offAll(type);
        } else {
            let callbackFunc: ((parameter: boolean) => void) = callback as ((parameter: boolean) => void); 
            offObserver(type, callbackFunc);
        }
    }

    export class EventInfo {
        constructor(type: EventType, bundleName: string, triggerAction: Action) {
            this.type = type;
            this.bundleName = bundleName;
            this.triggerAction = triggerAction;
        }
        type: EventType;
        windowUpdateType?: WindowUpdateType;
        bundleName: string;
        componentType?: string;
        pageId?: number;
        description?: string;
        triggerAction: Action;
        textMoveUnit?: TextMoveUnit;
        contents?: Array<string>;
        lastContent?: string;
        beginIndex?: number;
        currentIndex?: number;
        endIndex?: number;
        itemCount?: number;
        elementId?: number;
        textAnnouncedForAccessibility?: string;
        customId?: string;
    }

    export function sendAccessibilityEvent(event: EventInfo, callback:AccessibilityCallback<void>): void {
        let p1 = taskpool.execute(sendAccessibilityEventSync, event);
        p1.then((code: NullishType) => {
            let codeInt = code as int;
            let message = errMap.get(code as int) ?? '';
            let err: AccessibilityError = new AccessibilityError(codeInt, message);
            callback(err);
        });
    }

    export function sendAccessibilityEvent(event: EventInfo): Promise<void> {
        let p = new Promise<void> ((resolve: (v: undefined) => void, reject: (error: AccessibilityError) => void): void => {
            let p1 = taskpool.execute(sendAccessibilityEventSync, event);
            p1.then((code :NullishType) => {
                let codeInt = code as int;
                let message = errMap.get(code as int) ?? '';
                let err: AccessibilityError = new AccessibilityError(codeInt, message);
                if(err.code != 0){
                    reject(err);
                }
                else{
                    resolve(undefined);
                }
            });
        
        });
        return p;
    }
}