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

let ExtensionContext = requireNapi('application.ExtensionContext');

class AccessibilityExtensionContext extends ExtensionContext {
  constructor(obj) {
    super(obj);
  }

  setTargetBundleName(targetNames, callback) {
    console.log('setTargetBundleName');
    return this.__context_impl__.setTargetBundleName(targetNames, callback);
  }

  getFocusElement(isAccessibilityFocus, callback) {
    console.log('getFocusElement');
    return this.__context_impl__.getFocusElement(isAccessibilityFocus, callback);
  }

  getWindowRootElement(windowId, callback) {
    console.log('getWindowRootElement');
    return this.__context_impl__.getWindowRootElement(windowId, callback);
  }

  getWindows(displayId, callback) {
    console.log('getWindows');
    return this.__context_impl__.getWindows(displayId, callback);
  }

  injectGesture(gesture, callback) {
    console.log('injectGesture');
    return this.__context_impl__.injectGesture(gesture, callback);
  }
}

export default AccessibilityExtensionContext;