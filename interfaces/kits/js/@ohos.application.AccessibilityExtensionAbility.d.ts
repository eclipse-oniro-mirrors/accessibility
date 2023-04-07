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

import type accessibility from './@ohos.accessibility';
import type AccessibilityExtensionContext, { AccessibilityElement } from './application/AccessibilityExtensionContext';
import type { KeyEvent } from './@ohos.multimodalInput.keyEvent';

/**
 * class of accessibility extension ability.
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 9
 */
export default class AccessibilityExtensionAbility {
  /**
   * Indicates accessibility extension ability context.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 9
   */
  context: AccessibilityExtensionContext;

  /**
   * Called when extension ability is connected.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 9
   */
  onConnect(): void;

  /**
   * Called when extension ability is disconnected.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 9
   */
  onDisconnect(): void;

  /**
   * Called when an accessibility event occurs, such as when the user touches the application interface.
   *
   * @param { AccessibilityEvent } event Indicates an accessibility event.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 9
   */
  onAccessibilityEvent(event: AccessibilityEvent): void;

  /**
   * Called when a physical key is pressed, such as when the user presses the volume button .
   *
   * @param { KeyEvent } keyEvent Indicates the physical key event.
   * @returns { boolean }
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 9
   */
  onKeyEvent(keyEvent: KeyEvent): boolean;
}

/**
 * Indicates the accessibility event.
 * It provides the event type and the target element of the event if any.
 *
 * @typedef AccessibilityEvent
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 9
 */
declare interface AccessibilityEvent {
  eventType: accessibility.EventType | accessibility.WindowUpdateType | TouchGuideType | GestureType | PageUpdateType;
  target?: AccessibilityElement;
  timeStamp?: number;
}

/**
 * Indicates the gusture type.
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 9
 */
type GestureType = 'left' | 'leftThenRight' | 'leftThenUp' | 'leftThenDown' |
  'right' | 'rightThenLeft' | 'rightThenUp' | 'rightThenDown' |
  'up' | 'upThenLeft' | 'upThenRight' | 'upThenDown' |
  'down' | 'downThenLeft' | 'downThenRight' | 'downThenUp';

/**
 * Indicates the page update type.
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 9
 */
type PageUpdateType = 'pageContentUpdate' | 'pageStateUpdate';

/**
 * Indicates the type of touch event during touch browsing.
 *
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @since 9
 */
type TouchGuideType = 'touchBegin' | 'touchEnd';
