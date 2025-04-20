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

  injectGestureSync(gesture, callback) {
    console.log('injectGestureSync');
    return this.__context_impl__.injectGestureSync(gesture, callback);
  }

  startAbility(want, callback) {
    console.log('startAbility');
    return this.__context_impl__.startAbility(want, callback);
  }

  enableScreenCurtain(isEnable) {
    console.log('enableScreenCurtain');
    return this.__context_impl__.enableScreenCurtain(isEnable);
  }

  getElements(windowId, elementId) {
    console.log('getElements');
    return this.__context_impl__.getElements(windowId, elementId);
  }

  getDefaultFocusedElementIds(windowId) {
    console.log('getDefaultFocusedElementIds');
    return this.__context_impl__.getDefaultFocusedElementIds(windowId);
  }

  holdRunningLockSync() {
    console.log('holdRunningLockSync');
    return this.__context_impl__.holdRunningLockSync();
  }

  unholdRunningLockSync() {
    console.log('unholdRunningLockSync');
    return this.__context_impl__.unholdRunningLockSync();
  }

  on(type, callback) {
    console.log('on');
    return this.__context_impl__.on(type, callback);
  }

  off(type, callback) {
    console.log('off');
    return this.__context_impl__.off(type, callback);
  }

  notifyDisconnect() {
    console.log('notifyDisconnect');
    return this.__context_impl__.notifyDisconnect();
  }
}

export default AccessibilityExtensionContext;