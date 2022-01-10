/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import { Rect } from './rect'
//@note hide
export interface DisplayResizeInfo {
  /**
   * The rect of the display resized.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  rect: Rect;

  /**
   * The scale of the display resized.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  scale: number;

  /**
   * the x coordinate of the resize center.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  centerX: number;

  /**
   * the y coordinate of the resize center.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  centerY: number;
}