/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import accessibility from './@ohos.accessibility'
import AccessibilityExtensionContext, { AccessibilityElement } from './application/AccessibilityExtensionContext'
import { KeyEvent } from './@ohos.multimodalInput.keyEvent'

/**
 * class of accessibility extension ability.
 *
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
export default class AccessibilityExtensionAbility {
    /**
     * Indicates accessibility extension ability context.
     */
    context: AccessibilityExtensionContext;

    /**
     * Called when extension ability is connected.
     */
    onConnect(): void;

    /**
     * Called when extension ability is disconnected.
     */
    onDisconnect(): void;

    /**
     * Called when an accessibility event occurs, such as when the user touches the application interface.
     * @param event Indicates an accessibility event.
     */
    onAccessibilityEvent(event: AccessibilityEvent): void;

    /**
     * Called when a physical key is pressed, such as when the user presses the volume button .
     * @param keyEvent Indicates the physical key event.
     */
    onKeyEvent(keyEvent: KeyEvent): boolean;
}

/**
 * Indicates the accessibility event.
 * 
 * It provides the event type and the target element of the event if any.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
declare interface AccessibilityEvent {
    eventType: accessibility.EventType | accessibility.WindowUpdateType |
        TouchGuideType | GestureType | PageUpdateType;
    target?: AccessibilityElement;
    timeStamp?: number;
}

/**
 * Indicates the gusture type.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type GestureType = 'left' | 'leftThenRight' | 'leftThenUp' | 'leftThenDown' |
    'right' | 'rightThenLeft' | 'rightThenUp' | 'rightThenDown' |
    'up' | 'upThenLeft' | 'upThenRight' | 'upThenDown' |
    'down' | 'downThenLeft' | 'downThenRight' | 'downThenUp';

/**
 * Indicates the page update type.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type PageUpdateType = 'pageContentUpdate' | 'pageStateUpdate';

/**
 * Indicates the type of touch event during touch browsing.
 * @since 9
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 */
type TouchGuideType = 'touchBegin' | 'touchEnd';