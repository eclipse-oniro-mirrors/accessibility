/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

// LCOV_EXCL_START
#include "magnification_window_proxy.h"
#include "rwlock.h"
#include <dlfcn.h>

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string windowLibName_ = "libaams_ext.z.so";
    std::shared_ptr<MagnificationWindowProxy> g_Instance = nullptr;
    Utils::RWLock rwLock;
}

std::shared_ptr<MagnificationWindowProxy> MagnificationWindowProxy::GetInstance()
{
    Utils::UniqueWriteGuard<Utils::RWLock> wLock(rwLock);
    if (g_Instance == nullptr) {
        g_Instance = std::make_shared<MagnificationWindowProxy>();
    }
    return g_Instance;
}

MagnificationWindowProxy::MagnificationWindowProxy()
{
    handle_ = dlopen(windowLibName_.c_str(), RTLD_LAZY | RTLD_NOW);
    if (!handle_) {
        HILOG_ERROR("dlopen error: %{public}s", dlerror());
    }
}

MagnificationWindowProxy::~MagnificationWindowProxy()
{
    Utils::UniqueWriteGuard<Utils::RWLock> wLock(rwLock);
    if (!handle_) {
        return;
    }
 
    int ret = dlclose(handle_);
    HILOG_DEBUG("dlclose ret: %{public}d", ret);
    char* error = dlerror();
    if (error) {
        HILOG_ERROR("dlclose error: %{public}s", error);
    }
    handle_ = nullptr;
}

void* MagnificationWindowProxy::GetFunc(const std::string& funcName)
{
    HILOG_DEBUG("Get func start, %{public}s", funcName.c_str());
    void* func = dlsym(handle_, funcName.c_str());
    char* error = dlerror();
    if (error != nullptr) {
        HILOG_ERROR(" %{public}s", error);
        return nullptr;
    }
 
    HILOG_DEBUG("Get func end, %{public}s", funcName.c_str());
    return func;
}

// common
void MagnificationWindowProxy::EnableMagnification(uint32_t magnificationType, int32_t posX, int32_t posY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using Enabel = void (*)(uint32_t magnificationType, int32_t posX, int32_t posY);
    Enabel enabelFun = (Enabel)GetFunc("EnableMagnification");
    if (!enabelFun) {
        HILOG_ERROR("EnableMagnification func is null");
        return;
    }
    enabelFun(magnificationType, posX, posY);
}

void MagnificationWindowProxy::DisableMagnification(uint32_t magnificationType, bool needClear)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using Disable = void (*)(uint32_t magnificationType, bool needClear);
    Disable disableFun = (Disable)GetFunc("DisableMagnification");
    if (!disableFun) {
        HILOG_ERROR("DisableMagnification func is null");
        return;
    }
    disableFun(magnificationType, needClear);
}

void MagnificationWindowProxy::SetScale(uint32_t magnificationType, float scaleSpan)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using SetScale = void (*)(uint32_t magnificationType, float scaleSpan);
    SetScale setScaleFun = (SetScale)GetFunc("SetScale");
    if (!setScaleFun) {
        HILOG_ERROR("SetScale func is null");
        return;
    }
    setScaleFun(magnificationType, scaleSpan);
}

void MagnificationWindowProxy::MoveMagnification(uint32_t magnificationType, int32_t deltaX, int32_t deltaY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using MoveMagnification = void (*)(uint32_t magnificationType, int32_t deltaX, int32_t deltaY);
    MoveMagnification moveFun = (MoveMagnification)GetFunc("MoveMagnification");
    if (!moveFun) {
        HILOG_ERROR("MoveMagnification func is null");
        return;
    }
    moveFun(magnificationType, deltaX, deltaY);
}

PointerPos MagnificationWindowProxy::GetSourceCenter()
{
    PointerPos pos = {0, 0};
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return pos;
    }
 
    using GetSourceCenter = PointerPos (*)();
    GetSourceCenter getSourceCenterFun = (GetSourceCenter)GetFunc("GetSourceCenter");
    if (!getSourceCenterFun) {
        HILOG_ERROR("GetSourceCenter func is null");
        return pos;
    }
    return getSourceCenterFun();
}

void MagnificationWindowProxy::FollowFocuseElement(uint32_t magnificationType, int32_t centerX, int32_t centerY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using FollowFocuseElement = void (*)(uint32_t magnificationType, int32_t deltaX, int32_t deltaY);
    FollowFocuseElement followFocuseElementFun = (FollowFocuseElement)GetFunc("FollowFocuseElement");
    if (!followFocuseElementFun) {
        HILOG_ERROR("FollowFocuseElement func is null");
        return;
    }
    followFocuseElementFun(magnificationType, centerX, centerY);
}

void MagnificationWindowProxy::InitMagnificationParam(float scale)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using InitMagnificationParam = void (*)(float scale);
    InitMagnificationParam initParamFun = (InitMagnificationParam)GetFunc("InitMagnificationParam");
    if (!initParamFun) {
        HILOG_ERROR("InitMagnificationParam func is null");
        return;
    }
    initParamFun(scale);
}

float MagnificationWindowProxy::GetScale()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return DEFAULT_SCALE;
    }
 
    using GetScale = float (*)();
    GetScale getScaleFun = (GetScale)GetFunc("GetScale");
    if (!getScaleFun) {
        HILOG_ERROR("GetScale func is null");
        return DEFAULT_SCALE;
    }
    return getScaleFun();
}

void MagnificationWindowProxy::ShowMagnification(uint32_t magnificationType)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using ShowMagnification = void (*)(uint32_t magnificationType);
    ShowMagnification showFun = (ShowMagnification)GetFunc("ShowMagnification");
    if (!showFun) {
        HILOG_ERROR("ShowMagnification func is null");
        return;
    }
    showFun(magnificationType);
}

void MagnificationWindowProxy::RefreshWindowParam(uint32_t magnificationType, RotationType type)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using RefreshWindowParam = void (*)(uint32_t magnificationType, RotationType type);
    RefreshWindowParam refreshWindowParamFun = (RefreshWindowParam)GetFunc("RefreshWindowParam");
    if (!refreshWindowParamFun) {
        HILOG_ERROR("RefreshWindowParam func is null");
        return;
    }
    refreshWindowParamFun(magnificationType, type);
}

PointerPos MagnificationWindowProxy::ConvertCoordinates(int32_t posX, int32_t posY)
{
    PointerPos pos = {posX, posY};
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return pos;
    }
 
    using ConvertCoordinates = PointerPos (*)(int32_t posX, int32_t posY);
    ConvertCoordinates convertCoordinatesFun = (ConvertCoordinates)GetFunc("ConvertCoordinates");
    if (!convertCoordinatesFun) {
        HILOG_ERROR("ConvertCoordinates func is null");
        return pos;
    }
    return convertCoordinatesFun(posX, posY);
}

bool MagnificationWindowProxy::IsMagnificationWindowShow(uint32_t magnificationType)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
 
    using IsMagnificationWindowShow = bool (*)(uint32_t magnificationType);
    IsMagnificationWindowShow isShowFun = (IsMagnificationWindowShow)GetFunc("IsMagnificationWindowShow");
    if (!isShowFun) {
        HILOG_ERROR("IsMagnificationWindowShow func is null");
        return false;
    }
    return isShowFun(magnificationType);
}

// full magnification
PointerPos MagnificationWindowProxy::ConvertGesture(uint32_t type, PointerPos coordinates)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return coordinates;
    }
 
    using ConvertGesture = PointerPos (*)(uint32_t type, PointerPos coordinates);
    ConvertGesture convertGestureFun = (ConvertGesture)GetFunc("ConvertGesture");
    if (!convertGestureFun) {
        HILOG_ERROR("ConvertGesture func is null");
        return coordinates;
    }
    return convertGestureFun(type, coordinates);
}

uint32_t MagnificationWindowProxy::CheckTapOnHotArea(int32_t posX, int32_t posY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return INVALID_GESTURE_TYPE;
    }
 
    using CheckTapOnHotArea = uint32_t (*)(int32_t posX, int32_t posY);
    CheckTapOnHotArea checkTapOnHotAreaFun = (CheckTapOnHotArea)GetFunc("CheckTapOnHotArea");
    if (!checkTapOnHotAreaFun) {
        HILOG_ERROR("CheckTapOnHotArea func is null");
        return INVALID_GESTURE_TYPE;
    }
    return checkTapOnHotAreaFun(posX, posY);
}

// window magnification
bool MagnificationWindowProxy::IsTapOnHotArea(int32_t posX, int32_t posY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
 
    using IsTapOnHotArea = bool (*)(int32_t posX, int32_t posY);
    IsTapOnHotArea isTapOnHotAreaFun = (IsTapOnHotArea)GetFunc("IsTapOnHotArea");
    if (!isTapOnHotAreaFun) {
        HILOG_ERROR("IsTapOnHotArea func is null");
        return false;
    }
    return isTapOnHotAreaFun(posX, posY);
}

bool MagnificationWindowProxy::IsTapOnMagnificationWindow(int32_t posX, int32_t posY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
 
    using IsTapOnMagnificationWindow = bool (*)(int32_t posX, int32_t posY);
    IsTapOnMagnificationWindow isTapOnMagnificationWindowFun =
        (IsTapOnMagnificationWindow)GetFunc("IsTapOnMagnificationWindow");
    if (!isTapOnMagnificationWindowFun) {
        HILOG_ERROR("IsTapOnMagnificationWindow func is null");
        return false;
    }
    return isTapOnMagnificationWindowFun(posX, posY);
}

void MagnificationWindowProxy::FixSourceCenter(bool needFix)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using FixSourceCenter = void (*)(bool needFix);
    FixSourceCenter fixSourceCenterFun = (FixSourceCenter)GetFunc("FixSourceCenter");
    if (!fixSourceCenterFun) {
        HILOG_ERROR("FixSourceCenter func is null");
        return;
    }
    fixSourceCenterFun(needFix);
}

// menu
void MagnificationWindowProxy::ShowMenuWindow(uint32_t mode)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using ShowMenuWindow = void (*)(uint32_t mode);
    ShowMenuWindow showMenuFun = (ShowMenuWindow)GetFunc("ShowMenuWindow");
    if (!showMenuFun) {
        HILOG_ERROR("ShowMenuWindow func is null");
        return;
    }
    showMenuFun(mode);
}

void MagnificationWindowProxy::DisableMenuWindow()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using DisableMenuWindow = void (*)();
    DisableMenuWindow disableMenuFun = (DisableMenuWindow)GetFunc("DisableMenuWindow");
    if (!disableMenuFun) {
        HILOG_ERROR("DisableMenuWindow func is null");
        return;
    }
    disableMenuFun();
}

bool MagnificationWindowProxy::IsTapOnMenu(int32_t posX, int32_t posY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
 
    using IsTapOnMenu = bool (*)(int32_t posX, int32_t posY);
    IsTapOnMenu isTapOnMenuFun = (IsTapOnMenu)GetFunc("IsTapOnMenu");
    if (!isTapOnMenuFun) {
        HILOG_ERROR("IsTapOnMenu func is null");
        return false;
    }
    return isTapOnMenuFun(posX, posY);
}

void MagnificationWindowProxy::MoveMenuWindow(int32_t deltaX, int32_t deltaY)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using MoveMenuWindow = void (*)(int32_t deltaX, int32_t deltaY);
    MoveMenuWindow moveMenuFun = (MoveMenuWindow)GetFunc("MoveMenuWindow");
    if (!moveMenuFun) {
        HILOG_ERROR("MoveMenuWindow func is null");
        return;
    }
    moveMenuFun(deltaX, deltaY);
}

void MagnificationWindowProxy::SetCurrentType(uint32_t type)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using SetCurrentType = void (*)(uint32_t type);
    SetCurrentType setTypeFun = (SetCurrentType)GetFunc("SetCurrentType");
    if (!setTypeFun) {
        HILOG_ERROR("SetCurrentType func is null");
        return;
    }
    setTypeFun(type);
}

void MagnificationWindowProxy::AttachToEdge()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using AttachToEdge = void (*)();
    AttachToEdge attachToEdgeFun = (AttachToEdge)GetFunc("AttachToEdge");
    if (!attachToEdgeFun) {
        HILOG_ERROR("AttachToEdge func is null");
        return;
    }
    attachToEdgeFun();
}

void MagnificationWindowProxy::RefreshWindowParamMenu()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using RefreshWindowParamMenu = void (*)();
    RefreshWindowParamMenu refreshMenuParamFun =
        (RefreshWindowParamMenu)GetFunc("RefreshWindowParamMenu");
    if (!refreshMenuParamFun) {
        HILOG_ERROR("RefreshWindowParamMenu func is null");
        return;
    }
    refreshMenuParamFun();
}

uint32_t MagnificationWindowProxy::ChangeMode()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
 
    using ChangeMode = uint32_t (*)();
    ChangeMode changeModeFun = (ChangeMode)GetFunc("ChangeMode");
    if (!changeModeFun) {
        HILOG_ERROR("ChangeMode func is null");
        return 0;
    }
    return changeModeFun();
}

bool MagnificationWindowProxy::IsMenuShown()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return false;
    }
 
    using IsMenuShown = bool (*)();
    IsMenuShown isMenuShownFun = (IsMenuShown)GetFunc("IsMenuShown");
    if (!isMenuShownFun) {
        HILOG_ERROR("IsTapOnMenu func is null");
        return false;
    }
    return isMenuShownFun();
}

int32_t MagnificationWindowProxy::PublishIgnoreRepeatClickReminder()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return -1;
    }
 
    using PublishIgnoreRepeatClickReminderFunc = int32_t (*)();
    PublishIgnoreRepeatClickReminderFunc func =
        (PublishIgnoreRepeatClickReminderFunc)GetFunc("PublishIgnoreRepeatClickReminderFunc");
    if (!func) {
        HILOG_ERROR("PublishIgnoreRepeatClickReminder func is null");
        return -1;
    }
    return func();
}

void MagnificationWindowProxy::CancelNotification()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using CancelNotificationFunc = void (*)();
    CancelNotificationFunc func = (CancelNotificationFunc)GetFunc("CancelNotification");
    if (!func) {
        HILOG_ERROR("CancelNotification func is null");
        return;
    }
    func();
}

int32_t MagnificationWindowProxy::RegisterTimers(uint64_t beginTime)
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return -1;
    }
 
    using RegisterTimersFunc = int32_t (*)(uint64_t beginTime);
    RegisterTimersFunc func = (RegisterTimersFunc)GetFunc("RegisterTimers");
    if (!func) {
        HILOG_ERROR("RegisterTimers func is null");
        return -1;
    }
    return func(beginTime);
}
void MagnificationWindowProxy::DestoryTimers()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return;
    }
 
    using DestoryTimersFunc = void (*)();
    DestoryTimersFunc func = (DestoryTimersFunc)GetFunc("DestoryTimers");
    if (!func) {
        HILOG_ERROR("DestoryTimers func is null");
        return;
    }
    func();
}
int64_t MagnificationWindowProxy::GetWallTimeMs()
{
    if (!handle_) {
        HILOG_ERROR("handle is null");
        return 0;
    }
 
    using GetWallTimeMsFunc = int64_t (*)();
    GetWallTimeMsFunc func = (GetWallTimeMsFunc)GetFunc("GetWallTimeMs");
    if (!func) {
        HILOG_ERROR("GetWallTimeMs func is null");
        return 0;
    }
    return func();
}
} // namespace Accessibility
} // namespace OHOS
// LCOV_EXCL_STOP