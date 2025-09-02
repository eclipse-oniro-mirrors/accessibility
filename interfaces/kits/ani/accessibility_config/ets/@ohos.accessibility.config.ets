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

import accessibility from '@ohos.accessibility';
import { AsyncCallback, BusinessError, Callback } from '@ohos.base';

native function enableAbilitySync(name: string, capability: Array<accessibility.Capability>): void;

native function disableAbilitySync(name: string): void;

namespace config {
    loadLibrary("accessibility_config");

    export type ObserverConfigType = 'enabledAccessibilityExtensionListChange' | 'installedAccessibilityListChange';

    export type DaltonizationColorFilter = 'Normal' | 'Protanomaly' | 'Deuteranomaly' | 'Tritanomaly';

    export type ClickResponseTime = 'Short' | 'Medium' | 'Long';

    export type RepeatClickInterval = 'Shortest' | 'Short' | 'Medium' | 'Long' | 'Longest';

    export function enableAbility(name: string, capability: Array<accessibility.Capability>): Promise<void> {
        return new Promise<void> ((resolve: (v: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                return enableAbilitySync(name, capability);
            })
            .then((ret: NullishType): void => {
                resolve(ret as undefined);
            })
            .catch((ret: NullishType): void => {
                reject(ret as BusinessError);
            });
        });
    }

    export function enableAbility(name: string, capability: Array<accessibility.Capability>,
        callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return enableAbilitySync(name, capability);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
    }

    export function disableAbility(name: string): Promise<void> {
        return new Promise<void> ((resolve: (v: undefined) => void, reject: (error: BusinessError) => void): void => {
            taskpool.execute((): void => {
                return disableAbilitySync(name);
            })
            .then((ret: NullishType): void => {
                resolve(ret as undefined);
            })
            .catch((ret: NullishType): void => {
                reject(ret as BusinessError);
            });
        });
    }

    export function disableAbility(name: string, callback: AsyncCallback<void>): void {
        taskpool.execute((): void => {
            return disableAbilitySync(name);
        })
        .then((ret: NullishType): void => {
            callback(new BusinessError(), ret as undefined);
        })
        .catch((ret: NullishType): void => {
            callback(ret as BusinessError, undefined);
        });
    }

    export native function on(type: ObserverConfigType, callback: Callback<void>): void;

    export native function offObserver(type: ObserverConfigType, callback: Callback<void>): void;

    export native function offObservers(type: ObserverConfigType): void;

    export function off (type: ObserverConfigType, callback?: Callback<void>): void {
        if (callback === undefined) {
            offObservers(type);
        } else {
            offObserver(type, callback);
        }
    }

    interface Config<T> {
        setReturnsPromise(value: T): Promise<void>;
        setWithCallback(value: T, callback: AsyncCallback<void>): void;
        getReturnsPromise(): Promise<T>;
        getWithCallback(callback: AsyncCallback<T>): void;
        overload set { setReturnsPromise, setWithCallback };
        overload get { getReturnsPromise, getWithCallback };
        on(callback: Callback<T>): void;
        off(callback?: Callback<T>): void;
    }

    enum CONFIG_ID {
        CONFIG_HIGH_CONTRAST_TEXT = 0,
        CONFIG_INVERT_COLOR,
        CONFIG_DALTONIZATION_COLOR_FILTER,
        CONFIG_CONTENT_TIMEOUT,
        CONFIG_ANIMATION_OFF,
        CONFIG_BRIGHTNESS_DISCOUNT,
        CONFIG_AUDIO_MONO,
        CONFIG_AUDIO_BALANCE,
        CONFIG_MOUSE_KEY,
        CONFIG_SHORT_KEY,
        CONFIG_CAPTION_STATE,
        CONFIG_CAPTION_STYLE,
        CONFIG_SCREEN_MAGNIFICATION,
        CONFIG_SHORT_KEY_TARGET,
        CONFIG_MOUSE_AUTOCLICK,
        CONFIG_DALTONIZATION_STATE,
        CONIFG_CLICK_RESPONSE_TIME,
        CONFIG_IGNORE_REPEAT_CLICK_STATE,
        CONFIG_IGNORE_REPEAT_CLICK_TIME,
        CONFIG_SHORT_KEY_MULTI_TARGET,
        CONFIG_ID_MAX,
    }

    export class ConfigImplBoolean implements Config<boolean> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: boolean): void;
        native getSync(id: CONFIG_ID): boolean;
        native onOnly(id: CONFIG_ID, callback: Callback<boolean>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: boolean) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: boolean): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: boolean, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<boolean> {
            return new Promise<boolean>((resolve, reject): void => {
                taskpool.execute((): boolean => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as boolean);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<boolean>): void {
            taskpool.execute((): boolean => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as boolean);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, false);
            });
        }

        off(callback?: Callback<boolean>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: boolean) => void) = callback as ((parameter: boolean) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<boolean>): void {
            this.onOnly(this.id, callback);
        }
    }

    export class ConfigImplInt implements Config<int> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: int): void;
        native getSync(id: CONFIG_ID): int;
        native onOnly(id: CONFIG_ID, callback: Callback<int>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: int) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: int): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: int, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<int> {
            return new Promise<int>((resolve, reject): void => {
                taskpool.execute((): int => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as int);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<int>): void {
            taskpool.execute((): int => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as int);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<int>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: int) => void) = callback as ((parameter: int) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<int>): void {
            this.onOnly(this.id, callback);
        }
    }

    export class ConfigImplDouble implements Config<double> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: double): void;
        native getSync(id: CONFIG_ID): double;
        native onOnly(id: CONFIG_ID, callback: Callback<double>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: double) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: double): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: double, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<double> {
            return new Promise<double>((resolve, reject): void => {
                taskpool.execute((): double => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as double);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<double>): void {
            taskpool.execute((): double => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as double);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<double>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: double) => void) = callback as ((parameter: double) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<double>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigImplString implements Config<string> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: string): void;
        native getSync(id: CONFIG_ID): string;
        native onOnly(id: CONFIG_ID, callback: Callback<string>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: string) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: string): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: string, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<string> {
            return new Promise<string>((resolve, reject): void => {
                taskpool.execute((): string => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as string);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<string>): void {
            taskpool.execute((): string => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as string);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<string>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: string) => void) = callback as ((parameter: string) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<string>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigImplArrString implements Config<Array<string>> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: Array<string>): void;
        native getSync(id: CONFIG_ID): Array<string>;
        native onOnly(id: CONFIG_ID, callback: Callback<Array<string>>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: Array<string>) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: Array<string>): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: Array<string>, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<Array<string>> {
            return new Promise<Array<string>>((resolve, reject): void => {
                taskpool.execute((): Array<string> => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as Array<string>);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<Array<string>>): void {
            taskpool.execute((): Array<string> => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as Array<string>);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<Array<string>>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: Array<string>) => void) = callback as ((parameter: Array<string>) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<Array<string>>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigImplDalColFilter implements Config<DaltonizationColorFilter> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: DaltonizationColorFilter): void;
        native getSync(id: CONFIG_ID): DaltonizationColorFilter;
        native onOnly(id: CONFIG_ID, callback: Callback<DaltonizationColorFilter>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: DaltonizationColorFilter) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: DaltonizationColorFilter): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: DaltonizationColorFilter, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<DaltonizationColorFilter> {
            return new Promise<DaltonizationColorFilter>((resolve, reject): void => {
                taskpool.execute((): DaltonizationColorFilter => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as DaltonizationColorFilter);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<DaltonizationColorFilter>): void {
            taskpool.execute((): DaltonizationColorFilter => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as DaltonizationColorFilter);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<DaltonizationColorFilter>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: DaltonizationColorFilter) => void) = callback as ((parameter: DaltonizationColorFilter) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<DaltonizationColorFilter>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigImplCliResTime implements Config<ClickResponseTime> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: ClickResponseTime): void;
        native getSync(id: CONFIG_ID): ClickResponseTime;
        native onOnly(id: CONFIG_ID, callback: Callback<ClickResponseTime>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: ClickResponseTime) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: ClickResponseTime): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: ClickResponseTime, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<ClickResponseTime> {
            return new Promise<ClickResponseTime>((resolve, reject): void => {
                taskpool.execute((): ClickResponseTime => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as ClickResponseTime);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<ClickResponseTime>): void {
            taskpool.execute((): ClickResponseTime => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as ClickResponseTime);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<ClickResponseTime>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: ClickResponseTime) => void) = callback as ((parameter: ClickResponseTime) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<ClickResponseTime>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigRepeateClickInterval implements Config<RepeatClickInterval> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: RepeatClickInterval): void;
        native getSync(id: CONFIG_ID): RepeatClickInterval;
        native onOnly(id: CONFIG_ID, callback: Callback<RepeatClickInterval>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: RepeatClickInterval) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: RepeatClickInterval): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: RepeatClickInterval, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<RepeatClickInterval> {
            return new Promise<RepeatClickInterval>((resolve, reject): void => {
                taskpool.execute((): RepeatClickInterval => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as RepeatClickInterval);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<RepeatClickInterval>): void {
            taskpool.execute((): RepeatClickInterval => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as RepeatClickInterval);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<RepeatClickInterval>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: RepeatClickInterval) => void) = callback as ((parameter: RepeatClickInterval) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<RepeatClickInterval>): void {
            this.onOnly(this.id, callback);
        }
    }

   export class ConfigImplCaptionsStyle implements Config<accessibility.CaptionsStyle> {
        id : CONFIG_ID = CONFIG_ID.CONFIG_ID_MAX;
        native setSync(id: CONFIG_ID, value: accessibility.CaptionsStyle): void;
        native getSync(id: CONFIG_ID): accessibility.CaptionsStyle;
        native onOnly(id: CONFIG_ID, callback: Callback<accessibility.CaptionsStyle>): void;
        native offOnly(id: CONFIG_ID): void;
        native offWithCallback(id: CONFIG_ID, callback: ((parameter: accessibility.CaptionsStyle) => void)): void;

        constructor(id: CONFIG_ID) {
            this.id = id;
        }

        setReturnsPromise(value: accessibility.CaptionsStyle): Promise<void> {
            return new Promise<void>((resolve, reject): void => {
                taskpool.execute((): void => {
                    return this.setSync(this.id, value);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as undefined);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        setWithCallback(value: accessibility.CaptionsStyle, callback: AsyncCallback<void>): void {
            taskpool.execute((): void => {
                return this.setSync(this.id, value);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as undefined);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        getReturnsPromise(): Promise<accessibility.CaptionsStyle> {
            return new Promise<accessibility.CaptionsStyle>((resolve, reject): void => {
                taskpool.execute((): accessibility.CaptionsStyle => {
                    return this.getSync(this.id);
                })
                .then((ret: NullishType): void => {
                    resolve(ret as accessibility.CaptionsStyle);
                })
                .catch((ret: NullishType): void => {
                    reject(ret as BusinessError);
                });
            });
        }

        getWithCallback(callback: AsyncCallback<accessibility.CaptionsStyle>): void {
            taskpool.execute((): accessibility.CaptionsStyle => {
                return this.getSync(this.id);
            })
            .then((ret: NullishType): void => {
                callback(new BusinessError(), ret as accessibility.CaptionsStyle);
            })
            .catch((ret: NullishType): void => {
                callback(ret as BusinessError, undefined);
            });
        }

        off(callback?: Callback<accessibility.CaptionsStyle>): void {
            if (callback === undefined) {
                this.offOnly(this.id);
            } else {
                let callbackFunc: ((parameter: accessibility.CaptionsStyle) => void) = callback as ((parameter: accessibility.CaptionsStyle) => void);
                this.offWithCallback(this.id, callbackFunc);
            }
        }

        on(callback: Callback<accessibility.CaptionsStyle>): void {
            this.onOnly(this.id, callback);
        }
    }

    export const daltonizationState: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_DALTONIZATION_STATE);
    export const audioMono: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_AUDIO_MONO);
    export let highContrastText: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_HIGH_CONTRAST_TEXT);
    export let invertColor: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_INVERT_COLOR);
    export let animationOff: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_ANIMATION_OFF);
    export let mouseKey: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_MOUSE_KEY);
    export let shortkey: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_SHORT_KEY);
    export let captions: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_CAPTION_STATE);
    export const ignoreRepeatClick: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_IGNORE_REPEAT_CLICK_STATE);
    export const screenMagnification: Config<boolean> = new ConfigImplBoolean(CONFIG_ID.CONFIG_SCREEN_MAGNIFICATION);

    export let contentTimeout: Config<int> = new ConfigImplInt(CONFIG_ID.CONFIG_CONTENT_TIMEOUT);
    export let mouseAutoClick: Config<int> = new ConfigImplInt(CONFIG_ID.CONFIG_MOUSE_AUTOCLICK);
    export const audioBalance: Config<double> = new ConfigImplDouble(CONFIG_ID.CONFIG_AUDIO_BALANCE);
    export let brightnessDiscount: Config<double> = new ConfigImplDouble(CONFIG_ID.CONFIG_BRIGHTNESS_DISCOUNT);

    export let shortkeyTarget: Config<string> = new ConfigImplString(CONFIG_ID.CONFIG_SHORT_KEY_TARGET);
    export const shortkeyMultiTargets: Config<Array<string>> = new ConfigImplArrString(CONFIG_ID.CONFIG_SHORT_KEY_MULTI_TARGET);

    export let daltonizationColorFilter: Config<DaltonizationColorFilter> = new ConfigImplDalColFilter(CONFIG_ID.CONFIG_DALTONIZATION_COLOR_FILTER);
    export const clickResponseTime: Config<ClickResponseTime> = new ConfigImplCliResTime(CONFIG_ID.CONIFG_CLICK_RESPONSE_TIME);
    export const repeatClickInterval: Config<RepeatClickInterval> = new ConfigRepeateClickInterval(CONFIG_ID.CONFIG_IGNORE_REPEAT_CLICK_TIME);

    export let captionsStyle: Config<accessibility.CaptionsStyle> = new ConfigImplCaptionsStyle(CONFIG_ID.CONFIG_CAPTION_STYLE);
}
export default config;