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

#include "accessibility_ability_info.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
AccessibilityAbilityInfo::AccessibilityAbilityInfo(const AccessibilityAbilityInitParams &initParams)
{
    HILOG_DEBUG();

    name_ = initParams.name;
    rationale_ = initParams.rationale;
    bundleName_ = initParams.bundleName;
    moduleName_ = initParams.moduleName;
    description_ = initParams.description;
    label_ = initParams.label;
    staticCapabilities_ = initParams.staticCapabilities;
    settingsAbility_ = initParams.settingsAbility;
    abilityTypes_ = initParams.abilityTypes;
    isImportant_ = initParams.isImportant;
    needHide_ = initParams.needHide;
    eventConfigure_ = initParams.eventConfigure;
    readableRules_ = initParams.readableRules;

    HILOG_DEBUG("ability name:[%{public}s], bundle name:[%{public}s], module name:[%{public}s],"
        "capabilities:[%{public}d], rationale:[%{public}s], settingsAbility:[%{public}s],"
        "abilityTypes:[%{public}d], isImportant:[%{public}d], needHide:[%{public}d]",
        name_.c_str(), bundleName_.c_str(), moduleName_.c_str(), capabilities_,
        rationale_.c_str(), settingsAbility_.c_str(), abilityTypes_, isImportant_, needHide_);
}

uint32_t AccessibilityAbilityInfo::GetAccessibilityAbilityType()
{
    HILOG_DEBUG("types(%{public}d)start.", abilityTypes_);
    return abilityTypes_;
}

uint32_t AccessibilityAbilityInfo::GetCapabilityValues() const
{
    HILOG_DEBUG();
    return capabilities_;
}

uint32_t AccessibilityAbilityInfo::GetStaticCapabilityValues() const
{
    HILOG_DEBUG("staticCapabilities(%{public}d).", staticCapabilities_);
    return staticCapabilities_;
}

const std::string &AccessibilityAbilityInfo::GetDescription() const
{
    HILOG_DEBUG();
    return description_;
}

uint32_t AccessibilityAbilityInfo::GetEventTypes()
{
    HILOG_DEBUG();
    return eventTypes_;
}

std::string AccessibilityAbilityInfo::GetId() const
{
    HILOG_DEBUG();
    return bundleName_ + "/" + name_;
}

const std::string &AccessibilityAbilityInfo::GetName() const
{
    HILOG_DEBUG();
    return name_;
}

const std::string &AccessibilityAbilityInfo::GetPackageName() const
{
    HILOG_DEBUG();
    return bundleName_;
}

const std::string &AccessibilityAbilityInfo::GetModuleName() const
{
    HILOG_DEBUG();
    return moduleName_;
}

void AccessibilityAbilityInfo::SetPackageName(const std::string &bundleName)
{
    HILOG_DEBUG();
    bundleName_ = bundleName;
}

const std::vector<std::string> &AccessibilityAbilityInfo::GetFilterBundleNames() const
{
    HILOG_DEBUG();
    return targetBundleNames_;
}

const std::string &AccessibilityAbilityInfo::GetSettingsAbility() const
{
    HILOG_DEBUG();
    return settingsAbility_;
}

bool AccessibilityAbilityInfo::IsImportant() const
{
    HILOG_DEBUG();
    return false;
}

bool AccessibilityAbilityInfo::NeedHide() const
{
    HILOG_DEBUG();
    return needHide_;
}

const std::string &AccessibilityAbilityInfo::GetLabel() const
{
    HILOG_DEBUG();
    return label_;
}

void AccessibilityAbilityInfo::GetEventConfigure(std::vector<uint32_t> &needEvents)
{
    HILOG_DEBUG();
    static const std::map<std::string, EventType> EvtTypeTable = {
        {"accessibilityFocus", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT},
        {"accessibilityFocusClear", EventType::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT},
        {"click", EventType::TYPE_VIEW_CLICKED_EVENT},
        {"longClick", EventType::TYPE_VIEW_LONG_CLICKED_EVENT},
        {"focus", EventType::TYPE_VIEW_FOCUSED_EVENT},
        {"select", EventType::TYPE_VIEW_SELECTED_EVENT},
        {"scroll", EventType::TYPE_VIEW_SCROLLED_EVENT},
        {"scrolling", EventType::TYPE_VIEW_SCROLLING_EVENT},
        {"hoverEnter", EventType::TYPE_VIEW_HOVER_ENTER_EVENT},
        {"hoverExit", EventType::TYPE_VIEW_HOVER_EXIT_EVENT},
        {"textUpdate", EventType::TYPE_VIEW_TEXT_UPDATE_EVENT},
        {"textSelectionUpdate", EventType::TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT},
        {"requestFocusForAccessibility", EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY},
        {"announceForAccessibility", EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY},
        {"announceForAccessibilityNotInterrupt", EventType::TYPE_VIEW_ANNOUNCE_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"requestFocusForAccessibilityNotInterrupt",
            EventType::TYPE_VIEW_REQUEST_FOCUS_FOR_ACCESSIBILITY_NOT_INTERRUPT},
        {"pageContentUpdate", EventType::TYPE_PAGE_CONTENT_UPDATE},
        {"pageStateUpdate", EventType::TYPE_PAGE_STATE_UPDATE},
        {"pageOpen", EventType::TYPE_PAGE_OPEN},
        {"pageClose", EventType::TYPE_PAGE_CLOSE},
        {"allEvents", EventType::TYPES_ALL_MASK},
        {"noneEvents", EventType::TYPE_VIEW_INVALID}
    };
    
    for (auto &event : eventConfigure_) {
        HILOG_DEBUG("ability config event is (%{public}s).", event.c_str());
        auto evtType = EvtTypeTable.find(event);
        if (evtType != EvtTypeTable.end()) {
            needEvents.push_back(evtType->second);
        } else {
            auto widType = std::find(needEvents.begin(), needEvents.end(),
                EventType::TYPE_WINDOW_UPDATE);
            if (widType == needEvents.end()) {
                HILOG_DEBUG("ability change widTyp");
                needEvents.push_back(EventType::TYPE_WINDOW_UPDATE);
            }
        }
    }
}
} // namespace Accessibility
} // namespace OHOS