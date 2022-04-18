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
AccessibilityWindowManager::AccessibilityWindowManager()
{
    windowListener_ = new AccessibilityWindowListener(*this);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(aams->GetMainRunner());
}

AccessibilityWindowManager &AccessibilityWindowManager::GetInstance()
{
    static AccessibilityWindowManager windowManager;
    return windowManager;
}

void AccessibilityWindowManager::OnWindowUpdate(const sptr<Rosen::AccessibilityWindowInfo>& windowInfo,
    Rosen::WindowUpdateType type)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    std::function<void()> windowUpdateFunc = std::bind(&AccessibilityWindowManager::WindowUpdate,
        this, windowInfo, type);
    eventHandler_->PostTask(windowUpdateFunc, "TASK_ON_WINDOW_UPDATE");
}

void AccessibilityWindowManager::WindowUpdate(const sptr<Rosen::AccessibilityWindowInfo>& windowInfo,
    Rosen::WindowUpdateType type)
{
    HILOG_DEBUG("windowId[%{public}d] type[%{public}d]", windowInfo->currentWindowInfo_->wid_, type);
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }

    auto& winMgr = AccessibilityWindowManager::GetInstance();
    switch (type) {
        case Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED: {
            WindowUpdateAdded(windowInfo);
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED: {
            WindowUpdateRemoved(windowInfo);
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS: {
            AccessibilityEventInfo evtInfBounds(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_BOUNDS);
            aams->SendEvent(evtInfBounds, aams->GetCurrentAccountId());
            break;
        }
        case Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE:
            if (!winMgr.a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                auto a11yWindowInfoActive = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
                winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoActive);
            }
            winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
            break;
        case Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED: {
            if (!winMgr.a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                auto a11yWindowInfoFocused = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
                winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoFocused);
            }
            winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
            AccessibilityEventInfo evtInfFocused(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_FOCUSED);
            aams->SendEvent(evtInfFocused, aams->GetCurrentAccountId());
            break;
        }
        default:
            break;
    }
    winMgr.UpdateWindowLayer(windowInfo);
    HILOG_DEBUG("winMgr.a11yWindows[%{public}zu]", winMgr.a11yWindows_.size());
}

int32_t AccessibilityWindowManager::ConvertToRealWindowId(int32_t windowId, int32_t focusType)
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

AccessibilityWindowInfo AccessibilityWindowManager::CreateAccessibilityWindowInfo(
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
        windowInfo.currentWindowInfo_->windowRect_.posX_ +
        static_cast<int32_t>(windowInfo.currentWindowInfo_->windowRect_.width_),
        windowInfo.currentWindowInfo_->windowRect_.posY_ +
        static_cast<int32_t>(windowInfo.currentWindowInfo_->windowRect_.height_));
    info.SetRectInScreen(bound);
    HILOG_DEBUG("Create WindowInfo Id(%{public}d) type(%{public}d) posX(%{public}d) posY(%{public}d)"
        "witdth(%{public}d) height(%{public}d)",
        windowInfo.currentWindowInfo_->wid_, windowInfo.currentWindowInfo_->type_,
        windowInfo.currentWindowInfo_->windowRect_.posX_, windowInfo.currentWindowInfo_->windowRect_.posY_,
        windowInfo.currentWindowInfo_->windowRect_.width_, windowInfo.currentWindowInfo_->windowRect_.height_);
    return info;
}

void AccessibilityWindowManager::RegisterWindowChangeListener()
{
    if (!registerFlag_) {
        OHOS::Rosen::WindowManager::GetInstance().RegisterWindowUpdateListener(windowListener_);
        registerFlag_ = true;
    }
    return;
}

void AccessibilityWindowManager::DeregisterWindowChangeListener()
{
    if (registerFlag_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterWindowUpdateListener(windowListener_);
        registerFlag_ = false;
    }
    return;
}

void AccessibilityWindowManager::SetActiveWindow(int32_t windowId)
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
        if (aams) {
            AccessibilityEventInfo evtInf(activeWindowId_, WINDOW_UPDATE_ACTIVE);
            aams->SendEvent(evtInf, aams->GetCurrentAccountId());
        }
    }
    HILOG_DEBUG("activeWindowId is %{public}d", activeWindowId_);
}

void AccessibilityWindowManager::SetAccessibilityFocusedWindow(int32_t windowId)
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
        HILOG_ERROR("Window id is not found");
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

void AccessibilityWindowManager::SetInputFocusedWindow(int32_t windowId)
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

std::vector<AccessibilityWindowInfo> AccessibilityWindowManager::GetAccessibilityWindows()
{
    HILOG_DEBUG("a11yWindows_ size[%{public}zu]", a11yWindows_.size());
    std::vector<AccessibilityWindowInfo> windows;
    for (auto window : a11yWindows_) {
        windows.push_back(window.second);
    }
    HILOG_DEBUG("window size[%{public}zu]", windows.size());
    for (auto logWindow : windows) {
        HILOG_DEBUG("logWindow id[%{public}d]", logWindow.GetWindowId());
    }
    return windows;
}

bool AccessibilityWindowManager::GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    if (a11yWindows_.count(windowId)) {
        window = a11yWindows_[windowId];
        return true;
    }
    return false;
}

bool AccessibilityWindowManager::IsValidWindow(int32_t windowId)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    if (a11yWindows_.count(windowId)) {
        return true;
    }
    return false;
}

void AccessibilityWindowManager::SetWindowSize(int32_t windowId, Rect rect)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    for (auto &window : a11yWindows_) {
        if (window.first == windowId) {
            window.second.SetRectInScreen(rect);
            return;
        }
    }
}

void AccessibilityWindowManager::UpdateWindowLayer(const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG("start");
    int32_t layer = 0;
    for (auto& window : windowInfo->windowList_) {
        auto it = a11yWindows_.find(window->wid_);
        if (it == a11yWindows_.end()) {
            HILOG_ERROR("The window(%{public}d) not in a11yWindows_", window->wid_);
            layer++;
        } else {
            it->second.SetWindowLayer(layer);
            layer++;
        }
    }
}

void AccessibilityWindowManager::WindowUpdateAdded(const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG("start");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }

    auto& winMgr = AccessibilityWindowManager::GetInstance();
    auto a11yWindowInfoAdded = winMgr.CreateAccessibilityWindowInfo(*windowInfo);
    winMgr.a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoAdded);
    AccessibilityEventInfo evtInfAdded(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_ADDED);
    aams->SendEvent(evtInfAdded, aams->GetCurrentAccountId());
    if (a11yWindowInfoAdded.IsFocused()) {
        winMgr.SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
    }
}

void AccessibilityWindowManager::WindowUpdateRemoved(const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG("start");
    auto aams = DelayedSingleton<AccessibleAbilityManagerService>::GetInstance();
    if (!aams) {
        HILOG_ERROR("aams is nullptr");
        return;
    }

    auto& winMgr = AccessibilityWindowManager::GetInstance();
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
    AccessibilityEventInfo evtInfRemoved(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_REMOVED);
    aams->SendEvent(evtInfRemoved, aams->GetCurrentAccountId());
}
} // namespace Accessibility
} // namespace OHOS