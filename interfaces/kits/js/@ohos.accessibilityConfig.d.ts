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

import AccessibilityExtension from "./@ohos.application.AccessibilityExtension";
import accessibility from "./@ohos.accessibility"
import { AsyncCallback } from './basic';

declare namespace accessibilityConfig {
  /**
   * Enabled the accessibility abilities by names.
   * @since 9
   * @sysCap Accessibility
   * @param names The strings formated by 'bundleName/abilityName'.
   * @return Returns {@code true} if set extension disabled successed; returns {@code false} otherwise.
  */
  function extensionEnabled(names: string, capability: Array<accessibility.Capability>, callback: AsyncCallback<boolean>): void;
  function extensionEnabled(names: string, capability: Array<accessibility.Capability>): Promise<boolean>;

  /**
   * Disabled the accessibility abilities by names.
   * @since 9
   * @sysCap Accessibility
   * @param names The strings formated by 'bundleName/abilityName'.
   * @return Returns {@code true} if set extension disabled successed; returns {@code false} otherwise.
  */
  function extensionDisabled(names: string, callback: AsyncCallback<boolean>): void;
  function extensionDisabled(names: string): Promise<boolean>;
  var highContrastText: Config<boolean>;
  var invertColor: Config<boolean>;
  var daltonizationColorFilter: Config<DaltonizationColorFilter>;
  var contentTimeout: Config<number>;
  var animationOff: Config<boolean>;
  var brightnessDiscount: Config<number>;
  var screenMagnifier: Config<boolean>;
  var audioMono: Config<boolean>;
  var audioBalance: Config<number>;
  var mouseKey: Config<boolean>;
  var mouseAutoClick: Config<number>;
  var shortkey: Config<boolean>;
  var shortkey_target: Config<string>;
  var caption: Config<boolean>;
  var captionStyle: Config<accessibility.CaptionsStyle>;

  interface Config<T> {
    // permission: accessibility.config.write
    set(value: T, callback?: Function): void;
    // permission: accessibility.config.read
    get(callback?: (value: T) => void): void;
    // permission: accessibility.config.read
    on(callback: (value: T) => void): void;
    off(callback?: (value: T) => void): void;
  }

  type DaltonizationColorFilter = 'Normal' | 'Protanomaly' | 'Deuteranomaly' | 'Tritanomaly';

}
export default accessibilityConfig;