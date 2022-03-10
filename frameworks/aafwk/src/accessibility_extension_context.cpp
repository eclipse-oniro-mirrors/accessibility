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

#include "accessibility_extension_context.h"
#include "accessibility_element_info.h"
#include "accessible_ability_client_stub_impl.h"
#include "accessible_ability_manager_service_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
bool AccessibilityExtensionContext::GetFocusElementInfo(
    uint32_t focusType, std::optional<AccessibilityElementInfo>& elementInfo)
{
    HILOG_DEBUG("start.");
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

bool AccessibilityExtensionContext::GestureSimulate(uint32_t sequence,
    const std::vector<GesturePathDefine>& gesturePathDefineList,
    const std::shared_ptr<GestureResultListener>& listener)
{
    HILOG_DEBUG("start.");

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

    if (listener) {
        gestureResultListenerInfos_.insert(make_pair(sequence, listener));
    }

    AccessibilityOperator::GetInstance().SendSimulateGesture(channelId_, sequence, gesturePathDefineList);
    return true;
}

std::shared_ptr<DisplayResizeController>& AccessibilityExtensionContext::GetDisplayResizeController()
{
    HILOG_DEBUG("start.");

    // this is a temp deal: To make sure the id of DEFAULT_DISPALY.
    uint32_t DEFAULT_DISPALY = 0;
    return GetDisplayResizeController(DEFAULT_DISPALY);
}

std::shared_ptr<DisplayResizeController>& AccessibilityExtensionContext::GetDisplayResizeController(uint32_t displayId)
{
    HILOG_DEBUG("start.");

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

bool AccessibilityExtensionContext::GetRootElementInfo(std::optional<AccessibilityElementInfo>& elementInfo)
{
    HILOG_DEBUG("start.");
    AccessibilityElementInfo info {};
    bool result = AccessibilityOperator::GetInstance().GetRoot(channelId_, info);
    elementInfo = info;
    return result;
}

std::vector<AccessibilityWindowInfo>& AccessibilityExtensionContext::GetWindows()
{
    HILOG_DEBUG("start.");
    accessibilityWindow_ = AccessibilityOperator::GetInstance().GetWindows(channelId_);
    return accessibilityWindow_;
}

bool AccessibilityExtensionContext::ExecuteCommonAction(uint32_t action)
{
    HILOG_DEBUG("start.");
    if (!CheckCommonAction(action)) {
        HILOG_ERROR("action is not allowed.");
        return false;
    }

    return AccessibilityOperator::GetInstance().ExecuteCommonAction(channelId_, action);
}

void AccessibilityExtensionContext::DispatchOnSimulationGestureResult(uint32_t sequence, bool result)
{
    HILOG_DEBUG("start.");

    if (gestureResultListenerInfos_.empty()) {
        HILOG_ERROR("There is no information of gestureResultListener");
        return;
    }

    shared_ptr<GestureResultListener> gestureResultListener = nullptr;
    auto it = gestureResultListenerInfos_.find(sequence);
    if (it != gestureResultListenerInfos_.end()) {
        HILOG_DEBUG("gestureResultListenerInfo has been found.");
        gestureResultListener = gestureResultListenerInfos_[sequence];
    }

    if (!gestureResultListener) {
        HILOG_ERROR("There is no gestureResultListenerInfo in gestureResultListenerInfos_[%{public}d", sequence);
        return;
    }

    HILOG_DEBUG("GestureInject result is %{public}d", result);
    gestureResultListener->OnGestureInjectResult(sequence, result);
}

void AccessibilityExtensionContext::SetChannelId(uint32_t channelId)
{
    HILOG_DEBUG("start.");
    channelId_ = channelId;
}

bool AccessibilityExtensionContext::CheckCommonAction(uint32_t action)
{
    HILOG_DEBUG("start.");
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