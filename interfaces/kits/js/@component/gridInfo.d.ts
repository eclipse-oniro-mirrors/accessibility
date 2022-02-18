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
export interface GridInfo {
  /**
   * Row count of the collection.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  rowCount: number;

  /**
   * Column count of the collection.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  columnCount: number;

  /**
   * Selection mode of collection.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  selectionMode: number;
}

export interface GridItemInfo {
  /**
   * Whether is head.
   * @default false
   * @since 3
   * @sysCap Accessibility
   */
  heading: boolean;

  /**
   * The index of the column.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  columnIndex: number;

  /**
   * The index of the row.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  rowIndex: number;

  /**
   * column span.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  columnSpan: number;

  /**
   * row span.
   * @default 0
   * @since 3
   * @sysCap Accessibility
   */
  rowSpan: number;

  /**
   * Whether is selected.
   * @default false
   * @since 3
   * @sysCap Accessibility
   */
  selected: boolean;

}