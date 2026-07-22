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

#ifndef ACCESSIBILITY_SHORT_KEY_DIALOG_H
#define ACCESSIBILITY_SHORT_KEY_DIALOG_H

#include "ability_connection.h"
#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "message_parcel.h"
#include "nocopyable.h"
#include <cstdint>
#include "extension_manager_client.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {

enum ShortKeyDialogType {
    FUNCTION_SELECT = 0,
    RECONFIRM = 1,
    READER_EXCLUSIVE = 2,
    ZOOM_GESTURE_CONFLICT = 3
};

class ShortkeyAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ShortkeyAbilityConnection(const std::string commandStr)
    {
        commandStr_ = commandStr;
    }

    virtual ~ShortkeyAbilityConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    std::string GetCommandString();

private:
    std::string commandStr_;
};

class ReConfirmAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ReConfirmAbilityConnection(const std::string commandStr)
    {
        commandStr_ = commandStr;
    }

    virtual ~ReConfirmAbilityConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    std::string GetCommandString();

private:
    std::string commandStr_;
};

class ExclusiveAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ExclusiveAbilityConnection(const std::string commandStr)
    {
        commandStr_ = commandStr;
    }
 
    virtual ~ExclusiveAbilityConnection() = default;
 
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    std::string GetCommandString();
 
private:
    std::string commandStr_;
};

class ZoomGestureConflictAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ZoomGestureConflictAbilityConnection(const std::string &commandStr)
    {
        commandStr_ = commandStr;
    }
 
    virtual ~ZoomGestureConflictAbilityConnection() = default;
 
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    std::string GetCommandString();
 
private:
    std::string commandStr_;
};


class SettingsExtServiceAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit SettingsExtServiceAbilityConnection(bool &enable)
    {
        enable_ = enable;
    }
    virtual ~SettingsExtServiceAbilityConnection() = default;
    static void UpdateSearchItem(bool enable)
    {
        HILOG_DEBUG();
        AAFwk::Want want;
        std::string bundleName = "com.ohos.settings";
        std::string abilityName = "SettingsExtService";
        want.SetElementName(bundleName, abilityName);
        auto settingsExtServiceAbilityConnection = new(std::nothrow) SettingsExtServiceAbilityConnection(enable);
        if (settingsExtServiceAbilityConnection == nullptr) {
            HILOG_ERROR("settingsExtServiceAbilityConnection is nullptr");
            return;
        }
        AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(want,
            settingsExtServiceAbilityConnection, nullptr, -1);
    }
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override
    {
        HILOG_INFO("on ability connected");
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;
        if (enable_) {
            data.WriteString16(Str8ToStr16(GetEnableCommandStr()));
        } else {
            data.WriteString16(Str8ToStr16(GetDisableCommandStr()));
        }
        int32_t enbaleSearch = 2;
        remoteObject->SendRequest(enbaleSearch, data, reply, option);
        AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(this);
    }
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override
    {
        HILOG_INFO("on ability disconnected");
    }
private:
    bool enable_ = false;
    inline std::string GetEnableCommandStr()
    {
        return "{\"method\":\"enableSearchItems\",\"extra\":{\"itemList\":[\"zoom_zone_text\", "
               "\"zoom_trigger_method\"]}}";
    }
    inline std::string GetDisableCommandStr()
    {
        return "{\"method\":\"disableSearchItems\",\"extra\":{\"itemList\":[\"zoom_zone_text\", "
               "\"zoom_trigger_method\"]}}";
    }
};

class AccessibilityShortkeyDialog {
public:
    AccessibilityShortkeyDialog();
    ~AccessibilityShortkeyDialog();
    bool ConnectDialog(ShortKeyDialogType dialogType);

private:
    bool ConnectExtension(ShortKeyDialogType dialogType);
    bool ConnectExtensionAbility(const AAFwk::Want &want, const std::string commandStr, ShortKeyDialogType dialogType);
    bool DisconnectExtension(ShortKeyDialogType dialogType) const;
    std::string BuildStartCommand();

private:
    sptr<ShortkeyAbilityConnection> functionSelectConn_ {nullptr};
    sptr<ReConfirmAbilityConnection> reConfirmConn_ {nullptr};
    sptr<ExclusiveAbilityConnection> readerExclusiveConn_ {nullptr};
    sptr<ZoomGestureConflictAbilityConnection> zoomGestureConflictConn_ {nullptr};
};

} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SHORT_KEY_DIALOG_H