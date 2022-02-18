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

var ExtensionContext = requireNapi("application.ExtensionContext")

class AccessibilityExtensionContext extends ExtensionContext {
    constructor(obj) {
        super(obj);
    }

    getFocusElementInfo(focusType, callback) {
        console.log('getFocusElementInfo');
        return this.__context_impl__.getFocusElementInfo(focusType, callback);
    }

    getRootElementInfo(callback) {
        console.log('getRootElementInfo');
        return this.__context_impl__.getRootElementInfo(callback);
    }

    getWindows(callback) {
        console.log('getWindows');
        return this.__context_impl__.getWindows(callback);
    }

    executeCommonAction(action, callback) {
        console.log('executeCommonAction');
        return this.__context_impl__.executeCommonAction(action, callback);
    }

    gestureInject(gesture, listener, callback) {
        console.log('gestureInject');
        return this.__context_impl__.gestureInject(gesture, listener, callback);
    }
}

export default AccessibilityExtensionContext