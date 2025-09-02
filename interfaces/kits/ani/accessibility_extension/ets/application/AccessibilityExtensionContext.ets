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

    /**
     * Constructor for ANI AccessibilityExtensionContext
     * @param nativeObj - Native object pointer
     */
    constructor() {
        super();
        console.log('STS AccessibilityExtensionContext constructor');
    }

    /**
     * Implementation of startAbility method with Promise
     * Starts a new ability using ANI native implementation
     *
     * @param want - Indicates the ability to start
     * @returns Promise<void>
     */
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

    /**
     * Get all subElements in window.
     * @param windowId - Indicates the window ID.
     * @param elementId - Indicates the elementId.
     * @returns Promise<Array<AccessibilityElement>>
     */
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

    /**
     * Implementation of getDefaultFocusedElementIds method
     * Gets the default focused element IDs for a specific window using ANI native implementation
     *
     * @param windowId - The window ID to query
     * @returns Promise<Array<long>> - Array of element IDs
     */
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

    /**
     * Native method declarations for ANI implementation
     * These methods are implemented in C++ and called through ANI bridge
     */
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
  /**
   * Get the position of cursor in TextInput.
   *
   * @param { AsyncCallback<int> } callback Indicates the listener.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  getCursorPosition(callback: AsyncCallback<int>): void;

  /**
   * Get the position of cursor in TextInput.
   *
   * @returns { Promise<int> }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  getCursorPosition(): Promise<int>;

  /**
   * Set the screen curtain enable or disable.
   *
   * @param { boolean } isEnable Indicates whether the screen curtain is enabled.
   * @throws { BusinessError } 202 - Permission verification failed. A non-system application calls a system API.
   * @throws { BusinessError } 401 - Input parameter error. Possible causes:
   *     1. Mandatory parameters are left unspecified;
   *     2. Incorrect parameter types;
   *     3. Parameter verification failed.
   * @throws { BusinessError } 9300003 -  No accessibility permission to perform the operation.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  enableScreenCurtain(isEnable: boolean): void;

  /**
   * Find elements that match the condition.
   *
   * @param { 'textType' } type The type of query condition is text type.
   * @param { string } condition Indicates the specific content to be queried.
   * @returns { Promise<Array<AccessibilityElement>> }
   * @throws { BusinessError } 401 - Input parameter error. Possible causes:
   *     1. Mandatory parameters are left unspecified;
   *     2. Incorrect parameter types;
   *     3. Parameter verification failed.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  findElement(type: 'textType', condition: string): Promise<Array<AccessibilityElement>>;

  /**
   * Find elements that match the condition.
   *
   * @param { 'elementId' } type The type of query condition is element id.
   * @param { long } condition Indicates the specific content to be queried.
   * @returns { Promise<AccessibilityElement> }
   * @throws { BusinessError } 401 - Input parameter error. Possible causes:
   *     1. Mandatory parameters are left unspecified;
   *     2. Incorrect parameter types;
   *     3. Parameter verification failed.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  findElement(type: 'elementId', condition: long): Promise<AccessibilityElement>;

  /**
   * Indicates the main windowId of component.
   *
   * @type { ?int }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  mainWindowId?: int;

}

/**
 * Indicates an accessibility element.
 * Supports querying element attributes, requesting execution actions, and finding child elements by condition.
 */
export class AccessibilityElementImpl implements AccessibilityElement {
    private nativePtr: long = 0;
    private cleaner: AccessibilityElementCleaner | null = null;
    mainWindowId?: int = 0;

    /**
     * Constructor for AccessibilityElement
     * @param nativeObj - Native object pointer
     */
    constructor() {
        super();
        this.cleaner = new AccessibilityElementCleaner(this.nativePtr, this);
        console.log('STS AccessibilityElement constructor');
    }

    /**
    * Get the position of cursor in TextInput.
    *
    * @param { AsyncCallback<int> } callback Indicates the listener.
    */
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

    /**
    * Get the position of cursor in TextInput.
    *
    * @returns { Promise<int> }
    */
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

    /**
    * Set the screen curtain enable or disable.
    *
    * @param { boolean } isEnable Indicates whether the screen curtain is enabled.
    * @throws { BusinessError } 202 - Permission verification failed. A non-system application calls a system API.
    * @throws { BusinessError } 401 - Input parameter error. Possible causes:
    *     1. Mandatory parameters are left unspecified;
    *     2. Incorrect parameter types;
    *     3. Parameter verification failed.
    * @throws { BusinessError } 9300003 -  No accessibility permission to perform the operation.
    */
    enableScreenCurtain(isEnable: boolean): void {
        console.log("enableScreenCurtain " + isEnable);
        this.enableScreenCurtainNative(isEnable);
    }

    /**
    * Find elements that match the condition.
    *
    * @param { 'textType' } type The type of query condition is text type.
    * @param { string } condition Indicates the specific content to be queried.
    * @returns { Promise<Array<AccessibilityElement>> }
    * @throws { BusinessError } 401 - Input parameter error. Possible causes:
    *     1. Mandatory parameters are left unspecified;
    *     2. Incorrect parameter types;
    *     3. Parameter verification failed.
    */
    findElement(type: 'textType', condition: string): Promise<Array<AccessibilityElement>> {
        console.log("findElement " + type);
        return new Promise<Array<AccessibilityElement>>((resolve: (data: Array<AccessibilityElement>) => void,
            reject: (err: BusinessError) => void): void => {
            let p1 = taskpool.execute(this.findElementsNative, type, condition) as Promise<Array<AccessibilityElement>>;
            p1.then((code: Array<AccessibilityElement>) => {
                console.log("resolve findElement size " + code.length);
                resolve(code);
            }).catch((err: Error) => {
                reject(err as BusinessError);
            });
        });
    }

    /**
    * Find elements that match the condition.
    *
    * @param { 'elementId' } type The type of query condition is element id.
    * @param { long } condition Indicates the specific content to be queried.
    * @returns { Promise<AccessibilityElement> }
    * @throws { BusinessError } 401 - Input parameter error. Possible causes:
    *     1. Mandatory parameters are left unspecified;
    *     2. Incorrect parameter types;
    *     3. Parameter verification failed.
    */
    findElement(type: 'elementId', condition: long): Promise<AccessibilityElement> {
        console.log("findElement " + type);
        return new Promise<AccessibilityElement>((resolve: (data: AccessibilityElement) => void,
            reject: (err: BusinessError) => void): void => {
            let p1 = taskpool.execute(this.findElementNative, type, condition) as Promise<AccessibilityElement>;
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

/**
 * Indicates the possible attributes of the element and the type of the attribute value.
 *
 * @typedef ElementAttributeValues
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since arkts {'1.1':'11', '1.2':'20'}
 * @arkts 1.1&1.2
 */
interface ElementAttributeValues {
  /**
   * Indicates accessibility focus state.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  accessibilityFocused: boolean;
  /**
   * Indicates the bundle name to which it belongs.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  bundleName: string;
  /**
   * Indicates whether the element is checkable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  checkable: boolean;
  /**
   * Indicates whether the element is checked.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  checked: boolean;
  /**
   * Indicates all child elements.
   *
   * @type {Array<AccessibilityElement>}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  children: Array<AccessibilityElement>;
  /**
   * Indicates whether the element is clickable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  clickable: boolean;
  /**
   * Indicates the component ID to which the element belongs.
   *
   * @type {long}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  componentId: long;
  /**
   * Indicates the component type to which the element belongs.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  componentType: string;
  /**
   * Indicates the content.
   *
   * @type {Array<string>}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  contents: Array<string>;
  /**
   * Indicates the index of the current item.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  currentIndex: int;
  /**
   * Indicates the description of the element.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  description: string;
  /**
   * Indicates whether the element is editable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  editable: boolean;
  /**
   * Indicates the list index of the last item displayed on the screen.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  endIndex: int;
  /**
   * Indicates the string of error state.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  error: string;
  /**
   * Indicates whether the element is focusable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  focusable: boolean;
  /**
   * Indicates the hint text.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  hintText: string;
  /**
   * Indicates the type of input text.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  inputType: int;
  /**
   * Indicates the inspector key.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  inspectorKey: string;
  /**
   * Indicates whether the element is active or not.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isActive: boolean;
  /**
   * Indicates whether the element is enable or not.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isEnable: boolean;
  /**
   * Indicates whether the element is hint state or not.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isHint: boolean;
  /**
   * Indicates whether the element is focused or not.
   * 
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isFocused: boolean;
  /**
   * Indicates whether the element is password or not.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isPassword: boolean;
  /**
   * Indicates whether the element is visible or not.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  isVisible: boolean;
  /**
   * Indicates the total count of the items.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  itemCount: int;
  /**
   * Indicates the last content.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  lastContent: string;
  /**
   * Indicates the display layer of the element.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  layer: int;
  /**
   * Indicates whether the element is long clickable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  longClickable: boolean;
  /**
   * Indicates the page id.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  pageId: int;
  /**
   * Indicates the parent of the element.
   *
   * @type {AccessibilityElement}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  parent: AccessibilityElement;
  /**
   * Indicates whether the element supports multiple lines of text.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  pluralLineSupported: boolean;
  /**
   * Indicates the area of the element.
   *
   * @type {Rect}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  rect: Rect;
  /**
   * Indicates the resource name of the element.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  resourceName: string;
  /**
   * Indicates the root element of the window element.
   *
   * @type {AccessibilityElement}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  rootElement: AccessibilityElement;
  /**
   * Indicates the display area of the element.
   *
   * @type {Rect}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  screenRect: Rect;
  /**
   * Indicates whether the element is scrollable.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  scrollable: boolean;
  /**
   * Indicates whether the element is selected.
   *
   * @type {boolean}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  selected: boolean;
  /**
   * Indicates the list index of the first item displayed on the screen.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  startIndex: int;
  /**
   * Indicates the text of the element.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  text: string;
  /**
   * Indicates the maximum length limit of the element text.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  textLengthLimit: int;
  /**
   * Indicates the unit of movement of the element text as it is read.
   *
   * @type {accessibility.TextMoveUnit}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  textMoveUnit: accessibility.TextMoveUnit;
  /**
   * Indicates the action that triggered the element event.
   *
   * @type {accessibility.Action}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  triggerAction: accessibility.Action;
  /**
   * Indicates the window type of the element.
   *
   * @type {WindowType}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  type: WindowType;
  /**
   * Indicates the maximum value.
   *
   * @type {double}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  valueMax: double;
  /**
   * Indicates the minimum value.
   *
   * @type {double}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  valueMin: double;
  /**
   * Indicates the current value.
   * 
   * @type {double}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  valueNow: double;
  /**
   * Indicates the window id.
   *
   * @type {int}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  windowId: int;
  /**
   * Indicates the offset.
   *
   * @type {double}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  offset: double;
  /**
   * Indicates the text type.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  textType: string;
  /**
   * Indicates the accessibility text of component.
   *
   * @type {string}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  accessibilityText: string;
  /**
   * Indicates the hot area of the element.
   *
   * @type {Rect}
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'12', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  hotArea: Rect;
  /**
   * Indicates the component next accessibility focus id.
   * @type { ?long  }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'18', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  accessibilityNextFocusId?: long ;
  /**
   * Indicates the custom component type.
   *
   * @type { ?string }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'18', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  customComponentType?: string;
  /**
   * Indicates the extra info of the element.
   * @type { ?string }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'18', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  extraInfo?: string;
  /**
   * Indicates the component Previous accessibility focus id.
   *
   * @type { ?long  }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'18', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  accessibilityPreviousFocusId?: long ;
  /**
   * Indicates whether the element is scrollable for accessibility.
   *
   * @type { ?boolean }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'18', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  accessibilityScrollable?: boolean;
}

/**
 * Indicates the possible attributes of the element and the type of the attribute value.
 *
 * @typedef ElementAttributeValues
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 20
 */
export class ElementAttributeValuesImpl implements ElementAttributeValues {
    /**
     * Indicates accessibility focus state.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    accessibilityFocused: boolean = false;
    /**
     * Indicates the bundle name to which it belongs.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    bundleName: string = "";
    /**
     * Indicates whether the element is checkable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    checkable: boolean = false;
    /**
     * Indicates whether the element is checked.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    checked: boolean = false;
    /**
     * Indicates all child elements.
     *
     * @type {Array<AccessibilityElement>}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    children: Array<AccessibilityElement> = [];
    /**
     * Indicates whether the element is clickable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    clickable: boolean = false;
    /**
     * Indicates the component ID to which the element belongs.
     *
     * @type {long}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    componentId: long = 0;
    /**
     * Indicates the component type to which the element belongs.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    componentType: string = "";
    /**
     * Indicates the content.
     *
     * @type {Array<string>}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    contents: Array<string> = [];
    /**
     * Indicates the index of the current item.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    currentIndex: int = 0;
    /**
     * Indicates the description of the element.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    description: string = "";
    /**
     * Indicates whether the element is editable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    editable: boolean = false;
    /**
     * Indicates the list index of the last item displayed on the screen.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    endIndex: int = 0;
    /**
     * Indicates the string of error state.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    error: string = "";
    /**
     * Indicates whether the element is focusable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    focusable: boolean = false;
    /**
     * Indicates the hint text.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    hintText: string = "";
    /**
     * Indicates the type of input text.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    inputType: int = 0;
    /**
     * Indicates the inspector key.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    inspectorKey: string = "";
    /**
     * Indicates whether the element is active or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isActive: boolean = true;
    /**
     * Indicates whether the element is enable or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isEnable: boolean = true;
    /**
     * Indicates whether the element is hint state or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isHint: boolean = false;
    /**
     * Indicates whether the element is focused or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isFocused: boolean = false;
    /**
     * Indicates whether the element is password or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isPassword: boolean = false;
    /**
     * Indicates whether the element is visible or not.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    isVisible: boolean = true;
    /**
     * Indicates the total count of the items.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    itemCount: int = 0;
    /**
     * Indicates the last content.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    lastContent: string = "";
    /**
     * Indicates the display layer of the element.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    layer: int = 0;
    /**
     * Indicates whether the element is long clickable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    longClickable: boolean = false;
    /**
     * Indicates the page id.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    pageId: int = 0;
    /**
     * Indicates the parent of the element.
     *
     * @type {AccessibilityElement}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    parent: AccessibilityElement = new AccessibilityElementImpl();
    /**
     * Indicates whether the element supports multiple lines of text.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    pluralLineSupported: boolean = false;
    /**
     * Indicates the area of the element.
     *
     * @type {Rect}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    rect: Rect = new RectImpl();
    /**
     * Indicates the resource name of the element.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    resourceName: string = '';
    /**
     * Indicates the root element of the window element.
     *
     * @type {AccessibilityElement}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    rootElement: AccessibilityElement = new AccessibilityElementImpl();
    /**
     * Indicates the display area of the element.
     *
     * @type {Rect}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    screenRect: Rect = new RectImpl();
    /**
     * Indicates whether the element is scrollable.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    scrollable: boolean = false;
    /**
     * Indicates whether the element is selected.
     *
     * @type {boolean}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    selected: boolean = false;
    /**
     * Indicates the list index of the first item displayed on the screen.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    startIndex: int = 0;
    /**
     * Indicates the text of the element.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    text: string = '';
    /**
     * Indicates the maximum length limit of the element text.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    textLengthLimit: int = 0;
    /**
     * Indicates the unit of movement of the element text as it is read.
     *
     * @type {accessibility.TextMoveUnit}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    textMoveUnit: accessibility.TextMoveUnit = 'char';
    /**
     * Indicates the action that triggered the element event.
     *
     * @type {accessibility.Action}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    triggerAction: accessibility.Action = 'accessibilityFocus';
    /**
     * Indicates the window type of the element.
     *
     * @type {WindowType}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    type: WindowType = 'application';
    /**
     * Indicates the maximum value.
     *
     * @type {double}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    valueMax: double = 0;
    /**
     * Indicates the minimum value.
     *
     * @type {double}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    valueMin: double = 0;
    /**
     * Indicates the current value.
     *
     * @type {double}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    valueNow: double = 0;
    /**
     * Indicates the window id.
     *
     * @type {int}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    windowId: int = 0;
    /**
     * Indicates the offset.
     *
     * @type {double}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    offset: double = 0;
    /**
     * Indicates the text type.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    textType: string = '';
    /**
     * Indicates the accessibility text of component.
     *
     * @type {string}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    accessibilityText: string = '';
    /**
     * Indicates the hot area of the element.
     *
     * @type {Rect}
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    hotArea: Rect = new RectImpl();
    /**
     * Indicates the component next accessibility focus id.
     * @type { ?long }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    accessibilityNextFocusId?: long = 0;
    /**
     * Indicates the custom component type.
     *
     * @type { ?string }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    customComponentType?: string = '';
    /**
     * Indicates the extra info of the element.
     * @type { ?string }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    extraInfo?: string = '';
    /**
     * Indicates the component Previous accessibility focus id.
     *
     * @type { ?long }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    accessibilityPreviousFocusId?: long = 0;
    /**
     * Indicates whether the element is scrollable for accessibility.
     *
     * @type { ?boolean }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    accessibilityScrollable?: boolean = false;
}

/**
 * Indicates the direction of the search focus.
 *
 * @typedef {'up' | 'down' | 'left' | 'right' | 'forward' | 'backward'}
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 20
 */
export type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';

/**
 * Indicates the type of the focus.
 *
 * @typedef {'accessibility' | 'normal'}
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 20
 */
export type FocusType = 'accessibility' | 'normal';

/**
 * Indicates the type of the window.
 *
 * @typedef {'application' | 'system'}
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 20
 */
export type WindowType = 'application' | 'system';

/**
 * Indicates rectangle.
 *
 * @typedef Rect
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since arkts {'1.1':'9', '1.2':'20'}
 * @arkts 1.1&1.2
 */
interface Rect {
  /**
   * The left position of Rect
   *
   * @type { int }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  left: int;
  /**
   * The top position of Rect
   *
   * @type { int }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  top: int;
  /**
   * The width position of Rect
   *
   * @type { int }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  width: int;
  /**
   * The height position of Rect
   *
   * @type { int }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since arkts {'1.1':'9', '1.2':'20'}
   * @arkts 1.1&1.2
   */
  height: int;
}

/**
 * Indicates rectangle.
 *
 * @typedef Rect
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 20
 */
export class RectImpl implements Rect {
    /**
     * The left position of Rect
     *
     * @type { int }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    left: int = 0;
    /**
     * The top position of Rect
     *
     * @type { int }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    top: int = 0;
    /**
     * The width position of Rect
     *
     * @type { int }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    width: int = 0;
    /**
     * The height position of Rect
     *
     * @type { int }
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @since 20
     */
    height: int = 0;
}
