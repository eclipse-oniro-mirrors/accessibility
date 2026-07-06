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

#include "accessibility_element_operator_proxy.h"
#include "accessibility_element_info_parcel.h"
#include "hilog_wrapper.h"
#include <cinttypes>

namespace OHOS {
namespace Accessibility {
AccessibilityElementOperatorProxy::AccessibilityElementOperatorProxy(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IAccessibilityElementOperator>(impl)
{}

bool AccessibilityElementOperatorProxy::WriteInterfaceToken(MessageParcel &data)
{
    HILOG_DEBUG();

    if (!data.WriteInterfaceToken(AccessibilityElementOperatorProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::SendTransactCmd(AccessibilityInterfaceCode code,
    MessageParcel &data, MessageParcel &reply,  MessageOption &option)
{
    HILOG_DEBUG();
    sptr<IRemoteObject> remoteObj = Remote();
    if (remoteObj == nullptr) {
        HILOG_ERROR("fail to send transact cmd %{public}d due to remote object", code);
        return false;
    }
    int32_t result = remoteObj->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != NO_ERROR) {
        HILOG_ERROR("receive error transact code %{public}d in transact cmd %{public}d", result, code);
        return false;
    }
    return true;
}

RetError AccessibilityElementOperatorProxy::SearchElementInfoByAccessibilityId(const int64_t elementId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return RET_ERR_FAILED;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write parcelable element id failed");
        return RET_ERR_FAILED;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return RET_ERR_FAILED;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return RET_ERR_NULLPTR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return RET_ERR_FAILED;
    }

    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("connection write parcelable mode failed");
        return RET_ERR_FAILED;
    }

    if (!data.WriteBool(isFilter)) {
        HILOG_ERROR("connection write parcelable isFilter failed");
        return RET_ERR_FAILED;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_ACCESSIBILITY_ID,
        data, reply, option)) {
        HILOG_ERROR("search element info by accessibility id failed");
        return RET_ERR_FAILED;
    }
    return RET_OK;
}

void AccessibilityElementOperatorProxy::SearchDefaultFocusedByWindowId(const int32_t windowId,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback, const int32_t mode,
    bool isFilter)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);
 
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
 
    if (!data.WriteInt32(windowId)) {
        HILOG_ERROR("connection write parcelable window id failed");
        return;
    }
 
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }
 
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }
 
    if (!data.WriteInt32(mode)) {
        HILOG_ERROR("connection write parcelable mode failed");
        return;
    }
 
    if (!data.WriteBool(isFilter)) {
        HILOG_ERROR("connection write parcelable isFilter failed");
        return;
    }
 
    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_WINDOW_ID,
        data, reply, option)) {
        HILOG_ERROR("search element info by accessibility id failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SearchElementInfosByText(const int64_t elementId,
    const std::string &text,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write parcelable elementId failed");
        return;
    }
    if (!data.WriteString(text)) {
        HILOG_ERROR("connection write text failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_TEXT, data, reply, option)) {
        HILOG_ERROR("search element infos by text failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FindFocusedElementInfo(const int64_t elementId,
    const int32_t focusType, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!data.WriteInt32(focusType)) {
        HILOG_ERROR("connection write focusType failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write requestId failed");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::FIND_FOCUSED_INFO, data, reply, option)) {
        HILOG_ERROR("find focused element info failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearch(const int64_t elementId,
    const int32_t direction, const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("fail, connection write Token");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("fail, connection write elementId error");
        return;
    }

    if (!data.WriteInt32(direction)) {
        HILOG_ERROR("fail, connection write focusType error");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("fail, connection write requestId error");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr.");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("fail, connection write callback error");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::FOCUS_FIND, data, reply, option)) {
        HILOG_ERROR("focus move search failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::ExecuteAction(const int64_t elementId, const int32_t action,
    const std::map<std::string, std::string> &arguments, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write element id failed");
        return;
    }
    if (!data.WriteInt32(action)) {
        HILOG_ERROR("connection write focus type failed");
        return;
    }
    std::vector<std::string> keys;
    std::vector<std::string> values;
    for (const auto& [key, value] : arguments) {
        keys.push_back(key);
        values.push_back(value);
    }
    if (!data.WriteStringVector(keys)) {
        HILOG_ERROR("connection write argument keys failed");
        return;
    }
    if (!data.WriteStringVector(values)) {
        HILOG_ERROR("connection write argument values failed");
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write request id failed");
        return;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::PERFORM_ACTION_ELEMENT, data, reply, option)) {
        callback->SetExecuteActionResult(false, requestId);
        HILOG_ERROR("execute action failed");
        return;
    }
}


void AccessibilityElementOperatorProxy::GetCursorPosition(const int64_t elementId, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write requestId failed");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::CURSOR_POSITION, data, reply, option)) {
        HILOG_ERROR("find cursor position info failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::ClearFocus()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::CLEAR_FOCUS, data, reply, option)) {
        HILOG_ERROR("clear focus failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::OutsideTouch()
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::OUTSIDE_TOUCH, data, reply, option)) {
        HILOG_ERROR("outside touch failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SetChildTreeIdAndWinId(const int64_t elementId, const int32_t treeId,
    const int32_t childWindowId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!data.WriteInt32(treeId)) {
        HILOG_ERROR("connection write treeId failed");
        return;
    }

    if (!data.WriteInt32(childWindowId)) {
        HILOG_ERROR("connection write childWindowId failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_CHILDTREEID, data, reply, option)) {
        HILOG_ERROR("clear focus failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SetBelongTreeId(const int32_t treeId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt32(treeId)) {
        HILOG_ERROR("connection write elementId failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_BELONGTREEID, data, reply, option)) {
        HILOG_ERROR("clear focus failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SetParentWindowId(const int32_t iParentWindowId)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC | MessageOption::TF_ASYNC_WAKEUP_LATER);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt32(iParentWindowId)) {
        HILOG_ERROR("connection write iParentWindowId failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SET_PARENTWINDOWID, data, reply, option)) {
        HILOG_ERROR("clear focus failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::SearchElementInfoBySpecificProperty(const int64_t elementId,
    const SpecificPropertyParam& param, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG("elementId:%{public}" PRId64 ", propertyTarget:%{public}s, propertyType:%{public}u, "
        "requestId:%{public}d", elementId, param.propertyTarget.c_str(),
        static_cast<uint32_t>(param.propertyType), requestId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("connection write parcelable element id failed");
        return;
    }

    if (!data.WriteString(param.propertyTarget)) {
        HILOG_ERROR("connection write parcelable property target failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(param.propertyType))) {
        HILOG_ERROR("connection write parcelable property type failed");
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::SEARCH_BY_SPECIFIC_PROPERTY,
        data, reply, option)) {
        HILOG_ERROR("search element info by specific property failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::FocusMoveSearchWithCondition(const AccessibilityElementInfo &info,
    const AccessibilityFocusMoveParam &param, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    AccessibilityElementInfoParcel infoParcel(info);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }

    if (!data.WriteParcelable(&infoParcel)) {
        HILOG_ERROR("connection write info failed");
        return;
    }

    if (!data.WriteInt32(param.direction)) {
        return;
    }

    if (!data.WriteInt32(param.condition)) {
        return;
    }

    if (!data.WriteInt64(param.parentId)) {
        return;
    }

    if (!data.WriteBool(param.detectParent)) {
        return;
    }

    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }

    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }

    if (!SendTransactCmd(AccessibilityInterfaceCode::ASAC_FOCUS_MOVE_SEARCH_WITH_CONDITION,
        data, reply, option)) {
        HILOG_ERROR("search element info by specific property failed");
        return;
    }
}

void AccessibilityElementOperatorProxy::UpdateCustomAccessibilityProperty(const int64_t elementId,
    const AccessibilityVirtualNode& accessibilityVirtualNode,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
    if (!data.WriteInt64(elementId)) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", elementId);
        return;
    }
    if (!WriteAccessibilityVirtualNode(data, accessibilityVirtualNode)) {
        return;
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::ASAC_UPDATE_ACCESSIBILITY_ELEMENT_INFO,
        data, reply, option)) {
        HILOG_ERROR("fail to update accessibility element info");
        return;
    }
}
 
void AccessibilityElementOperatorProxy::AddAccessibilityVirtualNode(const int64_t rootId,
    const std::vector<AccessibilityVirtualNode> &nodes, const int32_t requestId,
    const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
    if (!data.WriteInt64(rootId)) {
        HILOG_ERROR("rootId write error: %{public}" PRId64 "", rootId);
        return;
    }
    int32_t nodeCount = nodes.size();
    if (!data.WriteInt32(nodeCount)) {
        HILOG_ERROR("nodeCount write error");
        return;
    }
    for (int32_t i = 0; i < nodeCount; i++) {
        if (!WriteAccessibilityVirtualNode(data, nodes[i])) {
            HILOG_ERROR("node[%{public}d] write error", i);
            return;
        }
    }
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::ASAC_ADD_ACCESSIBILITY_VIRTUAL_NODE,
        data, reply, option)) {
        HILOG_ERROR("fail to add accessibility virtual node");
        return;
    }
}
 
void AccessibilityElementOperatorProxy::RemoveAccessibilityVirtualNode(const int64_t id,
    const int32_t requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
{
    HILOG_DEBUG();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
 
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("connection write token failed");
        return;
    }
 
    if (!data.WriteInt64(id)) {
        HILOG_ERROR("id write error: %{public}" PRId64 "", id);
        return;
    }
 
    if (!data.WriteInt32(requestId)) {
        HILOG_ERROR("connection write parcelable request id failed");
        return;
    }
 
    if (callback == nullptr) {
        HILOG_ERROR("callback is nullptr");
        return;
    }
 
    if (!data.WriteRemoteObject(callback->AsObject())) {
        HILOG_ERROR("connection write parcelable callback failed");
        return;
    }
    if (!SendTransactCmd(AccessibilityInterfaceCode::ASAC_REMOVE_ACCESSIBILITY_VIRTUAL_NODE,
        data, reply, option)) {
        HILOG_ERROR("fail to remove accessibility virtual node");
        return;
    }
}

bool AccessibilityElementOperatorProxy::WriteAccessibilityVirtualNode(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!WriteVirtualNodeBasicProperties(data, accessibilityVirtualNode)) {
        return false;
    }
    if (!WriteVirtualNodeRectProperties(data, accessibilityVirtualNode)) {
        return false;
    }
    if (!WriteVirtualNodeBoolProperties(data, accessibilityVirtualNode)) {
        return false;
    }
    if (!WriteVirtualNodeOtherProperties(data, accessibilityVirtualNode)) {
        return false;
    }
    if (!WriteVirtualNodeRelationProperties(data, accessibilityVirtualNode)) {
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::WriteVirtualNodeBasicProperties(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!data.WriteInt64(accessibilityVirtualNode.GetId())) {
        HILOG_ERROR("id write error: %{public}" PRId64 "", accessibilityVirtualNode.GetId());
        return false;
    }
    if (!data.WriteString(accessibilityVirtualNode.GetText())) {
        HILOG_ERROR("text write error");
        return false;
    }
    if (!data.WriteString(accessibilityVirtualNode.GetAccessibilityText())) {
        HILOG_ERROR("accessibilityText write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetAccessibilityGroup())) {
        HILOG_ERROR("accessibilityGroup write error");
        return false;
    }
    if (!data.WriteString(accessibilityVirtualNode.GetAccessibilityLevel())) {
        HILOG_ERROR("accessibilityLevel write error");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::WriteVirtualNodeRectProperties(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!data.WriteInt32(accessibilityVirtualNode.GetRect().GetLeftTopXScreenPostion())) {
        HILOG_ERROR("rect leftTopX write error");
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetRect().GetLeftTopYScreenPostion())) {
        HILOG_ERROR("rect leftTopY write error");
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetRect().GetRightBottomXScreenPostion())) {
        HILOG_ERROR("rect rightBottomX write error");
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetRect().GetRightBottomYScreenPostion())) {
        HILOG_ERROR("rect rightBottomY write error");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::WriteVirtualNodeBoolProperties(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!data.WriteBool(accessibilityVirtualNode.GetCheckable())) {
        HILOG_ERROR("checkable write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetChecked())) {
        HILOG_ERROR("checked write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetClickable())) {
        HILOG_ERROR("clickable write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetEnabled())) {
        HILOG_ERROR("enabled write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetSelected())) {
        HILOG_ERROR("selected write error");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::WriteVirtualNodeOtherProperties(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!data.WriteString(accessibilityVirtualNode.GetCustomComponentType())) {
        HILOG_ERROR("customComponentType write error");
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetPoint().GetX())) {
        HILOG_ERROR("point.x write error");
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetPoint().GetY())) {
        HILOG_ERROR("point.y write error");
        return false;
    }
    if (!data.WriteBool(accessibilityVirtualNode.GetAccessibilityFocused())) {
        HILOG_ERROR("accessibilityFocused write error");
        return false;
    }
    return true;
}

bool AccessibilityElementOperatorProxy::WriteVirtualNodeRelationProperties(MessageParcel &data,
    const AccessibilityVirtualNode& accessibilityVirtualNode)
{
    if (!data.WriteInt64(accessibilityVirtualNode.GetParentId())) {
        HILOG_ERROR("parentId write error: %{public}" PRId64 "", accessibilityVirtualNode.GetParentId());
        return false;
    }
    int32_t childNodeCount = accessibilityVirtualNode.GetChildNodeIds().size();
    if (!data.WriteInt32(childNodeCount)) {
        HILOG_ERROR("childNodeCount write error");
        return false;
    }
    for (int32_t i = 0; i < childNodeCount; i++) {
        if (!data.WriteInt64(accessibilityVirtualNode.GetChildNodeIds()[i])) {
            HILOG_ERROR("childNodeId[%{public}d] write error", i);
            return false;
        }
    }
    if (!data.WriteInt64(accessibilityVirtualNode.GetElementId())) {
        HILOG_ERROR("elementId write error: %{public}" PRId64 "", accessibilityVirtualNode.GetElementId());
        return false;
    }
    if (!data.WriteInt32(accessibilityVirtualNode.GetWindowId())) {
        HILOG_ERROR("windowId write error: %{public}d", accessibilityVirtualNode.GetWindowId());
        return false;
    }
    if (!data.WriteUint64(accessibilityVirtualNode.GetSupportAction())) {
        HILOG_ERROR("supportAction write error");
        return false;
    }
    return true;
}
} // namespace Accessibility
} // namespace OHOS