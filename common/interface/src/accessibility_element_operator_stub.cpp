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

#include "accessibility_element_operator_stub.h"
#include "accessibility_element_operator_callback_proxy.h"
#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"
#include <cinttypes>

#define SWITCH_BEGIN(code) switch (code) {
#define SWITCH_CASE(case_code, func)     \
    case case_code: {                    \
        result_code = func(data, reply); \
        break;                           \
    }

#define SWITCH_END()                                                                                \
    default: {                                                                                      \
        result_code = ERR_CODE_DEFAULT;                                                             \
        HILOG_WARN("AccessibilityElementOperatorStub::OnRemoteRequest, default case, need check."); \
        break;                                                                                      \
    }                                                                                               \
        }

#define ACCESSIBILITY_ELEMENT_OPERATOR_STUB_CASES()                                                               \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_BY_ACCESSIBILITY_ID, HandleSearchElementInfoByAccessibilityId) \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_BY_TEXT, HandleSearchElementInfosByText)                       \
    SWITCH_CASE(AccessibilityInterfaceCode::FIND_FOCUSED_INFO, HandleFindFocusedElementInfo)                      \
    SWITCH_CASE(AccessibilityInterfaceCode::FOCUS_FIND, HandleFocusFind)                                          \
    SWITCH_CASE(AccessibilityInterfaceCode::PERFORM_ACTION_ELEMENT, HandleExecuteAction)                          \
    SWITCH_CASE(AccessibilityInterfaceCode::CURSOR_POSITION, HandleGetCursorPosition)                             \
    SWITCH_CASE(AccessibilityInterfaceCode::CLEAR_FOCUS, HandleClearFocus)                                        \
    SWITCH_CASE(AccessibilityInterfaceCode::OUTSIDE_TOUCH, HandleOutsideTouch)                                    \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_CHILDTREEID, HandleSetChildTreeIdAndWinId)                        \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_BELONGTREEID, HandleSetBelongTreeId)                              \
    SWITCH_CASE(AccessibilityInterfaceCode::SET_PARENTWINDOWID, HandleSetParentWindowId)                          \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_BY_WINDOW_ID, HandleSearchDefaultFocusedByWindowId)            \
    SWITCH_CASE(AccessibilityInterfaceCode::SEARCH_BY_SPECIFIC_PROPERTY, HandleSearchElementInfoBySpecificProperty) \
    SWITCH_CASE(AccessibilityInterfaceCode::ASAC_FOCUS_MOVE_SEARCH_WITH_CONDITION, HandleFocusMoveSearchWithCondition) \

namespace OHOS {
namespace Accessibility {
constexpr int32_t ERR_CODE_DEFAULT = -1000;

AccessibilityElementOperatorStub::AccessibilityElementOperatorStub()
{
}

AccessibilityElementOperatorStub::~AccessibilityElementOperatorStub()
{
}

int AccessibilityElementOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    HILOG_DEBUG("cmd = %{public}u, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = AccessibilityElementOperatorStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    ErrCode result_code = ERR_NONE;
    SWITCH_BEGIN(code)
    ACCESSIBILITY_ELEMENT_OPERATOR_STUB_CASES()
    SWITCH_END()

    if (result_code != ERR_CODE_DEFAULT) {
        return result_code;
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfoByAccessibilityId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int64_t elementId = data.ReadInt64();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t mode = data.ReadInt32();
    bool isFilter = data.ReadBool();
    SearchElementInfoByAccessibilityId(elementId, requestId, callback, mode, isFilter);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchDefaultFocusedByWindowId(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();
 
    int32_t windowId = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
 
    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t mode = data.ReadInt32();
    bool isFilter = data.ReadBool();
    SearchDefaultFocusedByWindowId(windowId, requestId, callback, mode, isFilter);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfosByText(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int64_t elementId = data.ReadInt64();
    std::string text = data.ReadString();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    SearchElementInfosByText(elementId, text, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFindFocusedElementInfo(MessageParcel &data,
    MessageParcel &reply)
{
    HILOG_DEBUG();

    int64_t elementId = data.ReadInt64();
    int32_t focusType = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    FindFocusedElementInfo(elementId, focusType, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusFind(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    int64_t elementId = data.ReadInt64();
    int32_t direction = data.ReadInt32();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    FocusMoveSearch(elementId, direction, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleExecuteAction(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    std::vector<std::string> argumentKey;
    std::vector<std::string> argumentValue;
    int64_t elementId = data.ReadInt64();
    int32_t action = data.ReadInt32();
    if (!data.ReadStringVector(&argumentKey)) {
        return ERR_TRANSACTION_FAILED;
    }
    if (!data.ReadStringVector(&argumentValue)) {
        return ERR_TRANSACTION_FAILED;
    }
    if (argumentKey.size() != argumentValue.size()) {
        return ERR_TRANSACTION_FAILED;
    }
    std::map<std::string, std::string> arguments;
    for (size_t i = 0;i < argumentKey.size(); i++) {
        arguments.insert(std::pair<std::string, std::string>(argumentKey[i], argumentValue[i]));
    }
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    ExecuteAction(elementId, action, arguments, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleGetCursorPosition(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int64_t elementId = data.ReadInt64();
    int32_t requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    GetCursorPosition(elementId, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleClearFocus(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    ClearFocus();

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleOutsideTouch(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();

    OutsideTouch();

    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSetChildTreeIdAndWinId(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int64_t elementId = data.ReadInt64();
    int32_t treeId = data.ReadInt32();
    int32_t childWindowId = data.ReadInt32();
    SetChildTreeIdAndWinId(elementId, treeId, childWindowId);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSetBelongTreeId(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t treeId = data.ReadInt32();

    SetBelongTreeId(treeId);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSetParentWindowId(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    int32_t iParentWindowId = data.ReadInt32();

    SetParentWindowId(iParentWindowId);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleSearchElementInfoBySpecificProperty(MessageParcel &data,
    MessageParcel &reply)
{
    int64_t elementId = data.ReadInt64();

    SpecificPropertyParam param;
    param.propertyTarget = data.ReadString();
    param.propertyType = static_cast<SEARCH_TYPE>(data.ReadUint32());

    int32_t requestId = data.ReadInt32();

    HILOG_DEBUG("elementId:%{public}" PRId64 ", propertyTarget:%{public}s, propertyType:%{public}u, "
        "requestId:%{public}d", elementId, param.propertyTarget.c_str(),
        static_cast<uint32_t>(param.propertyType), requestId);

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }

    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    SearchElementInfoBySpecificProperty(elementId, param, requestId, callback);
    return NO_ERROR;
}

ErrCode AccessibilityElementOperatorStub::HandleFocusMoveSearchWithCondition(MessageParcel &data, MessageParcel &reply)
{
    HILOG_DEBUG();
    sptr<AccessibilityElementInfoParcel> info = data.ReadStrongParcelable<AccessibilityElementInfoParcel>();
    if (info == nullptr) {
        HILOG_ERROR("ReadStrongParcelable<AccessibilityElementInfoParcel> failed");
        return ERR_INVALID_VALUE;
    }
    AccessibilityFocusMoveParam param;
    param.direction = static_cast<FocusMoveDirection>(data.ReadInt32());
    param.condition = static_cast<DetailCondition>(data.ReadInt32());
    param.parentId = data.ReadInt64();
    param.detectParent = data.ReadBool();
    int requestId = data.ReadInt32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("remote is nullptr.");
        return ERR_INVALID_VALUE;
    }
    sptr<IAccessibilityElementOperatorCallback> callback = iface_cast<IAccessibilityElementOperatorCallback>(remote);
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return ERR_INVALID_VALUE;
    }
    FocusMoveSearchWithCondition(*info, param, requestId, callback);
    return NO_ERROR;
}
} // namespace Accessibility
} // namespace OHOS