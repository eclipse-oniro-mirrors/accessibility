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

#include "accessibility_account_data.h"
#include "accessibility_window_manager.h"
#include "accessible_ability_manager_service.h"
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
    std::transform(args.begin(), args.end(), std::back_inserter(params),
        [](const std::u16string &arg) { return Str16ToStr8(arg); });

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
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    std::ostringstream oss;
    size_t index = 0;
    std::map<int32_t, sptr<AccessibilityWindowConnection>> connectedWindowList = currentAccount->GetAsacConnections();
    oss << "connected window id: ";
    for (const auto &iter : connectedWindowList) {
        index++;
        oss << iter.first;

        if (index != connectedWindowList.size()) {
            oss << ", ";
        }
    }

    index = 0;
    const std::map<int32_t, AccessibilityWindowInfo> &windows = Singleton<AccessibilityWindowManager>::
        GetInstance().a11yWindows_;
    oss << std::endl << "a11yWindows_ id: ";
    for (const auto &iter : windows) {
        index++;
        oss << iter.first;

        if (index != connectedWindowList.size()) {
            oss << ", ";
        }
    }

    oss << std::endl << "active window id: " <<
        Singleton<AccessibilityWindowManager>::GetInstance().activeWindowId_ << std::endl;
    oss << "accessibility focused window id: " <<
        Singleton<AccessibilityWindowManager>::GetInstance().a11yFocusedWindowId_ << std::endl;

    dumpInfo.append(oss.str());
    return 0;
}

void ConvertCapabilities(const uint32_t value, std::string &capabilities)
{
    capabilities = "";
    if (value & Capability::CAPABILITY_RETRIEVE) {
        capabilities += "retrieve/";
    }
    if (value & Capability::CAPABILITY_TOUCH_GUIDE) {
        capabilities += "touchGuide/";
    }
    if (value & Capability::CAPABILITY_KEY_EVENT_OBSERVER) {
        capabilities += "keyEventObserver/";
    }
    if (value & Capability::CAPABILITY_ZOOM) {
        capabilities += "zoom/";
    }
    if (value & Capability::CAPABILITY_GESTURE) {
        capabilities += "gesture/";
    }
}

void ConvertAbilityTypes(const uint32_t value, std::string &abilityTypes)
{
    abilityTypes = "";
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_SPOKEN) {
        abilityTypes += "spoken/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_HAPTIC) {
        abilityTypes += "haptic/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_AUDIBLE) {
        abilityTypes += "audible/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_VISUAL) {
        abilityTypes += "visual/";
    }
    if (value & AccessibilityAbilityTypes::ACCESSIBILITY_ABILITY_TYPE_GENERIC) {
        abilityTypes += "generic/";
    }
}

void ConvertEventTypes(const uint32_t value, std::string &eventTypes)
{
    eventTypes = "";
    std::map<EventType, std::string> accessibilityEventTable = {{EventType::TYPE_VIEW_CLICKED_EVENT, "click"},
        {EventType::TYPE_VIEW_LONG_CLICKED_EVENT, "longClick"},
        {EventType::TYPE_VIEW_SELECTED_EVENT, "select"},
        {EventType::TYPE_VIEW_FOCUSED_EVENT, "focus"},
        {EventType::TYPE_VIEW_TEXT_UPDATE_EVENT, "textUpdate"},
        {EventType::TYPE_PAGE_STATE_UPDATE, "pageStateUpdate"},
        {EventType::TYPE_NOTIFICATION_UPDATE_EVENT, "notificationUpdate"},
        {EventType::TYPE_VIEW_HOVER_ENTER_EVENT, "hoverEnter"},
        {EventType::TYPE_VIEW_HOVER_EXIT_EVENT, "hoverExit"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_BEGIN, "touchGuideGestureBegin"},
        {EventType::TYPE_TOUCH_GUIDE_GESTURE_END, "touchGuideGestureEnd"},
        {EventType::TYPE_PAGE_CONTENT_UPDATE, "pageContentUpdate"},
        {EventType::TYPE_VIEW_SCROLLED_EVENT, "scroll"},
        {EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT, "textSelectionUpdate"},
        {EventType::TYPE_PUBLIC_NOTICE_EVENT, "publicNotice"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT, "accessibilityFocus"},
        {EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT, "accessibilityFocusClear"},
        {EventType::TYPE_VIEW_TEXT_MOVE_UNIT_EVENT, "textMoveUnit"},
        {EventType::TYPE_TOUCH_GUIDE_BEGIN, "touchGuideBegin"},
        {EventType::TYPE_TOUCH_GUIDE_END, "touchGuideEnd"},
        {EventType::TYPE_TOUCH_BEGIN, "touchBegin"},
        {EventType::TYPE_TOUCH_END, "touchEnd"},
        {EventType::TYPE_WINDOW_UPDATE, "windowUpdate"},
        {EventType::TYPE_INTERRUPT_EVENT, "interrupt"},
        {EventType::TYPE_GESTURE_EVENT, "gesture"}};

    for (auto itr = accessibilityEventTable.begin(); itr != accessibilityEventTable.end(); ++itr) {
        if (value & itr->first) {
            eventTypes += itr->second;
            eventTypes += "/";
        }
    }
}

int AccessibilityDumper::DumpAccessibilityClientInfo(std::string& dumpInfo) const
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    auto connectedAbilities = currentAccount->GetConnectedA11yAbilities();
    std::ostringstream oss;
    oss << "client num:  " << connectedAbilities.size() << std::endl;

    // Dump client info details
    size_t index = 0;
    for (const auto &iter : connectedAbilities) {
        oss << std::endl << "client[" << index++ << "] info details: " << std::endl;
        if (!iter.second) {
            HILOG_ERROR("The connected ability[%{public}s] is null", iter.first.c_str());
            continue;
        }
        AccessibilityAbilityInfo accessibilityAbilityInfo = iter.second->GetAbilityInfo();
        oss << "    bundleName: " << accessibilityAbilityInfo.GetPackageName() << std::endl;
        oss << "    moduleName: " << accessibilityAbilityInfo.GetModuleName() << std::endl;
        oss << "    abilityName: " << accessibilityAbilityInfo.GetName() << std::endl;
        oss << "    description: " << accessibilityAbilityInfo.GetDescription() << std::endl;

        std::string capabilities;
        ConvertCapabilities(accessibilityAbilityInfo.GetCapabilityValues(), capabilities);
        oss << "    capabilities: " << capabilities << std::endl;

        std::string abilityTypes;
        ConvertAbilityTypes(accessibilityAbilityInfo.GetAccessibilityAbilityType(), abilityTypes);
        oss << "    abilityTypes: " << abilityTypes << std::endl;

        std::string eventTypes;
        ConvertEventTypes(accessibilityAbilityInfo.GetEventTypes(), eventTypes);
        oss << "    eventTypes: " << eventTypes << std::endl;

        std::vector<std::string> targetBundleNames = accessibilityAbilityInfo.GetFilterBundleNames();
        if (targetBundleNames.empty()) {
            oss << "    targetBundleNames: " << "all" << std::endl;
        } else {
            oss << "    targetBundleNames: " << std::endl;
            for (const auto &targetBundleName : targetBundleNames) {
                oss << "        " << targetBundleName << std::endl;
            }
        }

        if (index != connectedAbilities.size()) {
            oss << std::endl << "    -------------------------------" << std::endl << std::endl;
        }
    }
    dumpInfo.append(oss.str());
    return 0;
}

int AccessibilityDumper::DumpAccessibilityUserInfo(std::string& dumpInfo) const
{
    HILOG_INFO();
    sptr<AccessibilityAccountData> currentAccount =
        Singleton<AccessibleAbilityManagerService>::GetInstance().GetCurrentAccountData();
    if (!currentAccount) {
        HILOG_ERROR("currentAccount is null");
        return -1;
    }

    std::ostringstream oss;
    oss << "id:  " << currentAccount->GetAccountId() << std::endl;

    std::shared_ptr<AccessibilitySettingsConfig> config = currentAccount->GetConfig();
    if (!config) {
        HILOG_ERROR("config is null");
        return -1;
    }

    // Dump Capabilities
    oss << "accessible:  " << config->GetEnabledState() << std::endl;
    oss << "touchGuide:  " << config->GetTouchGuideState() << std::endl;
    oss << "gesture:  " << config->GetGestureState() << std::endl;
    oss << "keyEventObserver:  " << config->GetKeyEventObserverState() << std::endl;

    // Dump setting info
    oss << "screenMagnification:  " << config->GetScreenMagnificationState() << std::endl;
    oss << "mouseKey:  " << config->GetMouseKeyState() << std::endl;
    oss << "shortKey:  " << config->GetShortKeyState() << std::endl;
    oss << "animationOff:  " << config->GetAnimationOffState() << std::endl;
    oss << "invertColor:  " << config->GetInvertColorState() << std::endl;
    oss << "highContrastText:  " << config->GetHighContrastTextState() << std::endl;
    oss << "audioMono:  " << config->GetAudioMonoState() << std::endl;
    oss << "shortkeyTarget:  " << config->GetShortkeyTarget() << std::endl;
    oss << "mouseAutoClick:  " << config->GetMouseAutoClick() << std::endl;
    oss << "daltonizationColorFilter:  " << config->GetDaltonizationColorFilter() << std::endl;
    oss << "contentTimeout:  " << config->GetContentTimeout() << std::endl;
    oss << "brightnessDiscount:  " << config->GetBrightnessDiscount() << std::endl;
    oss << "audioBalance:  " << config->GetAudioBalance() << std::endl;

    // Dump caption info
    oss << "captionState:  " << config->GetCaptionState() << std::endl;
    if (config->GetCaptionState()) {
        AccessibilityConfig::CaptionProperty captionProperty = config->GetCaptionProperty();
        oss << "    fontFamily:  " << captionProperty.GetFontFamily() << std::endl;
        oss << "    fontScale:  " << captionProperty.GetFontScale() << std::endl;
        oss << "    fontColor:  " << captionProperty.GetFontColor() << std::endl;
        oss << "    fontEdgeType:  " << captionProperty.GetFontEdgeType() << std::endl;
        oss << "    backgroundColor:  " << captionProperty.GetBackgroundColor() << std::endl;
        oss << "    windowColor:  " << captionProperty.GetWindowColor() << std::endl;
    }
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
        .append("|dump accessibility current user in the system\n")
        .append(" -c                    ")
        .append("|dump accessibility client in the system\n")
        .append(" -w                    ")
        .append("|dump accessibility window info in the system\n");
}
} // Accessibility
} // OHOS