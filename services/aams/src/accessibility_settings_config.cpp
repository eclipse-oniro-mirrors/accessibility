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

#include "accessibility_settings_config.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string PREF_PATH = "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
} // namespace
AccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("start.");
    accountId_ = id;
}

bool AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("start.");
    enabled_ = state;
    return SetStatePref(STATE::ACCESSIBILITY);
}

bool AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("start.");
    eventTouchGuideState_ = state;
    return SetStatePref(STATE::TOUCHGUIDE);
}

bool AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("start.");
    gesturesSimulation_ = state;
    return SetStatePref(STATE::GESTURE);
}

bool AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("start.");
    filteringKeyEvents_ = state;
    return SetStatePref(STATE::KEYEVENT);
}


void AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("start.");
    isCaptionState_ = state;
    SetStatePref(STATE::CAPTION);
}

void AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("start.");
    isScreenMagnificationState_ = state;
    SetStatePref(STATE::SCREENMAGNIFIER);
}

void AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isShortKeyState_ = state;
    SetStatePref(STATE::SHORTKEY);
}

void AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("start.");
    isMouseKeyState_ = state;
    SetStatePref(STATE::MOUSEKEY);
}

void AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("start.");
    mouseAutoClick_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutInt("MouseAutoClick", mouseAutoClick_);
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("start.");
    shortkeyTarget_ = name;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutString("ShortkeyTarget", shortkeyTarget_);
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("start.");
    highContrastTextState_ = state;
    SetStatePref(STATE::HIGHCONTRASTTEXT);
}

void AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("start.");
    invertColorState_ = state;
    SetStatePref(STATE::INVERTCOLORSTATE);
}

void AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("start.");
    animationOffState_ = state;
    SetStatePref(STATE::ANIMATIONOFF);
}

void AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("start.");
    audioMonoState_ = state;
    SetStatePref(STATE::AUDIOMONO);
}

void AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("start.");
    daltonizationColorFilter_ = filter;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutInt("daltonizationColorFilter", static_cast<int32_t>(daltonizationColorFilter_));
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("start.");
    contentTimeout_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutInt("contentTimeout", static_cast<int32_t>(contentTimeout_));
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("start.");
    brightnessDiscount_ = discount;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutFloat("brightnessDiscount", brightnessDiscount_);
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("start.");
    audioBalance_ = balance;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }

    pref_->PutFloat("audioBalance", audioBalance_);
    pref_->Flush();
}

void AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG("start.");
    captionProperty_ = caption;

    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }
    const std::string FONTFAMILY = captionProperty_.GetFontFamily();
    int32_t FONTSCALE = captionProperty_.GetFontScale();
    uint32_t FONTCOLOR = captionProperty_.GetFontColor();
    const std::string FONTEDGETYPE = captionProperty_.GetFontEdgeType();
    uint32_t BACKGROUNDCOLOR = captionProperty_.GetBackgroundColor();
    uint32_t WINDOWCOLOR = captionProperty_.GetWindowColor();

    pref_->PutString("fontFamily", FONTFAMILY);
    pref_->PutInt("fontColor", static_cast<int32_t>(FONTCOLOR));
    pref_->PutString("fontEdgeType", FONTEDGETYPE);
    pref_->PutInt("backgroundColor", static_cast<int32_t>(BACKGROUNDCOLOR));
    pref_->PutInt("windowColor", static_cast<int32_t>(WINDOWCOLOR));
    pref_->PutInt("fontScale", FONTSCALE);
    pref_->Flush();
}

bool AccessibilitySettingsConfig::SetStatePref(int32_t type)
{
    HILOG_DEBUG("start.");
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return false;
    }

    std::string strValue = "";
    switch (type) {
        case STATE::ACCESSIBILITY:
            strValue = StateChange(enabled_);
            pref_->PutString("accessible", strValue);
            break;
        case STATE::TOUCHGUIDE:
            strValue = StateChange(eventTouchGuideState_);
            pref_->PutString("touchGuide", strValue);
            break;
        case STATE::GESTURE:
            strValue = StateChange(gesturesSimulation_);
            pref_->PutString("gesture", strValue);
            break;
        case STATE::KEYEVENT:
            strValue = StateChange(filteringKeyEvents_);
            pref_->PutString("keyEventObserver", strValue);
            break;
        case STATE::CAPTION:
            strValue = StateChange(isCaptionState_);
            pref_->PutString("CaptionState", strValue);
            break;
        case STATE::SCREENMAGNIFIER:
            strValue = StateChange(isScreenMagnificationState_);
            pref_->PutString("ScreenMagnification", strValue);
            break;
        case STATE::SHORTKEY:
            strValue = StateChange(isShortKeyState_);
            pref_->PutString("ShortKey", strValue);
            break;
        case STATE::MOUSEKEY:
            strValue = StateChange(isMouseKeyState_);
            pref_->PutString("MouseKey", strValue);
            break;
        case STATE::HIGHCONTRASTTEXT:
            strValue = StateChange(highContrastTextState_);
            pref_->PutString("highContrastText", strValue);
            break;
        case STATE::INVERTCOLORSTATE:
            strValue = StateChange(invertColorState_);
            pref_->PutString("invertColor", strValue);
            break;
        case STATE::ANIMATIONOFF:
            strValue = StateChange(animationOffState_);
            pref_->PutString("animationOff", strValue);
            break;
        case STATE::AUDIOMONO:
            strValue = StateChange(audioMonoState_);
            pref_->PutString("audioMono", strValue);
            break;
        default:
            HILOG_ERROR("Invalid parameter.");
            return false;
    }
    pref_->Flush();
    return true;
}

std::string AccessibilitySettingsConfig::StateChange(bool state)
{
    HILOG_DEBUG("start.");
    std::string STATE_TRUE = "on";
    std::string STATE_FALSE = "off";
    if (state) {
        return STATE_TRUE;
    } else {
        return STATE_FALSE;
    }
}

bool AccessibilitySettingsConfig::GetCaptionState()
{
    return isCaptionState_;
}

bool AccessibilitySettingsConfig::GetScreenMagnificationState()
{
    return isScreenMagnificationState_;
}

bool AccessibilitySettingsConfig::GetShortKeyState()
{
    return isShortKeyState_;
}

bool AccessibilitySettingsConfig::GetMouseKeyState()
{
    return isMouseKeyState_;
}

int32_t AccessibilitySettingsConfig::GetMouseAutoClick()
{
    return mouseAutoClick_;
}

const std::string &AccessibilitySettingsConfig::GetShortkeyTarget()
{
    return shortkeyTarget_;
}

bool AccessibilitySettingsConfig::GetHighContrastTextState()
{
    return highContrastTextState_;
}

bool AccessibilitySettingsConfig::GetInvertColorState()
{
    return invertColorState_;
}

bool AccessibilitySettingsConfig::GetAnimationOffState()
{
    return animationOffState_;
}

bool AccessibilitySettingsConfig::GetAudioMonoState()
{
    return audioMonoState_;
}

uint32_t AccessibilitySettingsConfig::GetDaltonizationColorFilter()
{
    return daltonizationColorFilter_;
}

uint32_t AccessibilitySettingsConfig::GetContentTimeout()
{
    return contentTimeout_;
}

float AccessibilitySettingsConfig::GetBrightnessDiscount()
{
    return brightnessDiscount_;
}

float AccessibilitySettingsConfig::GetAudioBalance()
{
    return audioBalance_;
}

bool AccessibilitySettingsConfig::GetEnabledState()
{
    return enabled_;
}

bool AccessibilitySettingsConfig::GetTouchGuideState()
{
    return eventTouchGuideState_;
}

bool AccessibilitySettingsConfig::GetGestureState()
{
    return gesturesSimulation_;
}

bool AccessibilitySettingsConfig::GetKeyEventObserverState()
{
    return filteringKeyEvents_;
}

const AccessibilityConfig::CaptionProperty &AccessibilitySettingsConfig::GetCaptionProperty()
{
    return captionProperty_;
};

uint32_t AccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG("start.");
    uint32_t state = 0;
    if (isCaptionState_) {
        state |= STATE_CAPTION_ENABLED;
    }

    if (isScreenMagnificationState_) {
        state |= STATE_SCREENMAGNIFIER_ENABLED;
    }

    if (isMouseKeyState_) {
        state |= STATE_MOUSEKEY_ENABLED;
    }

    if (isShortKeyState_) {
        state |= STATE_SHORTKEY_ENABLED;
    }

    if (highContrastTextState_) {
        state |= STATE_HIGHCONTRAST_ENABLED;
    }

    if (invertColorState_) {
        state |= STATE_INVETRTCOLOR_ENABLED;
    }

    if (animationOffState_) {
        state |= STATE_ANIMATIONOFF_ENABLED;
    }

    if (audioMonoState_) {
        state |= STATE_AUDIOMONO_ENABLED;
    }
    return state;
}

void AccessibilitySettingsConfig::InitCaption()
{
    HILOG_DEBUG("start.");
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("CaptionState", "");
    HILOG_DEBUG(" pref_->GetString() = %{public}s.", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        isCaptionState_ = true;
    } else {
        isCaptionState_ = false;
    }

    std::string FONTFAMILY = pref_->GetString("fontFamily", "default");
    HILOG_DEBUG(" pref_->GetString() = %{public}s.", FONTFAMILY.c_str());

    int32_t FONTSCALE =  static_cast<int32_t>(pref_->GetInt("fontScale", 0));
    HILOG_DEBUG(" pref_->GetString() = %{public}d.", FONTSCALE);

    uint32_t FONTCOLOR = static_cast<uint32_t>(pref_->GetInt("fontColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref_->GetString() = 0x%{public}x.", FONTCOLOR);

    std::string FONTEDGETYPE = pref_->GetString("fontEdgeType", "none");
    HILOG_DEBUG(" pref_->GetString() = 0x%{public}s.", FONTEDGETYPE.c_str());

    uint32_t BACKGROUNDCOLOR = static_cast<uint32_t>(pref_->GetInt("backgroundColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref_->GetString() = 0x%{public}x.", BACKGROUNDCOLOR);

    uint32_t WINDOWCOLOR = static_cast<uint32_t>(pref_->GetInt("windowColor", DEFAULT_COLOR));
    HILOG_DEBUG(" pref_->GetString() = 0x%{public}x.", WINDOWCOLOR);

    captionProperty_.SetFontFamily(FONTFAMILY);
    captionProperty_.SetFontScale(FONTSCALE);
    captionProperty_.SetFontColor(FONTCOLOR);
    captionProperty_.SetFontEdgeType(FONTEDGETYPE);
    captionProperty_.SetBackgroundColor(BACKGROUNDCOLOR);
    captionProperty_.SetWindowColor(WINDOWCOLOR);
}

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG("start.");
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("ScreenMagnification", "");
    isScreenMagnificationState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("MouseKey", "");
    isMouseKeyState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("ShortKey", "");
    isShortKeyState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("animationOff", "");
    animationOffState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("invertColor", "");
    invertColorState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("highContrastText", "");
    highContrastTextState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    strValue = pref_->GetString("audioMono", "");
    audioMonoState_ = std::strcmp(strValue.c_str(), "on") ? false : true;

    shortkeyTarget_ = pref_->GetString("ShortkeyTarget", "none");
    mouseAutoClick_ = static_cast<int32_t>(pref_->GetInt("MouseAutoClick", -1));
    daltonizationColorFilter_ = static_cast<uint32_t>(pref_->GetInt("daltonizationColorFilter", 0));
    contentTimeout_ = static_cast<uint32_t>(pref_->GetInt("contentTimeout", 0));
    brightnessDiscount_ = static_cast<uint32_t>(pref_->GetFloat("brightnessDiscount", 0));
    audioBalance_ = static_cast<uint32_t>(pref_->GetFloat("audioBalance", 0));
}

void AccessibilitySettingsConfig::InitCapability()
{
    HILOG_DEBUG("start.");
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("accessible", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        enabled_ = true;
    } else {
        enabled_ = false;
    }

    strValue = pref_->GetString("touchGuide", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        HILOG_DEBUG("eventTouchGuideState_ = true");
        eventTouchGuideState_ = true;
    } else {
        HILOG_DEBUG("eventTouchGuideState_ = false");
        eventTouchGuideState_ = false;
    }

    strValue = pref_->GetString("gesture", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        filteringKeyEvents_ = true;
    } else {
        filteringKeyEvents_ = false;
    }

    strValue = pref_->GetString("keyEventObserver", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        gesturesSimulation_ = true;
    } else {
        gesturesSimulation_ = false;
    }
}

void AccessibilitySettingsConfig::InitEnabledList()
{
    HILOG_DEBUG("start.");
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("BundleName/AbilityName/Capabilities", "");
    HILOG_DEBUG("strValue = %{public}s", strValue.c_str());

    StringToVector(strValue, enabledAbilityInfos_);
}

void AccessibilitySettingsConfig::Init()
{
    HILOG_DEBUG("start.");
    int errCode = -1;

    pref_ = NativePreferences::PreferencesHelper::GetPreferences(PREF_PATH + std::to_string(accountId_) + ".xml",
                                                                 errCode);
    if (errCode) {
        HILOG_ERROR("GetPreferences failed! account id (%{public}d), errCode(%{public}d).", accountId_, errCode);
        return;
    }

    InitCaption();
    InitSetting();
    InitCapability();
    InitEnabledList();
}

void AccessibilitySettingsConfig::VectorToString(const std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG("start.");
    int32_t i = 0;
    for (auto& var : vectorVal) {
        if (i > 0) {
            stringOut = stringOut + ',';
        }
        stringOut = stringOut + var.c_str();
        i++;
    }
    HILOG_DEBUG("end stringOUT = %{public}s .", stringOut.c_str());
}

void AccessibilitySettingsConfig::StringToVector(const std::string &stringIn, std::vector<std::string> &vectorResult)
{
    HILOG_DEBUG("start.");
    int32_t strLength = static_cast<int32_t>(stringIn.size());
    std::vector<int32_t> position;

    if (strLength == 0) {
        return;
    }

    for (int32_t j = 0; j < strLength; j++) {
        if (stringIn[j] == ',') {
            position.push_back(j);
        }
    }

    int32_t wrodCount = static_cast<int32_t>(position.size());
    if ((wrodCount == 0) && (strLength > 0)) {
        vectorResult.push_back(stringIn);
    } else {
        int32_t startWrod = 0;
        int32_t length = 0;
        for (int32_t i = 0; i <= wrodCount; i++) {
            if (i == 0) {
                length = position[i];
                vectorResult.push_back(stringIn.substr(startWrod, length)); // First string
            } else if (i < wrodCount) {
                startWrod = position[i - 1] + 1;
                length = position[i] - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Second string to last-1 string
            } else {
                startWrod = position[i - 1] + 1;
                length = strLength - position[i - 1] - 1;
                vectorResult.push_back(stringIn.substr(startWrod, length)); // Last string
            }
        }
    }
    HILOG_DEBUG("strLength = %{public}d, wrodCount = %{public}d, stringIn : %{public}s",
        strLength, wrodCount, stringIn.c_str());
    for (auto& var : vectorResult) {
        HILOG_DEBUG("vectorResult = %{public}s ", var.c_str());
    }
    HILOG_DEBUG("end.");
}

const std::vector<std::string> &AccessibilitySettingsConfig::GetEnabledAbilityInfos()
{
    HILOG_DEBUG("start.");
    return enabledAbilityInfos_;
}

void AccessibilitySettingsConfig::UpdateEnabledAbilities(const std::vector<std::string> &vecvalue)
{
    HILOG_DEBUG("start.");
    enabledAbilityInfos_ = vecvalue;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return;
    }
    std::string stringOut = "";
    VectorToString(enabledAbilityInfos_, stringOut);
    pref_->PutString("BundleName/AbilityName/Capabilities", stringOut);
    pref_->Flush();
}

void AccessibilitySettingsConfig::ClearData()
{
    HILOG_DEBUG("start.");
    int errCode = -1;
    errCode = NativePreferences::PreferencesHelper::DeletePreferences(PREF_PATH + std::to_string(accountId_) + ".xml");
    if (errCode) {
        HILOG_ERROR("DeletePreferences failed! account id (%{public}d)", accountId_);
    }

    pref_ = nullptr;
}
} // namespace Accessibility
} // namespace OHOS