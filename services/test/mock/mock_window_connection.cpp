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

#include "accessibility_window_connection.h"

using namespace std;

namespace OHOS {
namespace Accessibility {
MockWindowConnection::MockWindowConnection(const int32_t windowId,
    const sptr<IAccessibilityElementOperator> &connection, const int32_t accountId)
{
    windowId_ = windowId;
    proxy_ = connection;
    accountId_ = accountId;
    cardProxy_.EnsureInsert(0, connection);
}

MockWindowConnection::MockWindowConnection(const int32_t windowId, const int32_t treeId,
    const sptr<IAccessibilityElementOperator> &connection, const int32_t accountId)
{
    windowId_ = windowId;
    treeId_ = treeId;
    accountId_ = accountId;
    cardProxy_.EnsureInsert(treeId, connection);
}

MockWindowConnection::~MockWindowConnection()
{
}

RetError MockWindowConnection::SetCardProxy(const int32_t treeId,
    sptr<IAccessibilityElementOperator> operation)
{
    if (!operation) {
        HILOG_DEBUG("SetCardProxy : operation is nullptr");
        return RET_ERR_FAILED;
    }
    cardProxy_.EnsureInsert(treeId, operation);
    return RET_OK;
}

sptr<IAccessibilityElementOperator> MockWindowConnection::GetCardProxy(const int32_t treeId)
{
    sptr<IAccessibilityElementOperator> connection = nullptr;
    bool ret = cardProxy_.Find(treeId, connection);
    HILOG_DEBUG("GetCardProxy : operation is %{public}d", connection != nullptr);
    return connection;
}

RetError MockWindowConnection::SetTokenIdMap(const int32_t treeId,
    const uint32_t tokenId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    tokenIdMap_.EnsureInsert(treeId, tokenId);
    return RET_OK;
}

uint32_t MockWindowConnection::GetTokenIdMap(const int32_t treeId)
{
    HILOG_DEBUG("treeId : %{public}d", treeId);
    return tokenIdMap_.ReadVal(treeId);
}

void MockWindowConnection::GetAllTreeId(std::vector<int32_t> &treeIds)
{
    auto getAllTreeIdFunc = [&treeIds](const int32_t treeId, sptr<IAccessibilityElementOperator> connection) {
        treeIds.emplace_back(treeId);
    };
    cardProxy_.Iterate(getAllTreeIdFunc);
}

RetError MockWindowConnection::GetRootParentId(int32_t treeId, int64_t &elementId)
{
    bool ret = treeIdParentId_.Find(treeId, elementId);
    return ret == true ? RET_OK : RET_ERR_FAILED;
}

RetError MockWindowConnection::SetRootParentId(const int32_t treeId, const int64_t elementId)
{
    treeIdParentId_.EnsureInsert(treeId, elementId);
    return RET_OK;
}

void MockWindowConnection::EraseProxy(const int32_t treeId)
{
    sptr<IAccessibilityElementOperator> connection = nullptr;
    bool ret = cardProxy_.Find(treeId, connection);
    if (ret) {
        cardProxy_.Erase(treeId);
    }
}

MockWindowManager::MockWindowManager()
{
}

bool MockWindowManager::Init()
{
    DeInit();
    HILOG_DEBUG("deinit before start");
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryWindowInfo");
#endif // OHOS_BUILD_ENABLE_HITRACE
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    if (err != Rosen::WMError::WM_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return false;
    }
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::DeInit()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    a11yWindows_.clear();
    subWindows_.clear();
    sceneBoardElementIdMap_.Clear();
    activeWindowId_ = INVALID_WINDOW_ID;
    a11yFocusedWindowId_ = INVALID_WINDOW_ID;
}

void MockWindowManager::WinDeInit()
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    a11yWindows_.clear();
    sceneBoardElementIdMap_.Clear();
    activeWindowId_ = INVALID_WINDOW_ID;
}

MockWindowManager::~MockWindowManager()
{
    DeregisterWindowListener();
}

void MockWindowManager::RegisterWindowListener(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
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

void MockWindowManager::DeregisterWindowListener()
{
    if (windowListener_) {
        OHOS::Rosen::WindowManager::GetInstance().UnregisterWindowUpdateListener(windowListener_);
        windowListener_ = nullptr;
        eventHandler_ = nullptr;
    }
}

void MockWindowManager::OnWindowUpdate(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos,
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
    eventHandler_->PostTask([=]() {
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
        }, "TASK_ON_WINDOW_UPDATE");
}

int32_t MockWindowManager::ConvertToRealWindowId(int32_t windowId, int32_t focusType)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

bool MockWindowManager::CheckIntegerOverflow(const Rosen::Rect& rect)
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

void MockWindowManager::UpdateAccessibilityWindowInfo(AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    accWindowInfo.SetScaleVal(windowInfo->scaleVal_);
    accWindowInfo.SetScaleX(windowInfo->scaleX_);
    accWindowInfo.SetScaleY(windowInfo->scaleY_);
    accWindowInfo.SetWindowId(windowInfo->wid_);
    accWindowInfo.SetMainWindowId(windowInfo->wid_);
    accWindowInfo.SetWindowType(static_cast<uint32_t>(windowInfo->type_));
    accWindowInfo.SetWindowMode(static_cast<uint32_t>(windowInfo->mode_));
    accWindowInfo.SetAccessibilityWindowType(ConvertWindowType(windowInfo->type_));
    accWindowInfo.SetFocused(windowInfo->focused_);
    accWindowInfo.SetWindowLayer(windowInfo->layer_);
    if (static_cast<int32_t>(windowInfo->type_) == 1 && (static_cast<int32_t>(windowInfo->windowRect_.width_) == 0 ||
        static_cast<int32_t>(windowInfo->windowRect_.height_) == 0)) {
        HILOG_WARN("invalid window parameters, windowId(%{public}d), posX(%{public}d, posY(%{public}d))",
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
        accWindowInfo.SetMainWindowId(windowInfo->innerWid_);
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

int32_t MockWindowManager::GetRealWindowId(const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    if (windowInfo->wid_ == SCENE_BOARD_WINDOW_ID) {
        return windowInfo->innerWid_;
    }
    return windowInfo->wid_;
}

bool MockWindowManager::IsSceneBoard(const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    if (windowInfo->wid_ == SCENE_BOARD_WINDOW_ID) {
        return true;
    }
    return false;
}

bool MockWindowManager::IsScenePanel(const sptr<Rosen::AccessibilityWindowInfo> windowInfo)
{
    return windowInfo->bundleName_.find(SCB_SCENE_PANEL) != std::string::npos;
}

AccessibilityWindowInfo MockWindowManager::CreateAccessibilityWindowInfo(
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

bool MockWindowManager::CheckEvents()
{
    sptr<AccessibilityAccountData> accountData =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr");
        return false;
    }
    std::vector<uint32_t> needEvents;
    needEvents = accountData->GetNeedEvents();
 
    auto isExit = std::find(needEvents.begin(), needEvents.end(), TYPE_WINDOW_UPDATE);
    auto isAllEvent = std::find(needEvents.begin(), needEvents.end(), TYPES_ALL_MASK);
    if (isAllEvent != needEvents.end() || isExit != needEvents.end()) {
        return true;
    }
    return false;
}

void MockWindowManager::SetActiveWindow(int32_t windowId, bool isSendEvent)
{
    HILOG_INFO("windowId is %{public}d, activeWindowId_: %{public}d", windowId, activeWindowId_);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    if (windowId == INVALID_WINDOW_ID) {
        ClearOldActiveWindow();
        activeWindowId_ = INVALID_WINDOW_ID;
        return;
    }

    if (!a11yWindows_.count(windowId)) {
        HILOG_WARN("Window id is not found");
        return;
    }

    bool isSendWindowEvent = CheckEvents();
    HILOG_DEBUG("isSendWindowEvent is: %{public}d", isSendWindowEvent);
    if (!isSendWindowEvent) {
        isSendEvent = false;
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
        AccessibilityEventInfoParcel evtInfParcel(evtInf);
        int32_t winId = windowId;
        if (sceneBoardElementIdMap_.CheckWindowIdPair(windowId)) {
            winId = SCENE_BOARD_WINDOW_ID;
        }
        SetEventInfoBundleName(evtInfParcel);
        if (aams.CheckWindowRegister(winId)) {
            HILOG_DEBUG("send active event, windowId: %{public}d", winId);
            aams.SendEvent(evtInfParcel, 0);
        } else {
            HILOG_DEBUG("wait for window register to process event, windowId: %{public}d", winId);
            aams.InsertWindowIdEventPair(winId, evtInfParcel);
        }
    }
    HILOG_DEBUG("activeWindowId is %{public}d", activeWindowId_);
}

int32_t MockWindowManager::GetActiveWindowId()
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    HILOG_DEBUG("activeWindowId_ is %{public}d", activeWindowId_);
    return activeWindowId_;
}

void MockWindowManager::SetAccessibilityFocusedWindow(int32_t windowId)
{
    HILOG_DEBUG("windowId is %{public}d", windowId);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

std::vector<AccessibilityWindowInfo> MockWindowManager::GetAccessibilityWindows()
{
    HILOG_DEBUG("a11yWindows_ size[%{public}zu]", a11yWindows_.size());
    XCollieHelper timer(TIMER_GET_ACCESSIBILITY_WINDOWS, WMS_TIMEOUT);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    std::vector<AccessibilityWindowInfo> windows;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    if (err != Rosen::WMError::WM_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return windows;
    }
    for (auto &info : windowInfos) {
        if (info == nullptr) {
            continue;
        }
        AccessibilityWindowInfo tmpWindowInfo;
        UpdateAccessibilityWindowInfo(tmpWindowInfo, info);
        if (tmpWindowInfo.IsFocused()) {
            HILOG_DEBUG("set active windowId: %{public}d", tmpWindowInfo.GetWindowId());
            tmpWindowInfo.SetActive(true);
        }
        windows.push_back(tmpWindowInfo);
    }
    return windows;
}

bool MockWindowManager::GetAccessibilityWindow(int32_t windowId, AccessibilityWindowInfo &window)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    XCollieHelper timer(TIMER_GET_ACCESSIBILITY_WINDOWS, WMS_TIMEOUT);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    std::vector<sptr<Rosen::AccessibilityWindowInfo>> windowInfos;
    Rosen::WMError err = OHOS::Rosen::WindowManager::GetInstance().GetAccessibilityWindowInfo(windowInfos);
    if (err != Rosen::WMError::WM_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::QUERY_EVENT, A11yError::ERROR_QUERY_WINDOW_INFO_FAILED);
        HILOG_ERROR("get window info from wms failed. err[%{public}d]", err);
        return false;
    }
    for (auto &info : windowInfos) {
        if (info == nullptr) {
            continue;
        }

        int32_t realWidId = GetRealWindowId(info);
        if (a11yWindows_.count(realWidId)) {
            UpdateAccessibilityWindowInfo(a11yWindows_[realWidId], info);
        } else {
            AccessibilityWindowInfo tmpWindowInfo;
            UpdateAccessibilityWindowInfo(tmpWindowInfo, info);
            a11yWindows_[realWidId] = tmpWindowInfo;
        }
    }
    if (a11yWindows_.count(windowId)) {
        window = a11yWindows_[windowId];
        return true;
    }
    return false;
}

bool MockWindowManager::IsValidWindow(int32_t windowId)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    auto it = std::find_if(a11yWindows_.begin(), a11yWindows_.end(),
        [windowId](const std::map<int32_t, AccessibilityWindowInfo>::value_type &window) {
            return window.first == windowId;
        });
    if (it == a11yWindows_.end()) {
        return false;
    }
    return true;
}

void MockWindowManager::SetWindowSize(int32_t windowId, Rect rect)
{
    HILOG_DEBUG("start windowId(%{public}d)", windowId);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    auto it = std::find_if(a11yWindows_.begin(), a11yWindows_.end(),
        [windowId](const std::map<int32_t, AccessibilityWindowInfo>::value_type &window) {
            return window.first == windowId;
        });
    if (it != a11yWindows_.end()) {
        it->second.SetRectInScreen(rect);
    }
}

bool MockWindowManager::CompareRect(const Rect &rectAccessibility, const Rosen::Rect &rectWindow)
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

bool MockWindowManager::EqualFocus(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG();
    if (accWindowInfo.IsFocused() == windowInfo->focused_) {
        HILOG_DEBUG("focus values are the same");
        return false;
    }
    return true;
}

bool MockWindowManager::EqualBound(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
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

bool MockWindowManager::EqualProperty(Accessibility::AccessibilityWindowInfo &accWindowInfo,
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
    for (uint32_t i = 0; i < accWindowInfo.GetTouchHotAreas().size(); i++) {
        if (CompareRect(accWindowInfo.GetTouchHotAreas()[i], windowInfo->touchHotAreas_[i])) {
            HILOG_DEBUG("touchHotAreas different");
            return true;
        }
    }
    return false;
}

bool MockWindowManager::EqualLayer(const Accessibility::AccessibilityWindowInfo &accWindowInfo,
    const sptr<Rosen::AccessibilityWindowInfo> &windowInfo)
{
    HILOG_DEBUG();
    if (static_cast<uint32_t>(accWindowInfo.GetWindowLayer()) == windowInfo->layer_) {
        HILOG_DEBUG("layer values are the same");
        return false;
    }
    return true;
}

void MockWindowManager::WindowUpdateAdded(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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
        bool isSendWindowEvent = CheckEvents();
        if (isSendWindowEvent) {
            AccessibilityEventInfo evtInfAdded(realWidId, WINDOW_UPDATE_ADDED);
            AccessibilityEventInfoParcel evtInfParcel(evtInfAdded);
            Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
        }
        if (a11yWindows_[realWidId].IsFocused()) {
            SetActiveWindow(realWidId);
        }
    }
}

void MockWindowManager::WindowUpdateRemoved(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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
        bool isSendWindowEvent = CheckEvents();
        if (isSendWindowEvent) {
            AccessibilityEventInfo evtInfRemoved(realWidId, WINDOW_UPDATE_REMOVED);
            AccessibilityEventInfoParcel evtInfParcel(evtInfRemoved);
            aams.SendEvent(evtInfParcel, 0);
        }
    }
}

void MockWindowManager::WindowUpdateFocused(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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
        bool isSendWindowEvent = CheckEvents();
        if (isSendWindowEvent) {
            AccessibilityEventInfo evtInfFocused(realWidId, WINDOW_UPDATE_FOCUSED);
            AccessibilityEventInfoParcel evtInfParcel(evtInfFocused);
            aams.SendEvent(evtInfParcel, 0);
        }
    }
}

void MockWindowManager::WindowUpdateBounds(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

        bool isSendWindowEvent = CheckEvents();
        if (isSendWindowEvent) {
            AccessibilityEventInfo evtInfBounds(realWidId, WINDOW_UPDATE_BOUNDS);
            AccessibilityEventInfoParcel evtInfParcel(evtInfBounds);
            Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
        }
    }
}

void MockWindowManager::WindowUpdateActive(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::WindowUpdateProperty(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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
        bool isSendWindowEvent = CheckEvents();
        if (isSendWindowEvent) {
            AccessibilityEventInfo evtInfProperty(realWidId, WINDOW_UPDATE_PROPERTY);
            AccessibilityEventInfoParcel evtInfParcel(evtInfProperty);
            Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
        }
    }
}

void MockWindowManager::WindowUpdateTypeEventAdded(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    AccessibilityEventInfo evtInfAdded(realWidId, WINDOW_UPDATE_ADDED);
    SetEventInfoBundleNameOld(evtInfAdded, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfAdded);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
    if (a11yWindows_[realWidId].IsFocused()) {
        SetActiveWindow(realWidId);
    }
}

void MockWindowManager::WindowUpdateTypeEventRemoved(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    if (realWidId == activeWindowId_) {
        SetActiveWindow(INVALID_WINDOW_ID);
    }
    if (realWidId == a11yFocusedWindowId_) {
        SetAccessibilityFocusedWindow(INVALID_WINDOW_ID);
    }

    AccessibilityEventInfo evtInfRemoved(realWidId, WINDOW_UPDATE_REMOVED);
    SetEventInfoBundleNameOld(evtInfRemoved, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfRemoved);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
}

void MockWindowManager::WindowUpdateTypeEventBounds(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    AccessibilityEventInfo evtInfBounds(realWidId, WINDOW_UPDATE_BOUNDS);
    SetEventInfoBundleNameOld(evtInfBounds, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfBounds);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
}

void MockWindowManager::WindowUpdateTypeEventFocused(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    SetActiveWindow(realWidId);
    AccessibilityEventInfo evtInfFocused(realWidId, WINDOW_UPDATE_FOCUSED);
    SetEventInfoBundleNameOld(evtInfFocused, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfFocused);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
}

void MockWindowManager::WindowUpdateTypeEventProperty(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    AccessibilityEventInfo evtInfProperty(realWidId, WINDOW_UPDATE_PROPERTY);
    SetEventInfoBundleNameOld(evtInfProperty, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfProperty);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
}

void MockWindowManager::WindowUpdateTypeEventLayer(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    AccessibilityEventInfo evtInfLayer(realWidId, WINDOW_UPDATE_LAYER);
    SetEventInfoBundleNameOld(evtInfLayer, realWidId, oldA11yWindows_);
    AccessibilityEventInfoParcel evtInfParcel(evtInfLayer);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(evtInfParcel, 0);
    }
}

void MockWindowManager::WindowUpdateTypeEvent(const int32_t realWidId,
    std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_, Accessibility::WindowUpdateType type)
{
    HILOG_DEBUG();
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    HILOG_DEBUG("WindowUpdateType type[%{public}d]", type);
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED: {
            WindowUpdateTypeEventAdded(realWidId, oldA11yWindows_);
            break;
            }
        case WindowUpdateType::WINDOW_UPDATE_REMOVED: {
            WindowUpdateTypeEventRemoved(realWidId, oldA11yWindows_);
            break;
            }
        case WindowUpdateType::WINDOW_UPDATE_BOUNDS: {
            WindowUpdateTypeEventBounds(realWidId, oldA11yWindows_);
            break;
            }
        case WindowUpdateType::WINDOW_UPDATE_FOCUSED: {
            WindowUpdateTypeEventFocused(realWidId, oldA11yWindows_);
            break;
            }
        case WindowUpdateType::WINDOW_UPDATE_PROPERTY: {
            WindowUpdateTypeEventProperty(realWidId, oldA11yWindows_);
            break;
            }
        case WindowUpdateType::WINDOW_UPDATE_LAYER: {
            WindowUpdateTypeEventLayer(realWidId, oldA11yWindows_);
            break;
        }
        default:
            break;
        }
}

void MockWindowManager::WindowUpdateAll(const std::vector<sptr<Rosen::AccessibilityWindowInfo>>& infos)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    auto oldA11yWindows_ = a11yWindows_;
    int32_t oldActiveWindowId = activeWindowId_;
    HILOG_INFO("WindowUpdateAll start activeWindowId_: %{public}d", activeWindowId_);
    WinDeInit();
    for (auto &window : infos) {
        if (window == nullptr) {
            HILOG_ERROR("window is nullptr");
            continue;
        }
        int32_t realWid = GetRealWindowId(window);
        HILOG_DEBUG("windowInfo wid: %{public}d, innerWid: %{public}d, focused: %{public}d",
            window->wid_, window->innerWid_, window->focused_);
        if (!a11yWindows_.count(realWid)) {
            auto a11yWindowInfo = CreateAccessibilityWindowInfo(window);
            a11yWindows_.emplace(realWid, a11yWindowInfo);
            HILOG_DEBUG("a11yWindowInfo bundleName(%{public}s)", a11yWindowInfo.GetBundleName().c_str());
        }
        if (IsSceneBoard(window)) {
            subWindows_.insert(realWid);
            sceneBoardElementIdMap_.InsertPair(realWid, window->uiNodeId_);
        }

        // IsScenePanel for recent-task window
        if (window->focused_ || IsScenePanel(window)) {
            SetActiveWindow(realWid);
        }

        WindowUpdateAllExec(oldA11yWindows_, realWid, window);
    }

    for (auto it = oldA11yWindows_.begin(); it != oldA11yWindows_.end(); ++it) {
        WindowUpdateTypeEvent(it->first, oldA11yWindows_, WINDOW_UPDATE_REMOVED);
    }
    HILOG_INFO("WindowUpdateAll end activeWindowId_: %{public}d", activeWindowId_);
}

void MockWindowManager::WindowUpdateAllExec(std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_,
    int32_t realWid, const sptr<Rosen::AccessibilityWindowInfo>& window)
{
    if (!oldA11yWindows_.count(realWid)) {
        WindowUpdateTypeEvent(realWid, oldA11yWindows_, WINDOW_UPDATE_ADDED);
    } else {
        if (EqualFocus(oldA11yWindows_[realWid], window)) {
            WindowUpdateTypeEvent(realWid, oldA11yWindows_, WINDOW_UPDATE_FOCUSED);
        }
        if (EqualBound(oldA11yWindows_[realWid], window)) {
            WindowUpdateTypeEvent(realWid, oldA11yWindows_, WINDOW_UPDATE_BOUNDS);
        }
        if (EqualProperty(oldA11yWindows_[realWid], window)) {
            WindowUpdateTypeEvent(realWid, oldA11yWindows_, WINDOW_UPDATE_PROPERTY);
        }
        if (EqualLayer(oldA11yWindows_[realWid], window)) {
            WindowUpdateTypeEvent(realWid, oldA11yWindows_, WINDOW_UPDATE_LAYER);
        }
        auto itr = oldA11yWindows_.find(realWid);
        if (itr != oldA11yWindows_.end()) {
            oldA11yWindows_.erase(itr);
        }
    }
}

void MockWindowManager::ClearOldActiveWindow()
{
    HILOG_DEBUG("active window id is %{public}d", activeWindowId_);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::ClearAccessibilityFocused()
{
    HILOG_DEBUG("a11yFocused window id is %{public}d", a11yFocusedWindowId_);
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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
    AccessibilityEventInfoParcel eventInfoParcel(eventInfo);
    bool isSendWindowEvent = CheckEvents();
    if (isSendWindowEvent) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().SendEvent(eventInfoParcel, 0);
    }
}

int64_t MockWindowManager::GetSceneBoardElementId(const int32_t windowId, const int64_t elementId)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::GetRealWindowAndElementId(int32_t& windowId, int64_t& elementId)
{
    // sceneboard window id, element id is not equal -1
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::GetSceneBoardInnerWinId(int32_t windowId, int64_t elementId,
    int32_t& innerWid)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
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

void MockWindowManager::SceneBoardElementIdMap::InsertPair(const int32_t windowId, const int64_t elementId)
{
    std::lock_guard<ffrt::mutex> lock(mapMutex_);
    windowElementMap_[windowId] = elementId;
}

void MockWindowManager::SceneBoardElementIdMap::RemovePair(const int32_t windowId)
{
    std::lock_guard<ffrt::mutex> lock(mapMutex_);
    windowElementMap_.erase(windowId);
}

bool MockWindowManager::SceneBoardElementIdMap::CheckWindowIdPair(const int32_t windowId)
{
    std::lock_guard<ffrt::mutex> lock(mapMutex_);
    return windowElementMap_.count(windowId);
}

void MockWindowManager::SceneBoardElementIdMap::Clear()
{
    std::lock_guard<ffrt::mutex> lock(mapMutex_);
    windowElementMap_.clear();
}

std::map<int32_t, int64_t> MockWindowManager::SceneBoardElementIdMap::GetAllPairs()
{
    std::lock_guard<ffrt::mutex> lock(mapMutex_);
    return windowElementMap_;
}

std::string MockWindowManager::GetA11yWindowsBundleName(int32_t windowId, std::string bundleName)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    for (auto iter = a11yWindows_.begin(); iter != a11yWindows_.end(); iter++) {
        if (iter->first == windowId) {
            AccessibilityWindowInfo tempWindowInfo = iter->second;
            HILOG_DEBUG("GetA11yWindowsBundleName windowId:[%{public}d], BundleName:[%{public}s]",
                windowId, tempWindowInfo.GetBundleName().c_str());
            bundleName = tempWindowInfo.GetBundleName();
            break;
        }
    }
    return bundleName;
}

void MockWindowManager::SetEventInfoBundleName(const AccessibilityEventInfo &uiEvent)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    std::string windowsBundleNameCache = "";
    windowsBundleNameCache = GetA11yWindowsBundleName(uiEvent.GetWindowId(), windowsBundleNameCache);
    if (windowsBundleNameCache != "") {
        const_cast<AccessibilityEventInfo&>(uiEvent).SetBundleName(windowsBundleNameCache);
        return;
    }

    std::vector<AccessibilityWindowInfo> windowsInfo = GetAccessibilityWindows();
    if (windowsInfo.empty()) {
        HILOG_DEBUG("GetAccessibilityWindows is empty");
        return;
    }
    for (auto &window : windowsInfo) {
        const std::string currentBundleName = window.GetBundleName();
        int32_t currentWid = window.GetWindowId();
        if (currentBundleName != "" && uiEvent.GetWindowId() == currentWid) {
            const_cast<AccessibilityEventInfo&>(uiEvent).SetBundleName(currentBundleName);
            HILOG_DEBUG("GetAccessibilityWindows windowId:[%{public}d], BundleName:[%{public}s]",
                currentWid, currentBundleName.c_str());
            break;
        }
    }
}

void MockWindowManager::SetEventInfoBundleNameOld(const AccessibilityEventInfo &uiEvent,
    const int32_t windowId, std::map<int32_t, AccessibilityWindowInfo> &oldA11yWindows_)
{
    std::lock_guard<ffrt::recursive_mutex> lock(interfaceMutex_);
    std::string bundNameCache = "";
    if (oldA11yWindows_.count(windowId)) {
        bundNameCache = oldA11yWindows_[windowId].GetBundleName();
        HILOG_DEBUG("SetEventInfoBundleNameOld windowId:[%{public}d], BundleName:[%{public}s]",
            windowId, bundNameCache.c_str());
        const_cast<AccessibilityEventInfo&>(uiEvent).SetBundleName(bundNameCache);
        return;
    }
    SetEventInfoBundleName(uiEvent);
}

RetError MockWindowManager::GetFocusedWindowId(int32_t &focusedWindowId)
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    HITRACE_METER_NAME(HITRACE_TAG_ACCESSIBILITY_MANAGER, "QueryFocusedWindowInfo");
#endif // OHOS_BUILD_ENABLE_HITRACE
    Rosen::FocusChangeInfo focusedWindowInfo;
    OHOS::Rosen::WindowManager::GetInstance().GetFocusWindowInfo(focusedWindowInfo);
    if (focusedWindowInfo.windowId_ == INVALID_WINDOW_ID) {
        return RET_ERR_INVALID_PARAM;
    }
    focusedWindowId = focusedWindowInfo.windowId_;
    return RET_OK;
}

bool MockWindowManager::IsInnerWindowRootElement(int64_t elementId)
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

void MockWindowManager::InsertTreeIdWindowIdPair(int32_t treeId, int32_t windowId)
{
    windowTreeIdMap_.EnsureInsert(treeId, windowId);
}

void MockWindowManager::RemoveTreeIdWindowIdPair(int32_t treeId)
{
    windowTreeIdMap_.Erase(treeId);
}

int32_t MockWindowManager::FindTreeIdWindowIdPair(int32_t treeId)
{
    return windowTreeIdMap_.ReadVal(treeId);
}

MockConnection::MockConnection(int32_t accountId, int32_t connectionId,
    AccessibilityAbilityInfo &abilityInfo)
    : accountId_(accountId), connectionId_(connectionId), abilityInfo_(abilityInfo)
{
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetMainRunner());
}

MockConnection::~MockConnection()
{
    HILOG_DEBUG();
    if (abilityClient_ && abilityClient_->AsObject()) {
        abilityClient_->AsObject()->RemoveDeathRecipient(deathRecipient_);
        abilityClient_ = nullptr;
    }
}

void MockConnection::HandleNoEventHandler(const AppExecFwk::ElementName &element)
{
    HILOG_ERROR("eventHandler_ is nullptr.");
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
    if (accountData != nullptr) {
        accountData->RemoveConnectingA11yAbility(Utils::GetUri(element));
    }
}

void MockConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_INFO("ResultCode is %{public}d", resultCode);
    if (!eventHandler_) {
        HandleNoEventHandler(element);
        return;
    }

    int32_t accountId = accountId_;
    eventHandler_->PostTask([accountId, element, remoteObject, resultCode]() {
#ifdef OHOS_BUILD_ENABLE_HITRACE
        FinishAsyncTrace(HITRACE_TAG_ACCESSIBILITY_MANAGER, "AccessibleAbilityConnect",
            static_cast<int32_t>(TraceTaskId::ACCESSIBLE_ABILITY_CONNECT));
#endif // OHOS_BUILD_ENABLE_HITRACE
        std::string bundleName = element.GetBundleName();
        std::string abilityName = element.GetAbilityName();
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
        if (!accountData) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            HILOG_ERROR("accountData is nullptr.");
            return;
        }

        if (resultCode != NO_ERROR) {
            accountData->RemoveEnabledAbility(Utils::GetUri(element));
            accountData->RemoveConnectingA11yAbility(Utils::GetUri(element));
            accountData->UpdateAbilities();
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            return;
        }

        if (!remoteObject) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            HILOG_ERROR("MockConnection::OnAbilityConnectDone get remoteObject failed");
            return;
        }

        sptr<MockConnection> connection = accountData->GetConnectingA11yAbility(Utils::GetUri(element));
        if (!connection) {
            HILOG_ERROR("connection is nullptr");
            return;
        }

        accountData->AddConnectedAbility(connection);
        accountData->RemoveConnectingA11yAbility(Utils::GetUri(element));
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
        connection->InitAbilityClient(remoteObject);
        accountData->UpdateEnableAbilityListsState();
        }, "OnAbilityConnectDone");
}

void MockConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    HILOG_INFO("resultCode[%{public}d]", resultCode);
    if (resultCode == NO_ERROR) {
        return;
    }
    // when calling ConnectAbility function, OnAbilityDisconnectDone could be called when ability crashed
    // and OnAbilityConnectDone won't be called. At this time, ConnectingA11yAbility should be cleaned up
    if (!eventHandler_) {
        HILOG_ERROR("eventHanlder_ is nullptr");
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
        if (accountData != nullptr) {
            accountData->RemoveConnectingA11yAbility(Utils::GetUri(element));
        }
        return;
    }
    
    int32_t accountId = accountId_;
    eventHandler_->PostTask([accountId, element]() {
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
        if (accountData != nullptr) {
            accountData->RemoveConnectingA11yAbility(Utils::GetUri(element));
        }
        }, "OnAbilityDisconnectDone");
}

void MockConnection::OnAccessibilityEvent(AccessibilityEventInfo &eventInfo)
{
    HILOG_DEBUG();
    if (!abilityClient_) {
        HILOG_ERROR("OnAccessibilityEvent​ failed");
        return;
    }

    std::vector<std::string> filterBundleNames = abilityInfo_.GetFilterBundleNames();
    if (IsWantedEvent(eventInfo.GetEventType()) && (filterBundleNames.empty() || find(filterBundleNames.begin(),
        filterBundleNames.end(), eventInfo.GetBundleName()) != filterBundleNames.end())) {
        abilityClient_->OnAccessibilityEvent(eventInfo);
        HILOG_DEBUG("windowId[%{public}d] evtType[%{public}d] windowChangeType[%{public}d] GestureId[%{public}d]",
            eventInfo.GetWindowId(), eventInfo.GetEventType(), eventInfo.GetWindowChangeTypes(),
            eventInfo.GetGestureType());
    }
}

bool MockConnection::OnKeyPressEvent(const MMI::KeyEvent &keyEvent, const int32_t sequence)
{
    if (!abilityClient_) {
        HILOG_ERROR("OnKeyPressEvent failed");
        return false;
    }

    if (!(abilityInfo_.GetCapabilityValues() & Capability::CAPABILITY_KEY_EVENT_OBSERVER)) {
        HILOG_ERROR("Capability failed");
        return false;
    }

    abilityClient_->OnKeyPressEvent(keyEvent, sequence);
    return true;
}

void MockConnection::SetAbilityInfoTargetBundleName(const std::vector<std::string> &targetBundleNames)
{
    HILOG_DEBUG();
    abilityInfo_.SetFilterBundleNames(targetBundleNames);
}

bool MockConnection::IsWantedEvent(int32_t eventType)
{
    uint32_t type = static_cast<uint32_t>(eventType);
    if ((type & abilityInfo_.GetEventTypes()) != type) {
        HILOG_DEBUG("EventType(%{public}d) is not wanted", type);
        return false;
    }
    return true;
}

AAFwk::Want CreateWant(AppExecFwk::ElementName& element)
{
    AAFwk::Want want;
    want.SetElement(element);

    return want;
}

void MockConnection::Disconnect()
{
    HILOG_DEBUG();

    if (accountId_ == Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId()) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    }

    if (!abilityClient_) {
        HILOG_ERROR("abilityClient is nullptr");
        return;
    }
    abilityClient_->Disconnect(connectionId_);

    if (isRegisterDisconnectCallback_) {
        int32_t accountId = accountId_;
        std::string clientName = Utils::GetUri(elementName_);
        if (!eventHandler_) {
            HILOG_ERROR("eventHanlder_ is nullptr");
            auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
            if (accountData != nullptr) {
                accountData->RemoveConnectingA11yAbility(clientName);
            }
            return;
        }
        eventHandler_->PostTask([this, accountId, clientName]() {
            DisconnectAbility();
            auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId);
            accountData->RemoveWaitDisconnectAbility(clientName);
            }, "DISCONNECT_" + elementName_.GetBundleName(), WAIT_NOTIFY_DISCONNECT_TIMEOUT);
    } else {
        DisconnectAbility();
    }
}

void MockConnection::DisconnectAbility()
{
    HILOG_INFO();
    if (deathRecipient_ == nullptr || !abilityClient_->AsObject() ||
        !abilityClient_->AsObject()->RemoveDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }

    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (!abilityManagerClient) {
        HILOG_ERROR("abilityManagerClient is nullptr");
        return;
    }
    if (abilityManagerClient->DisconnectAbility(this) != ERR_OK) {
        HILOG_ERROR("Disconnect failed!");
        return;
    }
}

void MockConnection::NotifyDisconnect()
{
    HILOG_INFO();
    if (!eventHandler_) {
        HILOG_ERROR("eventHanlder_ is nullptr");
        auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
        if (accountData != nullptr) {
            accountData->RemoveConnectingA11yAbility(Utils::GetUri(elementName_));
        }
        return;
    }
    eventHandler_->RemoveTask("DISCONNECT_" + elementName_.GetBundleName());
    DisconnectAbility();
}

bool MockConnection::Connect(const AppExecFwk::ElementName &element)
{
    HILOG_DEBUG();
#ifdef OHOS_BUILD_ENABLE_HITRACE
    StartAsyncTrace(HITRACE_TAG_ACCESSIBILITY_MANAGER, "AccessibleAbilityConnect",
        static_cast<int32_t>(TraceTaskId::ACCESSIBLE_ABILITY_CONNECT));
#endif // OHOS_BUILD_ENABLE_HITRACE
    std::string bundleName = element.GetBundleName();
    std::string abilityName = element.GetAbilityName();

    elementName_ = element;
    HILOG_DEBUG("bundleName[%{public}s], abilityName [%{public}s], accountId [%{public}d]",
        bundleName.c_str(), abilityName.c_str(), accountId_);

    int uid = Singleton<AccessibilityResourceBundleManager>::GetInstance().GetUidByBundleName(
        bundleName, abilityName, accountId_);
    HILOG_DEBUG("uid is %{public}d ", uid);

    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (!abilityManagerClient) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("abilityManagerClient is nullptr");
        return false;
    }
    AAFwk::Want want = CreateWant(elementName_);
    if (abilityManagerClient->ConnectAbility(want, this, nullptr, uid / UID_MASK) != ERR_OK) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("ConnectAbility failed!");
        return false;
    }
    return true;
}

int32_t MockConnection::GetChannelId()
{
    HILOG_DEBUG("connectionId_ is %{public}d", connectionId_);
    return connectionId_;
}

void MockConnection::OnAbilityConnectDoneSync(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject)
{
    HILOG_DEBUG();
    auto accountData = Singleton<AccessibleAbilityManagerService>::GetInstance().GetAccountData(accountId_);
    if (!accountData) {
        HILOG_ERROR("accountData is nullptr.");
        return;
    }
    if (!remoteObject) {
        HILOG_ERROR("MockConnection::OnAbilityConnectDone get remoteObject failed");
        return;
    }
    elementName_ = element;

    sptr<MockConnection> pointer = this;
    accountData->AddConnectedAbility(pointer);
    Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    InitAbilityClient(remoteObject);
}

void MockConnection::OnAbilityDisconnectDoneSync(const AppExecFwk::ElementName &element)
{
    HILOG_DEBUG();

    if (accountId_ == Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountId()) {
        Singleton<AccessibleAbilityManagerService>::GetInstance().UpdateAccessibilityManagerService();
    }

    if (!abilityClient_) {
        HILOG_ERROR("abilityClient_ is nullptr");
        return;
    }
    abilityClient_->Disconnect(connectionId_);
}

void MockConnection::InitAbilityClient(const sptr<IRemoteObject> &remoteObject)
{
    std::string bundleName = elementName_.GetBundleName();
    std::string abilityName = elementName_.GetAbilityName();
    abilityClient_ = new(std::nothrow) AccessibleAbilityClientProxy(remoteObject);
    if (!abilityClient_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("abilityClient_ is null");
        return;
    }

    if (!deathRecipient_) {
        deathRecipient_ = new(std::nothrow) MockConnectionDeathRecipient(
            accountId_, elementName_, eventHandler_);
        if (!deathRecipient_) {
            Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
                A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
            HILOG_ERROR("deathRecipient_ is null");
            return;
        }
    }

    if (!abilityClient_->AsObject() || !abilityClient_->AsObject()->AddDeathRecipient(deathRecipient_)) {
        HILOG_ERROR("Failed to add death recipient");
    }

    channel_ = new(std::nothrow) AccessibleAbilityChannel(accountId_, abilityInfo_.GetId());
    if (!channel_) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
            A11yError::ERROR_CONNECT_A11Y_APPLICATION_FAILED, bundleName, abilityName);
        HILOG_ERROR("channel_ is null");
        return;
    }
    abilityClient_->Init(channel_, connectionId_);
}

void MockConnection::MockConnectionDeathRecipient::OnRemoteDied(
    const wptr<IRemoteObject> &remote)
{
    Utils::RecordUnavailableEvent(A11yUnavailableEvent::CONNECT_EVENT,
        A11yError::ERROR_A11Y_APPLICATION_DISCONNECT_ABNORMALLY,
        recipientElementName_.GetBundleName(), recipientElementName_.GetAbilityName());
    if (!handler_) {
        HILOG_ERROR("handler_ is nullptr");
        return;
    }
    std::shared_ptr<AppExecFwk::ElementName> sharedElementName =
        std::make_shared<AppExecFwk::ElementName>(recipientElementName_);
    
    if (sharedElementName == nullptr) {
        HILOG_ERROR("sharedElementName is nullptr");
        return;
    }
    int32_t accountId = accountId_;
    handler_->PostTask([accountId, sharedElementName]() {
        HILOG_DEBUG();

        auto &aams = Singleton<AccessibleAbilityManagerService>::GetInstance();
        auto accountData = aams.GetAccountData(accountId);
        if (!accountData) {
            HILOG_ERROR("accountData is null.");
            return;
        }

        std::string uri = Utils::GetUri(*sharedElementName);
        sptr<MockConnection> connection = accountData->GetMockConnection(uri);
        if (!connection) {
            HILOG_ERROR("There is no connection for %{public}s.", uri.c_str());
            return;
        }
        accountData->RemoveConnectedAbility(*sharedElementName);
        accountData->RemoveEnabledAbility(uri);

        // the extension ability connected to aams by inner api
        if (sharedElementName->GetBundleName() == sharedElementName->GetAbilityName()) {
            accountData->RemoveInstalledAbility(sharedElementName->GetBundleName());
        }

#ifdef OHOS_BUILD_ENABLE_POWER_MANAGER
    std::string bundleName = sharedElementName->GetBundleName();
    auto &powerManager = Singleton<AccessibilityPowerManager>::GetInstance();
    if (!powerManager.UnholdRunningLock(bundleName)) {
        HILOG_DEBUG("OnRemoteDied UnholdRunningLock failed.");
    }
#endif
        accountData->UpdateAbilities();
        aams.UpdateAccessibilityManagerService();
        }, "OnRemoteDied");
}

bool MockConnection::GetIsRegisterDisconnectCallback() const
{
    return isRegisterDisconnectCallback_;
}

void MockConnection::SetIsRegisterDisconnectCallback(bool isRegister)
{
    isRegisterDisconnectCallback_ = isRegister;
}

MockAAMSEventHandler::MockAAMSEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{
}

MockAAMSEventHandler::~MockAAMSEventHandler()
{
}

void MockAAMSEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!event) {
        HILOG_ERROR("event is null");
        return;
    }
    HILOG_DEBUG("the eventId is %{public}d", event->GetInnerEventId());
}

void ExtensionAbilityConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    HILOG_INFO("on ability connected done");
    if (resultCode != ERR_OK) {
        HILOG_ERROR("ability connect failed: %{public}d", resultCode);
        return;
    }
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is null");
        return;
    }
    HILOG_INFO("got remoteObject");
    std::unique_lock<std::mutex> lock(mutex_);
    remoteObject_ = remoteObject;
    cv_.notify_all();
}

void ExtensionAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOG_INFO("on ability disconnected done");
    std::unique_lock<std::mutex> lock(mutex_);
    remoteObject_ = nullptr;
}

sptr<IRemoteObject> ExtensionAbilityConnection::GetRemoteObject()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.wait_for(lock, std::chrono::milliseconds(WAIT_INTERVAL), [this]() { return remoteObject_ != nullptr; })) {
        HILOG_ERROR("wait for remote object timeout");
        return nullptr;
    }
    return remoteObject_;
}

MockExtensionAbilityManager::MockExtensionAbilityManager()
{
    connection_ = sptr<ExtensionAbilityConnection> (new (std::nothrow) ExtensionAbilityConnection());
}

MockExtensionAbilityManager& MockExtensionAbilityManager::GetInstance()
{
    static MockExtensionAbilityManager instance;
    return instance;
}

void MockExtensionAbilityManager::VoiceRecognize(int32_t soundType)
{
    HILOG_INFO("called, soundType=%{public}d", soundType);
    sptr<IRemoteObject> remote = GetRemoteObject();
    if (remote == nullptr) {
        HILOG_ERROR("cannot get remote object");
        return;
    }
    std::string message = CreateJsonMessage(soundType);
    SendRequestToSetting(remote, message);
    DisconnectSettingsExtService();
}

int32_t MockExtensionAbilityManager::ConnectSettingsExtService()
{
    HILOG_INFO("called");
    AAFwk::Want want;
    want.SetElementName(SETTINGS_BUNDLE_NAME, SERVICE_EXTENSION_ABILITY_NAME);
    int32_t ret = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want, connection_,
        nullptr, DEFAULT_USER_ID);
    return ret;
}

void MockExtensionAbilityManager::DisconnectSettingsExtService()
{
    auto disconnectResult = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(connection_);
    if (disconnectResult != ERR_OK) {
        HILOG_ERROR("disconnect failed, ret=%{public}d", disconnectResult);
    }
}

sptr<IRemoteObject> MockExtensionAbilityManager::GetRemoteObject()
{
    int32_t connectResult = ConnectSettingsExtService();
    if (connectResult != ERR_OK) {
        HILOG_ERROR("connect failed, ret=%{public}d", connectResult);
        return nullptr;
    }
    HILOG_INFO("connect success");
    return connection_->GetRemoteObject();
}

std::string MockExtensionAbilityManager::CreateJsonMessage(int32_t soundType)
{
    nlohmann::json jsonMessageObj;
    nlohmann::json jsonExtraValue;

    jsonExtraValue[EXTRA_VALUE_KEY] = soundType;
    jsonMessageObj[METHOD_KEY] = METHOD_VOICE_RECOGNITION;
    jsonMessageObj[EXTRA_KEY] = jsonExtraValue;

    return jsonMessageObj.dump();
}

void MockExtensionAbilityManager::SendRequestToSetting(const sptr<IRemoteObject> &remoteObject,
    const std::string &message)
{
    if (remoteObject == nullptr) {
        HILOG_ERROR("remoteObject is nullptr!");
        return;
    }
    if (message.empty()) {
        HILOG_ERROR("send message is empty!");
        return;
    }
    HILOG_INFO("send message=%{public}s", message.c_str());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteString16(Str8ToStr16(message))) {
        HILOG_ERROR("write message failed!");
        return;
    }
    int32_t ret = remoteObject->SendRequest(SETTINGS_VOICE_RECOGNITION_CODE, data, reply, option);
    auto replyCode = reply.ReadInt32();
    HILOG_INFO("sendRequest ret=%{public}d, replyCode=%{public}d", ret, replyCode);
}

void MockFullScreenMagnificationManager::ShowMagnification()
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->ShowMagnification(FULL_SCREEN_MAGNIFICATION);
}

void MockFullScreenMagnificationManager::ShowMagnificationWithPosition(PointerPos pos)
{
    if (pos.posX == 0 && pos.posY == 0) {
        ShowMagnification();
    } else {
        EnableMagnification(pos.posX, pos.posY);
    }
}

void MockFullScreenMagnificationManager::DisableMagnification(bool needClear)
{
    HILOG_INFO();
    CHECK_PROXY_PTR_VOID()
    std::lock_guard<ffrt::mutex> lock(mutex_);
    windowProxy_->DisableMagnification(FULL_SCREEN_MAGNIFICATION, needClear);
}

void MockFullScreenMagnificationManager::SetScale(float scaleSpan)
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->SetScale(FULL_SCREEN_MAGNIFICATION, scaleSpan);
}

void MockFullScreenMagnificationManager::MoveMagnification(int32_t deltaX, int32_t deltaY)
{
    CHECK_PROXY_PTR_VOID()
    windowProxy_->MoveMagnification(FULL_SCREEN_MAGNIFICATION, deltaX, deltaY);
}

void MockFullScreenMagnificationManager::PersistScale()
{
    CHECK_PROXY_PTR_VOID()
    float scale = windowProxy_->GetScale();
    HILOG_DEBUG("scale = %{public}f", scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().SetMagnificationScale(scale);
    Singleton<AccessibleAbilityManagerService>::GetInstance().AnnouncedForMagnification(
        AnnounceType::ANNOUNCE_MAGNIFICATION_SCALE);
}

void MockFullScreenMagnificationManager::RefreshWindowParam()
{
    HILOG_DEBUG();
    CHECK_PROXY_PTR_VOID()
    windowProxy_->RefreshWindowParam(FULL_SCREEN_MAGNIFICATION);
}

PointerPos MockFullScreenMagnificationManager::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->ConvertCoordinates(posX, posY);
}

PointerPos MockFullScreenMagnificationManager::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return coordinates;
    }
    return windowProxy_->ConvertGesture(type, coordinates);
}

uint32_t MockFullScreenMagnificationManager::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return INVALID_GESTURE_TYPE;
    }
    return windowProxy_->CheckTapOnHotArea(posX, posY);
}

PointerPos MockFullScreenMagnificationManager::GetSourceCenter()
{
    PointerPos pos = {0, 0};
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return pos;
    }
    return windowProxy_->GetSourceCenter();
}

bool MockFullScreenMagnificationManager::IsMagnificationWindowShow()
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return false;
    }
    return windowProxy_->IsMagnificationWindowShow(FULL_SCREEN_MAGNIFICATION);
}

float MockFullScreenMagnificationManager::GetScale()
{
    if (windowProxy_ == nullptr) {
        HILOG_ERROR("windowProxy_ is nullptr.");
        return DEFAULT_SCALE;
    }
    return windowProxy_->GetScale();
}
} // namespace Accessibility
} // namespace OHOS
