/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_dumper.h"

#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>

#include "hilog_wrapper.h"
#include "string_ex.h"
#include "unique_fd.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string ARG_DUMP_HELP = "-h";
    const std::string ARG_DUMP_USER = "-u";
    const std::string ARG_DUMP_CLIENT = "-c";
    const std::string ARG_DUMP_ACCESSIBILITY_WINDOW = "-w";
}

int AccessibilityDumper::Dump(int fd, const std::vector<std::u16string>& args) const
{
    HILOG_DEBUG("Dump begin fd: %{public}d", fd);
    if (fd < 0) {
        return -1;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd); // auto close
    fd = ufd.Get();
    std::vector<std::string> params;
    for (auto& arg : args) {
        params.emplace_back(Str16ToStr8(arg));
    }

    std::string dumpInfo;
    if (params.empty()) {
        ShowIllegalArgsInfo(dumpInfo);
    } else if (params[0] == ARG_DUMP_HELP) {
        ShowHelpInfo(dumpInfo);
    } else {
        DumpAccessibilityInfo(params, dumpInfo);
    }
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        HILOG_ERROR("dprintf error");
        return -1;
    }
    HILOG_INFO("Dump end");
    return 0;
}


int AccessibilityDumper::DumpAccessibilityWindowInfo(std::string& dumpInfo) const
{
    HILOG_INFO("start");
    std::ostringstream oss;
    // list AccessibilityWindow attributes
    oss << "window info: "<< std::endl;
    dumpInfo.append(oss.str());
    return 0;
}

int AccessibilityDumper::DumpAccessibilityClientInfo(std::string& dumpInfo) const
{
    HILOG_INFO("start");
    std::ostringstream oss;
    oss << "client num :  "<< std::endl;
    oss << "client name :  "<< std::endl;
    dumpInfo.append(oss.str());
    return 0;
}

int AccessibilityDumper::DumpAccessibilityUserInfo(std::string& dumpInfo) const
{
    HILOG_INFO("start");
    std::ostringstream oss;
    oss << "current user info:  "<< std::endl;
    dumpInfo.append(oss.str());
    return 0;
}

int AccessibilityDumper::DumpAccessibilityInfo(const std::vector<std::string>& args, std::string& dumpInfo) const
{
    if (args.empty()) {
        return -1;
    }
    DumpType dumpType = DumpType::DUMP_NONE;
    if (args[0] == ARG_DUMP_USER) {
        dumpType = DumpType::DUMP_USER;
    } else if (args[0] == ARG_DUMP_CLIENT) {
        dumpType = DumpType::DUMP_CLIENT;
    } else if (args[0] == ARG_DUMP_ACCESSIBILITY_WINDOW) {
        dumpType = DumpType::DUMP_ACCESSIBILITY_WINDOW;
    }
    int ret = 0;
    switch (dumpType) {
        case DumpType::DUMP_USER:
            ret = DumpAccessibilityUserInfo(dumpInfo);
            break;
        case DumpType::DUMP_CLIENT:
            ret = DumpAccessibilityClientInfo(dumpInfo);
            break;
        case DumpType::DUMP_ACCESSIBILITY_WINDOW:
            ret = DumpAccessibilityWindowInfo(dumpInfo);
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}

void AccessibilityDumper::ShowIllegalArgsInfo(std::string& dumpInfo) const
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

void AccessibilityDumper::ShowHelpInfo(std::string& dumpInfo) const
{
    dumpInfo.append("Usage:\n")
        .append(" -h                    ")
        .append("|help text for the tool\n")
        .append(" -u                    ")
        .append("|dump accessibility user in the system\n")
        .append(" -c                    ")
        .append("|dump accessibility client in the system\n")
        .append(" -w                    ")
        .append("|dump accessibility window info in the system\n");
}
}
}