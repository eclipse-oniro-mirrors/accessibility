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

#include "accessibility_interaction_bridge.h"

#include <algorithm>
#include <new>
#include "accessibility_ability_info.h"
#include "accessibility_account_data.h"
#include "accessibility_display_manager.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_connection.h"

namespace OHOS {
namespace Accessibility {
const int INTERACTION_BRIDGE_CHANNEL_ID = 0;
const int NONE_ID = -1;

AccessibilityInteractionBridge& AccessibilityInteractionBridge::GetInstance()
{
    HILOG_DEBUG("start");
    static AccessibilityInteractionBridge instance_;

    return instance_;
}

AccessibilityInteractionBridge::AccessibilityInteractionBridge()
{
    HILOG_DEBUG("start");
    AppExecFwk::ExtensionAbilityInfo info;
    sptr<AccessibilityAbilityInfo> abilityInfo = new(std::nothrow) AccessibilityAbilityInfo(info);
    if (!abilityInfo) {
        HILOG_ERROR("abilityInfo is not matched");
        return;
    }
    abilityInfo->SetCapabilityValues(Capability::CAPABILITY_RETRIEVE);
    accountData_ = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance()->GetCurrentAccountData();
    connection_ = new(std::nothrow) AccessibleAbilityConnection(accountData_,
        INTERACTION_BRIDGE_CHANNEL_ID, *abilityInfo);
    if (!connection_) {
        HILOG_ERROR("connection_ is null");
        return;
    }
    channel_ = new(std::nothrow) AccessibleAbilityChannelStubImpl(*connection_);
    if (!channel_) {
        HILOG_ERROR("channel_ is null");
        return;
    }
    if (!channel_) {
        HILOG_DEBUG("channel is nullptr.");
        AccessibilityOperator::GetInstance().RemoveChannel(INTERACTION_BRIDGE_CHANNEL_ID);
    } else {
        HILOG_DEBUG("AddChannel start.");
        AccessibilityOperator::GetInstance().AddChannel(INTERACTION_BRIDGE_CHANNEL_ID, channel_);
    }
    HILOG_INFO("AccessibleAbilityManagerService::AccessibilityInteractionBridge successfully");
}

AccessibilityElementInfo AccessibilityInteractionBridge::FindFocusedElementInfo()
{
    HILOG_DEBUG("[without window]");

    return FindFocusedElementInfo(ANY_WINDOW_ID);
}

bool AccessibilityInteractionBridge::ExecuteActionOnAccessibilityFocused(const ActionType& action)
{
    HILOG_DEBUG("start");
    auto focus = FindFocusedElementInfo();
    std::map<std::string, std::string> actionArguments {};
    HILOG_DEBUG("[ExecuteAction]");
    return focus.ExecuteAction(action, actionArguments);
}

bool Intersect(Rect& focus, Rect source)
{
    HILOG_DEBUG("start");
    int minx = std::max(focus.GetLeftTopXScreenPostion(), source.GetLeftTopXScreenPostion());
    int miny = std::max(focus.GetLeftTopYScreenPostion(), source.GetLeftTopYScreenPostion());
    int maxx = std::min(focus.GetRightBottomXScreenPostion(), source.GetRightBottomXScreenPostion());
    int maxy = std::min(focus.GetRightBottomYScreenPostion(), source.GetRightBottomYScreenPostion());
    if ((minx > maxx) || (miny > maxy)) {
        HILOG_ERROR("The two Rects do not intersect");
        return false;
    }
    focus.SetLeftTopScreenPostion(minx, miny);
    focus.SetRightBottomScreenPostion(maxx, maxy);
    return true;
}

bool Intersect(Rect& rect, AccessibilityWindowInfo window)
{
    HILOG_DEBUG("start");
    if (!Intersect(rect, window.GetRectInScreen())) {
        return false;
    }
    return true;
}

bool Intersect(Rect& rect, Rosen::Display& display)
{
    HILOG_DEBUG("start");
    Rect source(0, 0, display.GetWidth(), display.GetHeight());
    if (!Intersect(rect, source)) {
        return false;
    }
    return true;
}

bool AccessibilityInteractionBridge::GetPointerItermOfAccessibilityFocusClick(MMI::PointerEvent::PointerItem &point)
{
    HILOG_DEBUG("start");
    auto focus = FindFocusedElementInfo();
    auto focusRect = focus.GetRectInScreen();
    float denominator = 2.0;
    // Apply magnification if needed.

    // Intersect with window
    auto& windowManager = AccessibilityWindowInfoManager::GetInstance();
    AccessibilityWindowInfo window;
    auto result = windowManager.GetAccessibilityWindow(windowManager.activeWindowId_, window);
    if (!result) {
        return false;
    }
    if (!Intersect(focusRect, window)) {
        return false;
    }
    // Intersect with display dummy
    auto display = AccessibilityDisplayManager::GetInstance().GetDefaultDisplay();
    if (!Intersect(focusRect, *display)) {
        return false;
    }

    float px = (focusRect.GetLeftTopXScreenPostion() + focusRect.GetRightBottomXScreenPostion()) / denominator;
    float py = (focusRect.GetLeftTopYScreenPostion() + focusRect.GetRightBottomYScreenPostion()) / denominator;
    point.SetGlobalX(px);
    point.SetGlobalY(py);
    return true;
}

AccessibilityElementInfo AccessibilityInteractionBridge::FindFocusedElementInfo(const int &windowId)
{
    HILOG_DEBUG("[windowId %{public}d]", windowId);
    AccessibilityElementInfo info {};
    AccessibilityOperator::GetInstance().FindFocusedElementInfo(INTERACTION_BRIDGE_CHANNEL_ID,
        windowId, NONE_ID, FOCUS_TYPE_ACCESSIBILITY, info);
    return info;
}
} // namespace Accessibility
} // namespace OHOS
