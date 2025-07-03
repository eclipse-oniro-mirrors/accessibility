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

import { BusinessError，AsyncCallback } from '@ohos.base';

export namespace config {
    loadLibrary("AccessibilityConfig");

    let highContrastText: Config<Boolean> =  new HighContrastTextConfig();

    interface Config<T> {
        set(value: T): Promise<void>;
        set(value: T, callback: AsyncCallback<void>): void;
        get(): Promise<T>;
        get(callback: AsyncCallback<T>): void;
    }

    class HighContrastTextConfig implements Config<Boolean> {
        native setSync(value: boolean): void;
        native getSync(): boolean;

        set(value: Boolean): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        set(value: Boolean, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError());
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError);
            });
        }

        get(): Promise<Boolean> {
            return new Promise<Boolean>((resolve, reject): void => {
                taskpool.execute((): Boolean => {
                    return this.getSync();
                })
                .then((ret: NullishType): void => {
                    resolve(ret as Boolean);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        get(callback: AsyncCallback<Boolean>): void {
            taskpool.execute((): Boolean => {
                return this.getSync();
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as Boolean);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, false);
            });
        }
    }
}