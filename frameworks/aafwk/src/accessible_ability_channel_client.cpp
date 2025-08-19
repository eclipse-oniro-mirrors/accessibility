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

#include "accessible_ability_channel_client.h"

#include <cinttypes>
#ifdef OHOS_BUILD_ENABLE_HITRACE
#include <hitrace_meter.h>
#endif // OHOS_BUILD_ENABLE_HITRACE
#include "accessibility_element_operator_callback_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr uint32_t TIME_OUT_OPERATOR = 5000;
    constexpr int32_t REQUEST_ID_MAX = 0x0000FFFF;
} // namespace

int32_t AccessibleAbilityChannelClient::GenerateRequestId()
{
    int32_t requestId = requestId_++;
    requestId = requestId % REQUEST_ID_MAX;

    return requestId;
}

sptr<IRemoteObject> AccessibleAbilityChannelClient::GetRemote()
{
    return proxy_->AsObject();
}

void AccessibleAbilityChannelClient::SetOnKeyPressEventResult(const bool handled, const int32_t sequence)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        proxy_->SetOnKeyPressEventResult(handled, sequence);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
    }
}

RetError AccessibleAbilityChannelClient::FindFocusedElementInfo(int32_t accessibilityWindowId,
    int64_t elementId, int32_t focusType, AccessibilityElementInfo &elementInfo, bool systemApi)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "FindFocusedElement");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("FindFocusedElementInfo Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }

    int32_t requestId = GenerateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("FindFocusedElementInfo Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();

    int32_t windowId = accessibilityWindowId;
    if (accessibilityWindowId == ANY_WINDOW_ID && focusType == FOCUS_TYPE_ACCESSIBILITY &&
        accessibilityFocusedWindowId_ != INVALID_WINDOW_ID) {
        windowId = accessibilityFocusedWindowId_;
        HILOG_INFO("Convert into accessibility focused window id[%{public}d]", windowId);
    }

    RetError ret = proxy_->FindFocusedElementInfo(windowId,
        elementId, focusType, requestId, elementOperator, systemApi);
    if (ret != RET_OK) {
        HILOG_ERROR("FindFocusedElementInfo failed. ret[%{public}d]", ret);
        return ret;
    }
    HILOG_DEBUG("channelId:%{public}d, windowId:%{public}d, elementId:%{public}" PRId64 ", focusType:%{public}d",
        channelId_, windowId, elementId, focusType);

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("FindFocusedElementInfo Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    if (elementOperator->accessibilityInfoResult_.GetAccessibilityId() ==
        AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
        HILOG_ERROR("FindFocusedElementInfo The elementInfo from ace is wrong");
        return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
    }
    HILOG_INFO("Get result successfully from ace.");

    elementInfo = elementOperator->accessibilityInfoResult_;
    elementInfo.SetMainWindowId(windowId);
    elementInfo.SetWindowId(windowId);
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::SendSimulateGesture(
    const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->SendSimulateGesture(gesturePath);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
}

RetError AccessibleAbilityChannelClient::GetCursorPosition(
    int32_t accessibilityWindowId, int64_t elementId, int32_t &position)
{
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "GetCursorPosition");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("GetCursorPosition Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }

    int32_t requestId = GenerateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("GetCursorPosition Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();

    RetError ret = proxy_->GetCursorPosition(accessibilityWindowId, elementId, requestId, elementOperator);
    if (ret != RET_OK) {
        HILOG_ERROR("ExecuteAction failed. ret[%{public}d]", ret);
        return ret;
    }

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("GetCursorPosition Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    position = elementOperator->CursorPosition_;
    HILOG_INFO("position%{public}d", position);
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::ExecuteAction(int32_t accessibilityWindowId,
    int64_t elementId, int32_t action, const std::map<std::string, std::string> &actionArguments)
{
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "ExecuteAction");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("ExecuteAction Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    if (action == ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS &&
        accessibilityFocusedElementId_ != INVALID_WINDOW_ID && accessibilityFocusedWindowId_ != INVALID_WINDOW_ID) {
        ExecuteAction(accessibilityFocusedWindowId_, accessibilityFocusedElementId_,
            ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS, actionArguments);
    }

    int32_t requestId = GenerateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("ExecuteAction Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();

    RetError ret = proxy_->ExecuteAction(accessibilityWindowId,
        elementId, action, actionArguments, requestId, elementOperator);
    if (ret != RET_OK) {
        HILOG_ERROR("ExecuteAction failed. action[%{public}d], ret[%{public}d]", action, ret);
        return ret;
    }

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("execute action: %{public}d failed to wait result", action);
        return RET_ERR_TIME_OUT;
    }
    HILOG_INFO("action:[%{public}d], executeActionResult_[%{public}d], elementId:%{public}" PRId64 "",
        action, elementOperator->executeActionResult_, elementId);

    if (elementOperator->executeActionResult_) {
        switch (action) {
            case ActionType::ACCESSIBILITY_ACTION_ACCESSIBILITY_FOCUS:
                accessibilityFocusedWindowId_ = accessibilityWindowId;
                accessibilityFocusedElementId_ = elementId;
                break;
            case ActionType::ACCESSIBILITY_ACTION_CLEAR_ACCESSIBILITY_FOCUS:
                accessibilityFocusedWindowId_ = INVALID_WINDOW_ID;
                accessibilityFocusedElementId_ = INVALID_WINDOW_ID;
                break;
            default:
                break;
        }
    }
    return elementOperator->executeActionResult_ ? RET_OK : RET_ERR_PERFORM_ACTION_FAILED_BY_ACE;
}

RetError AccessibleAbilityChannelClient::EnableScreenCurtain(bool isEnable)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("EnableScreenCurtain Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return  proxy_->EnableScreenCurtain(isEnable) ? RET_OK : RET_ERR_PERFORM_ACTION_FAILED_BY_ACE;
}

RetError AccessibleAbilityChannelClient::HoldRunningLock()
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("HoldRunningLock Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->HoldRunningLock();
}

RetError AccessibleAbilityChannelClient::UnholdRunningLock()
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("UnholdRunningLock Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->UnholdRunningLock();
}

RetError AccessibleAbilityChannelClient::SearchElementInfosByAccessibilityId(int32_t accessibilityWindowId,
    int64_t elementId, int32_t mode, std::vector<AccessibilityElementInfo> &elementInfos, int32_t treeId,
    bool isFilter, bool systemApi)
{
    int32_t requestId = GenerateRequestId();
    HILOG_DEBUG("channelId:%{public}d, elementId:%{public}" PRId64 ", windowId:%{public}d, requestId:%{public}d",
        channelId_, elementId, accessibilityWindowId, requestId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SearchElementById");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("SearchElementInfosByAccessibilityId Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }

    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("SearchElementInfosByAccessibilityId Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();
    ElementBasicInfo elementBasicInfo {};
    elementBasicInfo.windowId = accessibilityWindowId;
    elementBasicInfo.treeId = treeId;
    elementBasicInfo.elementId = elementId;

    RetError ret = proxy_->SearchElementInfoByAccessibilityId(elementBasicInfo, requestId,
        elementOperator, mode, isFilter, systemApi);
    if (ret != RET_OK) {
        HILOG_ERROR("searchElement failed. ret: %{public}d. elementId: %{public}" PRId64 ", requestId :[%{public}d]",
            ret, elementId, requestId);
        return ret;
    }

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("SearchElementInfosByAccessibilityId Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    for (auto &info : elementOperator->elementInfosResult_) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfosByAccessibilityId The elementInfo from ace is wrong");
            return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
        }
    }
    HILOG_DEBUG("Get result successfully from ace. size[%{public}zu]", elementOperator->elementInfosResult_.size());
    elementInfos = elementOperator->elementInfosResult_;
    if (!elementInfos.empty()) {
        for (auto &element : elementInfos) {
            element.SetMainWindowId(accessibilityWindowId);
            element.SetWindowId(accessibilityWindowId);
        }
    }
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::SearchDefaultFocusedByWindowId(int32_t accessibilityWindowId,
    int64_t elementId, int32_t mode, std::vector<AccessibilityElementInfo> &elementInfos, int32_t treeId, bool isFilter)
{
    int32_t requestId = GenerateRequestId();
    HILOG_DEBUG("channelId:%{public}d, windowId:%{public}d, requestId:%{public}d",
        channelId_, accessibilityWindowId, requestId);
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SearchDefaultFocusedByWindowId");
    if (proxy_ == nullptr) {
        HILOG_ERROR("SearchDefaultFocusedByWindowId Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }
 
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("SearchDefaultFocusedByWindowId Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();
    ElementBasicInfo elementBasicInfo {};
    elementBasicInfo.windowId = accessibilityWindowId;
    elementBasicInfo.elementId = elementId;
    elementBasicInfo.treeId = treeId;
 
    RetError ret = proxy_->SearchDefaultFocusedByWindowId(elementBasicInfo, requestId,
        elementOperator, mode, isFilter);
    if (ret != RET_OK) {
        HILOG_ERROR("searchElement failed. ret: %{public}d, requestId :[%{public}d]",
            ret, requestId);
        return ret;
    }
 
    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("SearchElementInfosByAccessibilityId Failed to wait result");
        return RET_ERR_TIME_OUT;
    }
 
    for (auto &info : elementOperator->elementInfosResult_) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfosByAccessibilityId The elementInfo from ace is wrong");
            return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
        }
    }
    elementInfos = elementOperator->elementInfosResult_;
    HILOG_DEBUG("Get result successfully from arkUI elementInfos size[%{public}zu]", elementInfos.size());
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::GetWindow(const int32_t windowId, AccessibilityWindowInfo &windowInfo)
{
    HILOG_DEBUG("[channelId:%{public}d] [windowId:%{public}d]", channelId_, windowId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER(HITRACE_TAG_ACCESSIBILITY_MANAGER);
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->GetWindow(windowId, windowInfo);
}

RetError AccessibleAbilityChannelClient::GetWindows(std::vector<AccessibilityWindowInfo> &windows, bool systemApi)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER(HITRACE_TAG_ACCESSIBILITY_MANAGER);
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_) {
        return proxy_->GetWindows(windows, systemApi);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
}

RetError AccessibleAbilityChannelClient::GetWindows(const uint64_t displayId,
    std::vector<AccessibilityWindowInfo> &windows, bool systemApi) const
{
    HILOG_DEBUG("[channelId:%{public}d] [displayId:%{public}" PRIu64 "]", channelId_, displayId);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "GetWindowsByDisplayId");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_) {
        return proxy_->GetWindowsByDisplayId(displayId, windows, systemApi);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
}

RetError AccessibleAbilityChannelClient::SearchElementInfosByText(int32_t accessibilityWindowId,
    int64_t elementId, const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos, bool systemApi)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "SearchElementByText");
#endif // OHOS_BUILD_ENABLE_HITRACE
    if (proxy_ == nullptr) {
        HILOG_ERROR("SearchElementInfosByText Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }

    int32_t requestId = GenerateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("SearchElementInfosByText Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();

    RetError ret = proxy_->SearchElementInfosByText(accessibilityWindowId,
        elementId, text, requestId, elementOperator, systemApi);
    if (ret != RET_OK) {
        HILOG_ERROR("SearchElementInfosByText failed. ret[%{public}d]", ret);
        return ret;
    }

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("SearchElementInfosByText Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    for (auto &info : elementOperator->elementInfosResult_) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfosByText The elementInfo from ace is wrong");
            return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
        }
    }
    HILOG_INFO("Get result successfully from ace. size[%{public}zu]", elementOperator->elementInfosResult_.size());
    elementInfos = elementOperator->elementInfosResult_;
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::FocusMoveSearch(int32_t accessibilityWindowId,
    int64_t elementId, int32_t direction, AccessibilityElementInfo &elementInfo, bool systemApi)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("FocusMoveSearch Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }

    int32_t requestId = GenerateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (elementOperator == nullptr) {
        HILOG_ERROR("FocusMoveSearch Failed to create elementOperator.");
        return RET_ERR_NULLPTR;
    }
    ffrt::future<void> promiseFuture = elementOperator->promise_.get_future();

    RetError ret =
        proxy_->FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, elementOperator, systemApi);
    if (ret != RET_OK) {
        HILOG_ERROR("FocusMoveSearch failed. ret[%{public}d]", ret);
        return ret;
    }

    ffrt::future_status wait = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != ffrt::future_status::ready) {
        HILOG_ERROR("FocusMoveSearch Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    if (elementOperator->accessibilityInfoResult_.GetAccessibilityId() ==
        AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
        HILOG_ERROR("FocusMoveSearch The elementInfo from ace is wrong");
        return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
    }

    HILOG_INFO("Get result successfully from ace");
    elementInfo = elementOperator->accessibilityInfoResult_;
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::SetTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->SetTargetBundleName(targetBundleNames);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
}

RetError AccessibleAbilityChannelClient::SetIsRegisterDisconnectCallback(bool isRegister)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("SetIsRegisterDisconnectCallback Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->SetIsRegisterDisconnectCallback(isRegister);
}

RetError AccessibleAbilityChannelClient::NotifyDisconnect()
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_ == nullptr) {
        HILOG_ERROR("NotifyDisconnect Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->NotifyDisconnect();
}

RetError AccessibleAbilityChannelClient::ConfigureEvents(const std::vector<uint32_t> needEvents)
{
    HILOG_INFO("uitest config need events size is %{public}zu", needEvents.size());
    if (proxy_ == nullptr) {
        HILOG_ERROR("ConfigureEvents Failed to connect to aams [channelId:%{public}d]", channelId_);
        return RET_ERR_SAMGR;
    }
    return proxy_->ConfigureEvents(needEvents);
}

RetError AccessibleAbilityChannelClient::SearchElementInfosBySpecificProperty(int32_t accessibilityWindowId,
    int64_t elementId, const SpecificPropertyParam& param, std::vector<AccessibilityElementInfo> &infos,
    std::vector<AccessibilityElementInfo> &treeInfos, int32_t treeId)
{
    int32_t requestId = GenerateRequestId();
    HILOG_DEBUG("channelId:%{public}d, elementId:%{public}" PRId64 ", windowId:%{public}d, "
        "requestId:%{public}d, propertyTarget:%{public}s, propertyType:%{public}u",
        channelId_, elementId, accessibilityWindowId, requestId, param.propertyTarget.c_str(),
        static_cast<uint32_t>(param.propertyType));

    if (proxy_ == nullptr) {
        HILOG_ERROR("SearchElementInfosBySpecificProperty Failed to connect to aams [channelId:%{public}d]",
            channelId_);
        return RET_ERR_SAMGR;
    }

    ElementBasicInfo elementBasicInfo;
    elementBasicInfo.windowId = accessibilityWindowId;
    elementBasicInfo.elementId = elementId;
    elementBasicInfo.treeId = treeId;

    sptr<AccessibilityElementOperatorCallbackImpl> callback =
        new(std::nothrow) AccessibilityElementOperatorCallbackImpl();
    if (callback == nullptr) {
        HILOG_ERROR("SearchElementInfosBySpecificProperty Failed to create callback");
        return RET_ERR_NULLPTR;
    }

    ffrt::future<void> promiseFuture = callback->promise_.get_future();
    proxy_->SearchElementInfoBySpecificProperty(elementBasicInfo, param, requestId, callback);

    ffrt::future_status waitFocus = promiseFuture.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (waitFocus != ffrt::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return RET_ERR_TIME_OUT;
    }

    if (!callback->elementInfosResult_.empty()) {
        RetError ret = ValidateAndProcessElementInfos(callback->elementInfosResult_, infos, treeInfos,
            accessibilityWindowId, "elementInfosResult_");
        if (ret != RET_OK) {
            return ret;
        }
    } else if (!callback->treeInfosResult_.empty()) {
        RetError ret = ValidateAndProcessElementInfos(callback->treeInfosResult_, treeInfos, infos,
            accessibilityWindowId, "treeInfosResult_");
        if (ret != RET_OK) {
            return ret;
        }
    } else {
        infos.clear();
        treeInfos.clear();
        HILOG_DEBUG("Both result sets are empty");
    }
    return RET_OK;
}

RetError AccessibleAbilityChannelClient::ValidateAndProcessElementInfos(
    const std::vector<AccessibilityElementInfo>& sourceInfos,
    std::vector<AccessibilityElementInfo>& targetInfos,
    std::vector<AccessibilityElementInfo>& clearInfos,
    int32_t accessibilityWindowId,
    const std::string& logType)
{
    for (const auto &info : sourceInfos) {
        if (info.GetAccessibilityId() == AccessibilityElementInfo::UNDEFINED_ACCESSIBILITY_ID) {
            HILOG_ERROR("SearchElementInfosBySpecificProperty The %{public}s from ace is wrong", logType.c_str());
            return RET_ERR_INVALID_ELEMENT_INFO_FROM_ACE;
        }
    }

    targetInfos = sourceInfos;
    clearInfos.clear();

    for (auto &element : targetInfos) {
        element.SetMainWindowId(accessibilityWindowId);
        element.SetWindowId(accessibilityWindowId);
    }

    HILOG_DEBUG("Found results in %{public}s, size: %{public}zu", logType.c_str(), targetInfos.size());
    return RET_OK;
}
} // namespace Accessibility
} // namespace OHOS
