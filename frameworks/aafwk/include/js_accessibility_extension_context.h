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

#ifndef JS_ACCESSIBILITY_EXTENSION_CONTEXT_H
#define JS_ACCESSIBILITY_EXTENSION_CONTEXT_H

#include "accessibility_extension_context.h"
#include "gesture_simulation.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

class NativeEngine;
class NativeValue;
class NativeReference;

namespace OHOS {
namespace Accessibility {

class JSGestureResultListener : public GestureResultListener {
public:
    JSGestureResultListener() = default;
    virtual ~JSGestureResultListener() = default;

    void OnGestureInjectResult(uint32_t sequence, bool result) override;
};

struct jsGestureResultListenerInfo {
    napi_env env_;
    napi_ref callback_;
    std::shared_ptr<GestureResultListener> listener_ = nullptr;
};

NativeValue* CreateJsAccessibilityExtensionContext(
    NativeEngine& engine, std::shared_ptr<AccessibilityExtensionContext> context);

static uint32_t gestureInjectSequence = 0;
static std::map<uint32_t, std::shared_ptr<jsGestureResultListenerInfo>> jsGestureResultListenerInfos {};
} // namespace Accessibility
} // namespace OHOS

#endif  // JS_ACCESSIBILITY_EXTENSION_CONTEXT_H