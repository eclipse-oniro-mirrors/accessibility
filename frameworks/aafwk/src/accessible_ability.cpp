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

#include <algorithm>
#include <optional>
#include "accessible_ability.h"
#include "accessibility_element_info.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "accessible_ability_manager_service_proxy.h"
#include "dummy.h"

using namespace std;

const string AA_NAME = "AccessibleAbility";

namespace OHOS {
namespace Accessibility {

AccessibleAbility& AccessibleAbility::GetInstance()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    static AccessibleAbility accessibleAbility;
    return accessibleAbility;
}

AccessibleAbility::AccessibleAbility()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!Init()) {
        HILOG_ERROR("%{public}s Init failed!", __func__);
    }
}

bool AccessibleAbility::Init()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create(AA_NAME);
        if (!runner_) {
            HILOG_ERROR("Create AA runner fail");
            return false;
        }
    }
    HILOG_DEBUG("Get runner_ successfully");

    if (!accessibleAbilityEventHandler_) {
        accessibleAbilityEventHandler_ = std::make_shared<AccessibleAbilityEventHandler>(runner_);
        if (!accessibleAbilityEventHandler_) {
            HILOG_ERROR("Create AA event handler fail");
            return false;
        }
    }
    HILOG_DEBUG("Get accessibleAbilityEventHandler_ successfully");

    stub_ = new AccessibleAbilityClientStubImpl(accessibleAbilityEventHandler_);
    if (!stub_) {
        HILOG_ERROR("stub_ is nullptr.");
        return false;
    }
    HILOG_DEBUG("Get stub_ successfully");

    return true;
}

void AccessibleAbility::RegisterListener(const std::shared_ptr<AccessibleAbilityListener> &listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }

    if (!stub_) {
        HILOG_ERROR("AccessibleAbility::ConnectToAAMS fail, no stub_");
        return;
    }
    stub_->RegisterListenerImpl(listener);
}

void AccessibleAbility::ConnectToAAMS()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOG_ERROR("AccessibleAbility::ConnectToAAMS Failed to get ISystemAbilityManager");
        return;
    }
    HILOG_DEBUG("AccessibleAbility::ConnectToAAMS ISystemAbilityManager obtained");

    sptr<IRemoteObject> object = samgr->GetSystemAbility(ACCESSIBLE_ABILITY_MANAGER_SERVICE_ID);
    if (!object) {
        HILOG_ERROR("[%s]Get IAccessibleAbilityManagerServiceClient object from samgr failed", __func__);
        return;
    }

    HILOG_DEBUG("Get remote object ok");
    proxy_ = iface_cast<AccessibleAbilityManagerServiceClientProxy>(object);
    if (!proxy_) {
        HILOG_ERROR("AccessibleAbility::ConnectToAAMS IAccessibleAbilityManagerServiceClient iface_cast failed");
        return;
    }

    if (!stub_) {
        HILOG_ERROR("AccessibleAbility::stub_ is nullptr");
        return;
    }
    proxy_->RegisterAbilityConnectionClientTmp(stub_);
}

void AccessibleAbility::DisableAbility()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    AccessibilityOperator::GetInstance().DisableAbility(channelId_);
}

bool AccessibleAbility::GetFocusElementInfo(uint32_t focusType, std::optional<AccessibilityElementInfo>& elementInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if ((focusType != FOCUS_TYPE_INPUT) && (focusType != FOCUS_TYPE_ACCESSIBILITY)) {
        HILOG_DEBUG("focusType is not allowed.");
        return false;
    }

    AccessibilityElementInfo info {};
    bool result = AccessibilityOperator::GetInstance().FindFocusedElementInfo(channelId_,
                            ANY_WINDOW_ID, ROOT_NODE_ID, focusType, info);
    elementInfo = info;
    return result;
}

bool AccessibleAbility::GestureSimulate(std::vector<GesturePathDefine>& gesturePathDefineList,
                                        std::shared_ptr<GestureResultListener>& listener)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (gesturePathDefineList.size() > gesturePathDefineList.front().GetMaxStrokes() ||
        gesturePathDefineList.size() <= 0) {
        HILOG_ERROR("The number of gesturePathDefine : [%{public}d] is not allowed.", gesturePathDefineList.size());
        return false;
    }

    uint32_t totalDurationTime = 0;
    for (auto gesturePath : gesturePathDefineList) {
        totalDurationTime += gesturePath.GetDurationTime();
    }
    HILOG_DEBUG("The total duration time is %{public}d.", totalDurationTime);

    if (totalDurationTime > gesturePathDefineList.front().GetMaxStrokeDuration()) {
        HILOG_ERROR("The total duration time : [%{public}d] is not allowed.", totalDurationTime);
        return false;
    }

    gestureStatusListenerSequence_++;

    if (listener) {
        gestureResultListenerInfos_.insert(make_pair(gestureStatusListenerSequence_,
            make_shared<GestureResultListenerInfo>(gesturePathDefineList, listener)));
    }

    AccessibilityOperator::GetInstance().SendSimulateGesture(
        channelId_, gestureStatusListenerSequence_, gesturePathDefineList);

    return true;
}

std::shared_ptr<DisplayResizeController>& AccessibleAbility::GetDisplayResizeController()
{
    HILOG_DEBUG("%{public}s start.", __func__);

    // TODO: To make sure the id of DEFAULT_DISPALY.
    uint32_t DEFAULT_DISPALY = 0;
    return GetDisplayResizeController(DEFAULT_DISPALY);
}

std::shared_ptr<DisplayResizeController>& AccessibleAbility::GetDisplayResizeController(uint32_t displayId)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    auto it = displayResizeControllers_.find(displayId);
    if (it != displayResizeControllers_.end()) {
        return displayResizeControllers_[displayId];
    } else {
        HILOG_DEBUG("Have no DisplayResizeController and make a new one.");
        displayResizeControllers_.insert(make_pair(displayId,
            make_shared<DisplayResizeController>(channelId_, displayId)));
        return displayResizeControllers_[displayId];
    }
}

bool AccessibleAbility::GetRootElementInfo(std::optional<AccessibilityElementInfo>& elementInfo)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    AccessibilityElementInfo info {};
    bool result = AccessibilityOperator::GetInstance().GetRoot(channelId_, info);
    elementInfo = info;
    return result;
}

std::vector<AccessibilityWindowInfo>& AccessibleAbility::GetWindows()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    accessibilityWindow_ = AccessibilityOperator::GetInstance().GetWindows(channelId_);
    return accessibilityWindow_;
}

bool AccessibleAbility::PerformCommonAction(uint32_t action)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!CheckCommonAction(action)) {
        HILOG_ERROR("action is not allowed.");
        return false;
    }

    return AccessibilityOperator::GetInstance().PerformCommonAction(channelId_, action);
}

std::shared_ptr<FingerprintController>& AccessibleAbility::GetFingerprintController()
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if (!fingerprintController_) {
        fingerprintController_ = make_shared<FingerprintController>(channelId_);
    }
    return fingerprintController_;
}

void AccessibleAbility::DispatchOnSimulationGestureResult(uint32_t sequence, bool result)
{
    HILOG_DEBUG("%{public}s start.", __func__);

    if (gestureResultListenerInfos_.empty()) {
        HILOG_ERROR("There is no informations of gestureResultListener");
        return;
    }

    shared_ptr<GestureResultListenerInfo> gestureResultListenerInfo = nullptr;
    auto it = gestureResultListenerInfos_.find(sequence);
    if (it != gestureResultListenerInfos_.end()) {
        HILOG_DEBUG("gestureResultListenerInfo has been found.");
        gestureResultListenerInfo = gestureResultListenerInfos_[sequence];
    }

    if (!gestureResultListenerInfo) {
        HILOG_ERROR("There is no gestureResultListenerInfo in gestureResultListenerInfos_[%{public}d", sequence);
        return;
    }

    shared_ptr<GestureResultListener> listener = gestureResultListenerInfo->GetGestureResultListener();
    if (!listener) {
        HILOG_ERROR("listener is nullptr.");
        return;
    }

    vector<GesturePathDefine> gesturePath = gestureResultListenerInfo->GetGesturePathDefine();
    if (gesturePath.empty()) {
        HILOG_ERROR("There is no gesturePath");
        return;
    }

    if (result) {
        HILOG_DEBUG("Gesture has been sent successfully and result is %{public}d", result);
        listener->OnCompleted(gesturePath);
    } else {
        HILOG_DEBUG("Gesture has been sent failed and result is %{public}d", result);
        listener->OnCancelled(gesturePath);
    }
}

void AccessibleAbility::SetChannelId(uint32_t channelId)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    channelId_ = channelId;
}

bool AccessibleAbility::CheckCommonAction(uint32_t action)
{
    HILOG_DEBUG("%{public}s start.", __func__);
    if ((action != GLOBAL_ACTION_BACK) && (action != GLOBAL_ACTION_HOME) &&
        (action != GLOBAL_ACTION_RECENT) && (action != GLOBAL_ACTION_NOTIFICATION) &&
        (action != GLOBAL_ACTION_POP_UP_POWER_DIALOG) && (action != GLOBAL_ACTION_DIVIDE_SCREEN) &&
        (action != GLOBAL_ACTION_LOCK_SCREEN) && (action != GLOBAL_ACTION_CAPTURE_SCREEN)) {
        return false;
    } else {
        return true;
    }
}

} // namespace Accessibility
} // namespace OHOS