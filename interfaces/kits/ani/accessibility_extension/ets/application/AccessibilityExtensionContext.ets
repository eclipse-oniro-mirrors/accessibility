/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import ExtensionContext from 'application.ExtensionContext';
import Want from '@ohos.app.ability.Want';
import { AsyncCallback, BusinessError, Callback } from '@ohos.base';
import accessibility from '@ohos.accessibility';

export default class AccessibilityExtensionContext extends ExtensionContext {
    private nativePtr: long = 0;

    constructor() {
        super();
        console.log('STS AccessibilityExtensionContext constructor');
    }

    startAbility(want: Want): Promise<void> {
        console.log('ani AccessibilityExtensionContext startAbility Promise');
        return taskpool.execute(this.startAbilityNative, want).then(() => {
            console.log('ani AccessibilityExtensionContext startAbility Promise success');
        }).catch((err: Error) => {
            console.log('ani AccessibilityExtensionContext startAbility Promise catch');
            let error = err as BusinessError;
            throw error;
        });
    }

    getElements(windowId: int, elementId?: long): Promise<Array<AccessibilityElement>> {
        console.log("getElements begin");
        return new Promise<Array<AccessibilityElement>>((resolve: (data: Array<AccessibilityElement>) => void,
            reject: (err: BusinessError) => void): void => {
            taskpool.execute(this.getElementsNative, windowId, elementId).then((code) => {
                console.log('resolve getElements');
                resolve(code as Array<AccessibilityElement>);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    getDefaultFocusedElementIds(windowId: int): Promise<Array<long>> {
        console.log("getDefaultFocusedElementIds begin");
        return new Promise<Array<long>>((resolve: (data: Array<long>) => void,
            reject: (err: BusinessError) => void): void => {
            taskpool.execute(this.getDefaultFocusedElementIdsNative, windowId).then((code) => {
                console.log('resolve getDefaultFocusedElementIds');
                resolve(code as Array<long>);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    getRootInActiveWindow(windowId?: int): Promise<AccessibilityElement> {
        console.log("getRootInActiveWindow " + windowId);
        return new Promise<AccessibilityElement>((resolve: (data: AccessibilityElement) => void,
            reject: (err: BusinessError) => void): void => {
            taskpool.execute(this.getRootInActiveWindowNative, windowId).then((code) => {
                console.log("resolve getRootInActiveWindow " + windowId);
                resolve(code as AccessibilityElement);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    native startAbilityNative(want: Want): void;
    native getElementsNative(windowId: int, elementId?: long): Array<AccessibilityElement>;
    native getDefaultFocusedElementIdsNative(windowId: int): Array<long>;
    native getRootInActiveWindowNative(windowId?: int): AccessibilityElement;
}

class AccessibilityElementCleaner {
    private static readonly registry = new FinalizationRegistry<AccessibilityElementCleaner>((cleaner) => {
        console.log('STS AccessibilityExtensionElement cleaner works');
        cleaner.cleanNative();
    });

    private static readonly unregisterToken = new object();

    private nativePtr: long;

    constructor(ptr: long, owner: object) {
        this.nativePtr = ptr;
        AccessibilityElementCleaner.registry.register(owner, this, AccessibilityElementCleaner.unregisterToken);
        console.log('STS AccessibilityExtensionElement cleaner constructor');
    }

    static unregister(owner: object): void {
        AccessibilityElementCleaner.registry.unregister(AccessibilityElementCleaner.unregisterToken);
    }

    native cleanNative(): void
}

interface AccessibilityElement {
  getCursorPosition(callback: AsyncCallback<int>): void;
  getCursorPosition(): Promise<int>;
  enableScreenCurtain(isEnable: boolean): void;
  findElementByTextType(condition: string): Promise<Array<AccessibilityElement>>;
  findElementByElementId(condition: long): Promise<AccessibilityElement>;
  mainWindowId?: int;
}

export class AccessibilityElementImpl implements AccessibilityElement {
    private nativePtr: long = 0;
    private cleaner: AccessibilityElementCleaner | null = null;
    mainWindowId?: int = 0;

    constructor() {
        super();
        this.cleaner = new AccessibilityElementCleaner(this.nativePtr, this);
        console.log('STS AccessibilityElement constructor');
    }

    getCursorPosition(callback: AsyncCallback<int>): void {
        console.log("getCursorPosition begin");
        let p1 = taskpool.execute(this.getCursorPositionNative, this) as Promise<int>;
        p1.then((code: int) => {
            console.log("getCursorPosition callback" + code);
            callback(null, code);
        }).catch((err: Error) => {
            callback(err as BusinessError, undefined);
        });
    }

    getCursorPosition(): Promise<int> {
        console.log("getCursorPosition begin ");
        return new Promise<int>((resolve: (data: int) => void, reject: (err: BusinessError) => void): void => {
            let p1 = taskpool.execute(this.getCursorPositionNative, this) as Promise<int>;
            p1.then((code: int) => {
                console.log("resolve getCursorPosition " + code);
                resolve(code);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    enableScreenCurtain(isEnable: boolean): void {
        console.log("enableScreenCurtain " + isEnable);
        this.enableScreenCurtainNative(isEnable);
    }

    findElementByTextType(condition: string): Promise<Array<AccessibilityElement>> {
        console.log("findElement by textType");
        return new Promise<Array<AccessibilityElement>>((resolve: (data: Array<AccessibilityElement>) => void,
            reject: (err: BusinessError) => void): void => {
            let p1 = taskpool.execute(this.findElementsNative, 'textType', condition) as Promise<Array<AccessibilityElement>>;
            p1.then((code: Array<AccessibilityElement>) => {
                console.log("resolve findElement size " + code.length);
                resolve(code);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    findElementByElementId(condition: long): Promise<AccessibilityElement> {
        console.log("findElement by elementId");
        return new Promise<AccessibilityElement>((resolve: (data: AccessibilityElement) => void,
            reject: (err: BusinessError) => void): void => {
            let p1 = taskpool.execute(this.findElementNative, 'elementId', condition) as Promise<AccessibilityElement>;
            p1.then((code: AccessibilityElement) => {
                console.log("resolve findElement");
                resolve(code);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    native getCursorPositionNative(): int;
    native enableScreenCurtainNative(isEnable: boolean): void;
    native findElementNative(type: string, condition: long): AccessibilityElement;
    native findElementsNative(type: string, condition: string): Array<AccessibilityElement>;
}

export type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';

export type WindowType = 'application' | 'system';

interface Rect {
  left: int;
  top: int;
  width: int;
  height: int;
}

export class RectImpl implements Rect {
    left: int = 0;
    top: int = 0;
    width: int = 0;
    height: int = 0;
}
