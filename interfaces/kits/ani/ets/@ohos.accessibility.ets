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

import { BusinessError, AsyncCallback, Callback} from '@ohos.base';
import { Resource } from 'global.resource';

native function sendAccessibilityEventSync(event: accessibility.EventInfo): void;

/**
 * AccessibilityEvent type
 *
 * @enum { number }
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
export enum AccessibilityEventType {
  //EventType
  /**
   * AccessibilityFocus type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_ACCESSIBILITY_FOCUS = 0,

  /**
   * AccessibilityFocusClear type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_ACCESSIBILITY_FOCUS_CLEAR = 1,

  /**
   * Click type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_CLICK = 2,

  /**
   * LongClick type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_LONG_CLICK = 3,

  /**
   * Select type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SELECT = 4,

  /**
   * HoverEnter type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_HOVER_ENTER = 5,

  /**
   * HoverExit type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_HOVER_EXIT = 6,

  /**
   * Focus type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOCUS = 7,

  /**
   * TextUpdate type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TEXT_UPDATE = 8,

  /**
   * TextSelectionUpdate type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TEXT_SELECTION_UPDATE = 9,

  /**
   * Scroll type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SCROLL = 10,

  /**
   * RequestFocusForAccessibility type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY = 11,

  /**
   * AnnounceForAccessibility type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_ANNOUNCE_FOR_ACCESSIBILITY = 12,

  /**
   * RequestFocusForAccessibilityNotInterrupt type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT = 13,

  /**
   * AnnounceForAccessibilityNotInterrupt type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT = 14,

  /**
   * ElementInfoChange type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_ELEMENT_INFO_CHANGE = 15,

  /**
   * Scrolling type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SCROLLING = 16,

  // WindowUpdateType
  /**
   * Window add type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_ADD = 17,

  /**
   * Window remove type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_REMOVE = 18,

  /**
   * Window bounds type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_BOUNDS = 19,

  /**
   * Window active type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_ACTIVE = 20,

  /**
   * Window focus type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_FOCUS = 21,

  /**
   * Window property type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_PROPERTY = 22,

  /**
   * Window layer type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_WINDOW_LAYER = 23,

  //TouchGuideType
  /**
   * TouchBegin type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TOUCH_BEGIN = 24,

  /**
   * TouchEnd type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TOUCH_END = 25,

  //PageUpdateType
  /**
   * PageContentUpdate type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_PAGE_CONTENT_UPDATE = 26,

  /**
   * PageStateUpdate type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_PAGE_STATE_UPDATE = 27,

  /**
   * PageOpen type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_PAGE_OPEN = 28,

  /**
   * PageClose type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_PAGE_CLOSE = 29,

  //GestureType
  /**
   * left type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_LEFT = 30,

  /**
   * leftThenRight type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_LEFT_THEN_RIGHT = 31,

  /**
   * leftThenUp type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_LEFT_THEN_UP = 32,

  /**
   * leftThenDown type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_LEFT_THEN_DOWN = 33,

  /**
   * right type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_RIGHT = 34,

  /**
   * rightThenLeft type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_RIGHT_THEN_LEFT = 35,

  /**
   * rightThenUp type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_RIGHT_THEN_UP = 36,

  /**
   * rightThenDown type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_RIGHT_THEN_DOWN = 37,

  /**
   * up type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_UP = 38,

  /**
   * upThenLeft type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_UP_THEN_LEFT = 39,

  /**
   * upThenRight type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_UP_THEN_RIGHT = 40,

  /**
   * upThenDown type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_UP_THEN_DOWN = 41,

  /**
   * down type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_DOWN = 42,

  /**
   * downThenLeft type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_DOWN_THEN_LEFT = 43,

  /**
 * downThenRight type
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
  TYPE_SWIPE_DOWN_THEN_RIGHT = 44,

  /**
   * downThenUp type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_SWIPE_DOWN_THEN_UP = 45,

  /**
   * twoFingerSingleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TWO_FINGER_SINGLE_TAP = 46,

  /**
   * twoFingerDoubleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TWO_FINGER_DOUBLE_TAP = 47,

  /**
   * twoFingerDoubleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD = 48,

  /**
 * twoFingerTripleTap type
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
  TYPE_TWO_FINGER_TRIPLE_TAP = 49,

  /**
   * twoFingerTripleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD = 50,

  /**
   * threeFingerSingleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_SINGLE_TAP = 51,

  /**
   * threeFingerDoubleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_DOUBLE_TAP = 52,

  /**
   * threeFingerDoubleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD = 53,

  /**
 * threeFingerTripleTap type
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
  TYPE_THREE_FINGER_TRIPLE_TAP = 54,

  /**
   * threeFingerTripleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD = 55,

  /**
   * fourFingerSingleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_SINGLE_TAP = 56,

  /**
   * fourFingerDoubleTap type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_DOUBLE_TAP = 57,

  /**
   * fourFingerDoubleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD = 58,

  /**
 * fourFingerTripleTap type
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
  TYPE_FOUR_FINGER_TRIPLE_TAP = 59,

  /**
   * fourFingerTripleTapAndHold type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD = 60,

  /**
   * threeFingerSwipeUp type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_SWIPE_UP = 61,

  /**
   * threeFingerSwipeDown type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_SWIPE_DOWN = 62,

  /**
   * threeFingerSwipeLeft type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_SWIPE_LEFT = 63,

  /**
   * threeFingerSwipeRight type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_THREE_FINGER_SWIPE_RIGHT = 64,

  /**
   * fourFingerSwipeUp type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_SWIPE_UP = 65,

  /**
   * fourFingerSwipeDown type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_SWIPE_DOWN = 66,

  /**
 * fourFingerSwipeLeft type
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 20
 * @arkts 1.1&1.2
 */
  TYPE_FOUR_FINGER_SWIPE_LEFT = 67,

  /**
   * fourFingerSwipeRight type
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 20
   * @arkts 1.1&1.2
   */
  TYPE_FOUR_FINGER_SWIPE_RIGHT = 68
}

namespace accessibility {
    loadLibrary("accessibility_ani");

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

    export type observerType = 'accessibilityStateChange' | 'touchGuideStateChange' | 'screenReaderStateChange';

    export type AbilityType = 'audible' | 'generic' | 'haptic' | 'spoken' | 'visual' | 'all';

    export type AbilityState = 'enable' | 'disable' | 'install';

    export type Capability = 'retrieve' | 'touchGuide' | 'keyEventObserver' | 'zoom' | 'gesture';

    export native function isOpenTouchGuideSync(): boolean;

    export native function isOpenAccessibilitySync(): boolean;

    export native function isScreenReaderOpenSync(): boolean;

    export native function getAccessibilityExtensionListSync(abilityType: AbilityType, stateType: AbilityState
        ): Array<AccessibilityAbilityInfo>;

    export function getAccessibilityExtensionList(abilityType: AbilityType,
        stateType: AbilityState): Promise<Array<AccessibilityAbilityInfo>> {
        return new Promise<Array<AccessibilityAbilityInfo>> ((resolve: (v: Array<AccessibilityAbilityInfo>) => void,
            reject: (error: BusinessError) => void): void => {
            taskpool.execute(getAccessibilityExtensionListSync, abilityType, stateType).then((code :NullishType): void => {
                resolve(code as Array<AccessibilityAbilityInfo>);
            }).catch((err: NullishType): void => {
                reject(err as BusinessError);
            });
        });
    }

    export function getAccessibilityExtensionList(abilityType: AbilityType, stateType: AbilityState,
        callback: AsyncCallback<Array<AccessibilityAbilityInfo>>
    ): void {
        let p1 = taskpool.execute(getAccessibilityExtensionListSync, abilityType, stateType);
        p1.then((ret: NullishType): void => {
            callback(new BusinessError(), ret as Array<AccessibilityAbilityInfo>)
        }).catch((err: NullishType): void => {
            callback(err as BusinessError, [] as Array<AccessibilityAbilityInfo>)
        });
    }

    native function onStateChange(type: observerType, callback: Callback<boolean>): void;

    export function onAccessibilityStateChange(type: 'accessibilityStateChange', callback: Callback<boolean>): void {
        onStateChange(type, callback);
    }

    export function onTouchGuideStateChange(type: 'touchGuideStateChange', callback: Callback<boolean>): void {
        onStateChange(type, callback);
    }

    export function onScreenReaderStateChange(type: 'screenReaderStateChange', callback: Callback<boolean>): void {
        onStateChange(type, callback);
    }

    export overload on { onAccessibilityStateChange, onTouchGuideStateChange, onScreenReaderStateChange };

    native function offStateChange(type: observerType, callback: Callback<boolean>): void;

    native function offAll(type: observerType): void;

    export function offAccessibilityStateChange(type: 'accessibilityStateChange', callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll(type);
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange(type, callbackFunc);
        }
    }

    export function offTouchGuideStateChange(type: 'touchGuideStateChange', callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll(type);
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange(type, callbackFunc);
        }
    }

    export function offScreenReaderStateChange(type: 'screenReaderStateChange', callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll(type);
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange(type, callbackFunc);
        }
    }

    export overload off { offAccessibilityStateChange, offTouchGuideStateChange, offScreenReaderStateChange };

    export type CaptionsFontEdgeType = 'none' | 'raised' | 'depressed' | 'uniform' | 'dropShadow';

    export type CaptionsFontFamily = 'default' | 'monospacedSerif' | 'serif' | 'monospacedSansSerif' | 'sansSerif' |
        'casual' | 'cursive' | 'smallCapitals';

    export interface CaptionsStyle {
        fontFamily: CaptionsFontFamily;

        fontScale: int;

        fontColor: int | string;

        fontEdgeType: CaptionsFontEdgeType;

        backgroundColor: int | string;

        windowColor: int | string;
    }

    export class CaptionsStyleImpl implements CaptionsStyle {
        fontFamily: CaptionsFontFamily = 'default';

        fontScale: int = 75;

        fontColor: int | string = 0xff0000;

        fontEdgeType: CaptionsFontEdgeType = 'none';

        backgroundColor: int | string = 0xff0000;

        windowColor: int | string = 0xff0000;
    }

    export interface AccessibilityAbilityInfo {
        id: string;
        name: string;
        bundleName: string;
        targetBundleNames: Array<string>;
        abilityTypes: Array<AbilityType>;
        capabilities: Array<Capability>;
        description: string;
        eventTypes: Array<EventType>;
        needHide: boolean;
        label: string;
    }

    class AccessibilityAbilityInfoImpl implements AccessibilityAbilityInfo {
        id: string = "";
        name: string = "";
        bundleName: string = "";
        targetBundleNames: Array<string> = [];
        abilityTypes: Array<AbilityType> = [];
        capabilities: Array<Capability> = [];
        description: string = "";
        eventTypes: Array<EventType> = [];
        needHide: boolean = false;
        label: string = "";
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
        pageId?: int;
        description?: string;
        triggerAction: Action;
        textMoveUnit?: TextMoveUnit;
        contents?: Array<string>;
        lastContent?: string;
        beginIndex?: int;
        currentIndex?: int;
        endIndex?: int;
        itemCount?: int;
        elementId?: int;
        textAnnouncedForAccessibility?: string;
        textResourceAnnouncedForAccessibility?: Resource;
        customId?: string;
    }

    export function sendAccessibilityEventWithCallback(event: EventInfo, callback: AsyncCallback<void>): void {
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

    export function sendAccessibilityEventReturnsPromise(event: EventInfo): Promise<void> {
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

    export overload sendAccessibilityEvent { sendAccessibilityEventWithCallback, sendAccessibilityEventReturnsPromise };
}
export default accessibility;
