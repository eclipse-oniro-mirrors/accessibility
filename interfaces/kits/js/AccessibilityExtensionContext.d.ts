/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import { AsyncCallback } from "../basic";
import ExtensionContext from "./ExtensionContext";
import accessibility from "../@ohos.accessibility";
import { GesturePath } from "../@ohos.accessibility.GesturePath";

/**
 * The accessibility extension context. Used to configure, query information, and inject gestures.
 *
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
export default class AccessibilityExtensionContext extends ExtensionContext {
    /**
     * Set the name of the bundle name that is interested in sending the event.
     * @param targetNames 
     * @throws { BusinessError } 401 - Input parameter error.
     */
    setTargetBundleName(targetNames: Array<string>): Promise<void>;
    setTargetBundleName(targetNames: Array<string>, callback: AsyncCallback<void>): void;

    /**
     * Get focus element.
     * @param isAccessibilityFocus Indicates whether the acquired element has an accessibility focus.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300003 -  Do not have accessibility right for this operation.
     */
    getFocusElement(isAccessibilityFocus?: boolean): Promise<AccessibilityElement>;
    getFocusElement(callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElement(isAccessibilityFocus: boolean, callback: AsyncCallback<AccessibilityElement>): void;

    /**
     * Get window root element.
     * @param windowId Indicates the window ID.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300003 -  Do not have accessibility right for this operation.
     */
    getWindowRootElement(windowId?: number): Promise<AccessibilityElement>;
    getWindowRootElement(callback: AsyncCallback<AccessibilityElement>): void;
    getWindowRootElement(windowId: number, callback: AsyncCallback<AccessibilityElement>): void;

    /**
     * Get window list.
     * @param displayId Indicates the display ID.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300003 -  Do not have accessibility right for this operation.
     */
    getWindows(displayId?: number): Promise<Array<AccessibilityElement>>;
    getWindows(callback: AsyncCallback<Array<AccessibilityElement>>): void;
    getWindows(displayId: number, callback: AsyncCallback<Array<AccessibilityElement>>): void;

    /**
     * Inject gesture path events.
     * @param gesturePath Indicates the gesture path.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300003 -  Do not have accessibility right for this operation.
     */
    injectGesture(gesturePath: GesturePath): Promise<void>;
    injectGesture(gesturePath: GesturePath, callback: AsyncCallback<void>): void;
}

/**
 * Indicates an accessibility element.
 *
 * Supports querying element attributes, requesting execution actions, and finding child elements by condition.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
declare interface AccessibilityElement {
    /**
     * Get a list of attribute names.
     */
    attributeNames<T extends keyof ElementAttributeValues>(): Promise<Array<T>>;
    attributeNames<T extends keyof ElementAttributeValues>(callback: AsyncCallback<Array<T>>): void;

    /**
     * Get the value of an attribute.
     * @param attributeName Indicates the attribute name.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300004 - This property does not exist.
     */
    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T): Promise<ElementAttributeValues[T]>;
    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T,
        callback: AsyncCallback<ElementAttributeValues[T]>): void;

    /**
     * Get a list of supported actions.
     */
    actionNames(): Promise<Array<string>>;
    actionNames(callback: AsyncCallback<Array<string>>): void;

    /**
     * Perform the specified action.
     * @param actionName Indicates the action name.
     * @param parameters Indicates the parameters needed to execute the action.
     * @throws { BusinessError } 401 - Input parameter error.
     * @throws { BusinessError } 9300005 - This action is not supported.
     */
    performAction(actionName: string, parameters?: object): Promise<void>;
    performAction(actionName: string, callback: AsyncCallback<void>): void;
    performAction(actionName: string, parameters: object, callback: AsyncCallback<void>): void;
    
    /**
     * Find elements that match the condition.
     * @param type The type of query condition is content.
     * @param condition Indicates the specific content to be queried.
     * @throws { BusinessError } 401 - Input parameter error.
     */
    findElement(type: 'content', condition: string): Promise<Array<AccessibilityElement>>;
    findElement(type: 'content', condition: string, callback: AsyncCallback<Array<AccessibilityElement>>): void

    /**
     * Find elements that match the condition.
     * @param type The type of query condition is focus type.
     * @param condition Indicates the type of focus to query.
     * @throws { BusinessError } 401 - Input parameter error.
     */
    findElement(type: 'focusType', condition: FocusType): Promise<AccessibilityElement>;
    findElement(type: 'focusType', condition: FocusType, callback: AsyncCallback<AccessibilityElement>): void

    /**
     * Find elements that match the condition.
     * @param type The type of query condition is focus direction.
     * @param condition Indicates the direction of search focus to query.
     * @throws { BusinessError } 401 - Input parameter error.
     */
    findElement(type: 'focusDirection', condition: FocusDirection): Promise<AccessibilityElement>;
    findElement(type: 'focusDirection', condition: FocusDirection, callback: AsyncCallback<AccessibilityElement>): void
}

/**
 * Indicates the possible attributes of the element and the type of the attribute value.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type ElementAttributeValues = {
    /**
     * Indicates accessibility focus state.
     */
    'accessibilityFocused': boolean;
    /**
     * Indicates the bundle name to which it belongs.
     */
    'bundleName': string;
    /**
     * Indicates whether the element is checkable.
     */
    'checkable': boolean;
    /**
     * Indicates whether the element is checked.
     */
    'checked': boolean;
    /**
     * Indicates all child elements.
     */
    'children': Array<AccessibilityElement>;
    /**
     * Indicates whether the element is clickable.
     */
    'clickable': boolean;
    /**
     * Indicates the component ID to which the element belongs.
     */
    'componentId': number;
    /**
     * Indicates the component type to which the element belongs.
     */
    'componentType': string;
    /**
     * Indicates the content.
     */
    'contents': Array<string>;
    /**
     * Indicates the index of the current item.
     */
    'currentIndex': number;
    /**
     * Indicates the description of the element.
     */
    'description': string;
    /**
     * Indicates whether the element is editable.
     */
    'editable': boolean;
    /**
     * Indicates the list index of the last item displayed on the screen.
     */
    'endIndex': number;
    /**
     * Indicates the string of error state.
     */
    'error': string;
    /**
     * Indicates whether the element is focusable.
     */
    'focusable': boolean;
    /**
     * Indicates the hint text.
     */
    'hintText': string;
    /**
     * Indicates the type of input text.
     */
    'inputType': number;
    /**
     * Indicates the inspector key.
     */
    'inspectorKey': string
    /**
     * Indicates whether the element is active or not.
     */
    'isActive': boolean;
    /**
     * Indicates whether the element is enable or not.
     */
    'isEnable': boolean;
    /**
     * Indicates whether the element is hint state or not.
     */
    'isHint': boolean;
    /**
     * Indicates whether the element is focused or not.
     */
    'isFocused': boolean;
    /**
     * Indicates whether the element is password or not.
     */
    'isPassword': boolean;
    /**
     * Indicates whether the element is visible or not.
     */
    'isVisible': boolean;
    /**
     * Indicates the total count of the items.
     */
    'itemCount': number;
    /**
     * Indicates the last content.
     */
    'lastContent': string;
    /**
     * Indicates the display layer of the element.
     */
    'layer': number;
    /**
     * Indicates whether the element is long clickable.
     */
    'longClickable': boolean;
    /**
     * Indicates the page id.
     */
    'pageId': number;
    /**
     * Indicates the parent of the element.
     */
    'parent': AccessibilityElement;
    /**
     * Indicates whether the element supports multiple lines of text.
     */
    'pluralLineSupported': boolean;
    /**
     * Indicates the area of the element.
     */
    'rect': Rect;
    /**
     * Indicates the resource name of the element.
     */
    'resourceName': string;
    /**
     * Indicates the root element of the window element.
     */
    'rootElement': AccessibilityElement;
    /**
     * Indicates the display area of the element.
     */
    'screenRect': Rect;
    /**
     * Indicates whether the element is scrollable.
     */
    'scrollable': boolean;
    /**
     * Indicates whether the element is selected.
     */
    'selected': boolean;
    /**
     * Indicates the list index of the first item displayed on the screen.
     */
    'startIndex': number;
    /**
     * Indicates the text of the element.
     */
    'text': string;
    /**
     * Indicates the maximum length limit of the element text.
     */
    'textLengthLimit': number;
    /**
     * Indicates the unit of movement of the element text as it is read.
     */
    'textMoveUnit': accessibility.TextMoveUnit;
    /**
     * Indicates the action that triggered the element event.
     */
    'triggerAction': accessibility.Action;
    /**
     * Indicates the window type of the element.
     */
    'type': WindowType;
    /**
     * Indicates the maximum value.
     */
    'valueMax': number;
    /**
     * Indicates the minimum value.
     */
    'valueMin': number;
    /**
     * Indicates the current value.
     */
    'valueNow': number;
    /**
     * Indicates the window id.
     */
    'windowId': number;
}

/**
* Indicates the direction of the search focus.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';

/**
 * Indicates the type of the focus.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type FocusType = 'accessibility' | 'normal';

/**
 * Indicates the type of the window.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type WindowType = 'application' | 'system';

/**
 * Indicates rectangle.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
interface Rect {
    left: number;
    top: number;
    width: number;
    height: number;
}
