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
import type { AsyncCallback, Callback } from './basic';

/**
 * Configuration of the accessibility.
 *
 * @namespace config
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @systemapi
 * @since 9
 */
declare namespace config {
  /**
   * Indicates the configuration of high-contrast text.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let highContrastText: Config<boolean>;
  /**
   * Indicates the configuration of invert color.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let invertColor: Config<boolean>;
  /**
   * Indicates the configuration of daltonization color filter.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let daltonizationColorFilter: Config<DaltonizationColorFilter>;
  /**
   * Indicates the configuration of content timeout.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let contentTimeout: Config<number>;
  /**
   * Indicates the configuration of animation state.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let animationOff: Config<boolean>;
  /**
   * Indicates the configuration of brightness discount.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let brightnessDiscount: Config<number>;
  /**
   * Indicates the configuration of screen magnifier.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let screenMagnifier: Config<boolean>;
  /**
   * Indicates the configuration of audio mono.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let audioMono: Config<boolean>;
  /**
   * Indicates the configuration of audio balance.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let audioBalance: Config<number>;
  /**
   * Indicates the configuration of mouse key state.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let mouseKey: Config<boolean>;
  /**
   * Indicates the configuration of mouse auto click.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let mouseAutoClick: Config<number>;
  /**
   * Indicates the configuration of short key state.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let shortkey: Config<boolean>;
  /**
   * Indicates the configuration of short key target.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let shortkeyTarget: Config<string>;
  /**
   * Indicates the configuration of captions state.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let captions: Config<boolean>;
  /**
   * Indicates the configuration of captions style.
   *
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  let captionsStyle: Config<accessibility.CaptionsStyle>;

  /**
   * Enable the accessibility extension ability.
   *
   * @param { string } name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   * @param { Array<accessibility.Capability> } capability Indicates the ability.
   * @returns { Promise<void> }
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Input parameter error.
   * @throws { BusinessError } 9300001 - Invalid bundle name or ability name.
   * @throws { BusinessError } 9300002 - Target ability already enabled.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function enableAbility(name: string, capability: Array<accessibility.Capability>): Promise<void>;

  /**
   * Enable the accessibility extension ability.
   *
   * @param { string } name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   * @param { Array<accessibility.Capability> } capability Indicates the ability.
   * @param { AsyncCallback<void> } callback
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Not system App.
   * @throws { BusinessError } 401 - Input parameter error.
   * @throws { BusinessError } 9300001 - Invalid bundle name or ability name.
   * @throws { BusinessError } 9300002 - Target ability already enabled.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function enableAbility(
    name: string,
    capability: Array<accessibility.Capability>,
    callback: AsyncCallback<void>
  ): void;

  /**
   * Disable the accessibility extension ability.
   *
   * @param { string } name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   * @returns { Promise<void> }
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 401 - Input parameter error.
   * @throws { BusinessError } 9300001 - Invalid bundle name or ability name.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function disableAbility(name: string): Promise<void>;

  /**
   * Disable the accessibility extension ability.
   *
   * @param { string } name Indicates the accessibility extension name, in "bundleName/abilityName" format.
   * @param { AsyncCallback<void> } callback
   * @throws { BusinessError } 201 - Permission denied.
   * @throws { BusinessError } 202 - Not system App.
   * @throws { BusinessError } 401 - Input parameter error.
   * @throws { BusinessError } 9300001 - Invalid bundle name or ability name.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function disableAbility(name: string, callback: AsyncCallback<void>): void;

  /**
   * Register the listener that watches for changes in the enabled status of accessibility extensions.
   *
   * @param { 'enabledAccessibilityExtensionListChange' } type Indicates the type of event.
   * @param { Callback<void> } callback Indicates the listener.
   * @throws { BusinessError } 401 - Input parameter error.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function on(type: 'enabledAccessibilityExtensionListChange', callback: Callback<void>): void;

  /**
   * Deregister listener that watches for changes in the enabled status of accessibility extensions.
   *
   * @param { 'enabledAccessibilityExtensionListChange' } type Indicates the type of event.
   * @param { Callback<void> } callback Indicates the listener.
   * @throws { BusinessError } 401 - Input parameter error.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  function off(type: 'enabledAccessibilityExtensionListChange', callback?: Callback<void>): void;

  /**
   * Indicates setting, getting, and listening to changes in configuration.
   *
   * @typedef Config
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @systemapi
   * @since 9
   */
  interface Config<T> {
    /**
     * Setting configuration value.
     *
     * @param { T } value Indicates the value.
     * @returns { Promise<void> }
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    set(value: T): Promise<void>;

    /**
     * Setting configuration value.
     *
     * @param { T } value Indicates the value.
     * @param { AsyncCallback<void> } callback
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 202 - Not system App.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    set(value: T, callback: AsyncCallback<void>): void;

    /**
     * Getting configuration value.
     *
     * @returns { Promise<T> }
     * @throws { BusinessError } 201 - Permission denied.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    get(): Promise<T>;

    /**
     * Getting configuration value.
     *
     * @param { AsyncCallback<T> } callback
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 202 - Not system App.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    get(callback: AsyncCallback<T>): void;

    /**
     * Register the listener to listen for configuration changes.
     *
     * @param { Callback<T> } callback Indicates the listener.
     * @throws { BusinessError } 201 - Permission denied.
     * @throws { BusinessError } 401 - Input parameter error.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    on(callback: Callback<T>): void;

    /**
     * Deregister the listener to listen for configuration changes.
     *
     * @param { Callback<T> } callback Indicates the listener.
     * @syscap SystemCapability.BarrierFree.Accessibility.Core
     * @systemapi
     * @since 9
     */
    off(callback?: Callback<T>): void;
  }

  /**
   * Indicates the type of daltonization color filter.
   *
   * @systemapi
   */
  type DaltonizationColorFilter = 'Normal' | 'Protanomaly' | 'Deuteranomaly' | 'Tritanomaly';
}
export default config;
