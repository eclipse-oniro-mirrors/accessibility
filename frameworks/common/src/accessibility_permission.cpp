/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "accessibility_permission.h"
#include <accesstoken_kit.h>
#include <ipc_skeleton.h>
#include "hilog_wrapper.h"
#include "tokenid_kit.h"

// LCOV_EXCL_START
namespace OHOS {
namespace Accessibility {
bool Permission::CheckCallingPermission(const std::string& permission)
{
    HILOG_DEBUG("permission %{public}s", permission.c_str());
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission) !=
        Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        HILOG_DEBUG("permission denied");
        return false;
    }

    HILOG_DEBUG("permission ok!");
    return true;
}

bool Permission::IsStartByHdcd()
{
    auto id = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::NativeTokenInfo info;
    auto ret = Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(id, info);
    if (ret != 0) {
        HILOG_DEBUG("get native token info failed %{public}d", ret);
        return false;
    }

    if (info.processName.compare("hdcd") == 0) {
        HILOG_DEBUG("process name is hdcd");
        return true;
    }

    return false;
}

bool Permission::IsSystemApp()
{
    HILOG_DEBUG();

    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType !=  Security::AccessToken::TOKEN_HAP) {
        HILOG_INFO("Caller is not a application.");
        return true;
    }
    uint64_t accessTokenId = IPCSkeleton::GetCallingFullTokenID();
    bool isSystemApplication = Security::AccessToken::AccessTokenKit::IsSystemAppByFullTokenID(accessTokenId);
    return isSystemApplication;
}

bool Permission::CheckPermission(const std::string &permission)
{
    HILOG_DEBUG();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    int result = Security::AccessToken::TypePermissionState::PERMISSION_GRANTED;
    Security::AccessToken::ATokenTypeEnum tokenType =
        Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType == Security::AccessToken::TOKEN_INVALID) {
        HILOG_WARN("AccessToken type invalid!");
        return false;
    } else {
        result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken, permission);
    }
    if (result == Security::AccessToken::TypePermissionState::PERMISSION_DENIED) {
        HILOG_WARN("AccessTokenID denied!");
        return false;
    }
    HILOG_DEBUG("tokenType %{private}d dAccessTokenID:%{private}u, permission:%{private}s matched!",
        tokenType, callerToken, permission.c_str());
    return true;
}
// LCOV_EXCL_STOP
} // namespace Accessibility
} // namespace OHOS