/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_accessibility_extension.h"
#include "ani_accessibility_extension_context.h"
#include "ets_native_reference.h"
#include "accessibility_def.h"
#include "accessibility_element_info.h"
#include "accessible_ability_client.h"
#include "ani_accessibility_element.h"
#include "ani_utils.h"
#include "accessibility_utils.h"

#include <ani_signature_builder.h>

namespace OHOS {
namespace Accessibility {
using namespace arkts::ani_signature;

constexpr int64_t VIRTUAL_COMPONENT_ID = -1;
constexpr const char* ANI_ACCESSIBILITY_EXTENSION_CLS =
    "@ohos.application.AccessibilityExtensionAbility.AccessibilityExtensionAbility";

struct ExtensionCallbackInfo {
    ani_env *env_;
    AniAccessibilityExtension *extension_;
};

struct KeyEventCallbackInfo : public ExtensionCallbackInfo {
    std::shared_ptr<MMI::KeyEvent> keyEvent_;
};

struct AccessibilityEventInfoCallbackInfo : public ExtensionCallbackInfo {
    std::string eventType_ = "";
    int64_t timeStamp_ = 0;
    std::shared_ptr<AccessibilityElement> element_ = nullptr;
    int64_t elementId_ = 0;
    std::string textAnnouncedForAccessibility_ = "";
    std::string extraInfo_ = "";
};

struct AccessibilityEventTypeCallbackInfo : public AccessibilityEventInfoCallbackInfo {
    AccessibilityEventType AccessibilityEventType_ = TYPE_ERROR;
};

AniAccessibilityExtension* AniAccessibilityExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    HILOG_INFO("AniAccessibilityExtension begin Create");
    return new (std::nothrow) AniAccessibilityExtension(static_cast<AbilityRuntime::ETSRuntime&>(*runtime));
}

AniAccessibilityExtension::AniAccessibilityExtension(AbilityRuntime::ETSRuntime &stsRuntime): stsRuntime_(stsRuntime)
{
        listener_ = std::make_shared<AbilityListener>(*this);
}

AniAccessibilityExtension::~AniAccessibilityExtension()
{
    DeleteContextRef();
    auto context = GetContext();
    if (context) {
        context->Unbind();
    }
}

void AniAccessibilityExtension::DeleteContextRef()
{
    HILOG_DEBUG("AniAccessibilityExtension DeleteContextRef");
    if (env_ == nullptr || jsObj_ == nullptr) {
        HILOG_ERROR("env_ is null or jsObj_ is null");
        return;
    }
    ani_field contextField;
    ani_class cls = nullptr;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_EXTENSION_CLS);
    if ((env_->FindClass(className.Descriptor().c_str(), &cls)) != ANI_OK) {
        HILOG_ERROR("FindClass err: accessibility extension");
        return;
    }
    auto status = env_->Class_FindField(cls, "context", &contextField);
    if (status != ANI_OK) {
        HILOG_ERROR("Class_GetField context failed");
        return;
    }
    ani_ref contextRef = nullptr;
    if (env_->Object_GetField_Ref(jsObj_->aniObj, contextField, &contextRef) != ANI_OK) {
        HILOG_ERROR("Object_GetField_Ref contextObj failed");
        return;
    }
    if (contextRef != nullptr) {
        env_->GlobalReference_Delete(contextRef);
    }
}

bool AniAccessibilityExtension::GetSrcPathAndModuleName(std::string &srcPath, std::string &moduleName)
{
    if (!Extension::abilityInfo_) {
        HILOG_ERROR("abilityInfo_ is nullptr");
        return false;
    }
    if (!Extension::abilityInfo_->isModuleJson) {
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
    } else if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        size_t pos = srcPath.rfind('.');
        if (pos != std::string::npos) {
            srcPath.erase(pos);
            srcPath.append(".abc");
        }
    } else {
        HILOG_ERROR("Failed to get srcPath");
        return false;
    }
    moduleName = Extension::abilityInfo_->moduleName;
    moduleName.append("::").append(abilityInfo_->name);
    HILOG_INFO("moduleName:%{public}s, srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());
    return true;
}

void AniAccessibilityExtension::BindContext(ani_env*env, std::shared_ptr<AAFwk::Want> want,
    const std::shared_ptr<AbilityRuntime::OHOSApplication> &application)
{
    if (env == nullptr || want == nullptr) {
        HILOG_ERROR("env is null or want is null");
        return;
    }
    auto context = GetContext();
    if (!context) {
        HILOG_ERROR("Failed to get context");
        return;
    }
    ani_object contextObj = CreateAniAccessibilityExtensionContext(env, context, application);
    if (contextObj == nullptr) {
        HILOG_ERROR("contextObj is null");
        return;
    }
    ani_field contextField;
    ani_class cls = nullptr;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_EXTENSION_CLS);
    if ((env->FindClass(className.Descriptor().c_str(), &cls)) != ANI_OK) {
        HILOG_ERROR("FindClass err: accessibility extension");
        return;
    }
    auto status = env->Class_FindField(cls, "context", &contextField);
    if (status != ANI_OK) {
        HILOG_ERROR("Class_GetField context failed");
        return;
    }
    ani_ref contextRef = nullptr;
    if (env->GlobalReference_Create(contextObj, &contextRef) != ANI_OK) {
        HILOG_ERROR("GlobalReference_Create contextObj failed");
        return;
    }
    if (env->Object_SetField_Ref(jsObj_->aniObj, contextField, contextRef) != ANI_OK) {
        HILOG_ERROR("Object_SetField_Ref contextObj failed");
        return;
    }
}

void AniAccessibilityExtension::Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
    std::shared_ptr<AppExecFwk::AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    HILOG_INFO("Init begin");
    AccessibilityExtension::Init(record, application, handler, token);
    std::string srcPath = "";
    std::string moduleName = "";
    if (!GetSrcPathAndModuleName(srcPath, moduleName)) {
        return;
    }
    env_ = stsRuntime_.GetAniEnv();
    if (env_ == nullptr) {
        HILOG_ERROR("null env");
        return;
    }
    HILOG_DEBUG("moduleName:%{private}s, srcPath:%{private}s.", moduleName.c_str(), srcPath.c_str());
    jsObj_ = stsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == AppExecFwk::CompileMode::ES_MODULE, false, abilityInfo_->srcEntrance);
    HILOG_INFO(" AniAccessibilityExtension create end");
    if (jsObj_ == nullptr) {
        HILOG_ERROR("Failed to get jsobj_");
        return;
    }
    BindContext(env_, record->GetWant(), application);
    HILOG_INFO("Init end");
}

void AniAccessibilityExtension::onAccessibilityConnect(ani_env *env, ani_object object)
{
    HILOG_DEBUG();
    ani_class cls;
    ani_method managedMethod;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_EXTENSION_CLS);
    if (ANI_OK != env->FindClass(className.Descriptor().c_str(), &cls)) {
        HILOG_ERROR("class not found");
        return ;
    }
    if (ANI_OK != env->Class_FindMethod(cls, "onAccessibilityConnect", ":", &managedMethod)) {
        HILOG_ERROR("Class_FindMethod Failed");
        return ;
    }
    if (ANI_OK != env->Object_CallMethod_Void(object, managedMethod)) {
        HILOG_ERROR("Object_CallMethod_Void Failed");
        return ;
    }
}

void AniAccessibilityExtension::OnAbilityConnected()
{
    HILOG_DEBUG();
    ani_object thisObj = jsObj_->aniObj;
    if (thisObj == nullptr) {
        HILOG_ERROR("thisObj is null");
        return;
    }
    ani_env *env = env_;
        if (env == nullptr) {
        HILOG_ERROR("env is null");
        return;
    }
    auto task = [this, env, thisObj]() {
        onAccessibilityConnect(env, thisObj);
    };
    ANIUtils::SendEventToMainThread(task);
}

void AniAccessibilityExtension::OnAbilityDisconnected()
{
    HILOG_DEBUG();
    ani_object thisObj = jsObj_->aniObj;
    if (thisObj == nullptr) {
        HILOG_ERROR("thisObj is null");
        return;
    }
    ani_env *env = env_;
    if (env == nullptr) {
        HILOG_ERROR("env is null");
        return;
    }
    auto task = [this, env, thisObj]() {
        onAccessibilityDisconnect(env, thisObj);
    };
    ANIUtils::SendEventToMainThread(task);
}

void AniAccessibilityExtension::onAccessibilityDisconnect(ani_env *env, ani_object object)
{
    HILOG_DEBUG();
    ani_method managedMethod;
    ani_class cls = nullptr;
    arkts::ani_signature::Type className = arkts::ani_signature::Builder::BuildClass(ANI_ACCESSIBILITY_EXTENSION_CLS);
    if ((env->FindClass(className.Descriptor().c_str(), &cls)) != ANI_OK) {
        HILOG_ERROR("OnAccessibilityDisconnect FindClass err: accessibility extension");
        return;
    }
    if (ANI_OK != env->Class_FindMethod(cls, "onAccessibilityDisconnect", ":", &managedMethod)) {
        HILOG_ERROR("Class_FindMethod Failed");
        return ;
    }
    env->Object_CallMethod_Void(object, managedMethod);
    return ;
}

std::string GetEventExtraInfo(const AccessibilityEventInfo& eventInfo)
{
    std::map<std::string, std::string> mapValIsStr = eventInfo.GetExtraEvent().GetExtraEventInfoValueStr();
    nlohmann::json extraInfoValue;
    for (auto &iterStr : mapValIsStr) {
        extraInfoValue[iterStr.first] = iterStr.second;
    }
    HILOG_DEBUG("GetEventExtraInfo extraInfoValue is [%{public}s]", extraInfoValue.dump().c_str());
    return extraInfoValue.dump().c_str();
}

void CreateElementInfoByEventInfo(const AccessibilityEventInfo& eventInfo,
    const std::shared_ptr<AccessibilityElementInfo> &elementInfo)
{
    HILOG_DEBUG();
    if (!elementInfo) {
        HILOG_ERROR("elementInfo is nullptr");
        return;
    }
    if (elementInfo->GetAccessibilityId() < 0) {
        elementInfo->SetComponentId(VIRTUAL_COMPONENT_ID);
    }
    elementInfo->SetBundleName(eventInfo.GetBundleName());
    elementInfo->SetComponentType(eventInfo.GetComponentType());
    elementInfo->SetPageId(eventInfo.GetPageId());
    elementInfo->SetDescriptionInfo(eventInfo.GetDescription());
    elementInfo->SetTriggerAction(eventInfo.GetTriggerAction());
    elementInfo->SetTextMovementStep(eventInfo.GetTextMovementStep());
    elementInfo->SetContentList(eventInfo.GetContentList());
    elementInfo->SetLatestContent(eventInfo.GetLatestContent());
    elementInfo->SetBeginIndex(eventInfo.GetBeginIndex());
    elementInfo->SetCurrentIndex(eventInfo.GetCurrentIndex());
    elementInfo->SetEndIndex(eventInfo.GetEndIndex());
    elementInfo->SetItemCounts(eventInfo.GetItemCounts());
}

AccessibilityEventType ConvertStringToAccessibilityEventType(const std::string &eventType)
{
    static const std::map<const std::string, AccessibilityEventType> eventTypeTable = {
        {"accessibilityFocus", AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS},
        {"accessibilityFocusClear", AccessibilityEventType::TYPE_ACCESSIBILITY_FOCUS_CLEAR},
        {"click", AccessibilityEventType::TYPE_CLICK},
        {"longClick", AccessibilityEventType::TYPE_LONG_CLICK},
        {"select", AccessibilityEventType::TYPE_SELECT},
        {"hoverEnter", AccessibilityEventType::TYPE_HOVER_ENTER},
        {"hoverExit", AccessibilityEventType::TYPE_HOVER_EXIT},
        {"focus", AccessibilityEventType::TYPE_FOCUS},
        {"textUpdate", AccessibilityEventType::TYPE_TEXT_UPDATE},
        {"textSelectionUpdate", AccessibilityEventType::TYPE_TEXT_SELECTION_UPDATE},
        {"scroll", AccessibilityEventType::TYPE_SCROLL},
        {"requestFocusForAccessibility", AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY},
        {"announceForAccessibility", AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY},
        {"requestFocusForAccessibilityNotInterrupt",
            AccessibilityEventType::TYPE_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"announceForAccessibilityNotInterrupt",
            AccessibilityEventType::TYPE_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"elementInfoChange", AccessibilityEventType::TYPE_ELEMENT_INFO_CHANGE},
        {"scrolling", AccessibilityEventType::TYPE_SCROLLING},
        {"add", AccessibilityEventType::TYPE_WINDOW_ADD},
        {"remove", AccessibilityEventType::TYPE_WINDOW_REMOVE},
        {"bounds", AccessibilityEventType::TYPE_WINDOW_BOUNDS},
        {"active", AccessibilityEventType::TYPE_WINDOW_ACTIVE},
        {"focus", AccessibilityEventType::TYPE_WINDOW_FOCUS},
        {"property", AccessibilityEventType::TYPE_WINDOW_PROPERTY},
        {"layer", AccessibilityEventType::TYPE_WINDOW_LAYER},
        {"touchBegin", AccessibilityEventType::TYPE_TOUCH_BEGIN},
        {"touchEnd", AccessibilityEventType::TYPE_TOUCH_END},
        {"pageContentUpdate", AccessibilityEventType::TYPE_PAGE_CONTENT_UPDATE},
        {"pageStateUpdate", AccessibilityEventType::TYPE_PAGE_STATE_UPDATE},
        {"pageOpen", AccessibilityEventType::TYPE_PAGE_OPEN},
        {"pageClose", AccessibilityEventType::TYPE_PAGE_CLOSE},
        {"left", AccessibilityEventType::TYPE_SWIPE_LEFT},
        {"leftThenRight", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_RIGHT},
        {"leftThenUp", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_UP},
        {"leftThenDown", AccessibilityEventType::TYPE_SWIPE_LEFT_THEN_DOWN},
        {"right", AccessibilityEventType::TYPE_SWIPE_RIGHT},
        {"rightThenLeft", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_LEFT},
        {"rightThenUp", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_UP},
        {"rightThenDown", AccessibilityEventType::TYPE_SWIPE_RIGHT_THEN_DOWN},
        {"up", AccessibilityEventType::TYPE_SWIPE_UP},
        {"upThenLeft", AccessibilityEventType::TYPE_SWIPE_UP_THEN_LEFT},
        {"upThenRight", AccessibilityEventType::TYPE_SWIPE_UP_THEN_RIGHT},
        {"upThenDown", AccessibilityEventType::TYPE_SWIPE_UP_THEN_DOWN},
        {"down", AccessibilityEventType::TYPE_SWIPE_DOWN},
        {"downThenLeft", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_LEFT},
        {"downThenRight", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_RIGHT},
        {"downThenUp", AccessibilityEventType::TYPE_SWIPE_DOWN_THEN_UP},
        {"twoFingerSingleTap", AccessibilityEventType::TYPE_TWO_FINGER_SINGLE_TAP},
        {"twoFingerDoubleTap", AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP},
        {"twoFingerDoubleTapAndHold", AccessibilityEventType::TYPE_TWO_FINGER_DOUBLE_TAP_AND_HOLD},
        {"twoFingerTripleTap", AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP},
        {"twoFingerTripleTapAndHold", AccessibilityEventType::TYPE_TWO_FINGER_TRIPLE_TAP_AND_HOLD},
        {"threeFingerSingleTap", AccessibilityEventType::TYPE_THREE_FINGER_SINGLE_TAP},
        {"threeFingerDoubleTap", AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP},
        {"threeFingerDoubleTapAndHold", AccessibilityEventType::TYPE_THREE_FINGER_DOUBLE_TAP_AND_HOLD},
        {"threeFingerTripleTap", AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP},
        {"threeFingerTripleTapAndHold", AccessibilityEventType::TYPE_THREE_FINGER_TRIPLE_TAP_AND_HOLD},
        {"fourFingerSingleTap", AccessibilityEventType::TYPE_FOUR_FINGER_SINGLE_TAP},
        {"fourFingerDoubleTap", AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP},
        {"fourFingerDoubleTapAndHold", AccessibilityEventType::TYPE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD},
        {"fourFingerTripleTap", AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP},
        {"fourFingerTripleTapAndHold", AccessibilityEventType::TYPE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD},
        {"threeFingerSwipeUp", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_UP},
        {"threeFingerSwipeDown", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_DOWN},
        {"threeFingerSwipeLeft", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_LEFT},
        {"threeFingerSwipeRight", AccessibilityEventType::TYPE_THREE_FINGER_SWIPE_RIGHT},
        {"fourFingerSwipeUp", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_UP},
        {"fourFingerSwipeDown", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_DOWN},
        {"fourFingerSwipeLeft", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_LEFT},
        {"fourFingerSwipeRight", AccessibilityEventType::TYPE_FOUR_FINGER_SWIPE_RIGHT},
    };
    if (eventTypeTable.find(eventType) == eventTypeTable.end()) {
        return AccessibilityEventType::TYPE_ERROR;
    }
    return eventTypeTable.at(eventType);
}

AccessibilityEventType ConvertStringToAccessibilityEventType(const AccessibilityEventInfo &eventInfo,
    const std::string &eventTypeString)
{
    EventType type = eventInfo.GetEventType();
    AccessibilityEventType accessibilityEventType = ConvertStringToAccessibilityEventType(eventTypeString);
    if (type == TYPE_WINDOW_UPDATE && accessibilityEventType == AccessibilityEventType::TYPE_FOCUS) {
        return AccessibilityEventType::TYPE_WINDOW_FOCUS;
    }
    return accessibilityEventType;
}

std::string ConvertGestureTypeToString(GestureType type)
{
    static const std::map<GestureType, const std::string> gestureTypeTable = {
        {GestureType::GESTURE_SWIPE_LEFT, "left"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_RIGHT, "leftThenRight"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_UP, "leftThenUp"},
        {GestureType::GESTURE_SWIPE_LEFT_THEN_DOWN, "leftThenDown"},
        {GestureType::GESTURE_SWIPE_RIGHT, "right"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_LEFT, "rightThenLeft"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_UP, "rightThenUp"},
        {GestureType::GESTURE_SWIPE_RIGHT_THEN_DOWN, "rightThenDown"},
        {GestureType::GESTURE_SWIPE_UP, "up"},
        {GestureType::GESTURE_SWIPE_UP_THEN_LEFT, "upThenLeft"},
        {GestureType::GESTURE_SWIPE_UP_THEN_RIGHT, "upThenRight"},
        {GestureType::GESTURE_SWIPE_UP_THEN_DOWN, "upThenDown"},
        {GestureType::GESTURE_SWIPE_DOWN, "down"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_LEFT, "downThenLeft"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_RIGHT, "downThenRight"},
        {GestureType::GESTURE_SWIPE_DOWN_THEN_UP, "downThenUp"},
        {GestureType::GESTURE_TWO_FINGER_SINGLE_TAP, "twoFingerSingleTap"},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP, "twoFingerDoubleTap"},
        {GestureType::GESTURE_TWO_FINGER_DOUBLE_TAP_AND_HOLD, "twoFingerDoubleTapAndHold"},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP, "twoFingerTripleTap"},
        {GestureType::GESTURE_TWO_FINGER_TRIPLE_TAP_AND_HOLD, "twoFingerTripleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_SINGLE_TAP, "threeFingerSingleTap"},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP, "threeFingerDoubleTap"},
        {GestureType::GESTURE_THREE_FINGER_DOUBLE_TAP_AND_HOLD, "threeFingerDoubleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP, "threeFingerTripleTap"},
        {GestureType::GESTURE_THREE_FINGER_TRIPLE_TAP_AND_HOLD, "threeFingerTripleTapAndHold"},
        {GestureType::GESTURE_FOUR_FINGER_SINGLE_TAP, "fourFingerSingleTap"},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP, "fourFingerDoubleTap"},
        {GestureType::GESTURE_FOUR_FINGER_DOUBLE_TAP_AND_HOLD, "fourFingerDoubleTapAndHold"},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP, "fourFingerTripleTap"},
        {GestureType::GESTURE_FOUR_FINGER_TRIPLE_TAP_AND_HOLD, "fourFingerTripleTapAndHold"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_UP, "threeFingerSwipeUp"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_DOWN, "threeFingerSwipeDown"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_LEFT, "threeFingerSwipeLeft"},
        {GestureType::GESTURE_THREE_FINGER_SWIPE_RIGHT, "threeFingerSwipeRight"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_UP, "fourFingerSwipeUp"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_DOWN, "fourFingerSwipeDown"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_LEFT, "fourFingerSwipeLeft"},
        {GestureType::GESTURE_FOUR_FINGER_SWIPE_RIGHT, "fourFingerSwipeRight"}
    };

    if (gestureTypeTable.find(type) == gestureTypeTable.end()) {
        return "";
    }
    return gestureTypeTable.at(type);
}

const std::string ConvertWindowUpdateTypeToString(WindowUpdateType type)
{
    static const std::map<WindowUpdateType, const std::string> windowUpdateTypeTable = {
        {WindowUpdateType::WINDOW_UPDATE_FOCUSED, "focus"},
        {WindowUpdateType::WINDOW_UPDATE_ACTIVE, "active"},
        {WindowUpdateType::WINDOW_UPDATE_ADDED, "add"},
        {WindowUpdateType::WINDOW_UPDATE_REMOVED, "remove"},
        {WindowUpdateType::WINDOW_UPDATE_BOUNDS, "bounds"},
        {WindowUpdateType::WINDOW_UPDATE_PROPERTY, "property"},
        {WindowUpdateType::WINDOW_UPDATE_LAYER, "layer"}};

    if (windowUpdateTypeTable.find(type) == windowUpdateTypeTable.end()) {
        return "";
    }
    return windowUpdateTypeTable.at(type);
}

const std::string ConvertAccessibilityEventTypeToString(EventType type)
{
    static const std::map<EventType, const std::string> a11yEvtTypeTable = {
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_PAGE_CONTENT_UPDATE, "pageContentUpdate"},
        {EventType::TYPE_PAGE_STATE_UPDATE, "pageStateUpdate"},
        {EventType::TYPE_TOUCH_BEGIN, "touchBegin"},
        {EventType::TYPE_TOUCH_END, "touchEnd"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY, "requestFocusForAccessibility"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY, "announceForAccessibility"},
        {EventType::TYPE_PAGE_OPEN, "pageOpen"},
        {EventType::TYPE_PAGE_CLOSE, "pageClose"},
        {EventType::TYPE_ELEMENT_INFO_CHANGE, "elementInfoChange"},
        {EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT, "announceForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT,
            "requestFocusForAccessibilityNotInterrupt"},
        {EventType::TYPE_VIEW_SCROLLING_EVENT, "scrolling"}};

    if (a11yEvtTypeTable.find(type) == a11yEvtTypeTable.end()) {
        return "";
    }

    return a11yEvtTypeTable.at(type);
}

void ConvertEventTypeToString(const AccessibilityEventInfo& eventInfo, std::string& eventTypeString)
{
    EventType type = eventInfo.GetEventType();
    switch (type) {
        case TYPE_GESTURE_EVENT: {
            GestureType gestureType = eventInfo.GetGestureType();
            eventTypeString = ConvertGestureTypeToString(gestureType);
            break;
        }
        case TYPE_WINDOW_UPDATE: {
            WindowUpdateType windowUpdateType = eventInfo.GetWindowChangeTypes();
            eventTypeString = ConvertWindowUpdateTypeToString(windowUpdateType);
            break;
        }
        default:
            eventTypeString = ConvertAccessibilityEventTypeToString(type);
            break;
    }
}

std::shared_ptr<AccessibilityElement> GetElement(const AccessibilityEventInfo& eventInfo)
{
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        return nullptr;
    }
    int64_t componentId = eventInfo.GetAccessibilityId();
    int32_t windowId = eventInfo.GetWindowId();
    std::shared_ptr<AccessibilityElement> element = nullptr;
    HILOG_DEBUG("GetElement componentId: %{public}" PRId64 ", windowId: %{public}d, eventType: %{public}d",
        componentId, windowId, eventInfo.GetEventType());
    if (componentId > 0) {
        std::shared_ptr<AccessibilityElementInfo> elementInfo =
            std::make_shared<AccessibilityElementInfo>(eventInfo.GetElementInfo());
        element = std::make_shared<AccessibilityElement>(elementInfo);
    } else if (windowId > 0) {
        std::shared_ptr<AccessibilityWindowInfo> windowInfo = std::make_shared<AccessibilityWindowInfo>();
        if (aaClient->GetWindow(windowId, *windowInfo) == RET_OK) {
            element = std::make_shared<AccessibilityElement>(windowInfo);
        }
    } else {
        std::shared_ptr<AccessibilityElementInfo> elementInfo = std::make_shared<AccessibilityElementInfo>();
        std::string inspectorKey = eventInfo.GetInspectorKey();
        RetError ret = RET_ERR_FAILED;
        AccessibilityElementInfo accessibilityElementInfo;
        if ((eventInfo.GetEventType() == TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY ||
            eventInfo.GetEventType() == TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT) &&
            inspectorKey != "") {
            ret = aaClient->SearchElementInfoByInspectorKey(inspectorKey, accessibilityElementInfo);
        }
        if (ret == RET_OK) {
            elementInfo = std::make_shared<AccessibilityElementInfo>(accessibilityElementInfo);
            elementInfo->SetBundleName(eventInfo.GetBundleName());
            elementInfo->SetTriggerAction(eventInfo.GetTriggerAction());
        } else {
            CreateElementInfoByEventInfo(eventInfo, elementInfo);
        }
        element = std::make_shared<AccessibilityElement>(elementInfo);
    }
    return element;
}

ani_object  ConvertAccessibilityElementToJS(ani_env *env,  const std::shared_ptr<AccessibilityElement>& element)
{
    if (env == nullptr || element == nullptr) {
        HILOG_ERROR("invalid args");
        return nullptr;
    }
    ani_object elementObj = ANIUtils::CreateObject(env, GetAniAccessibilityElementClass());
    if (elementObj == nullptr) {
        HILOG_ERROR("Failed to create AccessibilityElement object");
        return nullptr;
    }
    AccessibilityElement* pAccessibilityElement = new(std::nothrow) AccessibilityElement(*element);
    if (!pAccessibilityElement) {
        HILOG_ERROR("Failed to create AccessibilityElement.");
        return nullptr;
    }
    if (ANI_OK != ANIUtils::Wrap(env, elementObj, pAccessibilityElement)) {
        HILOG_ERROR("Cannot wrap AccessibilityElementInfo");
        return nullptr;
    }
    return elementObj;
}

void CreateJsAccessibilityAbilityEventInfoInner(ani_env *env,
    ani_object &object, std::shared_ptr<AccessibilityEventTypeCallbackInfo> callbackInfo)
{
    if (env == nullptr  || object == nullptr || callbackInfo == nullptr) {
        HILOG_ERROR("invalid args");
        return;
    }

    do {
        constexpr const char* eventTypeCls =
            "@ohos.accessibility.AccessibilityEventType";
        if (!ANIUtils::SetEnumProperty(env, object, eventTypeCls, "eventType",
            callbackInfo->AccessibilityEventType_)) {
            HILOG_ERROR("Failed to set eventType");
        }
        if (!ANIUtils::SetLongPropertyRef(env, object, "timestamp", callbackInfo->timeStamp_)) {
            HILOG_ERROR("set timestamp failed");
            return;
        }
        if (!ANIUtils::SetStringProperty(env, object, "extraInfo", callbackInfo->extraInfo_)) {
            HILOG_ERROR("set extraInfo failed");
            return;
        }
        ani_object elementObj = ConvertAccessibilityElementToJS(env, callbackInfo->element_);
        if (elementObj == nullptr) {
            HILOG_ERROR("ConvertAccessibilityElementToJS failed");
            return;
        }
        if ((env->Object_SetPropertyByName_Ref(object, "target", elementObj)) != ANI_OK) {
            HILOG_ERROR("set target failed");
            return;
        }
    } while (0);
    return;
}

void CreateAccessibilityAbilityEventInfo(ani_env *env, ani_object object,
    std::shared_ptr<AccessibilityEventTypeCallbackInfo> callbackInfo)
{
    ani_class infoCls = nullptr;
    ani_class abilityCls = nullptr;
    ani_method ctor = nullptr;
    ani_method callback = nullptr;
    ani_object infoObject = nullptr;
    constexpr const char* AccessibilityEventInfoimpl =
        "@ohos.application.AccessibilityExtensionAbility.AccessibilityEventInfoImpl";
    if (ANI_OK != env->FindClass(AccessibilityEventInfoimpl, &infoCls)) {
        HILOG_ERROR("class not found: %{public}s", AccessibilityEventInfoimpl);
        return ;
    }
    if (ANI_OK != env->Class_FindMethod(infoCls, "<ctor>", nullptr, &ctor)) {
        HILOG_ERROR("get ctor Failed: %{public}s", AccessibilityEventInfoimpl);
        return ;
    }
    if ((env->Object_New(infoCls, ctor, &infoObject)) != ANI_OK || infoObject == nullptr) {
        HILOG_ERROR("Object_New failed");
        return ;
    }
    static const char *className = ANI_ACCESSIBILITY_EXTENSION_CLS;
    if (ANI_OK != env->FindClass(className, &abilityCls)) {
        HILOG_ERROR("class not found: %{public}s", className);
        return ;
    }
    if (ANI_OK != env->Class_FindMethod(abilityCls, "onAccessibilityEventInfo", nullptr, &callback)) {
        HILOG_ERROR("Class_FindMethod Failed");
        return ;
    }
    CreateJsAccessibilityAbilityEventInfoInner(env, infoObject, callbackInfo);
    if (ANI_OK != env->Object_CallMethod_Void(object, callback, infoObject)) {
        HILOG_ERROR("Object_CallMethod_Void Failed");
        return ;
    }
}

bool OnAccessibilityEventExec(ani_env *env, ani_object object,
    std::shared_ptr<AccessibilityEventTypeCallbackInfo> callbackInfo)
{
    auto task = [callbackInfo, env, object]() {
        CreateAccessibilityAbilityEventInfo(env, object, callbackInfo);
    };
    ANIUtils::SendEventToMainThread(task);
    return true;
}

void AniAccessibilityExtension::OnAccessibilityEvent(const AccessibilityEventInfo& eventInfo)
{
    HILOG_DEBUG();
    std::string strType = "";
    ConvertEventTypeToString(eventInfo, strType);
    if (strType.empty()) {
        HILOG_DEBUG("eventType is invalid.");
        return;
    }
    auto env = env_;
    if (env == nullptr) {
        HILOG_ERROR("env is null");
        return;
    }
    auto callbackInfo = std::make_shared<AccessibilityEventTypeCallbackInfo>();

    std::shared_ptr<AccessibilityElement> element = GetElement(eventInfo);
    callbackInfo->env_ = env;
    callbackInfo->extension_ = this;
    callbackInfo->eventType_ = strType;
    callbackInfo->AccessibilityEventType_ = ConvertStringToAccessibilityEventType(eventInfo, strType);
    callbackInfo->timeStamp_ = eventInfo.GetTimeStamp();
    callbackInfo->element_ = element;
    callbackInfo->elementId_ = eventInfo.GetRequestFocusElementId();
    callbackInfo->textAnnouncedForAccessibility_ = eventInfo.GetTextAnnouncedForAccessibility();
    callbackInfo->extraInfo_ = GetEventExtraInfo(eventInfo);
    ani_object thisObj = jsObj_->aniObj;
    OnAccessibilityEventExec(env, thisObj, callbackInfo);
    return;
}

sptr<IRemoteObject> AniAccessibilityExtension::OnConnect(const AAFwk::Want &want)
{
    HILOG_DEBUG();
    Extension::OnConnect(want);
    sptr<AccessibleAbilityClient> aaClient = AccessibleAbilityClient::GetInstance();
    if (!aaClient) {
        HILOG_ERROR("aaClient is nullptr");
        return nullptr;
    }
    aaClient->RegisterAbilityListener(listener_);
    return aaClient->GetRemoteObject();
}
}
}