/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"

namespace OHOS {
namespace Accessibility {
void AccessibilityWindowListener::OnWindowUpdate(const sptr<Rosen::AccessibilityWindowInfo>& windowInfo,
    Rosen::WindowUpdateType type)
{
    HILOG_DEBUG("windowId[%{public}d] type[%{public}d]", windowInfo->currentWindowInfo_->wid_, type);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    auto accountData = aams->GetCurrentAccountData();

    auto& winMgr = AccessibilityWindowInfoManager::GetInstance();
    switch (type) {
        case Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED: {
            auto a11yWindowInfo = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
            winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfo);
            AccessibilityEventInfo evtInf(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_ADDED);
            aams->SendEvent(evtInf, aams->GetCurrentAccountId());
            if (a11yWindowInfo.IsFocused()) {
                winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
            }
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED: {
            if (!winMgr.a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                return;
            }
            if (windowInfo->currentWindowInfo_->wid_ == winMgr.activeWindowId_) {
                winMgr.SetActiveWindow(INVALID_WINDOW_ID);
            }
            if (windowInfo->currentWindowInfo_->wid_ == winMgr.a11yFocusedWindowId_) {
                winMgr.SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
            }
            if (windowInfo->currentWindowInfo_->wid_ == winMgr.inputFocusedWindowId_) {
                winMgr.SetInputFocusedWindow(INVALID_WINDOW_ID);
            }
            winMgr.a11yWindows_.erase(windowInfo->currentWindowInfo_->wid_);
            AccessibilityEventInfo evtInf(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_REMOVED);
            aams->SendEvent(evtInf, aams->GetCurrentAccountId());
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS: {
            AccessibilityEventInfo evtInf(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_BOUNDS);
            aams->SendEvent(evtInf, aams->GetCurrentAccountId());
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE:
            if (!winMgr.a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                auto a11yWindowInfo = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
                winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfo);
            }
            winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
            break;
        case Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED: {
            if (!winMgr.a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                auto a11yWindowInfo = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
                winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfo);
            }
            winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
            AccessibilityEventInfo evtInf(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_FOCUSED);
            aams->SendEvent(evtInf, aams->GetCurrentAccountId());
            break;
        }
        default:
            break;
    }
    HILOG_DEBUG("winMgr.a11yWindows[%{public}d]", winMgr.a11yWindows_.size());
}

AccessibilityWindowInfoManager &AccessibilityWindowInfoManager::GetInstance()
{
    static AccessibilityWindowInfoManager windowManager;
    return windowManager;
}

int AccessibilityWindowInfoManager::ConvertToRealWindowId(int windowId, int focusType)
{
    HILOG_DEBUG("ConvertToRealWindowId called, windowId[%{public}d], focusType[%{public}d]", windowId, focusType);
    if (windowId == ACTIVE_WINDOW_ID) {
        HILOG_DEBUG("ConvertToRealWindowId called, after convert windowId[%{public}d]", activeWindowId_);
        return activeWindowId_;
    }

    if (windowId == ANY_WINDOW_ID) {
        if (focusType == FOCUS_TYPE_ACCESSIBILITY) {
            HILOG_DEBUG("ConvertToRealWindowId called, after convert windowId[%{public}d]", a11yFocusedWindowId_);
            return a11yFocusedWindowId_;
        } else if (focusType == FOCUS_TYPE_INPUT) {
            HILOG_DEBUG("ConvertToRealWindowId called, after convert windowId[%{public}d]", inputFocusedWindowId_);
            return inputFocusedWindowId_;
        }
    }
    HILOG_DEBUG("ConvertToRealWindowId called, after convert windowId[%{public}d]", windowId);
    return windowId;
}

WindowType ConvertWindowType(Rosen::WindowType type)
{
    WindowType winType = TYPE_WINDOW_INVALID;

    if (type < Rosen::WindowType::SYSTEM_WINDOW_BASE) {
        winType = TYPE_APPLICATION;
    } else if ((type >= Rosen::WindowType::SYSTEM_WINDOW_BASE) && (type <= Rosen::WindowType::SYSTEM_WINDOW_END)) {
        winType = TYPE_SYSTEM;
    } else {
        HILOG_ERROR("Unknown windowType[%{public}d]", type);
    }
    return winType;
}

AccessibilityWindowInfo AccessibilityWindowInfoManager::CreateAccessibilityWindowInfo(
    Rosen::AccessibilityWindowInfo &windowInfo)
{
    AccessibilityWindowInfo info;
    info.SetWindowId(windowInfo.currentWindowInfo_->wid_);
    info.SetWindowType(ConvertWindowType(windowInfo.currentWindowInfo_->type_));
    info.SetFocused(windowInfo.currentWindowInfo_->focused_);
    Rect bound;
    bound.SetLeftTopScreenPostion(windowInfo.currentWindowInfo_->windowRect_.posX_,
        windowInfo.currentWindowInfo_->windowRect_.posY_);
    bound.SetRightBottomScreenPostion(
        windowInfo.currentWindowInfo_->windowRect_.posX_ +windowInfo.currentWindowInfo_->windowRect_.width_,
        windowInfo.currentWindowInfo_->windowRect_.posY_ + windowInfo.currentWindowInfo_->windowRect_.height_);
    info.SetRectInScreen(bound);
    HILOG_DEBUG("Create WindowInfo Id(%{public}d) type(%{public}d) posX(%{public}d) posY(%{public}d)"
        "witdth(%{public}d) height(%{public}d)",
        windowInfo.currentWindowInfo_->wid_, windowInfo.currentWindowInfo_->type_,
        windowInfo.currentWindowInfo_->windowRect_.posX_, windowInfo.currentWindowInfo_->windowRect_.posY_,
        windowInfo.currentWindowInfo_->windowRect_.width_, windowInfo.currentWindowInfo_->windowRect_.height_);
    return info;
}

void AccessibilityWindowInfoManager::RegisterWindowChangeListener()
{
    if (!registerFlag_) {
        OHOS::Rosen::WindowManager::GetInstance().RegisterWindowUpdateListener(windowListener_);
        registerFlag_ = true;
    }
    return;
}

void AccessibilityWindowInfoManager::DeregisterWindowChangeListener()
{
    if (registerFlag_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterWindowUpdateListener(windowListener_);
        registerFlag_ = false;
    }
    return;
}

void AccessibilityWindowInfoManager::SetActiveWindow(int windowId)
{
    HILOG_DEBUG("windowId(%{public}d)", windowId);
    if (windowId == INVALID_WINDOW_ID) {
        if (a11yWindows_.count(activeWindowId_)) {
            a11yWindows_[activeWindowId_].SetActive(false);
        }
        activeWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_WARN("Window id is not found");
        return;
    }

    if (activeWindowId_ != windowId) {
        if (a11yWindows_.count(activeWindowId_)) {
            a11yWindows_[activeWindowId_].SetActive(false);
        }
        activeWindowId_ = windowId;
        a11yWindows_[activeWindowId_].SetActive(true);
        auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
        AccessibilityEventInfo evtInf(activeWindowId_, WINDOW_UPDATE_ACTIVE);
        aams->SendEvent(evtInf, aams->GetCurrentAccountId());
    }
    HILOG_DEBUG("activeWindowId is %{public}d", activeWindowId_);
}

void AccessibilityWindowInfoManager::SetAccessibilityFocusedWindow(int windowId)
{
    HILOG_DEBUG("start");
    if (windowId == INVALID_WINDOW_ID) {
        if (a11yWindows_.count(a11yFocusedWindowId_)) {
            a11yWindows_[a11yFocusedWindowId_].SetAccessibilityFocused(false);
        }
        a11yFocusedWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_WARN("Window id is not found");
        return;
    }

    if (a11yFocusedWindowId_ != windowId) {
        if (a11yWindows_.count(a11yFocusedWindowId_)) {
            a11yWindows_[a11yFocusedWindowId_].SetAccessibilityFocused(false);
        }
        a11yFocusedWindowId_ = windowId;
        a11yWindows_[a11yFocusedWindowId_].SetAccessibilityFocused(true);
    }
    HILOG_DEBUG("a11yFocusedWindowId_ is %{public}d", a11yFocusedWindowId_);
}

void AccessibilityWindowInfoManager::SetInputFocusedWindow(int windowId)
{
    if (windowId == INVALID_WINDOW_ID) {
        if (a11yWindows_.count(inputFocusedWindowId_)) {
            a11yWindows_[inputFocusedWindowId_].SetFocused(false);
        }
        inputFocusedWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_WARN("Window id is not found");
        return;
    }

    if (inputFocusedWindowId_ != windowId) {
        if (a11yWindows_.count(inputFocusedWindowId_)) {
            a11yWindows_[inputFocusedWindowId_].SetFocused(false);
        }
        inputFocusedWindowId_ = windowId;
        a11yWindows_[inputFocusedWindowId_].SetFocused(true);
    }
}

std::vector<AccessibilityWindowInfo> AccessibilityWindowInfoManager::GetAccessibilityWindows()
{
    HILOG_INFO("start");
    HILOG_DEBUG("a11yWindows_ size[%{public}d]", a11yWindows_.size());
    std::vector<AccessibilityWindowInfo> windows;
    for (auto window : a11yWindows_) {
        windows.push_back(window.second);
    }
    HILOG_DEBUG("window size[%{public}d]", windows.size());
    for (auto logWindow : windows) {
        HILOG_DEBUG("logWindow id[%{public}d]", logWindow.GetWindowId());
    }
    return windows;
}

bool AccessibilityWindowInfoManager::GetAccessibilityWindow(int windowId, AccessibilityWindowInfo &window)
{
    HILOG_INFO("start");
    if (a11yWindows_.count(windowId)) {
        window = a11yWindows_[windowId];
        return true;
    }
    return false;
}

bool AccessibilityWindowInfoManager::IsValidWindow(int windowId)
{
    HILOG_INFO("start");
    if (a11yWindows_.count(windowId)) {
        return true;
    }
    return false;
}

void AccessibilityWindowInfoManager::SetWindowSize(int windowId, Rect rect)
{
    HILOG_INFO("start");
    for (auto &window : a11yWindows_) {
        if (window.first == windowId) {
            window.second.SetRectInScreen(rect);
            return;
        }
    }
}
} // namespace Accessibility
} // namespace OHOS