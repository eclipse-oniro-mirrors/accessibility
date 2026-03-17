/*
 * Copyright (C) 2022-2026 Huawei Device Co., Ltd.
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

#include "element_operator_callback_impl.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

void ElementOperatorCallbackImpl::SetFindFocusedElementInfoResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_ERROR("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void ElementOperatorCallbackImpl::SetSearchElementInfoByTextResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void ElementOperatorCallbackImpl::SetSearchElementInfoByAccessibilityIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

void ElementOperatorCallbackImpl::SetSearchElementInfoBySpecificPropertyResult(
    const std::list<AccessibilityElementInfo> &infos, const std::list<AccessibilityElementInfo> &treeInfos,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (!infos.empty()) {
        if (!ValidateElementInfos(infos)) {
            return;
        }
        elementInfosResult_.assign(infos.begin(), infos.end());
    } else if (!treeInfos.empty()) {
        if (!ValidateElementInfos(treeInfos)) {
            return;
        }
        elementInfosResult_.assign(treeInfos.begin(), treeInfos.end());
    }
    promise_.set_value();
}

void ElementOperatorCallbackImpl::SetFocusMoveSearchWithConditionResult(
    const std::list<AccessibilityElementInfo> &infos, const FocusMoveResult& result, const int32_t requestId)
{
    if (!infos.empty()) {
        elementInfosResult_.assign(infos.begin(), infos.end());
    }
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    focusMoveResult_ = result.resultType;
    nowLevelBelongTreeId_ = result.nowLevelBelongTreeId;
    parentWindowId_ = result.parentWindowId;
    changeToNewInfo_ = result.changeToNewInfo;
    needTerminate_ = result.needTerminate;
    promise_.set_value();
}

bool ElementOperatorCallbackImpl::ValidateElementInfos(
    const std::list<AccessibilityElementInfo>& infos)
{
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return false;
        }
    }
    return true;
}

void ElementOperatorCallbackImpl::SetFocusMoveSearchResult(
    const AccessibilityElementInfo &info, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
        info.GetAccessibilityId()) == RET_OK) {
        HILOG_DEBUG("VerifyingToKenId ok");
        accessibilityInfoResult_ = info;
        promise_.set_value();
    } else {
        HILOG_ERROR("VerifyingToKenId failed");
        promise_.set_value();
    }
}

void ElementOperatorCallbackImpl::SetExecuteActionResult(const bool succeeded,
    const int32_t requestId)
{
    HILOG_DEBUG("Response [result:%{public}d, requestId:%{public}d]", succeeded, requestId);
    executeActionResult_ = succeeded;
    promise_.set_value();
}

void ElementOperatorCallbackImpl::SetCursorPositionResult(const int32_t cursorPosition,
    const int32_t requestId)
{
    HILOG_INFO("ElementOperatorCallbackImpl::SetCursorPositionResult [result:%{public}d]",
        cursorPosition);
    HILOG_DEBUG("cursorPosition [result:%{public}d, requestId:%{public}d]", cursorPosition, requestId);
    callCursorPosition_ = cursorPosition;
    promise_.set_value();
}

void ElementOperatorCallbackImpl::SetSearchDefaultFocusByWindowIdResult(
    const std::vector<AccessibilityElementInfo> &infos, const int32_t requestId)
{
    HILOG_DEBUG("Response [requestId:%{public}d]", requestId);
    for (auto info : infos) {
        if (Singleton<AccessibleAbilityManagerService>::GetInstance().VerifyingToKenId(info.GetWindowId(),
            info.GetAccessibilityId()) == RET_OK) {
            HILOG_DEBUG("VerifyingToKenId ok");
        } else {
            HILOG_ERROR("VerifyingToKenId failed");
            elementInfosResult_.clear();
            promise_.set_value();
            return;
        }
        elementInfosResult_ = infos;
    }
    promise_.set_value();
}

} // namespace Accessibility
} // namespace OHOS