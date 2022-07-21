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

#include <hitrace_meter.h>

#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"
#include "utils.h"

namespace OHOS {
namespace Accessibility {
AccessibilityWindowManager::AccessibilityWindowManager()
{
}

bool AccessibilityWindowManager::Init()
{
    HILOG_DEBUG("start");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryWindowInfo");
    sptr<Rosen::AccessibilityWindowInfo> windowInfo = new(std::nothrow) Rosen::AccessibilityWindowInfo();
    if (!windowInfo) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("Create window info failed.");
        return false;
    }
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfo);
    if (err != Rosen::WMError::WM_OK || !windowInfo) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return false;
    }
    std::vector<sptr<Rosen::WindowInfo>> windows = windowInfo->windowList_;
    HILOG_DEBUG("window list is %{public}zu", windows.size());
    for (auto &window : windows) {
        if (!window) {
            HILOG_ERROR("window is nullptr");
            continue;
        }
        if (!a11yWindows_.count(window->wid_)) {
            auto a11yWindowInfo = CreateAccessibilityWindowInfo(window);
            a11yWindows_.emplace(window->wid_, a11yWindowInfo);
        }
        if (a11yWindows_[window->wid_].IsFocused()) {
            SetActiveWindow(window->wid_);
        }
    }
    UpdateWindowLayer(windowInfo);
    return true;
}

AccessibilityWindowManager::~AccessibilityWindowManager()
{
    DeregisterWindowListener();
}

void AccessibilityWindowManager::RegisterWindowListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    if (windowListener_) {
        HILOG_DEBUG("Window listener is already registered!");
        return;
    }

    eventHandler_ = handler;
    windowListener_ = new(std::nothrow) AccessibilityWindowListener(*this);
    if (!windowListener_) {
        HILOG_ERROR("Create window listener fail!");
        return;
    }
    OHOS::Rosen::WindowManager::GetInstance().RegisterWindowUpdateListener(windowListener_);
}

void AccessibilityWindowManager::DeregisterWindowListener()
{
    if (windowListener_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterWindowUpdateListener(windowListener_);
        windowListener_ = nullptr;
        eventHandler_ = nullptr;
    }
}

void AccessibilityWindowManager::OnWindowUpdate(const sptr<Rosen::AccessibilityWindowInfo>& windowInfo,
    Rosen::WindowUpdateType type)
{
    HILOG_DEBUG("start");
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    if (!windowInfo || !windowInfo->currentWindowInfo_) {
        HILOG_ERROR("window info is err");
        return;
    }
    eventHandler_->PostTask(std::bind([=]() -> void {
        HILOG_DEBUG("windowId[%{public}d] type[%{public}d]", windowInfo->currentWindowInfo_->wid_, type);
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        switch (type) {
            case Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED:
                WindowUpdateAdded(windowInfo);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED:
                WindowUpdateRemoved(windowInfo);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS: {
                AccessibilityEventInfo evtInfBounds(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_BOUNDS);
                aams.SendEvent(evtInfBounds);
                break;
            }
            case Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE:
                if (!a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                    auto a11yWindowInfoActive = CreateAccessibilityWindowInfo(windowInfo->currentWindowInfo_);
                    a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoActive);
                }
                SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED: {
                if (!a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                    auto a11yWindowInfoFocused = CreateAccessibilityWindowInfo(windowInfo->currentWindowInfo_);
                    a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoFocused);
                }
                SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
                AccessibilityEventInfo evtInfFocused(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_FOCUSED);
                aams.SendEvent(evtInfFocused);
                break;
            }
            case Rosen::WindowUpdateType::WINDOW_UPDATE_PROPERTY: {
                if (a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
                    UpdateAccessibilityWindowInfo(a11yWindows_[windowInfo->currentWindowInfo_->wid_],
                        windowInfo->currentWindowInfo_);
                }
                break;
            }
            default:
                break;
        }
        UpdateWindowLayer(windowInfo);
        HILOG_DEBUG("a11yWindows[%{public}zu]", a11yWindows_.size());
        }), "TASK_ON_WINDOW_UPDATE");
}

int32_t AccessibilityWindowManager::ConvertToRealWindowId(int32_t windowId, int32_t focusType)
{
    HILOG_DEBUG("ConvertToRealWindowId called, windowId[%{public}d], focusType[%{public}d]", windowId, focusType);
    if (windowId == ACTIVE_WINDOW_ID) {
        HILOG_INFO("After convert active windowId[%{public}d]", activeWindowId_);
        return activeWindowId_;
    }

    if (windowId == ANY_WINDOW_ID) {
        if (focusType == FOCUS_TYPE_ACCESSIBILITY) {
            HILOG_INFO("After convert a11yFocused windowId[%{public}d] by accessibility type", a11yFocusedWindowId_);
            return a11yFocusedWindowId_;
        } else if (focusType == FOCUS_TYPE_INPUT) {
            HILOG_INFO("After convert active windowId[%{public}d] by input type", activeWindowId_);
            return activeWindowId_;
        }
    }
    HILOG_INFO("After convert windowId[%{public}d]", windowId);
    return windowId;
}

AccessibilityWindowType ConvertWindowType(Rosen::WindowType type)
{
    AccessibilityWindowType winType = TYPE_WINDOW_INVALID;

    if (type < Rosen::WindowType::SYSTEM_WINDOW_BASE) {
        winType = TYPE_APPLICATION;
    } else if ((type >= Rosen::WindowType::SYSTEM_WINDOW_BASE) && (type <= Rosen::WindowType::SYSTEM_WINDOW_END)) {
        winType = TYPE_SYSTEM;
    } else {
        HILOG_ERROR("Unknown windowType[%{public}d]", type);
    }
    return winType;
}

void AccessibilityWindowManager::UpdateAccessibilityWindowInfo(AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::WindowInfo> windowInfo)
{
    accWindowInfo.SetWindowId(windowInfo->wid_);
    accWindowInfo.SetWindowType(static_cast<uint32_t>(windowInfo->type_));
    accWindowInfo.SetWindowMode(static_cast<uint32_t>(windowInfo->mode_));
    accWindowInfo.SetAccessibilityWindowType(ConvertWindowType(windowInfo->type_));
    accWindowInfo.SetFocused(windowInfo->focused_);
    Rect bound;
    bound.SetLeftTopScreenPostion(windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_);
    bound.SetRightBottomScreenPostion(
        windowInfo->windowRect_.posX_ + static_cast<int32_t>(windowInfo->windowRect_.width_),
        windowInfo->windowRect_.posY_ + static_cast<int32_t>(windowInfo->windowRect_.height_));
    accWindowInfo.SetRectInScreen(bound);
    accWindowInfo.SetDisplayId(windowInfo->displayId_);
    accWindowInfo.SetDecorEnable(windowInfo->isDecorEnable_);
}

AccessibilityWindowInfo AccessibilityWindowManager::CreateAccessibilityWindowInfo(
    const sptr<Rosen::WindowInfo> windowInfo)
{
    AccessibilityWindowInfo info;
    UpdateAccessibilityWindowInfo(info, windowInfo);
    HILOG_DEBUG("Create WindowInfo Id(%{public}d) type(%{public}d) posX(%{public}d) posY(%{public}d)"
        "witdth(%{public}d) height(%{public}d) display id(%{public}" PRIu64 ") isDecorEnable(%{public}d)",
        windowInfo->wid_, windowInfo->type_, windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_,
        windowInfo->windowRect_.width_, windowInfo->windowRect_.height_,
        windowInfo->displayId_, windowInfo->isDecorEnable_);
    return info;
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
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        AccessibilityEventInfo evtInf(activeWindowId_, WINDOW_UPDATE_ACTIVE);
        aams.SendEvent(evtInf);
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
        HILOG_ERROR("Window id[%{public}d] is not found", windowId);
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

std::vector<AccessibilityWindowInfo> AccessibilityWindowManager::GetAccessibilityWindows()
{
    HILOG_DEBUG("a11yWindows_ size[%{public}zu]", a11yWindows_.size());
    std::vector<AccessibilityWindowInfo> windows;
    for (auto &window : a11yWindows_) {
        windows.emplace_back(window.second);
    }
    HILOG_DEBUG("window size[%{public}zu]", windows.size());
    for (auto &logWindow : windows) {
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
        if (!window) {
            HILOG_ERROR("window is nullptr");
            continue;
        }

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
    if (!a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
        auto a11yWindowInfoAdded = CreateAccessibilityWindowInfo(windowInfo->currentWindowInfo_);
        a11yWindows_.emplace(windowInfo->currentWindowInfo_->wid_, a11yWindowInfoAdded);
    }
    AccessibilityEventInfo evtInfAdded(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_ADDED);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfAdded);
    if (a11yWindows_[windowInfo->currentWindowInfo_->wid_].IsFocused()) {
        SetActiveWindow(windowInfo->currentWindowInfo_->wid_);
    }
}

void AccessibilityWindowManager::WindowUpdateRemoved(const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG("start");
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    if (!a11yWindows_.count(windowInfo->currentWindowInfo_->wid_)) {
        return;
    }
    if (windowInfo->currentWindowInfo_->wid_ == activeWindowId_) {
        SetActiveWindow(INVALID_WINDOW_ID);
    }
    if (windowInfo->currentWindowInfo_->wid_ == a11yFocusedWindowId_) {
        SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
    }
    a11yWindows_.erase(windowInfo->currentWindowInfo_->wid_);
    AccessibilityEventInfo evtInfRemoved(windowInfo->currentWindowInfo_->wid_, WINDOW_UPDATE_REMOVED);
    aams.SendEvent(evtInfRemoved);
}
} // namespace Accessibility
} // namespace OHOS