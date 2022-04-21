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
#include "accessibility_element_operator_callback_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
static const uint32_t TIME_OUT_OPERATOR = 500;
static const int32_t REQUEST_ID_MAX = 0x7FFFFFFF;

int32_t AccessibleAbilityChannelClient::GenrateRequestId()
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

bool AccessibleAbilityChannelClient::FindFocusedElementInfo(int32_t accessibilityWindowId,
    int32_t elementId, int32_t focusType, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (!proxy_) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }

    int32_t requestId = GenrateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator = new AccessibilityElementOperatorCallbackImpl();
    std::future<void> promiseFutrue = elementOperator->promise_.get_future();

    if (!proxy_->FindFocusedElementInfo(accessibilityWindowId, elementId, focusType, requestId, elementOperator)) {
        return false;
    }
    HILOG_DEBUG("channelId:%{public}d, accessibilityWindowId:%{public}d, elementId:%{public}d, focusType:%{public}d",
        channelId_, accessibilityWindowId, elementId, focusType);

    std::future_status wait = promiseFutrue.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return false;
    }

    elementInfo = elementOperator->accessibilityInfoResult_;
    elementInfo.SetChannelId(channelId_);
    HILOG_DEBUG("[channelId:%{public}d] end", channelId_);

    return true;
}

void AccessibleAbilityChannelClient::SendSimulateGesture(const int32_t sequenceNum,
    const std::vector<AccessibilityGesturePath> &gestureSteps)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        proxy_->SendSimulateGesture(sequenceNum, gestureSteps);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
    }
}

bool AccessibleAbilityChannelClient::ExecuteAction(int32_t accessibilityWindowId,
    int32_t elementId, int32_t action,  std::map<std::string, std::string> &actionArguments)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (!proxy_) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }

    int32_t requestId = GenrateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator = new AccessibilityElementOperatorCallbackImpl();
    std::future<void> promiseFutrue = elementOperator->promise_.get_future();

    if (!proxy_->ExecuteAction(accessibilityWindowId, elementId, action, actionArguments, requestId, elementOperator)) {
        return false;
    }

    std::future_status wait = promiseFutrue.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return false;
    }

    return elementOperator->executeActionResult_;
}

bool AccessibleAbilityChannelClient::SearchElementInfosByAccessibilityId(int32_t accessibilityWindowId,
    int32_t elementId, int32_t mode, std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("[channelId:%{public}d] [windowId:%{public}d]", channelId_, accessibilityWindowId);
    if (!proxy_) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }

    int32_t requestId = GenrateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator = new AccessibilityElementOperatorCallbackImpl();
    std::future<void> promiseFutrue = elementOperator->promise_.get_future();

    if (!proxy_->SearchElementInfoByAccessibilityId(accessibilityWindowId, elementId, requestId,
        elementOperator, mode)) {
        return false;
    }

    std::future_status wait = promiseFutrue.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return false;
    }

    for (auto &info : elementOperator->elementInfosResult_) {
        info.SetChannelId(channelId_);
    }
    HILOG_DEBUG("search element info End[size:%{public}zu]", elementOperator->elementInfosResult_.size());
    elementInfos = elementOperator->elementInfosResult_;
    return true;
}

std::vector<AccessibilityWindowInfo> AccessibleAbilityChannelClient::GetWindows()
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        auto windows = proxy_->GetWindows();
        for (auto &window : windows) {
            window.SetChannelId(channelId_);
        }
        return windows;
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        std::vector<AccessibilityWindowInfo> temp;
        return temp;
    }
}

bool AccessibleAbilityChannelClient::SearchElementInfosByText(int32_t accessibilityWindowId,
    int32_t elementId, const std::string &text, std::vector<AccessibilityElementInfo> &elementInfos)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (!proxy_) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }

    int32_t requestId = GenrateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator = new AccessibilityElementOperatorCallbackImpl();
    std::future<void> promiseFutrue = elementOperator->promise_.get_future();

    if (!proxy_->SearchElementInfosByText(accessibilityWindowId, elementId, text, requestId, elementOperator)) {
        return false;
    }

    std::future_status wait = promiseFutrue.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return false;
    }

    for (auto &info : elementOperator->elementInfosResult_) {
        info.SetChannelId(channelId_);
    }
    HILOG_DEBUG("[size:%{public}zu] end", elementOperator->elementInfosResult_.size());
    elementInfos = elementOperator->elementInfosResult_;
    return true;
}

bool AccessibleAbilityChannelClient::FocusMoveSearch(int32_t accessibilityWindowId,
    int32_t elementId, int32_t direction, AccessibilityElementInfo &elementInfo)
{
    HILOG_DEBUG("[channelId:%{public}d]", channelId_);
    if (!proxy_) {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }

    int32_t requestId = GenrateRequestId();
    sptr<AccessibilityElementOperatorCallbackImpl> elementOperator = new AccessibilityElementOperatorCallbackImpl();
    std::future<void> promiseFutrue = elementOperator->promise_.get_future();

    if (!proxy_->FocusMoveSearch(accessibilityWindowId, elementId, direction, requestId, elementOperator)) {
        return false;
    }

    std::future_status wait = promiseFutrue.wait_for(std::chrono::milliseconds(TIME_OUT_OPERATOR));
    if (wait != std::future_status::ready) {
        HILOG_ERROR("Failed to wait result");
        return false;
    }

    elementInfo = elementOperator->accessibilityInfoResult_;
    elementInfo.SetChannelId(channelId_);
    return true;
}

bool AccessibleAbilityChannelClient::ExecuteCommonAction(const int32_t action)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->ExecuteCommonAction(action);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }
}

float AccessibleAbilityChannelClient::GetDisplayResizeCenterX(const int32_t displayId)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->GetDisplayResizeCenterX(displayId);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return 1.0f;
    }
}

float AccessibleAbilityChannelClient::GetDisplayResizeCenterY(const int32_t displayId)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->GetDisplayResizeCenterY(displayId);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return 1.0f;
    }
}

float AccessibleAbilityChannelClient::GetDisplayResizeScale(const int32_t displayId)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->GetDisplayResizeScale(displayId);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return 1.0f;
    }
}

Rect AccessibleAbilityChannelClient::GetDisplayResizeRect(const int32_t displayId)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->GetDisplayResizeRect(displayId);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        Rect temp {};
        return temp;
    }
}

bool AccessibleAbilityChannelClient::ResetDisplayResize(const int32_t displayId, const bool animate)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->ResetDisplayResize(displayId, animate);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }
}

bool AccessibleAbilityChannelClient::SetDisplayResizeScaleAndCenter(const int32_t displayId, const float scale,
    const float centerX, const float centerY, const bool animate)
{
    HILOG_INFO("[channelId:%{public}d]", channelId_);
    if (proxy_) {
        return proxy_->SetDisplayResizeScaleAndCenter(displayId, scale, centerX, centerY, animate);
    } else {
        HILOG_ERROR("Failed to connect to aams [channelId:%{public}d]", channelId_);
        return false;
    }
}
} // namespace Accessibility
} // namespace OHOS