/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
//@note hide
export class GesturePath {
  /**
  * The path list of the gesture.
  * @default -
  * @since 3
  * @sysCap AccessibilityAbility
  * @note The max number of the path is 10;
  */
  pos: Array<GesturePos>;

  /**
   * The start position of the gesture.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  startPos: GesturePos;

  /**
   * The end position of the gesture.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  endPos: GesturePos;

  /**
   * The duration time of the gesture.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   * @note The max of the durationtime is 60 * 1000 ms;
   */
  durationTime: number;
}
//@note hide
export class GesturePos {
  constructor(x: number, y: number);
  /**
   * The x coordinate of position.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  posX: number;

  /**
   * The y coordinate of position.
   * @default -
   * @since 3
   * @sysCap AccessibilityAbility
   */
  posY: number;
}