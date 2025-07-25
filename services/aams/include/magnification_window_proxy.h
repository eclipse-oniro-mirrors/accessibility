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

#ifndef MAGNIFICATION_WINDOW_PROXY_H
#define MAGNIFICATION_WINDOW_PROXY_H

#include "hilog_wrapper.h"
#include "magnification_def.h"

namespace OHOS {
namespace Accessibility {
#define CHECK_PROXY_PTR_VOID()                    \
    if (windowProxy_ == nullptr) {                \
        HILOG_ERROR("windowProxy_ is nullptr.");  \
        return;                                   \
    }

class MagnificationWindowProxy {
public:
    MagnificationWindowProxy();
    ~MagnificationWindowProxy();

    // common
    void EnableMagnification(uint32_t magnificationType, int32_t centerX, int32_t centerY);
    void DisableMagnification(uint32_t magnificationType, bool needClear = false);
    void SetScale(uint32_t magnificationType, float scaleSpan);
    void MoveMagnification(uint32_t magnificationType, int32_t deltaX, int32_t deltaY);
    PointerPos GetSourceCenter();
    void FollowFocuseElement(uint32_t magnificationType, int32_t centerX, int32_t centerY);
    float GetScale();
    bool IsMagnificationWindowShow(uint32_t magnificationType);
    void ShowMagnification(uint32_t magnificationType);
    void RefreshWindowParam(uint32_t magnificationType);
    PointerPos ConvertCoordinates(int32_t posX, int32_t posY);
    void InitMagnificationParam(float scale);

    // full magnification
    PointerPos ConvertGesture(uint32_t type, PointerPos coordinates);
    uint32_t CheckTapOnHotArea(int32_t posX, int32_t posY);

    // window magnification
    bool IsTapOnHotArea(int32_t posX, int32_t posY);
    bool IsTapOnMagnificationWindow(int32_t posX, int32_t posY);
    void FixSourceCenter(bool needFix);

    // menu
    void ShowMenuWindow(uint32_t mode);
    void DisableMenuWindow();
    bool IsTapOnMenu(int32_t posX, int32_t posY);
    void MoveMenuWindow(int32_t deltaX, int32_t deltaY);
    void SetCurrentType(uint32_t type);
    void AttachToEdge();
    void RefreshWindowParamMenu();
    uint32_t ChangeMode();
    bool IsMenuShown();

private:
    void* GetFunc(const std::string& funcName);
    const std::string windowLibName_ = "libaams_ext.z.so";
    void* handle_ = nullptr;
};
} // namespace Accessibility
} // namespace OHOS
#endif // MAGNIFICATION_WINDOW_PROXY_H