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

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "napi_accessibility_event_info.h"
#include "napi_accessibility_system_ability_client.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

EXTERN_C_START
static void Cleanup(void *data)
{
    HILOG_INFO("cleanup hook");
    if (NAccessibilityClient::accessibilityStateListeners_) {
        NAccessibilityClient::accessibilityStateListeners_->UnsubscribeFromFramework();
    }
    if (NAccessibilityClient::touchGuideStateListeners_) {
        NAccessibilityClient::touchGuideStateListeners_->UnsubscribeFromFramework();
    }
    if (NAccessibilityClient::screenReaderStateListeners_) {
        NAccessibilityClient::screenReaderStateListeners_->UnsubscribeFromFramework();
    }
    if (NAccessibilityClient::touchModeListeners_) {
        NAccessibilityClient::touchModeListeners_->UnsubscribeFromFramework();
    }
    if (NAccessibilityClient::captionListeners_) {
        NAccessibilityClient::captionListeners_->UnsubscribeFromFramework();
    }
}

static napi_value CreateIntObject(napi_env env, int32_t value)
{
    napi_value jsObject = nullptr;
    napi_create_int32(env, value, &jsObject);
    return jsObject;
}

static void CreateAccessibilityEventTypePartTwo(napi_env env, napi_value objValue)
{
    napi_set_named_property(env, objValue, "TYPE_WINDOW_LAYER",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_LAYER));
    napi_set_named_property(env, objValue, "TYPE_TOUCH_BEGIN",
        CreateIntObject(env, AccessibilityEventType::TYPE_TOUCH_BEGIN));
    napi_set_named_property(env, objValue, "TYPE_TOUCH_END",
        CreateIntObject(env, AccessibilityEventType::TYPE_TOUCH_END));
    napi_set_named_property(env, objValue, "TYPE_PAGE_CONTENT_UPDATE",
        CreateIntObject(env, AccessibilityEventType::TYPE_PAGE_CONTENT_UPDATE));
    napi_set_named_property(env, objValue, "TYPE_PAGE_STATE_UPDATE",
        CreateIntObject(env, AccessibilityEventType::TYPE_PAGE_STATE_UPDATE));
    napi_set_named_property(env, objValue, "TYPE_PAGE_OPEN",
        CreateIntObject(env, AccessibilityEventType::TYPE_PAGE_OPEN));
    napi_set_named_property(env, objValue, "TYPE_PAGE_CLOSE",
        CreateIntObject(env, AccessibilityEventType::TYPE_PAGE_CLOSE));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_LEFT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_LEFT_THEN_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_RIGHT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_LEFT_THEN_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_UP));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_LEFT_THEN_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_DOWN));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_RIGHT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_RIGHT_THEN_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_LEFT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_RIGHT_THEN_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_UP));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_RIGHT_THEN_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_DOWN));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_UP));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_UP_THEN_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_UP_THEN_LEFT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_UP_THEN_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_UP_THEN_RIGHT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_UP_THEN_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_UP_THEN_DOWN));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_DOWN));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_DOWN_THEN_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_LEFT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_DOWN_THEN_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_RIGHT));
    napi_set_named_property(env, objValue, "TYPE_SWIPE_DOWN_THEN_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_UP));
    napi_set_named_property(env, objValue, "TYPE_TWO_FINGER_SINGLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_TWO_FINGER_SINGLE_TAP));
}

static void CreateAccessibilityEventTypePartThree(napi_env env, napi_value objValue)
{
    napi_set_named_property(env, objValue, "TYPE_TWO_FINGER_DOUBLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_TWO_FINGER_TRIPLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_SINGLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_SINGLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_DOUBLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_TRIPLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_SINGLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_SINGLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_DOUBLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_TRIPLE_TAP",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_SWIPE_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_UP));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_SWIPE_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_DOWN));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_SWIPE_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_LEFT));
    napi_set_named_property(env, objValue, "TYPE_THREE_FINGER_SWIPE_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_RIGHT));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_SWIPE_UP",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_UP));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_SWIPE_DOWN",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_DOWN));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_SWIPE_LEFT",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_LEFT));
    napi_set_named_property(env, objValue, "TYPE_FOUR_FINGER_SWIPE_RIGHT",
        CreateIntObject(env, AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_RIGHT));
}

static napi_value CreateAccessibilityEventType(napi_env env)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    napi_set_named_property(env, objValue, "TYPE_ACCESSIBILITY_FOCUS",
        CreateIntObject(env, AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS));
    napi_set_named_property(env, objValue, "TYPE_ACCESSIBILITY_FOCUS_CLEAR",
        CreateIntObject(env, AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS_CLEAR));
    napi_set_named_property(env, objValue, "TYPE_CLICK", CreateIntObject(env, AccessibilityEventType::TYPE_CLICK));
    napi_set_named_property(env, objValue, "TYPE_LONG_CLICK",
        CreateIntObject(env, AccessibilityEventType::TYPE_LONG_CLICK));
    napi_set_named_property(env, objValue, "TYPE_SELECT", CreateIntObject(env, AccessibilityEventType::TYPE_SELECT));
    napi_set_named_property(env, objValue, "TYPE_HOVER_ENTER",
        CreateIntObject(env, AccessibilityEventType::TYPE_HOVER_ENTER));
    napi_set_named_property(env, objValue, "TYPE_HOVER_EXIT",
        CreateIntObject(env, AccessibilityEventType::TYPE_HOVER_EXIT));
    napi_set_named_property(env, objValue, "TYPE_FOCUS", CreateIntObject(env, AccessibilityEventType::TYPE_FOCUS));
    napi_set_named_property(env, objValue, "TYPE_TEXT_UPDATE",
        CreateIntObject(env, AccessibilityEventType::TYPE_TEXT_UPDATE));
    napi_set_named_property(env, objValue, "TYPE_TEXT_SELECTION_UPDATE",
        CreateIntObject(env, AccessibilityEventType::TYPE_TEXT_SELECTION_UPDATE));
    napi_set_named_property(env, objValue, "TYPE_SCROLL", CreateIntObject(env, AccessibilityEventType::TYPE_SCROLL));
    napi_set_named_property(env, objValue, "TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY",
        CreateIntObject(env, AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY));
    napi_set_named_property(env, objValue, "TYPE_ANNOUNCE_FOR_ACCESSIBILITY",
        CreateIntObject(env, AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY));
    napi_set_named_property(env, objValue, "TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT",
        CreateIntObject(env, AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT));
    napi_set_named_property(env, objValue, "TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT",
        CreateIntObject(env, AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT));
    napi_set_named_property(env, objValue, "TYPE_ELEMENT_INFO_CHANGE",
        CreateIntObject(env, AccessibilityEventType::TYPE_ELEMENT_INFO_CHANGE));
    napi_set_named_property(env, objValue, "TYPE_SCROLLING",
        CreateIntObject(env, AccessibilityEventType::TYPE_SCROLLING));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_ADD",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_ADD));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_REMOVE",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_REMOVE));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_BOUNDS",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_BOUNDS));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_ACTIVE",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_ACTIVE));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_FOCUS",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_FOCUS));
    napi_set_named_property(env, objValue, "TYPE_WINDOW_PROPERTY",
        CreateIntObject(env, AccessibilityEventType::TYPE_WINDOW_PROPERTY));
    CreateAccessibilityEventTypePartTwo(env, objValue);
    CreateAccessibilityEventTypePartThree(env, objValue);
    return objValue;
}

static napi_value CreateAccessibilityAction(napi_env env)
{
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    napi_set_named_property(env, objValue, "ACCESSIBILITY_FOCUS",
        CreateIntObject(env, AccessibilityAction::ACCESSIBILITY_FOCUS));
    napi_set_named_property(env, objValue, "CLEAR_ACCESSIBILITY_FOCUS",
        CreateIntObject(env, AccessibilityAction::CLEAR_ACCESSIBILITY_FOCUS));
    napi_set_named_property(env, objValue, "FOCUS", CreateIntObject(env, AccessibilityAction::FOCUS));
    napi_set_named_property(env, objValue, "CLEAR_FOCUS", CreateIntObject(env, AccessibilityAction::CLEAR_FOCUS));
    napi_set_named_property(env, objValue, "CLICK", CreateIntObject(env, AccessibilityAction::CLICK));
    napi_set_named_property(env, objValue, "LONG_CLICK", CreateIntObject(env, AccessibilityAction::LONG_CLICK));
    napi_set_named_property(env, objValue, "CUT", CreateIntObject(env, AccessibilityAction::CUT));
    napi_set_named_property(env, objValue, "COPY", CreateIntObject(env, AccessibilityAction::COPY));
    napi_set_named_property(env, objValue, "PASTE", CreateIntObject(env, AccessibilityAction::PASTE));
    napi_set_named_property(env, objValue, "SELECT", CreateIntObject(env, AccessibilityAction::SELECT));
    napi_set_named_property(env, objValue, "SET_TEXT", CreateIntObject(env, AccessibilityAction::SET_TEXT));
    napi_set_named_property(env, objValue, "SCROLL_FORWARD",
        CreateIntObject(env, AccessibilityAction::SCROLL_FORWARD));
    napi_set_named_property(env, objValue, "SCROLL_BACKWARD",
        CreateIntObject(env, AccessibilityAction::SCROLL_BACKWARD));
    napi_set_named_property(env, objValue, "SET_SELECTION", CreateIntObject(env, AccessibilityAction::SET_SELECTION));
    napi_set_named_property(env, objValue, "SET_CURSOR_POSITION",
        CreateIntObject(env, AccessibilityAction::SET_CURSOR_POSITION));
    napi_set_named_property(env, objValue, "HOME", CreateIntObject(env, AccessibilityAction::HOME));
    napi_set_named_property(env, objValue, "BACK", CreateIntObject(env, AccessibilityAction::BACK));
    napi_set_named_property(env, objValue, "RECENT_TASK", CreateIntObject(env, AccessibilityAction::RECENT_TASK));
    napi_set_named_property(env, objValue, "NOTIFICATION_CENTER",
        CreateIntObject(env, AccessibilityAction::NOTIFICATION_CENTER));
    napi_set_named_property(env, objValue, "CONTROL_CENTER",
        CreateIntObject(env, AccessibilityAction::CONTROL_CENTER));
    napi_set_named_property(env, objValue, "SPAN_CLICK", CreateIntObject(env, AccessibilityAction::SPAN_CLICK));
    return objValue;
}

/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("isOpenAccessibility", NAccessibilityClient::IsOpenAccessibility),
        DECLARE_NAPI_FUNCTION("isOpenAccessibilitySync", NAccessibilityClient::IsOpenAccessibilitySync),
        DECLARE_NAPI_FUNCTION("isOpenTouchGuide", NAccessibilityClient::IsOpenTouchExploration),
        DECLARE_NAPI_FUNCTION("isOpenTouchGuideSync", NAccessibilityClient::IsOpenTouchExplorationSync),
        DECLARE_NAPI_FUNCTION("isScreenReaderOpenSync", NAccessibilityClient::IsScreenReaderOpenSync),
        DECLARE_NAPI_FUNCTION("getTouchModeSync", NAccessibilityClient::GetTouchModeSync),
        DECLARE_NAPI_FUNCTION("getAbilityLists", NAccessibilityClient::GetAbilityList),
        DECLARE_NAPI_FUNCTION("getAccessibilityExtensionList", NAccessibilityClient::GetAccessibilityExtensionList),
        DECLARE_NAPI_FUNCTION("getAccessibilityExtensionListSync",
            NAccessibilityClient::GetAccessibilityExtensionListSync),
        DECLARE_NAPI_FUNCTION("on", NAccessibilityClient::SubscribeState),
        DECLARE_NAPI_FUNCTION("off", NAccessibilityClient::UnsubscribeState),
        DECLARE_NAPI_FUNCTION("sendEvent", NAccessibilityClient::SendEvent),
        DECLARE_NAPI_FUNCTION("sendAccessibilityEvent", NAccessibilityClient::SendAccessibilityEvent),
        DECLARE_NAPI_FUNCTION("getCaptionsManager", NAccessibilityClient::GetCaptionsManager),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    napi_set_named_property(env, exports, "AccessibilityEventType", CreateAccessibilityEventType(env));
    napi_set_named_property(env, exports, "AccessibilityAction", CreateAccessibilityAction(env));

    auto &instance = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
    (void)instance.InitializeContext();
    NAccessibilityClient::DefineJSCaptionsManager(env);
    NAccessibilityClient::DefineJSCaptionsStyle(env);
    NAccessibilityEventInfo::DefineJSAccessibilityEventInfo(env, exports);
    NAccessibilityClient::accessibilityStateListeners_->SubscribeToFramework();
    NAccessibilityClient::touchGuideStateListeners_->SubscribeToFramework();
    NAccessibilityClient::screenReaderStateListeners_->SubscribeToFramework();
    NAccessibilityClient::touchModeListeners_->SubscribeToFramework();
    NAccessibilityClient::captionListeners_->SubscribeToFramework();
    napi_status status = napi_add_env_cleanup_hook(env, Cleanup, &NAccessibilityClient::accessibilityStateListeners_);
    if (status != napi_ok) {
        HILOG_WARN("add cleanup hook failed %{public}d", status);
    }
    HILOG_INFO("-----Init end------");
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "accessibility",
    .nm_priv = ((void*)0),
    .reserved = {0},
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterNapiAccessibilityModule(void)
{
    napi_module_register(&_module);
}
