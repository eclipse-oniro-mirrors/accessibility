/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include <string>
#include <dlfcn.h>
#include "msdp_manager.h"
#include "message_parcel.h"
#include "extension_ability_manager.cpp"

namespace OHOS {
namespace Accessibility {
constexpr uint32_t FEATURE_USER_PREFERENCE_APP = 10;
constexpr uint32_t FEATURE_USER_FACE_ANGLE = 11;
constexpr uint32_t FEATURE_TIME_TUNNEL = 14;
constexpr uint32_t FEATURE_ENV_SOUND = 17;
constexpr int32_t MAX_APP_SIZE = 50;
const char* USER_STATUS_SO_NAME = "libuser_status_client.z.so";
const char* MSDP_SUBSCRIBE_CALLBACK_FUNC_NAME = "SubscribeCallback";
const char* MSDP_SUBSCRIBE_FUNC_NAME = "Subscribe";
const char* MSDP_UNSUBSCRIBE_FUNC_NAME = "Unsubscribe";
const std::string KEY_USER_STATUS_PREFERENCE_APP = "userStatusPreferenceApp";
const std::string KEY_FACE_ANGLE_STATUS = "userFaceAngleStatus";
const std::string TIME_TUNNEL_STATUS = "timeTunnelStatus";

typedef int32_t (*SubscribeCallbackFunc)(uint32_t feature, UserStatusDataCallbackFunc &callback);
typedef void (*SubscribeFunc)(uint32_t feature);
typedef void (*UnsubscribeFunc)(uint32_t feature);

UserStatusData::UserStatusData() {}

UserStatusData::~UserStatusData() {}

uint32_t UserStatusData::GetFeature() const
{
    return feature_;
}

void UserStatusData::SetFeature(uint32_t feature)
{
    feature_ = feature;
}

std::string UserStatusData::GetStatus() const
{
    return status_;
}

void UserStatusData::SetStatus(const std::string &status)
{
    status_ = status;
}

int32_t UserStatusData::GetResult() const
{
    return result_;
}

void UserStatusData::SetResult(int32_t result)
{
    result_ = result;
}

int32_t UserStatusData::GetErrorCode() const
{
    return errorCode_;
}

void UserStatusData::SetErrorCode(int32_t errorCode)
{
    errorCode_ = errorCode;
}

std::vector<std::string> UserStatusData::GetResultApps() const
{
    return resultApps_;
}

void UserStatusData::SetResultApps(const std::vector<std::string> resultApps)
{
    resultApps_ = resultApps;
}

std::string UserStatusData::GetHpeDeviceId() const
{
    return hpeDeviceId_;
}

void UserStatusData::SetHpeDeviceId(const std::string &hpeDeviceId)
{
    hpeDeviceId_ = hpeDeviceId;
}

int32_t UserStatusData::GetCoordinateX() const
{
    return coordinateX_;
}

void UserStatusData::SetCoordinateX(int32_t coordinateX)
{
    coordinateX_ = coordinateX;
}

int32_t UserStatusData::GetCoordinateY() const
{
    return coordinateY_;
}

void UserStatusData::SetCoordinateY(int32_t coordinateY)
{
    coordinateY_ = coordinateY;
}

int32_t UserStatusData::GetPointerAction() const
{
    return pointerAction_;
}

void UserStatusData::SetPointerAction(int32_t pointerAction)
{
    pointerAction_ = pointerAction;
}

int32_t UserStatusData::GetOrientation() const
{
    return orientation_;
}

void UserStatusData::SetOrientation(int32_t orientation)
{
    orientation_ = orientation;
}

std::string UserStatusData::Dump()
{
    std::string dumpInfo = DumpBaseData();

    if (feature_ == FEATURE_USER_PREFERENCE_APP) {
        dumpInfo.append(", resultApps_=");
        dumpInfo.append(std::to_string(resultApps_.size()));
        for (const auto& app : resultApps_) {
            dumpInfo.append(", ");
            dumpInfo.append(app.c_str());
        }
    }
    return dumpInfo;
}

std::string UserStatusData::DumpBaseData()
{
    std::ostringstream dumpInfo;
    dumpInfo << "feature=" << static_cast<uint32_t>(feature_)
             << ", status=" << status_
             << ", result=" << result_
             << ", errorCode_=" << errorCode_;
    if (feature_ == FEATURE_TIME_TUNNEL) {
        dumpInfo << ", coordinateX=" << coordinateX_
                 << ", coordinateY=" << coordinateY_
                 << ", pointerAction=" << pointerAction_
                 << ", orientation=" << orientation_;
    }
    return dumpInfo.str();
}

std::shared_ptr<UserStatusData> UserStatusData::Unmarshalling(Parcel &parcel)
{
    auto statusData = std::make_shared<UserStatusData>();
    if (!statusData->ReadFromParcel(parcel)) {
        HILOG_ERROR("read from parcel failed");
        return nullptr;
    }
    return statusData;
}

int32_t UserStatusData::UnMarshallingFeature(Parcel &parcel)
{
    MessageParcel& msgParcel = static_cast<MessageParcel&>(parcel);
    msgParcel.PrintBuffer(__FUNCTION__, __LINE__);
    return parcel.ReadUint32();
}

bool UserStatusData::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(feature_)) {
        HILOG_ERROR("write features_ is failed");
        return false;
    }
    if (!parcel.WriteInt32(result_)) {
        HILOG_ERROR("write result_ is failed");
        return false;
    }
    if (!parcel.WriteInt32(errorCode_)) {
        HILOG_ERROR("write errorCode_ is failed");
        return false;
    }
    if (!parcel.WriteString(status_)) {
        HILOG_ERROR("write status_ is failed");
        return false;
    }

    if (feature_ == FEATURE_USER_PREFERENCE_APP) {
        int32_t appSize = static_cast<int32_t>(resultApps_.size());
        if (appSize > MAX_APP_SIZE) {
            appSize = MAX_APP_SIZE;
        }
        if (!parcel.WriteInt32(appSize)) {
            HILOG_ERROR("write resultApps_ size is failed");
            return false;
        }
        for (int32_t i = 0; i < appSize; ++i) {
            if (!parcel.WriteString(resultApps_[i])) {
                HILOG_ERROR("write resultApps_ item is failed");
                return false;
            }
        }
    }
    if (feature_ == FEATURE_USER_FACE_ANGLE) {
        if (!parcel.WriteString(hpeDeviceId_)) {
            HILOG_ERROR("write hpeDeviceId_ is failed");
            return false;
        }
    }
    if (feature_ == FEATURE_TIME_TUNNEL) {
        if (!WriteTimeTunnelData(parcel)) {
            return false;
        }
    }
    return true;
}

bool UserStatusData::WriteTimeTunnelData(Parcel &parcel) const
{
    if (!parcel.WriteInt32(coordinateX_)) {
        HILOG_ERROR("write coordinateX_ is failed");
        return false;
    }
    if (!parcel.WriteInt32(coordinateY_)) {
        HILOG_ERROR("write coordinateY_ is failed");
        return false;
    }
    if (!parcel.WriteInt32(pointerAction_)) {
        HILOG_ERROR("write pointerAction_ is failed");
        return false;
    }
    if (!parcel.WriteInt32(orientation_)) {
        HILOG_ERROR("write orientation_ is failed");
        return false;
    }

    return true;
}

bool UserStatusData::ReadFromParcel(Parcel &parcel)
{
    result_ = parcel.ReadInt32();
    errorCode_ = parcel.ReadInt32();
    status_ = parcel.ReadString();
    if (status_.empty()) {
        HILOG_ERROR("Read status_ failed");
        return false;
    }

    if (status_ == KEY_USER_STATUS_PREFERENCE_APP) {
        int32_t size = parcel.ReadInt32();
        if (size > MAX_APP_SIZE || size < 0) {
            HILOG_ERROR("Invalid app size: %{public}d", size);
            return false;
        }
        std::string bundleName;
        for (int32_t i = 0; i < size; ++i) {
            bundleName = parcel.ReadString();
            if (bundleName.empty()) {
                HILOG_ERROR("Read bundle name failed, feature: %{public}d, result: %{public}d, errorCode: %{public}d",
                    feature_, result_, errorCode_);
                return false;
            }
            resultApps_.push_back(bundleName);
        }
    }
    if (status_ == KEY_FACE_ANGLE_STATUS) {
        hpeDeviceId_ = parcel.ReadString();
    }
    if (status_ == TIME_TUNNEL_STATUS) {
        coordinateX_ = parcel.ReadInt32();
        coordinateY_ = parcel.ReadInt32();
        pointerAction_ = parcel.ReadInt32();
        orientation_ = parcel.ReadInt32();
    }
    return true;
}

MsdpManager::MsdpManager()
{
    handle_ = dlopen(USER_STATUS_SO_NAME, RTLD_NOW);
    if (handle_ == nullptr) {
        HILOG_ERROR("user status dlopen failed : %{public}s", dlerror());
    }
}

MsdpManager::~MsdpManager()
{
    if (handle_ != nullptr) {
        dlclose(handle_);
    }
}

MsdpManager& MsdpManager::GetInstance()
{
    static MsdpManager instance;
    return instance;
}


int32_t MsdpManager::SubscribeVoiceRecognition()
{
    HILOG_INFO();
    if (handle_ == nullptr) {
        HILOG_ERROR("user status dlopen failed : %{public}s", dlerror());
        return ERR_INVALID_STATE;
    }
    SubscribeCallbackFunc subscribeCallbackFunc = reinterpret_cast<SubscribeCallbackFunc>(dlsym(handle_,
        MSDP_SUBSCRIBE_CALLBACK_FUNC_NAME));
    if (subscribeCallbackFunc == nullptr) {
        HILOG_ERROR("dlsym subscribeCallbackFunc failed : %{public}s", dlerror());
        return ERR_INVALID_STATE;
    }
    SubscribeFunc subscribeFunc = reinterpret_cast<SubscribeFunc>(dlsym(handle_, MSDP_SUBSCRIBE_FUNC_NAME));
    if (subscribeFunc == nullptr) {
        HILOG_ERROR("dlsym subscribeFunc failed : %{public}s", dlerror());
        return ERR_INVALID_STATE;
    }
    std::function<void(int32_t, std::shared_ptr<UserStatusData>)> func = [ = ](
        int32_t callbackId, std::shared_ptr<UserStatusData> userStatusData) {
        HILOG_INFO("voice recognition callback, Id: %{public}d, GetFeature: %{public}d, GetResult: %{public}d",
            callbackId, userStatusData->GetFeature(), userStatusData->GetResult());

        ExtensionAbilityManager::GetInstance().VoiceRecognize(userStatusData->GetResult());
    };
    int32_t ret = subscribeCallbackFunc(FEATURE_ENV_SOUND, func);
    subscribeFunc(FEATURE_ENV_SOUND);
    HILOG_INFO("userstatusClient.SubscribeCallback RET: %{public}d", ret);
    return ret;
}

void MsdpManager::UnSubscribeVoiceRecognition()
{
    HILOG_INFO();
    if (handle_ == nullptr) {
        HILOG_ERROR("user status dlopen failed : %{public}s", dlerror());
        return;
    }
    UnsubscribeFunc unsubscribeFunc = reinterpret_cast<UnsubscribeFunc>(dlsym(handle_, MSDP_UNSUBSCRIBE_FUNC_NAME));
    if (unsubscribeFunc == nullptr) {
        HILOG_ERROR("dlsym unsubscribeFunc failed : %{public}s", dlerror());
        return;
    }
    unsubscribeFunc(FEATURE_ENV_SOUND);
    HILOG_INFO("userstatusClient.unsubscribeFunc");
}
} // namespace Accessibility
} // namespace OHOS
