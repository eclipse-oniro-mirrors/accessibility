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
    DeInit();
    HILOG_DEBUG("deinit before start");
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryWindowInfo");
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    if (err != Rosen::WMError::WM_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return false;
    }
    HILOG_DEBUG("windowInfos size is %{public}zu", windowInfos.size());
    for (auto &window : windowInfos) {
        if (!window) {
            HILOG_ERROR("window is nullptr");
            continue;
        }

        int32_t realWid = GetRealWindowId(window);
        if (!a11yWindows_.count(realWid)) {
            auto a11yWindowInfo = CreateAccessibilityWindowInfo(window);
            a11yWindows_.emplace(realWid, a11yWindowInfo);
        }

        if (IsSceneBoard(window)) {
            subWindows_.insert(realWid);
            sceneBoardElementIdMap_.InsertPair(realWid, window->uiNodeId_);
        }

        if (a11yWindows_[realWid].IsFocused()) {
            SetActiveWindow(realWid);
        }
    }
    return true;
}

void AccessibilityWindowManager::DeInit()
{
    HILOG_DEBUG();
    a11yWindows_.clear();
    subWindows_.clear();
    sceneBoardElementIdMap_.Clear();
    activeWindowId_ = INVALID_WINDOW_ID;
    a11yFocusedWindowId_ = INVALID_WINDOW_ID;
}

void AccessibilityWindowManager::WinDeInit()
{
    HILOG_DEBUG();
    a11yWindows_.clear();
    subWindows_.clear();
    sceneBoardElementIdMap_.Clear();
}

AccessibilityWindowManager::~AccessibilityWindowManager()
{
    DeregisterWindowListener();
}

void AccessibilityWindowManager::RegisterWindowListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    DeregisterWindowListener();
    HILOG_DEBUG("deregister before register");
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

void AccessibilityWindowManager::OnWindowUpdate(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos,
    Rosen::WindowUpdateType type)
{
    HILOG_DEBUG("WindowUpdateType type[%{public}d]", type);
    if (!eventHandler_) {
        HILOG_ERROR("eventHandler_ is nullptr.");
        return;
    }
    if (infos.size() == 0) {
        HILOG_ERROR("window info is err");
        return;
    }
    eventHandler_->PostTask(std::bind([=]() -> void {
        switch (type) {
            case Rosen::WindowUpdateType::WINDOW_UPDATE_ADDED: // 1
                WindowUpdateAdded(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_REMOVED: // 2
                WindowUpdateRemoved(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_BOUNDS: // 4
                WindowUpdateBounds(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_ACTIVE: // 5
                WindowUpdateActive(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_FOCUSED: // 3
                WindowUpdateFocused(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_PROPERTY: // 6
                WindowUpdateProperty(infos);
                break;
            case Rosen::WindowUpdateType::WINDOW_UPDATE_ALL:
                WindowUpdateAll(infos);
                break;
            default:
                break;
        }
        HILOG_DEBUG("a11yWindows[%{public}zu]", a11yWindows_.size());
        }), "TASK_ON_WINDOW_UPDATE");
}

int32_t AccessibilityWindowManager::ConvertToRealWindowId(int32_t windowId, int32_t focusType)
{
    int32_t winId = windowId;
    HILOG_DEBUG("ConvertToRealWindowId called, windowId[%{public}d], focusType[%{public}d]", windowId, focusType);
    if (windowId == ACTIVE_WINDOW_ID) {
        HILOG_DEBUG("After convert active windowId[%{public}d]", activeWindowId_);
        winId = activeWindowId_;
    }

    if (windowId == ANY_WINDOW_ID) {
        if (focusType == FOCUS_TYPE_ACCESSIBILITY) {
            HILOG_DEBUG("After convert a11yFocused windowId[%{public}d] by accessibility type", a11yFocusedWindowId_);
            winId = a11yFocusedWindowId_;
        } else if (focusType == FOCUS_TYPE_INPUT) {
            HILOG_DEBUG("After convert active windowId[%{public}d] by input type", activeWindowId_);
            winId = activeWindowId_;
        }
    }

    if (subWindows_.count(winId)) {
        HILOG_DEBUG("After convert normal windowId[%{public}d]", SCENE_BOARD_WINDOW_ID);
        return SCENE_BOARD_WINDOW_ID;
    }
    HILOG_DEBUG("After convert windowId[%{public}d] and activeId[%{public}d]", winId, activeWindowId_);
    return winId;
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

bool AccessibilityWindowManager::CheckIntegerOverflow(const Rosen::Rect& rect)
{
    if ((rect.posX_ > 0) && (static_cast<int32_t>(rect.width_) > 0)) {
        int32_t leftX = INT32_MAX - rect.posX_;
        if (leftX < static_cast<int32_t>(rect.width_)) {
            HILOG_ERROR("input parameter invalid posX %{public}d, width_ %{public}u", rect.posX_,
                rect.width_);
            return false;
        }
    }

    if ((rect.posX_ < 0) && (static_cast<int32_t>(rect.width_) < 0)) {
        int32_t leftX = INT32_MIN - rect.posX_;
        if (leftX > static_cast<int32_t>(rect.width_)) {
            HILOG_ERROR("input parameter invalid posX %{public}d, width_ %{public}u", rect.posX_,
                rect.width_);
            return false;
        }
    }

    if ((rect.posY_ > 0) && (static_cast<int32_t>(rect.height_) > 0)) {
        int32_t leftY = INT32_MAX - rect.posY_;
        if (leftY < static_cast<int32_t>(rect.height_)) {
            HILOG_ERROR("input parameter invalid posX %{public}d, height_ %{public}u", rect.posY_,
                rect.height_);
            return false;
        }
    }

    if ((rect.posY_ < 0) && (static_cast<int32_t>(rect.height_) < 0)) {
        int32_t leftY = INT32_MIN - rect.posY_;
        if (leftY > static_cast<int32_t>(rect.height_)) {
            HILOG_ERROR("input parameter invalid posX %{public}d, height_ %{public}u", rect.posY_,
                rect.height_);
            return false;
        }
    }
    return true;
}

void AccessibilityWindowManager::UpdateAccessibilityWindowInfo(AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    accWindowInfo.SetScaleVal(windowInfo->scaleVal_);
    accWindowInfo.SetScaleX(windowInfo->scaleX_);
    accWindowInfo.SetScaleY(windowInfo->scaleY_);
    accWindowInfo.SetWindowId(windowInfo->wid_);
    accWindowInfo.SetWindowType(static_cast<uint32_t>(windowInfo->type_));
    accWindowInfo.SetWindowMode(static_cast<uint32_t>(windowInfo->mode_));
    accWindowInfo.SetAccessibilityWindowType(ConvertWindowType(windowInfo->type_));
    accWindowInfo.SetFocused(windowInfo->focused_);
    accWindowInfo.SetWindowLayer(windowInfo->layer_);
    if (static_cast<int32_t>(windowInfo->type_) == 1 && (static_cast<int32_t>(windowInfo->windowRect_.width_) == 0 ||
        static_cast<int32_t>(windowInfo->windowRect_.height_) == 0)) {
        HILOG_ERROR("invalid window parameters, windowId(%{public}d), posX(%{public}d, posY(%{public}d))",
            windowInfo->wid_, windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_);
    } else {
        Rect bound;
        bound.SetLeftTopScreenPostion(windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_);
        if (!CheckIntegerOverflow(windowInfo->windowRect_)) {
            bound.SetRightBottomScreenPostion(windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_);
        } else {
            bound.SetRightBottomScreenPostion(
                windowInfo->windowRect_.posX_ + static_cast<int32_t>(windowInfo->windowRect_.width_),
                windowInfo->windowRect_.posY_ + static_cast<int32_t>(windowInfo->windowRect_.height_));
        }
        accWindowInfo.SetRectInScreen(bound);
    }
    accWindowInfo.SetDisplayId(windowInfo->displayId_);
    accWindowInfo.SetDecorEnable(windowInfo->isDecorEnable_);
    accWindowInfo.SetUiNodeId(windowInfo->uiNodeId_);
    accWindowInfo.SetInnerWid(windowInfo->innerWid_);
    if (accWindowInfo.GetWindowId() == SCENE_BOARD_WINDOW_ID) {
        accWindowInfo.SetWindowId(windowInfo->innerWid_);
        HILOG_DEBUG("scene board window id 1 convert inner window id[%{public}d]", windowInfo->innerWid_);
    }
    HILOG_DEBUG("bundle name is [%{public}s] , touchHotAreas size(%{public}zu)",
        windowInfo->bundleName_.c_str(), windowInfo->touchHotAreas_.size());
    accWindowInfo.SetBundleName(windowInfo->bundleName_);
    HILOG_DEBUG("UpdateAccessibilityWindowInfo is set bundlename is [%{public}s]",
        accWindowInfo.GetBundleName().c_str());
    std::vector<Rect> tempTouchHotAreas = {};
    for (auto &rect : windowInfo->touchHotAreas_) {
        HILOG_DEBUG("Rosen::windowinfo x:[%{public}d], y:[%{public}d]; width:[%{public}d], height:[%{public}d]",
            rect.posX_, rect.posY_, rect.width_, rect.height_);
        Rect rectTemp;
        rectTemp.SetLeftTopScreenPostion(rect.posX_, rect.posY_);
        if (!CheckIntegerOverflow(rect)) {
            rectTemp.SetRightBottomScreenPostion(rect.posX_, rect.posY_);
        } else {
            rectTemp.SetRightBottomScreenPostion(
                rect.posX_ + static_cast<int32_t>(rect.width_),
                rect.posY_ + static_cast<int32_t>(rect.height_));
        }
        tempTouchHotAreas.push_back(rectTemp);
    }
    accWindowInfo.SetTouchHotAreas(tempTouchHotAreas);
    for (auto &outRect : accWindowInfo.GetTouchHotAreas()) {
        HILOG_DEBUG("left_x:[%{public}d], left_y:[%{public}d]; right_x:[%{public}d], right_y:[%{public}d]",
            outRect.GetLeftTopXScreenPostion(), outRect.GetLeftTopYScreenPostion(),
            outRect.GetRightBottomXScreenPostion(), outRect.GetRightBottomYScreenPostion());
    }
}

int32_t AccessibilityWindowManager::GetRealWindowId(const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    if (windowInfo->wid_ == SCENE_BOARD_WINDOW_ID) {
        return windowInfo->innerWid_;
    }
    return windowInfo->wid_;
}

bool AccessibilityWindowManager::IsSceneBoard(const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    if (windowInfo->wid_ == SCENE_BOARD_WINDOW_ID) {
        return true;
    }
    return false;
}

AccessibilityWindowInfo AccessibilityWindowManager::CreateAccessibilityWindowInfo(
    const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    AccessibilityWindowInfo info;
    UpdateAccessibilityWindowInfo(info, windowInfo);
    HILOG_DEBUG("Create WindowInfo Id(%{public}d) type(%{public}d) posX(%{public}d) posY(%{public}d)"
        "witdth(%{public}d) height(%{public}d) display id(%{public}" PRIu64 ") isDecorEnable(%{public}d)"
        "innerWid(%{public}d), uiNodeId(%{public}d)",
        windowInfo->wid_, windowInfo->type_, windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_,
        windowInfo->windowRect_.width_, windowInfo->windowRect_.height_, windowInfo->displayId_,
        windowInfo->isDecorEnable_, windowInfo->innerWid_, windowInfo->uiNodeId_);
    return info;
}

void AccessibilityWindowManager::SetActiveWindow(int32_t windowId, bool isSendEvent)
{
    HILOG_DEBUG("windowId is %{public}d", windowId);
    if (windowId == INVALID_WINDOW_ID) {
        ClearOldActiveWindow();
        activeWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_WARN("Window id is not found");
        return;
    }

    if (activeWindowId_ != windowId) {
        ClearOldActiveWindow();
        activeWindowId_ = windowId;
        a11yWindows_[activeWindowId_].SetActive(true);
        if (!isSendEvent) {
            HILOG_DEBUG("not send event, activeWindowId is %{public}d", activeWindowId_);
            return;
        }
        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        AccessibilityEventInfo evtInf(activeWindowId_, WINDOW_UPDATE_ACTIVE);
        int32_t winId = windowId;
        if (sceneBoardElementIdMap_.CheckWindowIdPair(windowId)) {
            winId = SCENE_BOARD_WINDOW_ID;
        }
        if (aams.CheckWindowRegister(winId)) {
            HILOG_DEBUG("send active event, windowId: %{public}d", winId);
            aams.SendEvent(evtInf);
        } else {
            HILOG_DEBUG("wait for window register to process event, windowId: %{public}d", winId);
            aams.InsertWindowIdEventPair(winId, evtInf);
        }
    }
    HILOG_DEBUG("activeWindowId is %{public}d", activeWindowId_);
}

void AccessibilityWindowManager::SetAccessibilityFocusedWindow(int32_t windowId)
{
    HILOG_DEBUG("windowId is %{public}d", windowId);
    if (windowId == INVALID_WINDOW_ID) {
        ClearAccessibilityFocused();
        a11yFocusedWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_ERROR("Window id[%{public}d] is not found", windowId);
        return;
    }

    if (a11yFocusedWindowId_ != windowId) {
        ClearAccessibilityFocused();
        a11yFocusedWindowId_ = windowId;
        a11yWindows_[a11yFocusedWindowId_].SetAccessibilityFocused(true);
    }
    HILOG_DEBUG("a11yFocusedWindowId_ is %{public}d", a11yFocusedWindowId_);
}

std::vector<AccessibilityWindowInfo> AccessibilityWindowManager::GetAccessibilityWindows()
{
    HILOG_DEBUG("a11yWindows_ size[%{public}zu]", a11yWindows_.size());
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    std::vector<AccessibilityWindowInfo> windows;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    HILOG_DEBUG("windowInfos size[%{public}zu]", windowInfos.size());
    if (err != Rosen::WMError::WM_OK) {
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return windows;
    }
    for (auto &info : windowInfos) {
        if (info == nullptr) {
            HILOG_DEBUG("info is nullptr");
            continue;
        }

        int32_t realWidId = GetRealWindowId(info);
        if (a11yWindows_.count(realWidId)) {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], info);
        }
    }
    std::transform(a11yWindows_.begin(), a11yWindows_.end(), std::back_inserter(windows),
        [](const std::map<int32_t, AccessibilityWindowInfo>::value_type &window) { return window.second; });

    HILOG_DEBUG("window size[%{public}zu]", windows.size());
    for (auto &logWindow : windows) {
        HILOG_DEBUG("logWindow id[%{public}d]", logWindow.GetWindowId());
    }
    return windows;
}

bool AccessibilityWindowManager::GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    if (err != Rosen::WMError::WM_OK) {
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return false;
    }
    for (auto &info : windowInfos) {
        if (info == nullptr) {
            continue;
        }

        int32_t realWidId = GetRealWindowId(info);
        if (info != nullptr && a11yWindows_.count(realWidId)) {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], info);
        }
    }
    if (a11yWindows_.count(windowId)) {
        window = a11yWindows_[windowId];
        return true;
    }
    return false;
}

bool AccessibilityWindowManager::IsValidWindow(int32_t windowId)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);

    auto it = std::find_if(a11yWindows_.begin(), a11yWindows_.end(),
        [windowId](const std::map<int32_t, AccessibilityWindowInfo>::value_type &window) {
            return window.first == windowId;
        });
    if (it == a11yWindows_.end()) {
        return false;
    }
    return true;
}

void AccessibilityWindowManager::SetWindowSize(int32_t windowId, Rect rect)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    auto it = std::find_if(a11yWindows_.begin(), a11yWindows_.end(),
        [windowId](const std::map<int32_t, AccessibilityWindowInfo>::value_type &window) {
            return window.first == windowId;
        });
    if (it != a11yWindows_.end()) {
        it->second.SetRectInScreen(rect);
    }
}

bool AccessibilityWindowManager::CompareRect(const Rect &rectAccessibility, const Rosen::Rect &rectWindow)
{
    HILOG_DEBUG();
    int32_t leftTopX_ = rectWindow.posX_;
    int32_t leftTopY_ = rectWindow.posY_;
    int32_t rightBottomX_ = 0;
    int32_t rightBottomY_ = 0;

    if (!CheckIntegerOverflow(rectWindow)) {
        rightBottomX_ = rectWindow.posX_;
        rightBottomY_ = rectWindow.posY_;
    } else {
        rightBottomX_ = rectWindow.posX_ + static_cast<int32_t>(rectWindow.width_);
        rightBottomY_ = rectWindow.posY_ + static_cast<int32_t>(rectWindow.height_);
    }

    if (rectAccessibility.GetLeftTopXScreenPostion() == leftTopX_ &&
        rectAccessibility.GetLeftTopYScreenPostion() == leftTopY_ &&
        rectAccessibility.GetRightBottomXScreenPostion() == rightBottomX_ &&
        rectAccessibility.GetRightBottomYScreenPostion() == rightBottomY_) {
        HILOG_DEBUG("rect values are the same");
        return false;
    }
    return true;
}

bool AccessibilityWindowManager::EqualFocus(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG();
    if (accWindowInfo.IsFocused() == windowInfo->focused_) {
        HILOG_DEBUG("focus values are the same");
        return false;
    }
    return true;
}

bool AccessibilityWindowManager::EqualBound(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG();
    if (static_cast<int32_t>(windowInfo->type_) == 1 && (static_cast<int32_t>(windowInfo->windowRect_.width_) == 0 ||
        static_cast<int32_t>(windowInfo->windowRect_.height_) == 0)) {
        HILOG_ERROR("invalid window parameters, windowId(%{public}d), posX(%{public}d, posY(%{public}d))",
            windowInfo->wid_, windowInfo->windowRect_.posX_, windowInfo->windowRect_.posY_);
        return false;
    }
    return CompareRect(accWindowInfo.GetRectInScreen(), windowInfo->windowRect_);
}

bool AccessibilityWindowManager::EqualProperty(Accessibility::AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG();
    std::ostringstream  accInfoStr;
    std::ostringstream  winInfoStr;

    accInfoStr << accWindowInfo.GetWindowMode()
               << accWindowInfo.GetWindowLayer()
               << accWindowInfo.IsDecorEnable()
               << accWindowInfo.GetWindowType()
               << accWindowInfo.GetDisplayId()
               << accWindowInfo.GetScaleVal()
               << accWindowInfo.GetScaleX()
               << accWindowInfo.GetScaleY();
    HILOG_DEBUG("Create accinfoStr windowMode_[%{public}d] Layer_[%{public}d] isDecorEnable_[%{public}d]"
        "windowType_[%{public}d] displayId:%{public}" PRIu64 " get scaleVal_ [%{public}f]"
        "get scaleX_ [%{public}f] get scaleY_ [%{public}f]",
        accWindowInfo.GetWindowMode(), accWindowInfo.GetWindowLayer(), accWindowInfo.IsDecorEnable(),
        accWindowInfo.GetWindowType(), accWindowInfo.GetDisplayId(), accWindowInfo.GetScaleVal(),
        accWindowInfo.GetScaleX(), accWindowInfo.GetScaleY());

    winInfoStr << static_cast<uint32_t>(windowInfo->mode_)
               << windowInfo->layer_
               << windowInfo->isDecorEnable_
               << static_cast<uint32_t>(windowInfo->type_)
               << windowInfo->displayId_
               << windowInfo->scaleVal_
               << windowInfo->scaleX_
               << windowInfo->scaleY_;
    HILOG_DEBUG("Create wininfoStr Mode_[%{public}d] Layer_[%{public}d] isDecorEnable_[%{public}d]"
        "Type_[%{public}d] displayId:%{public}" PRIu64 " scaleVal_ [%{public}f]"
        "scaleX_ [%{public}f] scaleY_ [%{public}f]",
        static_cast<uint32_t>(windowInfo->mode_), windowInfo->layer_, windowInfo->isDecorEnable_,
        static_cast<uint32_t>(windowInfo->type_), windowInfo->displayId_, windowInfo->scaleVal_,
        windowInfo->scaleX_, windowInfo->scaleY_);

    if (accInfoStr.str() != winInfoStr.str() ||
        windowInfo->touchHotAreas_.size() != accWindowInfo.GetTouchHotAreas().size()) {
        HILOG_DEBUG("Property different");
        return true;
    }
    for (auto i = 0; i < accWindowInfo.GetTouchHotAreas().size(); i++) {
        if (CompareRect(accWindowInfo.GetTouchHotAreas()[i], windowInfo->touchHotAreas_[i])) {
            HILOG_DEBUG("touchHotAreas different");
            return true;
        }
    }
    return false;
}

void AccessibilityWindowManager::WindowUpdateAdded(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (!a11yWindows_.count(realWidId)) {
            auto a11yWindowInfoAdded = CreateAccessibilityWindowInfo(windowInfo);
            a11yWindows_.emplace(realWidId, a11yWindowInfoAdded);
        } else {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], windowInfo);
        }

        if (IsSceneBoard(windowInfo)) {
            subWindows_.insert(realWidId);
            sceneBoardElementIdMap_.InsertPair(realWidId, windowInfo->uiNodeId_);
        }
        AccessibilityEventInfo evtInfAdded(realWidId, WINDOW_UPDATE_ADDED);
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfAdded);
        if (a11yWindows_[realWidId].IsFocused()) {
            SetActiveWindow(realWidId);
        }
    }
}

void AccessibilityWindowManager::WindowUpdateRemoved(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (!a11yWindows_.count(realWidId)) {
            return;
        }
        if (realWidId == activeWindowId_) {
            SetActiveWindow(INVALID_WINDOW_ID);
        }
        if (realWidId == a11yFocusedWindowId_) {
            SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
        }
        a11yWindows_.erase(realWidId);
        subWindows_.erase(realWidId);
        sceneBoardElementIdMap_.RemovePair(realWidId);
        AccessibilityEventInfo evtInfRemoved(realWidId, WINDOW_UPDATE_REMOVED);
        aams.SendEvent(evtInfRemoved);
    }
}

void AccessibilityWindowManager::WindowUpdateFocused(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (!a11yWindows_.count(realWidId)) {
            HILOG_DEBUG("window not created");
            auto a11yWindowInfoFocused = CreateAccessibilityWindowInfo(windowInfo);
            a11yWindows_.emplace(realWidId, a11yWindowInfoFocused);
        }

        if (IsSceneBoard(windowInfo)) {
            subWindows_.insert(realWidId);
            sceneBoardElementIdMap_.InsertPair(realWidId, windowInfo->uiNodeId_);
        }
        SetActiveWindow(realWidId);
        AccessibilityEventInfo evtInfFocused(realWidId, WINDOW_UPDATE_FOCUSED);
        aams.SendEvent(evtInfFocused);
    }
}

void AccessibilityWindowManager::WindowUpdateBounds(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (a11yWindows_.count(realWidId)) {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], windowInfo);
        }

        AccessibilityEventInfo evtInfBounds(realWidId, WINDOW_UPDATE_BOUNDS);
        aams.SendEvent(evtInfBounds);
    }
}

void AccessibilityWindowManager::WindowUpdateActive(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (!a11yWindows_.count(realWidId)) {
            auto a11yWindowInfoActive = CreateAccessibilityWindowInfo(windowInfo);
            a11yWindows_.emplace(realWidId, a11yWindowInfoActive);
        }

        if (IsSceneBoard(windowInfo)) {
            subWindows_.insert(realWidId);
            sceneBoardElementIdMap_.InsertPair(realWidId, windowInfo->uiNodeId_);
        }
        SetActiveWindow(realWidId);
    }
}

void AccessibilityWindowManager::WindowUpdateProperty(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    for (auto &windowInfo : infos) {
        if (!windowInfo) {
            HILOG_ERROR("invalid windowInfo");
            return;
        }

        int32_t realWidId = GetRealWindowId(windowInfo);
        if (a11yWindows_.count(realWidId)) {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], windowInfo);
        }
        AccessibilityEventInfo evtInfProperty(realWidId, WINDOW_UPDATE_PROPERTY);
        aams.SendEvent(evtInfProperty);
    }
}

void AccessibilityWindowManager::WindowUpdateTypeEvent(const int32_t realWidId, Accessibility::WindowUpdateType type)
{
    HILOG_DEBUG();
    HILOG_DEBUG("WindowUpdateType type[%{public}d]", type);
    auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED: {
                AccessibilityEventInfo evtInfAdded(realWidId, WINDOW_UPDATE_ADDED);
                Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfAdded);
                if (a11yWindows_[realWidId].IsFocused()) {
                    SetActiveWindow(realWidId);
                }
                break;
            }
        case WindowUpdateType::WINDOW_UPDATE_REMOVED: {
                if (realWidId == activeWindowId_) {
                    SetActiveWindow(INVALID_WINDOW_ID);
                }
                if (realWidId == a11yFocusedWindowId_) {
                    SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
                }

                AccessibilityEventInfo evtInfRemoved(realWidId, WINDOW_UPDATE_REMOVED);
                aams.SendEvent(evtInfRemoved);
                break;
            }
        case WindowUpdateType::WINDOW_UPDATE_BOUNDS: {
                AccessibilityEventInfo evtInfBounds(realWidId, WINDOW_UPDATE_BOUNDS);
                aams.SendEvent(evtInfBounds);
                break;
            }
        case WindowUpdateType::WINDOW_UPDATE_FOCUSED: {
                SetActiveWindow(realWidId);
                AccessibilityEventInfo evtInfFocused(realWidId, WINDOW_UPDATE_FOCUSED);
                aams.SendEvent(evtInfFocused);
                break;
            }
        case WindowUpdateType::WINDOW_UPDATE_PROPERTY: {
                AccessibilityEventInfo evtInfProperty(realWidId, WINDOW_UPDATE_PROPERTY);
                aams.SendEvent(evtInfProperty);
                break;
            }
        default:
            break;
        }
}

void AccessibilityWindowManager::WindowUpdateAll(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    auto oldA11yWindows_ = a11yWindows_;
    HILOG_DEBUG("WindowUpdateAll info size(%{public}zu), oldA11yWindows_ size(%{public}zu)",
        infos.size(), oldA11yWindows_.size());
    WinDeInit();
    for (auto &window : infos) {
        if (!window) {
            HILOG_ERROR("window is nullptr");
            continue;
        }
        int32_t realWid = GetRealWindowId(window);
        if (!a11yWindows_.count(realWid)) {
            auto a11yWindowInfo = CreateAccessibilityWindowInfo(window);
            a11yWindows_.emplace(realWid, a11yWindowInfo);
            HILOG_DEBUG("WindowUpdateAll a11yWindowInfo size(%{public}s)", a11yWindowInfo.GetBundleName().c_str());
        }
        if (IsSceneBoard(window)) {
            subWindows_.insert(realWid);
            sceneBoardElementIdMap_.InsertPair(realWid, window->uiNodeId_);
        }

        if (a11yWindows_[realWid].IsFocused()) {
            SetActiveWindow(realWid);
        }

        if (!oldA11yWindows_.count(realWid)) {
            WindowUpdateTypeEvent(realWid, WINDOW_UPDATE_ADDED);
        } else {
            if (EqualFocus(oldA11yWindows_[realWid], window)) {
                WindowUpdateTypeEvent(realWid, WINDOW_UPDATE_FOCUSED);
            }
            if (EqualBound(oldA11yWindows_[realWid], window)) {
                WindowUpdateTypeEvent(realWid, WINDOW_UPDATE_BOUNDS);
            }
            if (EqualProperty(oldA11yWindows_[realWid], window)) {
                WindowUpdateTypeEvent(realWid, WINDOW_UPDATE_PROPERTY);
            }
            auto itr = oldA11yWindows_.find(realWid);
            if (itr != oldA11yWindows_.end()) {
                oldA11yWindows_.erase(itr);
            }
        }
    }

    for (auto it = oldA11yWindows_.begin(); it != oldA11yWindows_.end(); ++it) {
        WindowUpdateTypeEvent(it->first, WINDOW_UPDATE_REMOVED);
    }
    HILOG_DEBUG("WindowUpdateAll a11yWindowInfo_ size(%{public}zu)", a11yWindows_.size());
}

void AccessibilityWindowManager::ClearOldActiveWindow()
{
    HILOG_DEBUG("active window id is %{public}d", activeWindowId_);
    if (activeWindowId_ == INVALID_WINDOW_ID) {
        HILOG_DEBUG("active window id is invalid");
        return;
    }

    if (a11yWindows_.count(activeWindowId_)) {
        a11yWindows_[activeWindowId_].SetActive(false);
    }
    if (activeWindowId_ == a11yFocusedWindowId_) {
        HILOG_DEBUG("Old active window is a11yFocused window.");
        SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
    }
}

void AccessibilityWindowManager::ClearAccessibilityFocused()
{
    HILOG_DEBUG("a11yFocused window id is %{public}d", a11yFocusedWindowId_);
    if (a11yFocusedWindowId_ == INVALID_WINDOW_ID) {
        HILOG_DEBUG("a11yFocused window id is invalid");
        return;
    }

    if (a11yWindows_.count(a11yFocusedWindowId_)) {
        a11yWindows_[a11yFocusedWindowId_].SetAccessibilityFocused(false);
    }

    int32_t windowId = a11yFocusedWindowId_;
    if (subWindows_.count(a11yFocusedWindowId_)) {
        windowId = SCENE_BOARD_WINDOW_ID;
    }
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return;
    }
    sptr<AccessibilityWindowConnection> connection =
        accountData->GetAccessibilityWindowConnection(windowId);
    if (!connection) {
        HILOG_ERROR("windowId[%{public}d] has no connection", windowId);
        return;
    }
    if (!connection->GetProxy()) {
        HILOG_ERROR("windowId[%{public}d] has no proxy", windowId);
        return;
    }
    connection->GetProxy()->ClearFocus();

    // Send event
    AccessibilityEventInfo eventInfo(TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT);
    eventInfo.SetWindowId(a11yFocusedWindowId_);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfo);
}

int64_t AccessibilityWindowManager::GetSceneBoardElementId(const int32_t windowId, const int64_t elementId)
{
    if (elementId != INVALID_SCENE_BOARD_ELEMENT_ID) {
        return elementId;
    }
    if (subWindows_.count(windowId)) {
        auto iter = a11yWindows_.find(windowId);
        if (iter != a11yWindows_.end()) {
            HILOG_DEBUG("GetSceneBoardElementId [%{public}" PRId64 "]", iter->second.GetUiNodeId());
            return iter->second.GetUiNodeId();
        }
    }
    return elementId;
}

void AccessibilityWindowManager::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    // sceneboard window id, element id is not equal -1
    if (subWindows_.count(windowId) && elementId != INVALID_SCENE_BOARD_ELEMENT_ID) {
        windowId = SCENE_BOARD_WINDOW_ID;
        HILOG_INFO("windowId %{public}d, elementId %{public}" PRId64 "", windowId, elementId);
        return;
    }

    if (elementId != INVALID_SCENE_BOARD_ELEMENT_ID) {
        return;
    }

    if (subWindows_.count(windowId)) {
        auto iter = a11yWindows_.find(windowId);
        if (iter != a11yWindows_.end()) {
            HILOG_DEBUG("GetRealWindowAndElementId [%{public}" PRId64 "]", iter->second.GetUiNodeId());
            windowId = SCENE_BOARD_WINDOW_ID;
            elementId = iter->second.GetUiNodeId();
            return;
        }
    }
}

void AccessibilityWindowManager::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    if (windowId != SCENE_BOARD_WINDOW_ID) {
        return;
    }

    for (auto iter = a11yWindows_.begin(); iter != a11yWindows_.end(); iter++) {
        if (iter->second.GetUiNodeId() == elementId) {
            innerWid = iter->second.GetInnerWid();
        }
    }

    return;
}

void AccessibilityWindowManager::SceneBoardElementIdMap::InsertPair(const int32_t windowId, const int64_t elementId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    windowElementMap_[windowId] = elementId;
}

void AccessibilityWindowManager::SceneBoardElementIdMap::RemovePair(const int32_t windowId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    windowElementMap_.erase(windowId);
}

bool AccessibilityWindowManager::SceneBoardElementIdMap::CheckWindowIdPair(const int32_t windowId)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    return windowElementMap_.count(windowId);
}

void AccessibilityWindowManager::SceneBoardElementIdMap::Clear()
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    windowElementMap_.clear();
}

std::map<int32_t, int64_t> AccessibilityWindowManager::SceneBoardElementIdMap::GetAllPairs()
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    return windowElementMap_;
}

RetError AccessibilityWindowManager::GetFocusedWindowId(int32_t &focusedWindowId)
{
    HILOG_DEBUG();
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryFocusedWindowInfo");
    Rosen::FocusChangeInfo focusedWindowInfo;
    OHOS::Rosen::WindowManager::GetInstance().GetFocusWindowInfo(focusedWindowInfo);
    if (focusedWindowInfo.windowId_ == INVALID_WINDOW_ID) {
        return RET_ERR_INVALID_PARAM;
    }
    focusedWindowId = focusedWindowInfo.windowId_;
    return RET_OK;
}

bool AccessibilityWindowManager::IsInnerWindowRootElement(int64_t elementId)
{
    HILOG_DEBUG("IsInnerWindowRootElement elementId: %{public}" PRId64 "", elementId);
    auto mapTable = sceneBoardElementIdMap_.GetAllPairs();
    for (auto iter = mapTable.begin(); iter != mapTable.end(); iter++) {
        if (elementId == iter->second) {
            return true;
        }
    }
    return false;
}
} // namespace Accessibility
} // namespace OHOS