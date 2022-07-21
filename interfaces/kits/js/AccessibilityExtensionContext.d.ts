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

import { Callback, AsyncCallback } from "../basic";
import ExtensionContext from "./ExtensionContext";
import accessibility from "../@ohos.accessibility";
import { GesturePath } from "../@ohos.application.AccessibilityExtensionAbility";

export default class AccessibilityExtensionContext extends ExtensionContext {
    setTargetBundleName(targetNames: Array<string>, callback: AsyncCallback<boolean>): boolean;
    setTargetBundleName(targetNames: Array<string>): Promise<boolean>;

    getFocusElement(callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElement(isAccessibilityFocus: boolean, callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElement(isAccessibilityFocus?: boolean): Promise<AccessibilityElement>;

    getWindowRootElement(callback: AsyncCallback<AccessibilityElement>): void;
    getWindowRootElement(windowId: number, callback: AsyncCallback<AccessibilityElement>): void;
    getWindowRootElement(windowId?: number): Promise<AccessibilityElement>;

    getWindows(callback: AsyncCallback<Array<AccessibilityElement>>): void;
    getWindows(displayId: number, callback: AsyncCallback<Array<AccessibilityElement>>): void;
    getWindows(displayId?: number): Promise<Array<AccessibilityElement>>;

    gestureInject(gesturePath: GesturePath, listener: Callback<boolean>, callback: AsyncCallback<boolean>): void;
    gestureInject(gesturePath: GesturePath, listener: Callback<boolean>): Promise<boolean>;
}

declare interface AccessibilityElement {
    attributeNames<T extends keyof ElementAttributeValues>(): Promise<Array<T>>;
    attributeNames<T extends keyof ElementAttributeValues>(callback: AsyncCallback<Array<T>>): void;

    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T): Promise<ElementAttributeValues[T]>;
    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T,
        callback: AsyncCallback<ElementAttributeValues[T]>): void;

    actionNames(): Promise<Array<string>>;
    actionNames(callback: AsyncCallback<Array<string>>): void;

    performAction(actionName: string, parameters?: object): Promise<boolean>;
    performAction(actionName: string, callback: AsyncCallback<boolean>): void;
    performAction(actionName: string, parameters: object, callback: AsyncCallback<boolean>): void;

    findElement(type: 'content', condition: string): Promise<Array<AccessibilityElement>>;
    findElement(type: 'content', condition: string, callback: AsyncCallback<Array<AccessibilityElement>>): void

    findElement(type: 'focusType', condition: FocusType): Promise<AccessibilityElement>;
    findElement(type: 'focusType', condition: FocusType, callback: AsyncCallback<AccessibilityElement>): void

    findElement(type: 'focusDirection', condition: FocusDirection): Promise<AccessibilityElement>;
    findElement(type: 'focusDirection', condition: FocusDirection, callback: AsyncCallback<AccessibilityElement>): void
}

type ElementAttributeValues = {
    /************************/
    /*     element info     */
    /************************/
    'componentId': number;  //The id of the view which the node in.
    'inspectorKey': string
    'bundleName': string; //The bundle name.
    'componentType': string;  //The type of the event source component,such as button, chart.
    'inputType': number;  //The type of the input text.
    'text': string;       //The text of the node.
    'hintText': string;   //The hint text of the node.
    'description': string; // The description of the node.
    'triggerAction': accessibility.Action;
    'textMoveUnit': accessibility.TextMoveUnit; //The movement step used for reading texts.
    'contents': Array<string>;
    'lastContent': string;
    // list
    'itemCount': number;  //The total of the items.
    'currentIndex': number; //The index of the current item on the screen.
    'startIndex': number; ///The start index of listed items on the screen.
    'endIndex': number; //The end index of listed items on the screen.
    'resourceName': string; //The resource name of the node.
    'textLengthLimit': number;  //The max text length of the node.
    'rect': Rect;       //The rect of the node.
    'checkable': boolean; //Whether the node can be check.
    'checked': boolean;   // Whether the node is checked.
    'focusable': boolean; //Whether the node can be focused.
    'isVisible': boolean; //Whether the node is visible.
    'selected': boolean;  //Whether the node is selected.
    'clickable': boolean; //Whether the node can be click.
    'longClickable': boolean; //Whether the node can be long click.
    'isEnable': boolean;  // Whether the node can be enable.
    'isPassword': boolean;  //Whether the node is password.
    'scrollable': boolean;  // Whether the node can be scrollable.
    'editable': boolean;  // Whether the node can be editable.
    'pluralLineSupported': boolean; //Whether the node is multiline.
    'parent': AccessibilityElement;
    'children': Array<AccessibilityElement>;
    /************************/
    /*     window info      */
    /************************/
    'isActive': boolean;
    'screenRect': Rect;
    'layer': number;
    'type': WindowType;
    'rootElement': AccessibilityElement;
    /************************/
    /*        common        */
    /************************/
    'isFocused': boolean;
}

type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';

type FocusType = 'accessibility' | 'normal';

interface Rect {
    left: number;
    top: number;
    width: number;
    height: number;
}

type WindowType = 'application' | 'system';
