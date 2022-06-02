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
import inputEventClient from './@ohos.multimodalInput.inputEventClient'
/**
 * class of accessibility extension ability.
 *
 * @since 9
 * @syscap SystemCapability.DistributedDataManager.DataShare.Provider
 * @systemapi Hide this for inner system use.
 */
export default class AccessibilityExtensionAbility {
    /**
     * Indicates accessibility extension context.
     *
     * @since 9
     * @sysCap AccessibilityExtension
     * @systemapi hide for inner use.
     * @StageModelOnly
     */
    context: AccessibilityExtensionContext;

    /**
    * Called when your accessibility service is successfully connected to the OS. You can implement
    * this method to perform subsequent initialization operations.
    * @since 9
    * @note hide
    * @sysCap AccessibilityExtension
    * @param -
    * @return -
    */
    onConnect(): void;
    /**
     * Called when your accessibility service is successfully disconnected to the OS.
     * @since 9
     * @note hide
     * @sysCap AccessibilityExtension
     * @param -
     * @return -
     */
    onDisconnect(): void;

    // tree change
    onAccessibilityEvent(event: AccessibilityEvent): void;

    /**
     * Called when a key event occurs, for example, a user presses the key to increase or decrease the volume.
     * @since 7
     * @sysCap AccessibilityExtension
     * @param -
     * @return -
     */
    onKeyEvent(keyEvent: inputEventClient.KeyEvent): boolean;
}

declare interface AccessibilityEvent {
    eventType: accessibility.EventType | GestureType | NotificationType | WindowUpdateType;
    target?: AccessibilityElement;
    timeStamp?: number;
    notificationContent?: string;
}

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
* The category of the notification
* @note 应用Notification的具体分类，用于朗读通知信息时提示是哪类通知.
* @since 9
* @sysCap Accessibility
*/
type NotificationType = 'call' | 'msg' | 'email' | 'event' |
    'promo' | 'alarm' | 'progress' | 'social' | 'err' | 'transport' | 'sys' | 'service';

type WindowUpdateType = 'add' | 'remove' | 'bounds' | 'active' | 'focus';