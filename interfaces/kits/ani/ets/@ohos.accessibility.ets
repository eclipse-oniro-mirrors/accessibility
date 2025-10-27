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

export enum AccessibilityEventType {
  TYPE_ACCESSIBILITY_FOCUS = 0,
  TYPE_ACCESSIBILITY_FOCUS_CLEAR = 1,
  TYPE_CLICK = 2,
  TYPE_LONG_CLICK = 3,
  TYPE_SELECT = 4,
  TYPE_HOVER_ENTER = 5,
  TYPE_HOVER_EXIT = 6,
  TYPE_FOCUS = 7,
  TYPE_TEXT_UPDATE = 8,
  TYPE_TEXT_SELECTION_UPDATE = 9,
  TYPE_SCROLL = 10,
  TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY = 11,
  TYPE_ANNOUNCE_FOR_ACCESSIBILITY = 12,
  TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT = 13,
  TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT = 14,
  TYPE_ELEMENT_INFO_CHANGE = 15,
  TYPE_SCROLLING = 16,
  TYPE_WINDOW_ADD = 17,
  TYPE_WINDOW_REMOVE = 18,
  TYPE_WINDOW_BOUNDS = 19,
  TYPE_WINDOW_ACTIVE = 20,
  TYPE_WINDOW_FOCUS = 21,
  TYPE_WINDOW_PROPERTY = 22,
  TYPE_WINDOW_LAYER = 23,
  TYPE_TOUCH_BEGIN = 24,
  TYPE_TOUCH_END = 25,
  TYPE_PAGE_CONTENT_UPDATE = 26,
  TYPE_PAGE_STATE_UPDATE = 27,
  TYPE_PAGE_OPEN = 28,
  TYPE_PAGE_CLOSE = 29,
  TYPE_SWIPE_LEFT = 30,
  TYPE_SWIPE_LEFT_THEN_RIGHT = 31,
  TYPE_SWIPE_LEFT_THEN_UP = 32,
  TYPE_SWIPE_LEFT_THEN_DOWN = 33,
  TYPE_SWIPE_RIGHT = 34,
  TYPE_SWIPE_RIGHT_THEN_LEFT = 35,
  TYPE_SWIPE_RIGHT_THEN_UP = 36,
  TYPE_SWIPE_RIGHT_THEN_DOWN = 37,
  TYPE_SWIPE_UP = 38,
  TYPE_SWIPE_UP_THEN_LEFT = 39,
  TYPE_SWIPE_UP_THEN_RIGHT = 40,
  TYPE_SWIPE_UP_THEN_DOWN = 41,
  TYPE_SWIPE_DOWN = 42,
  TYPE_SWIPE_DOWN_THEN_LEFT = 43,
  TYPE_SWIPE_DOWN_THEN_RIGHT = 44,
  TYPE_SWIPE_DOWN_THEN_UP = 45,
  TYPE_TWO_FINGER_SINGLE_TAP = 46,
  TYPE_TWO_FINGER_DOUBLE_TAP = 47,
  TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD = 48,
  TYPE_TWO_FINGER_TRIPLE_TAP = 49,
  TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD = 50,
  TYPE_THREE_FINGER_SINGLE_TAP = 51,
  TYPE_THREE_FINGER_DOUBLE_TAP = 52,
  TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD = 53,
  TYPE_THREE_FINGER_TRIPLE_TAP = 54,
  TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD = 55,
  TYPE_FOUR_FINGER_SINGLE_TAP = 56,
  TYPE_FOUR_FINGER_DOUBLE_TAP = 57,
  TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD = 58,
  TYPE_FOUR_FINGER_TRIPLE_TAP = 59,
  TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD = 60,
  TYPE_THREE_FINGER_SWIPE_UP = 61,
  TYPE_THREE_FINGER_SWIPE_DOWN = 62,
  TYPE_THREE_FINGER_SWIPE_LEFT = 63,
  TYPE_THREE_FINGER_SWIPE_RIGHT = 64,
  TYPE_FOUR_FINGER_SWIPE_UP = 65,
  TYPE_FOUR_FINGER_SWIPE_DOWN = 66,
  TYPE_FOUR_FINGER_SWIPE_LEFT = 67,
  TYPE_FOUR_FINGER_SWIPE_RIGHT = 68
}

namespace accessibility {
    loadLibrary("accessibility_ani");

    export type EventType = 'accessibilityFocus' | 'accessibilityFocusClear' |
        'click' | 'longClick' | 'focus' | 'select' | 'hoverEnter' | 'hoverExit' |
        'textUpdate' | 'textSelectionUpdate' | 'scroll' | 'requestFocusForAccessibility' |
        'announceForAccessibility' | 'requestFocusForAccessibilityNotInterrupt' |
        'announceForAccessibilityNotInterrupt' | 'scrolling';

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
            taskpool.execute(getAccessibilityExtensionListSync, abilityType, stateType).then((code :Any): void => {
                if (Array.isArray(code)) {
                    resolve(code as Array<AccessibilityAbilityInfo>);
                } else {
                    resolve([] as Array<AccessibilityAbilityInfo>);
                }
            }).catch((err: Any): void => {
                reject(err as BusinessError);
            });
        });
    }

    export function getAccessibilityExtensionList(abilityType: AbilityType, stateType: AbilityState,
        callback: AsyncCallback<Array<AccessibilityAbilityInfo>>
    ): void {
        let p1 = taskpool.execute(getAccessibilityExtensionListSync, abilityType, stateType);
        p1.then((ret: Any): void => {
            if (Array.isArray(ret)) {
                callback(new BusinessError(), ret as Array<AccessibilityAbilityInfo>);
            } else {
                callback(new BusinessError(), [] as Array<AccessibilityAbilityInfo>);
            }
        }).catch((err: Any): void => {
            callback(err as BusinessError, [] as Array<AccessibilityAbilityInfo>)
        });
    }

    native function onStateChange(type: observerType, callback: Callback<boolean>): void;

    export function onAccessibilityStateChange(callback: Callback<boolean>): void {
        onStateChange('accessibilityStateChange', callback);
    }

    export function onTouchGuideStateChange(callback: Callback<boolean>): void {
        onStateChange('touchGuideStateChange', callback);
    }

    export function onScreenReaderStateChange(callback: Callback<boolean>): void {
        onStateChange('screenReaderStateChange', callback);
    }

    native function offStateChange(type: observerType, callback: Callback<boolean>): void;

    native function offAll(type: observerType): void;

    export function offAccessibilityStateChange(callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll('accessibilityStateChange');
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange('accessibilityStateChange', callbackFunc);
        }
    }

    export function offTouchGuideStateChange(callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll('touchGuideStateChange');
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange('touchGuideStateChange', callbackFunc);
        }
    }

    export function offScreenReaderStateChange(callback?: Callback<boolean>): void {
        if (callback === undefined) {
            offAll('screenReaderStateChange');
        } else {
            let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
            offStateChange('screenReaderStateChange', callbackFunc);
        }
    }

    export native function getCaptionsManager(): CaptionsManager;

    export interface CaptionsManager {
        enabled: boolean;
        style: CaptionsStyle;
        onEnableChange(callback: Callback<boolean>): void;
        onStyleChange(callback: Callback<CaptionsStyle>): void;
        offEnableChange(callback?: Callback<boolean>): void;
        offStyleChange(callback?: Callback<CaptionsStyle>): void;
    }

    export class CaptionsManagerImpl implements CaptionsManager {
        native getEnabled(): boolean;
        native getStyle(): CaptionsStyle;
        native setEnabled(value: boolean): void;
        native setStyle(value: CaptionsStyle): void;
        get enabled(): boolean {
            return this.getEnabled();
        }
        get style(): CaptionsStyle {
            return this.getStyle();
        }
        set enabled(value: boolean) {
            return this.setEnabled(value);
        }
        set style(value: CaptionsStyle) {
            return this.setStyle(value);
        }

        native onEnableChange(callback: Callback<boolean>): void;
        native onStyleChange(callback: Callback<CaptionsStyle>): void;

        native offEnableChangeWithCallback(callback: Callback<boolean>): void;
        native offEnableChangeAll(): void;
        native offStyleChangeWithCallback(callback: Callback<CaptionsStyle>): void;
        native offStyleChangeAll(): void;
        offEnableChange(callback?: Callback<boolean>): void {
            if (callback === undefined) {
                this.offEnableChangeAll();
            } else {
                let callbackFunc: Callback<boolean> = callback as Callback<boolean>;
                this.offEnableChangeWithCallback(callbackFunc);
            }
        }
        offStyleChange(callback?: Callback<CaptionsStyle>): void {
            if (callback === undefined) {
                this.offStyleChangeAll();
            } else {
                let callbackFunc: Callback<CaptionsStyle> = callback as Callback<CaptionsStyle>;
                this.offStyleChangeWithCallback(callbackFunc);
            }
        }
    }

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

    export function sendAccessibilityEvent(event: EventInfo, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            return sendAccessibilityEventSync(event);
        })
        .then((ret: Any): void => {
            callback(new BusinessError(), ret as undefined);
        })
        .catch((ret: Any): void => {
            callback(ret as BusinessError, undefined);
        });
    }

    export function sendAccessibilityEvent(event: EventInfo): Promise<void> {
        return new Promise<void>((resolve, reject): void => {
            taskpool.execute((): void => {
                return sendAccessibilityEventSync(event);
            })
            .then((ret: Any): void => {
                resolve(ret as undefined);
            })
            .catch((ret: Any): void => {
                reject(ret as BusinessError);
            });
        });
    }
}
export default accessibility;
