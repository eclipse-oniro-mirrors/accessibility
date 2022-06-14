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
import { Rect } from './@componet/rect'
import { GridInfo, GridItemInfo } from "./@componet/gridInfo"
import { RangeInfo } from "./@componet/progress"
/**
 * The context of form extension. It allows access to
 * formExtension-specific resources.
 *
 * @since 9
 * @syscap SystemCapability.Ability.Form
 * @permission N/A
 * @StageModelOnly
 */
export default class AccessibilityExtensionContext extends ExtensionContext {

    /**
     * Set the accessibility Event which is wanted.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param type The type of accessibility Even.
     * @return -.
    */
    setEventTypeFilter(type: Array<accessibility.EventType>, callback: AsyncCallback<boolean>): boolean;
    setEventTypeFilter(type: Array<accessibility.EventType>): Promise<boolean>;

    /**
     * Set the targe abilityName which is interested.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param targetNames The bundle name of target ability.
     * @return -.
    */
    setTargetBundleName(targetNames: Array<string>, callback: AsyncCallback<boolean>): boolean;
    setTargetBundleName(targetNames: Array<string>): Promise<boolean>;


    /**
     * Obtains the accessibility node that gains the focus.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param isAccessibilityFocus The type of focus. default is false.
     * @return -.
    */
    getFocusElement(callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElement(isAccessibilityFocus: boolean, callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElement(isAccessibilityFocus?: boolean): Promise<AccessibilityElement>;

    /**
     * Obtains information about the accessibility root node.
     * 获取窗口根节点，如果不指定窗口ID，则获取激活窗口的根节点
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return The root node info.
    */
    getWindowRootElement(callback: AsyncCallback<AccessibilityElement>): void;
    getWindowRootElement(windowId: number, callback: AsyncCallback<AccessibilityElement>): void;
    getWindowRootElement(windowId?: number): Promise<AccessibilityElement>;

    /**
     * Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * 如果不指定屏幕ID，则获取的是默认屏幕上的窗口，列表顺序表示从上到下。
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return the list of interactive windows.
    */
    getWindows(callback: AsyncCallback<Array<AccessibilityElement>>): void;
    getWindows(displayId: number, callback: AsyncCallback<Array<AccessibilityElement>>): void;
    getWindows(displayId?: number): Promise<Array<AccessibilityElement>>;

    /**
     * Executes a specified action.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param action The action wanted to be excused.
     * @return The window info.
    */
    executeCommonAction(action: CommonAction, callback: AsyncCallback<boolean>): void;
    executeCommonAction(action: CommonAction): Promise<boolean>;

    /**
     * Sends customized gestures to the screen.
     * 
     * Currently only gesture injection for a single finger is implemented.That is,
     * the following interfaces only support the implementation with the parameter
     * "gesturePath: GesturePath", and do not support the implementation with the
     * parameter "gesturePath: Array<GesturePath>".
     * 
     * When injecting gestures of a single finger, the main usage methods are as follows:
     * 1. Click or long press gesture:
     *    call the gestureInject interface once, the positions in the parameter gesturePath
     *    only contain one GesturePos position information. durationTime represents the
     *    time of the click action.
     *    When simulating a click gesture, please set the time to less than 500ms;
     *    when simulating a long-press gesture, please set the time to greater than 500ms.
     * 
     * 2. Multiple click gestures in different positions:
     *    The gestureInject interface needs to be called multiple times, and the positions
     *    in the parameter gesturePath in each call only contains one GesturePos position
     *    information.
     * 
     * 3. Multiple click gestures at the same position:
     *    There are two methods. 1) Call the gestureInject interface multiple times, the
     *    positions in the parameter gesturePath in each call only contain one GesturePos
     *    position information; 2) Call the gestureInject interface once, and the positions in
     *    the parameter gesturePath set multiple GesturePos with the same position information.
     * 
     * 4. Moving gesture:
     *    call the gestureInject interface once, and the positions in the parameter gesturePath
     *    set multiple GesturePos with different position information.
     * 
     * Note:
     *    1) In a gestureInject interface call, if the size of positions is greater than 1,
     *       and the first two GesturePos in positions have the same position information,
     *       the gesture will be converted into a click action corresponding to the position
     *       information of each GesturePos.
     *    2) When simulating the click gesture, the time of each click is equal to
     *       (durationTime / positions.size());
     *       when simulating the moving gesture, the movement time between two adjacent points
     *       is equal to (durationTime /(positions.size() - 1)).
     * @since 9
     * @note hide
     * @sysCap AccessibilityAbility
     * @param gesturePath The path of simulation gesture.
     * @param listener The result of simulation gesture.
     * @return -.
    */
    gestureInject(gesturePath: GesturePath, listener: Callback<boolean>, callback: AsyncCallback<boolean>): void;
    gestureInject(gesturePath: GesturePath, listener: Callback<boolean>): Promise<boolean>;
    gestureInject(gesturePath: Array<GesturePath>, listener: Callback<boolean>, callback: AsyncCallback<boolean>): void;
    gestureInject(gesturePath: Array<GesturePath>, listener: Callback<boolean>): Promise<boolean>;
}

// 无权限时，不允许访问父子节点
declare interface AccessibilityElement {
    attributeNames<T extends keyof ElementAttributeValues>(): Promise<Array<T>>;
    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T): Promise<ElementAttributeValues[T]>;
    actionNames(): Promise<Array<string>>;
    performAction(actionName: string, args?: object): Promise<boolean>;
    findElement(type: 'content', condition: string): Promise<Array<AccessibilityElement>>;
    findElement(type: 'focusType', condition: FocusType): Promise<AccessibilityElement>;
    findElement(type: 'focusDirection', condition: FocusDirection): Promise<AccessibilityElement>;
}

type ElementAttributeValues = {
    /************************/
    /*     element info     */
    /************************/
    'componentId': number;  //The id of the view which the node in.
    'pageId': number;
    'parentId': number; 
    'inspectorKey': string
    'bundleName': string; //The bundle name.
    'componentType': string;  //The type of the event source component,such as button, chart.
    'inputType': number;  //The type of the input text.
    'text': string;       //The text of the node.
    'hintText': string;   //The hint text of the node.
    'description': string; // The description of the node.
    'resourceName': string; //The resource name of the node.
    'childNodeIds': Array<number>;  //The list of the children node.
    'textLengthLimit': number;  //The max text length of the node.
    'rect': Rect;       //The rect of the node.
    'checkable': boolean; //Whether the node can be check.
    'checked': boolean;   // Whether the node is checked.
    'focusable': boolean; //Whether the node can be focused.
    'isVisible': boolean; //Whether the node is visible.
    'accessibilityFocused': boolean;  //Whether the node is accessibility focused.
    'selected': boolean;  //Whether the node is selected.
    'clickable': boolean; //Whether the node can be click.
    'longClickable': boolean; //Whether the node can be long click.
    'isEnable': boolean;  // Whether the node can be enable.
    'isPassword': boolean;  //Whether the node is password.
    'scrollable': boolean;  // Whether the node can be scrollable.
    'editable': boolean;  // Whether the node can be editable.
    'popupSupported': boolean;  //Whether the node can popup.
    'pluralLineSupported': boolean; //Whether the node is multiline.
    'deleteable': boolean;  //Whether the node can be delete.
    'isHint': boolean;  //Whether the node is displaying hint.
    'isEssential': boolean; //Whether the node is important.
    // list
    'itemCount': number;  //The total of the items.
    'currentIndex': number; //The index of the current item on the screen.
    'startIndex': number; ///The start index of listed items on the screen.
    'endIndex': number; //The end index of listed items on the screen.
    // progress
    'rangeInfo': RangeInfo; //Range info of the progress node.
    //grid
    'grid': GridInfo; //It is used when the node is collection.
    'gridItem': GridItemInfo; //collection item info.
    'activeRegion': number; //The live range of the node.
    'isContentInvalid': boolean;  //Whether the content is invalid.
    'error': string; //error information.
    'label': number;  // The label of the node.
    'beginSelected': number;  //The start position of text selected.
    'endSelected': number;  //The end position of text selected.
    // text move
    'textMoveUnit': accessibility.TextMoveUnit; //The movement step used for reading texts.
    'parent': AccessibilityElement;
    'children': Array<AccessibilityElement>;
    'triggerAction': accessibility.Action;
    'contents': Array<string>;
    'lastContent': string;
    /************************/
    /*     window info      */
    /************************/
    'isActive': boolean;
    'screenRect': Rect;
    'layer': number;
    'type': WindowType;
    'anchor': AccessibilityElement;
    'rootElement': AccessibilityElement;
    /************************/
    /*        common        */
    /************************/
    'windowId': number; //The id of the window which the node in.
    'isFocused': boolean;
}

type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';
/**
 * The type of the focus.
 * @note accessibility: accessibility focus; input: input focus;
 * @since 9
 * @sysCap Accessibility
 */
type FocusType = 'accessibility' | 'normal';

declare enum CommonAction {
    'back',
    'home',
    'recent',
    'notification',
    'lockScreen'
}

interface GesturePath {
    /**
     * The positions wanted to be injected.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    positions: Array<GesturePos>;

    /**
     * The duration time of the gesture.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     * @note The max of the duration time is 60 * 1000 ms;
     */
    durationTime: number;
}
//@note hide
interface GesturePos {
    /**
     * The x coordinate of position.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    posX: number;

    /**
     * The y coordinate of position.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    posY: number;
}

/**
 * The type of the window.
 * @note -
 * @since 9
 * @sysCap Accessibility
 */
type WindowType = 'application' | 'inputMethod' | 'system' | 'accessibilityOverlay' | 'screenDivider';