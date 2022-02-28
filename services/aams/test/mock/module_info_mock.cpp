/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "module_info.h"

#include "app_log_wrapper.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
bool ModuleInfo::ReadFromParcel(Parcel &parcel)
{
    return true;
}

ModuleInfo *ModuleInfo::Unmarshalling(Parcel &parcel)
{
    ModuleInfo *info = new (std::nothrow) ModuleInfo();
    return info;
}

bool ModuleInfo::Marshalling(Parcel &parcel) const
{
    return true;
}

void to_json(nlohmann::json &jsonObject, const ModuleInfo &moduleInfo)
{
    jsonObject = nlohmann::json {
        {"moduleName", moduleInfo.moduleName},
        {"moduleSourceDir", moduleInfo.moduleSourceDir}
    };
}

void from_json(const nlohmann::json &jsonObject, ModuleInfo &moduleInfo)
{
    moduleInfo.moduleName = jsonObject.at("moduleName").get<std::string>();
    moduleInfo.moduleSourceDir = jsonObject.at("moduleSourceDir").get<std::string>();
}
}  //namespace AppExecFwk
}  // namespace OHOS