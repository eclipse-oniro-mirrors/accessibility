/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback } from './basic';
import { Callback } from './basic';

import { KeyEvent } from 'keyevent'
import { Rect } from './@componet/rect'
import { GridInfo, GridItemInfo } from "./@componet/gridInfo"
import { RangeInfo } from "./@componet/progress"
import { DisplayResizeInfo } from "./@componet/displayResizeInfo"
import ExtensionContext from "./ExtensionContext";
/**
 * Accessibility
 * @name Accessibility
 * @since 7
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @import basic,abilityInfo
 */
declare namespace accessibility {
  class GesturePath {
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
     * @note The max of the durationtime is 60 * 1000 ms;
     */
    durationTime: number;
  }
  //@note hide
  class GesturePos {
    constructor(x: number, y: number);
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

  /************************************************************************************************
   * Type define
  ***********************************************************************************************/
  /**
   * The type of the Ability app.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type AbilityType = 'audible' | 'generic' | 'haptic' | 'spoken' | 'visual' | 'all';

  /**
   * The action that the ability can execute.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type Action = 'accessibilityFocus' | 'clearAccessibilityFocus' | 'focus' | 'clearFocus' | 'clearSelection' |
    'click' | 'longClick' | 'cut' | 'copy' | 'paste' | 'select' | 'setText' | 'delete' |
    'scrollForward' | 'scrollBackward' |
    'setSelection' |
    'unfold' | 'fold' | 'nextText' | 'previousText' | 'nextHtmlItem' | 'previousHtmlItem';

  /**
   * The type of the accessibility event.
   * @note windowsChange
   * @note windowContentChange
   * @note windowStateChange
   * @note announcement
   * @note notificationChange
   * @note textTraversedAtMove
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type EventType = 'accessibilityFocus' | 'accessibilityFocusClear' |
    'click' | 'longClick' | 'focus' | 'select' | 'hoverEnter' | 'hoverExit' |
    'textUpdate' | 'textSelectionUpdate' | 'scroll' | 'textMoveUnit' |
    'touchBegin' | 'touchEnd' | 'touchGuideBegin' | 'touchGuideEnd' |
    'touchGuideGestureBegin' | 'touchGuideGestureEnd' |
    'windowUpdate' | 'pageContentUpdate' | 'pageStateUpdate' |
    'publicNotice' | 'notificationUpdate' | 'gesture';

  /**
   * The change type of the windowsChange event.
   * @note It's used when received the {@code windowsChange} event.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type UpdateType = 'add' | 'remove' | 'bounds' | 'active' | 'focus';

  /**
   * The type of the window.
   * @note -
   * @since 9
   * @sysCap Accessibility
   */
  type WindowType = 'application' | 'inputMethod' | 'system' | 'accessibilityOverlay' | 'screenDivider';

  /**
   * The type of the focus.
   * @note accessibility: accessibility focus; input: input focus;
   * @since 9
   * @sysCap Accessibility
   */
  type FocusType = 'accessibility' | 'normal';

  /**
   * The type of the ability state.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type AbilityState = 'enable' | 'disable' | 'install';

  /**
   * The direction of the focus move.
   * @note -
   * @since 9
   * @sysCap Accessibility
   */
  type FocusDirection = 'up' | 'down' | 'left' | 'right' | 'forward' | 'backward';

  /**
   * The ability that accessibility subsystem support.
   * @note touchExplorer: Describes the capability to talkback.
   * magnification: Describes the capability to request to control the display magnification.
   * gesturesSimulation: Describes the capability to request to simulate the gesture.
   * windowContent: Describes the capability to search for the content of the active window.
   * filterKeyEvents: Describes the capability to request to filter key events.
   * fingerprintGesture: Describes the capability to request to fingerprint gesture.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type Capability = 'retrieve' | 'touchGuide' | 'keyEventObserver' | 'zoom' | 'gesture';

  /**
   * The global action that the ability can execute.
   * @note back: Indicates a global action of returning to the previous page.
    home: Indicates a global action of returning to the home page.
    recents: Indicates a global action of displaying a list of recent tasks.
    notifications: Indicates a global action of opening the notification panel.
    quickSettings: Indicates a global action of opening quick settings.
    powerDialog: Indicates a global action of pressing and holding the power button.
    toggleSplitScreen: Indicates a global action of switching between split screens.
    lockScreen: Indicates a global action of locking the screen.
    takeScreenShot: Indicates a global action of taking a screenshot.
   * @since 9
   * @sysCap Accessibility
   */
  type GlobalActionType = 'back' | 'home' | 'recent' | 'notification' | 'popUpPowerDialog' | 'divideScreen' |
    'lockScreen' | 'captureScreen';

  /**
   * The types of swipe gestures which are performed on the touch screen.
   * And they are also the gesture's interception type.
   * @note -
   * @since 9
   * @sysCap Accessibility
   */
  type GestureType = 'left' | 'leftThenRight' | 'leftThenUp' | 'leftThenDown' |
    'right' | 'rightThenLeft' | 'rightThenUp' | 'rightThenDown' |
    'up' | 'upThenLeft' | 'upThenRight' | 'upThenDown' |
    'down' | 'downThenLeft' | 'downThenRight' | 'downThenUp';

  /**
   * The granularity of text move.
   * @note The granularity of text move.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type TextMoveUnit = 'char' | 'word' | 'line' | 'page' | 'paragraph';

  /**
   * The argument type for operation.
   * @note -
   * @since 9
   * @sysCap Accessibility
   */
  type OperatorParamType = 'selectTextBegin' | 'selectTextEnd' | 'htmlItem' | 'setText' | 'textMoveUnit';

  /**
   * The category of the notification
   * @note 应用Notification的具体分类，用于朗读通知信息时提示是哪类通知.
   * @since 9
   * @sysCap Accessibility
   */
  type NotificationCategory = 'call' | 'msg' | 'email' | 'event' |
    'promo' | 'alarm' | 'progress' | 'social' | 'err' | 'transport' | 'sys' | 'service';

  /** TODO
   * The html element type. The arguments of  nextHtmlElement/previousHtmlElement
   * @note html元素的具体分类，用于实现网页的按类别导航，如“跳到下一个链接或按钮”.
   * @since 9
   * @sysCap Accessibility
   */
  type HtmlItemType = 'link' | 'control' | 'graphic' | 'listItem' | 'list' | 'table' |
    'combox' | 'heading' | 'button' | 'checkBox' | 'landmark' | 'textField' | 'focusable' |
    'h1' | 'h2' | 'h3' | 'h4' | 'h5' | 'h6';

  /**
   * Checks whether accessibility ability is enabled.
   * @since 7
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the accessibility is enabled; returns {@code false} otherwise.
  */
  function isOpenAccessibility(callback: AsyncCallback<boolean>): void;
  function isOpenAccessibility(): Promise<boolean>;

  /**
   * Checks touch browser ability (which is used by talkback) is enabled.
   * @since 7
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Vision
   * @return Returns {@code true} if the touch browser is enabled; returns {@code false} otherwise.
  */
  function isOpenTouchGuide(callback: AsyncCallback<boolean>): void;
  function isOpenTouchGuide(): Promise<boolean>;

  /**
   * Queries the list of accessibility abilities.
   * @since 7
   * @param abilityType The type of the accessibility ability. {@code AbilityType} eg.spoken
   * @param stateType The state of the accessibility ability.  {@code AbilityState} eg.installed
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns the list of abilityInfos.
  */
  function getAbilityLists(abilityType: AbilityType, stateType: AbilityState,
    callback: AsyncCallback<Array<AccessibilityAbilityInfo>>): void;
  function getAbilityLists(abilityType: AbilityType,
    stateType: AbilityState): Promise<Array<AccessibilityAbilityInfo>>;

  /**
   * Send accessibility Event.
   * @since 7
   * @param event The object of the accessibility {@code EventInfo} .
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if success ; returns {@code false} otherwise.
   */
  function sendEvent(event: AccessibilityEvent, callback: AsyncCallback<void>): void;
  function sendEvent(event: AccessibilityEvent): Promise<void>;

  /**
   * Register the observe of the accessibility state changed.
   * @since 7
   * @param type state event type.
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the register is success ; returns {@code false} otherwise.
   */
  function on(type: 'accessibilityStateChange', callback: Callback<boolean>): void;

  /**
   * Register the observe of the touchGuide state changed.
   * @since 7
   * @param type state event type.
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Vision
   * @return Returns {@code true} if the register is success ; returns {@code false} otherwise.
   */
  function on(type: 'touchGuideStateChange', callback: Callback<boolean>): void;

  /**
   * Deregister the observe of the accessibility state changed.
   * @since 7
   * @param type state event type
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the deregister is success ; returns {@code false} otherwise.
   */
  function off(type: 'accessibilityStateChange', callback?: Callback<boolean>): void;

  /**
   * Deregister the observe of the touchGuide state changed.
   * @since 7
   * @param type state event type
   * @param callback Asynchronous callback interface.
   * @return Returns {@code true} if the deregister is success ; returns {@code false} otherwise.
   */
   function off(type: 'touchGuideStateChange', callback?: Callback<boolean>): void;

  /**
   * Get the captions manager.
   * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
   * @since 8
   * @return Returns the captions manager.
   */
   function getCaptionsManager(): CaptionsManager;

   /**
    * Indicates the captions manager.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   interface CaptionsManager {
     /**
      * Indicates whether captions are enabled.
      */
     enabled: boolean;
     /**
      * Indicates the style of captions.
      */
     style: CaptionsStyle;

     /**
      * Register the observe of the enable state.
      */
     on(type: 'enableChange', callback: Callback<boolean>): void;
     /**
      * Register the observer of the style.
      */
     on(type: 'styleChange', callback: Callback<CaptionsStyle>): void;
     /**
      * Deregister the observe of the enable state.
      */
     off(type: 'enableChange', callback?: Callback<boolean>): void;
     /**
      * Deregister the observer of the style.
      */
     off(type: 'styleChange', callback?: Callback<CaptionsStyle>): void;
   }

   /**
    * Indicates the edge type of the captions font.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   type CaptionsFontEdgeType = 'none' | 'raised' | 'depressed' | 'uniform' | 'dropShadow';
   /**
    * Indicates the font family of captions.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   type CaptionsFontFamily = 'default' | 'monospacedSerif' | 'serif' |
     'monospacedSansSerif' | 'sansSerif' | 'casual' | 'cursive' | 'smallCapitals';
   /**
    * Indicates the style of captions.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   interface CaptionsStyle {
     /**
      * Indicates the font family of captions.
      */
     fontFamily: CaptionsFontFamily;
     /**
      * Indicates the font scaling of captions.
      */
     fontScale: number;
     /**
      * Indicates the font color of captions.
      * number: 0xFF112233(0xAARRGGBB )
      * string: #012F(#RGBA) #001122FF(#RRGGBBAA) #012(#RGB) #001122(#RRGGBB)
      *         red/green/blue/black/white/gray
      */
     fontColor: number | string;
     /**
      * Indicates the edge type of the captions font.
      */
     fontEdgeType: CaptionsFontEdgeType;
     /**
      * Indicates the background color of captions.
      * number: 0xFF112233(0xAARRGGBB )
      * string: #012F(#RGBA) #001122FF(#RRGGBBAA) #012(#RGB) #001122(#RRGGBB)
      *         red/green/blue/black/white/gray
      */
     backgroundColor: number | string;
     /**
      * Indicates the window color of captions.
      * number: 0xFF112233(0xAARRGGBB )
      * string: #012F(#RGBA) #001122FF(#RRGGBBAA) #012(#RGB) #001122(#RRGGBB)
      *         red/green/blue/black/white/gray
      */
     windowColor: number | string;
   }

  class AccessibilityExtensionContext extends ExtensionContext {
    /**
     * Set the accessibility ability type.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param type The type of ability. {@code AbilityType}
     * @return -.
    */
    setAbilityType(type: AbilityType, callback: AsyncCallback<void>): void;
    setAbilityType(type: AbilityType): Promise<void>;

    /**
     * Set the accessibility Event which is wanted.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param type The type of accessibility Even.
     * @return -.
    */
    setEventTypeFilter(type: Array<EventType>, callback: AsyncCallback<void>): void;
    setEventTypeFilter(type: Array<EventType>): Promise<void>;

    /**
     * Set the targe abilityName which is interested.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param targetNames The bundle name of target ability.
     * @return -.
    */
    setTargetBundleName(targetNames: Array<string>, callback: AsyncCallback<void>): void;
    setTargetBundleName(targetNames: Array<string>): Promise<void>;

    /**
     * Set the capability of the accessibility ability.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param business The capability of the accessibility ability.
     * @return -.
    */
    setCapability(business: Array<Capability>, callback: AsyncCallback<void>): void;
    setCapability(business: Array<Capability>): Promise<void>;

    /**
     * Obtains the accessibility node that gains the focus.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param focusType The type of focus. {@code FocusType}
     * @return -.
    */
    getFocusElementInfo(focusType: FocusType, callback: AsyncCallback<AccessibilityElement>): void;
    getFocusElementInfo(focusType: FocusType): Promise<AccessibilityElement>;

    /**
     * Obtains information about the accessibility root node.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return The root node info.
    */
    getRootElementInfo(callback: AsyncCallback<AccessibilityElement>): void;
    getRootElementInfo(): Promise<AccessibilityElement>;

    /**
     * Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return the list of interactive windows.
    */
    getWindows(callback: AsyncCallback<Array<WindowInfo>>): void;
    getWindows(): Promise<Array<WindowInfo>>;

    /**
     * Executes a specified action.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param action The action wanted to be excused.
     * @return The window info.
    */
    executeCommonAction(action: GlobalActionType, callback: AsyncCallback<boolean>): void;
    executeCommonAction(action: GlobalActionType): Promise<boolean>;

    /**
     * Sends customized gestures to the screen.
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

  interface AccessibilityAbilityInfo {
    /**
     * The ability id.
     * @since 7
     */
    readonly id: string;

    /* The ability name.
     * @since 7
     */
    readonly name: string;

    /* The bundle name of the ability.
     * @since 7
     */
    readonly bundleName: string;

    /**
     * The type of the ability.
     * @since 7
     */
    readonly abilityTypes: Array<AbilityType>;

    /**
     * The capabilities of the ability.
     * @since 7
     */
    readonly capabilities: Array<Capability>;

    /**
     * The description of the ability.
     * @since 7
     */
    readonly description: string;

    /**
     * The events which the accessibility ability wants to observe.
     * @since 7
     */
    readonly eventTypes: Array<EventType>;

    /**
     * The applition names which the accessiblity ability wants to observe.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    readonly filterBundleNames: Array<string>;
  }

  interface AccessibilityEvent {
    target: AccessibilityElement;
    eventType: EventType;
    windowUpdateType?: UpdateType;
    timeStamp?: number;
    triggerAction: Action; //The action that triggers the accessibility event, for example, clicking or focusing a view.
    categoryNotification?: NotificationCategory; //The category type for notificationChanged event.
    gestureType?: GestureType; //The gesture type for event.
    description?: string; //The accessibility event description.
  }

  interface AccessibilityElement {
    attributeNames<T extends keyof ElementAttributeValues>(): Promise<Array<T>>;
    attributeValue<T extends keyof ElementAttributeValues>(attributeName: T): Promise<ElementAttributeValues[T]>;
    isAttributeChangeable<T extends keyof ElementAttributeValues>(attributeName: T): Promise<boolean>;
    attributeUpdate<T extends keyof ElementAttributeValues>(attributeName: T, value: ElementAttributeValues[T]): Promise<boolean>;
    actionNames(): Promise<Array<Operation>>;
    performAction(actionName: string, args?: object): Promise<void>;
    findElement<T extends keyof FindElementCondition>(type: T, condition: FindElementCondition[T]): Promise<AccessibilityElement>;
  }

  type ElementAttributeValues = {
    'windowId': number; //The id of the window which the node in.
    'pageId': number;
    'accessibilityIdy': number; //The accessibility id of the node.
    'componentId': number;  //The id of the view which the node in.
    'bundleName': string; //The bundle name.
    'componentType': string;  //The type of the event source component,such as button, chart.
    'inputType': number;  //The type of the input text.
    'text': string;       //The text of the node.
    'hintText': string;   //The hint text of the node.
    'description': string; // The description of the node.
    'resourceName': string; //The resource name of the node.
    'childNodeIds': Array<number>;  //The list of the children node.
    'operations': Array<Operation>; //The operation list of the node.
    'textLengthLimit': number;  //The max text length of the node.
    'rect': Rect;       //The rect of the node.
    'checkable': boolean; //Whether the node can be check.
    'checked': boolean;   // Whether the node is checked.
    'focusable': boolean; //Whether the node can be focused.
    'focused': boolean; //Whether the node is focused.
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
    'error': boolean; //error information.
    'label': number;  // The label of the node.
    'beginSelected': number;  //The start position of text selected.
    'endSelected': number;  //The end position of text selected.
    // text move
    'textMoveUnit': TextMoveUnit; //The movement step used for reading texts.
    // WindowInfo
    'screenRect': Rect;
    'id': number;
    'layer': number;
    'title': string;
    'type': WindowType;
    'childIds': Array<number>;
    'parentId': number;
    'isAccessibilityFocused': boolean;
    'isActive': boolean;
    'isFocused': boolean;
    'anchor': AccessibilityElement;
    'rootElement': AccessibilityElement;
    'parent': AccessibilityElement;
    'childs': Array<AccessibilityElement>;
  };
  type FindElementCondition = {
    'content': string;  //Find node list information through text.
    'focusType': FocusType; //Obtains information about the node that gains accessibility focus.
    'focusDirection' : FocusDirection;  //The information of nodes near focus is retrieved according to the input direction.
    'parent': string;  //Gets the parent node information of this node.
    'child': string;  //Obtains information about the child node at a specified index.
  }

  // Defines an accessibility operation.
  interface Operation {
    /**
     * The type of the operation.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    readonly type: Action;

    /**
     * The description of the operation.
     * @default -
     * @since 9
     * @sysCap AccessibilityAbility
     */
    readonly description: string;
  }

  interface WindowInfo {
    /**
     * The rect of the window.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly screenRect: Rect;

    /**
     * The id of the window.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly id: number;

    /**
     * The layer of the window.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly layer: number;

    /**
     * The type of the window.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly type: WindowType;

    /**
     * The accessibility focus of the window.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly isAccessibilityFocused: boolean;

    /**
     * The status of the window is active or not.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly isActive: boolean;

    /**
     * The window gained focus or not.
     * @default -
     * @since 9
     * @sysCap Accessibility
     */
    readonly isFocused: boolean;

    /**
     * Obtains an anchor accessibility node that anchors this window to another window.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return An anchor accessibility node.
     */
    getAnchorElementInfo(callback: AsyncCallback<AccessibilityElement>): void;
    getAnchorElementInfo(): Promise<AccessibilityElement>;

    /**
     * Obtains an root accessibility in the active window.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param -
     * @return An anchor accessibility node.
     */
    getRootElementInfo(callback: AsyncCallback<AccessibilityElement>): void;
    getRootElementInfo(): Promise<AccessibilityElement>;

    /**
     * Obtains an parent AccessibilityWindowInfo instance.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param options The object of window
     * @return The instance of parent Accessibility WindowInfo.
     */
    getParent(callback: AsyncCallback<WindowInfo>): void;
    getParent(): Promise<WindowInfo>;

    /**
     * Obtains an child AccessibilityWindowInfo instance.
     * @since 9
     * @sysCap AccessibilityAbility
     * @param index The index of child window
     * @return The instance of Accessibility WindowInfo.
     */
    getChild(index: number, callback: AsyncCallback<WindowInfo>): void;
    getChild(index: number): Promise<WindowInfo>;
  }

}
export default accessibility;