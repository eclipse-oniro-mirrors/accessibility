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

import { AccessibilityError } from '@ohos.accessibility';

export namespace config {
    loadLibrary("AccessibilityConfig");

    let highContrastText: HighContrastTextConfig =  new HighContrastTextConfig();

    export class HighContrastTextConfig {
        native function setSync(value: boolean): void;
        native function getSync(): boolean;

        function set(value: boolean): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as AccessibilityError);
                });
            });
        }

        function set(value: boolean, callback:(err: AccessibilityError) => void): void {
            taskpool.execute((): void => {
                return this.setSync(value);
            })
            .then((ret: NullishType): void => {
                callback(new AccessibilityError(0, ""));
            })
            .catch((ret: NullishType): void => {
                callback(ret as AccessibilityError);
            });
        }

        function get(): Promise<boolean> {
            return new Promise<boolean>((resolve, reject): void => {
                taskpool.execute((): boolean => {
                    return this.getSync();
                })
                .then((ret: NullishType): void => {
                    resolve(ret as boolean);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as AccessibilityError);
                });
            });
        }

        function get(callback:(err: AccessibilityError, data: boolean) => void): void {
            taskpool.execute((): boolean => {
                return this.getSync();
            })
            .then((ret: NullishType): void => {
                callback(new AccessibilityError(0, ""), ret as boolean);
            })
            .catch((ret: NullishType): void => {
                callback(ret as AccessibilityError, false);
            });
        }
    }
}