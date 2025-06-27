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

import { BusinessError, AsyncCallback } from '@ohos.base';

native function sendAccessibilityEventSync(event: accessibility.EventInfo): void;

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

    export function sendAccessibilityEvent(event: EventInfo, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            return sendAccessibilityEventSync(event);
        })
        .then((ret: NullishType): void => {
            callback(new BusinessError(), ret as undefined);
        })
        .catch((ret: NullishType): void => {
            callback(ret as BusinessError, undefined);
        });
    }

    export function sendAccessibilityEvent(event: EventInfo): Promise<void> {
        return new Promise<void>((resolve, reject): void => {
            taskpool.execute((): void => {
                return sendAccessibilityEventSync(event);
            })
            .then((ret: NullishType): void => {
                resolve(ret as undefined);
            })
            .catch((ret: NullishType): void => {
                reject(ret as BusinessError);
            });
        });
    }
}