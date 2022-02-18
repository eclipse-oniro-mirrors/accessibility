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
//define for progressbar
export interface RangeInfo {
  /**
   * The min of the ProgressBar.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  min: number;

  /**
   * The max of the ProgressBar.
   * @default 100
   * @since 3
   * @sysCap Accessibility
   */
  max: number;

  /**
   * The current value of the ProgressBar.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  current: number;
}