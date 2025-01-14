/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef CJ_ACCESSIBILITY_FFI_H
#define CJ_ACCESSIBILITY_FFI_H
#include <cstdint>
#include "cj_common_ffi.h"
#include "native/ffi_remote_data.h"

namespace OHOS {
namespace Accessibility {
extern "C" {
const int32_t SUCCESS_CODE = 0;
const int32_t ERR_INPUT_INVALID = 401;

struct CArrString {
    char **head;
    int64_t size;
};

struct CAccessibilityAbilityInfo {
    char *id_;
    char *name_;
    char *bundleName_;
    CArrString targetBundleNames_;
    CArrString abilityTypes_;
    CArrString capabilities_;
    char *description_;
    CArrString eventTypes_;
    bool needHide_;
    char *label_;
};

struct CArrAccessibilityAbilityInfo {
    CAccessibilityAbilityInfo *head;
    int64_t size;
};

struct CEventInfo {
    char *type_;
    char *windowUpdateType_;
    char *bundleName_;
    char *componentType_;
    int32_t pageId_;
    char *description_;
    char *triggerAction_;
    char *textMoveUnit_;
    CArrString contents_;
    char *lastContent_;
    int32_t beginIndex_;
    int32_t currentIndex_;
    int32_t endIndex_;
    int32_t itemCount_;
    int64_t elementId_;
    char *textAnnouncedForAccessibility_;
    char *customId_;
};

/* Static Method */
FFI_EXPORT bool FfiAccIsOpenAccessibility(int32_t *errorCode);
FFI_EXPORT bool FfiAccIsOpenTouchGuide(int32_t *errorCode);
FFI_EXPORT bool FfiAccIsScreenReaderOpen(int32_t *errorCode);
FFI_EXPORT CArrAccessibilityAbilityInfo FfiAccGetAccessibilityExtensionList(char *cAbilityType, char *cStateType,
    int32_t *errorCode);
FFI_EXPORT CEventInfo FfiAccEventInfoInit(char *ctype, char *cBundleName, char *cTrigger, int32_t *errorCode);
FFI_EXPORT void FfiAccSendAccessibilityEvent(CEventInfo event, int32_t *errorCode);
FFI_EXPORT void FfiAccOn(char *cbType, void (*callback)(), int32_t *errorCode);
}
} // namespace Accessibility
} // namespace OHOS
#endif // CJ_ACCESSIBILITY_FFI_H