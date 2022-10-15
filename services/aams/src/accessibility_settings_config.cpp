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
#include "utils.h"

namespace OHOS {
namespace Accessibility {
namespace {
    const std::string PREF_PATH = "/data/service/el1/public/barrierfree/accessibility_ability_manager_service/";
    constexpr uint32_t DEFAULT_COLOR = 0xff000000;
} // namespace
AccessibilitySettingsConfig::AccessibilitySettingsConfig(int32_t id)
{
    HILOG_DEBUG("id = [%{public}d]", id);
    accountId_ = id;
}

bool AccessibilitySettingsConfig::SetEnabled(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    enabled_ = state;
    return SetStatePref(STATE::ACCESSIBILITY);
}

bool AccessibilitySettingsConfig::SetTouchGuideState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    eventTouchGuideState_ = state;
    return SetStatePref(STATE::TOUCHGUIDE);
}

bool AccessibilitySettingsConfig::SetGestureState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    gesturesSimulation_ = state;
    return SetStatePref(STATE::GESTURE);
}

bool AccessibilitySettingsConfig::SetKeyEventObserverState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    filteringKeyEvents_ = state;
    return SetStatePref(STATE::KEYEVENT);
}


RetError AccessibilitySettingsConfig::SetCaptionState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isCaptionState_ = state;
    return SetStatePref(STATE::CAPTION) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetScreenMagnificationState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isScreenMagnificationState_ = state;
    return SetStatePref(STATE::SCREENMAGNIFIER) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetShortKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isShortKeyState_ = state;
    return SetStatePref(STATE::SHORTKEY) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetMouseKeyState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    isMouseKeyState_ = state;
    return SetStatePref(STATE::MOUSEKEY) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetMouseAutoClick(const int32_t time)
{
    HILOG_DEBUG("time = [%{public}d]", time);
    mouseAutoClick_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutInt("MouseAutoClick", mouseAutoClick_);
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetShortkeyTarget(const std::string &name)
{
    HILOG_DEBUG("name = [%{public}s]", name.c_str());
    shortkeyTarget_ = name;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutString("ShortkeyTarget", shortkeyTarget_);
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetHighContrastTextState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    highContrastTextState_ = state;
    return SetStatePref(STATE::HIGHCONTRASTTEXT) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetInvertColorState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    invertColorState_ = state;
    return SetStatePref(STATE::INVERTCOLORSTATE) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetAnimationOffState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    animationOffState_ = state;
    return SetStatePref(STATE::ANIMATIONOFF) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetAudioMonoState(const bool state)
{
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    audioMonoState_ = state;
    return SetStatePref(STATE::AUDIOMONO) ? RET_OK : RET_ERR_FAILED;
}

RetError AccessibilitySettingsConfig::SetDaltonizationColorFilter(const uint32_t filter)
{
    HILOG_DEBUG("filter = [%{public}u]", filter);
    daltonizationColorFilter_ = filter;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutInt("daltonizationColorFilter", static_cast<int32_t>(daltonizationColorFilter_));
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetContentTimeout(const uint32_t time)
{
    HILOG_DEBUG("time = [%{public}u]", time);
    contentTimeout_ = time;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutInt("contentTimeout", static_cast<int32_t>(contentTimeout_));
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetBrightnessDiscount(const float discount)
{
    HILOG_DEBUG("discount = [%{public}f]", discount);
    brightnessDiscount_ = discount;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutFloat("brightnessDiscount", brightnessDiscount_);
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetAudioBalance(const float balance)
{
    HILOG_DEBUG("balance = [%{public}f]", balance);
    audioBalance_ = balance;
    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }

    pref_->PutFloat("audioBalance", audioBalance_);
    pref_->Flush();
    return RET_OK;
}

RetError AccessibilitySettingsConfig::SetCaptionProperty(const AccessibilityConfig::CaptionProperty& caption)
{
    HILOG_DEBUG();
    captionProperty_ = caption;

    if (!pref_) {
        HILOG_ERROR("pref_ is null!");
        return RET_ERR_NULLPTR;
    }
    const std::string& FONTFAMILY = captionProperty_.GetFontFamily();
    int32_t FONTSCALE = captionProperty_.GetFontScale();
    uint32_t FONTCOLOR = captionProperty_.GetFontColor();
    const std::string& FONTEDGETYPE = captionProperty_.GetFontEdgeType();
    uint32_t BACKGROUNDCOLOR = captionProperty_.GetBackgroundColor();
    uint32_t WINDOWCOLOR = captionProperty_.GetWindowColor();

    pref_->PutString("fontFamily", FONTFAMILY);
    pref_->PutInt("fontColor", static_cast<int32_t>(FONTCOLOR));
    pref_->PutString("fontEdgeType", FONTEDGETYPE);
    pref_->PutInt("backgroundColor", static_cast<int32_t>(BACKGROUNDCOLOR));
    pref_->PutInt("windowColor", static_cast<int32_t>(WINDOWCOLOR));
    pref_->PutInt("fontScale", FONTSCALE);
    pref_->Flush();
    return RET_OK;
}

bool AccessibilitySettingsConfig::SetStatePref(int32_t type)
{
    HILOG_DEBUG("type = [%{public}d]", type);
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
    HILOG_DEBUG("state = [%{public}s]", state ? "True" : "False");
    return state ? "on" : "off";
}

bool AccessibilitySettingsConfig::GetCaptionState() const
{
    return isCaptionState_;
}

bool AccessibilitySettingsConfig::GetScreenMagnificationState() const
{
    return isScreenMagnificationState_;
}

bool AccessibilitySettingsConfig::GetShortKeyState() const
{
    return isShortKeyState_;
}

bool AccessibilitySettingsConfig::GetMouseKeyState() const
{
    return isMouseKeyState_;
}

int32_t AccessibilitySettingsConfig::GetMouseAutoClick() const
{
    return mouseAutoClick_;
}

const std::string &AccessibilitySettingsConfig::GetShortkeyTarget() const
{
    return shortkeyTarget_;
}

bool AccessibilitySettingsConfig::GetHighContrastTextState() const
{
    return highContrastTextState_;
}

bool AccessibilitySettingsConfig::GetInvertColorState() const
{
    return invertColorState_;
}

bool AccessibilitySettingsConfig::GetAnimationOffState() const
{
    return animationOffState_;
}

bool AccessibilitySettingsConfig::GetAudioMonoState() const
{
    return audioMonoState_;
}

uint32_t AccessibilitySettingsConfig::GetDaltonizationColorFilter() const
{
    return daltonizationColorFilter_;
}

uint32_t AccessibilitySettingsConfig::GetContentTimeout() const
{
    return contentTimeout_;
}

float AccessibilitySettingsConfig::GetBrightnessDiscount() const
{
    return brightnessDiscount_;
}

float AccessibilitySettingsConfig::GetAudioBalance() const
{
    return audioBalance_;
}

bool AccessibilitySettingsConfig::GetEnabledState() const
{
    return enabled_;
}

bool AccessibilitySettingsConfig::GetTouchGuideState() const
{
    return eventTouchGuideState_;
}

bool AccessibilitySettingsConfig::GetGestureState() const
{
    return gesturesSimulation_;
}

bool AccessibilitySettingsConfig::GetKeyEventObserverState() const
{
    return filteringKeyEvents_;
}

const AccessibilityConfig::CaptionProperty &AccessibilitySettingsConfig::GetCaptionProperty() const
{
    return captionProperty_;
};

uint32_t AccessibilitySettingsConfig::GetConfigState()
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();
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
    HILOG_DEBUG("fontFamily = %{public}s.", FONTFAMILY.c_str());

    int32_t FONTSCALE =  static_cast<int32_t>(pref_->GetInt("fontScale", 0));
    HILOG_DEBUG("fontScale = %{public}d.", FONTSCALE);

    uint32_t FONTCOLOR = static_cast<uint32_t>(pref_->GetInt("fontColor", DEFAULT_COLOR));
    HILOG_DEBUG("fontColor = 0x%{public}x.", FONTCOLOR);

    std::string FONTEDGETYPE = pref_->GetString("fontEdgeType", "none");
    HILOG_DEBUG("FONTEDGETYPE = 0x%{public}s.", FONTEDGETYPE.c_str());

    uint32_t BACKGROUNDCOLOR = static_cast<uint32_t>(pref_->GetInt("backgroundColor", DEFAULT_COLOR));
    HILOG_DEBUG("BACKGROUNDCOLOR = 0x%{public}x.", BACKGROUNDCOLOR);

    uint32_t WINDOWCOLOR = static_cast<uint32_t>(pref_->GetInt("windowColor", DEFAULT_COLOR));
    HILOG_DEBUG("WINDOWCOLOR = 0x%{public}x.", WINDOWCOLOR);

    captionProperty_.SetFontFamily(FONTFAMILY);
    captionProperty_.SetFontScale(FONTSCALE);
    captionProperty_.SetFontColor(FONTCOLOR);
    captionProperty_.SetFontEdgeType(FONTEDGETYPE);
    captionProperty_.SetBackgroundColor(BACKGROUNDCOLOR);
    captionProperty_.SetWindowColor(WINDOWCOLOR);
}

void AccessibilitySettingsConfig::InitSetting()
{
    HILOG_DEBUG();
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
    brightnessDiscount_ = static_cast<float>(pref_->GetFloat("brightnessDiscount", 1.0));
    audioBalance_ = static_cast<float>(pref_->GetFloat("audioBalance", 0));
}

void AccessibilitySettingsConfig::InitCapability()
{
    HILOG_DEBUG();
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("accessible", "");
    HILOG_DEBUG("accessible = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        enabled_ = true;
    } else {
        enabled_ = false;
    }

    strValue = pref_->GetString("touchGuide", "");
    HILOG_DEBUG("touchGuide = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        eventTouchGuideState_ = true;
    } else {
        eventTouchGuideState_ = false;
    }

    strValue = pref_->GetString("gesture", "");
    HILOG_DEBUG("gesture = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        gesturesSimulation_ = true;
    } else {
        gesturesSimulation_ = false;
    }

    strValue = pref_->GetString("keyEventObserver", "");
    HILOG_DEBUG("keyEventObserver = %{public}s", strValue.c_str());
    if (!std::strcmp(strValue.c_str(), "on")) {
        filteringKeyEvents_ = true;
    } else {
        filteringKeyEvents_ = false;
    }
}

void AccessibilitySettingsConfig::InitEnabledList()
{
    HILOG_DEBUG();
    if (!pref_) {
        HILOG_ERROR("Input Parameter is nullptr");
        return;
    }

    std::string strValue = pref_->GetString("BundleName/AbilityName/Capabilities", "");
    HILOG_DEBUG("Capabilities = %{public}s", strValue.c_str());

    StringToVector(strValue, enabledAbilityInfos_);
}

void AccessibilitySettingsConfig::Init()
{
    HILOG_INFO();
    int errCode = -1;

    pref_ = NativePreferences::PreferencesHelper::GetPreferences(PREF_PATH + std::to_string(accountId_) + ".xml",
                                                                 errCode);
    if (errCode) {
        Utils::RecordUnavailableEvent(A11yUnavailableEvent::READ_EVENT, A11yError::ERROR_READ_FAILED);
        HILOG_ERROR("GetPreferences failed! account id (%{public}d), errCode(%{public}d).", accountId_, errCode);
        return;
    }

    InitCaption();
    InitSetting();
}

void AccessibilitySettingsConfig::VectorToString(const std::vector<std::string> &vectorVal, std::string &stringOut)
{
    HILOG_DEBUG();
    int32_t i = 0;
    for (auto& var : vectorVal) {
        if (i > 0) {
            stringOut = stringOut + ',';
        }
        stringOut = stringOut + var.c_str();
        i++;
    }
    HILOG_DEBUG("stringOUT = %{public}s .", stringOut.c_str());
}

void AccessibilitySettingsConfig::StringToVector(const std::string &stringIn, std::vector<std::string> &vectorResult)
{
    HILOG_DEBUG();
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
}

const std::vector<std::string> &AccessibilitySettingsConfig::GetEnabledAbilityInfos()
{
    HILOG_DEBUG();
    return enabledAbilityInfos_;
}

void AccessibilitySettingsConfig::UpdateEnabledAbilities(const std::vector<std::string> &vecvalue)
{
    HILOG_DEBUG();
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
    HILOG_DEBUG();
    int errCode = -1;
    errCode = NativePreferences::PreferencesHelper::DeletePreferences(PREF_PATH + std::to_string(accountId_) + ".xml");
    if (errCode) {
        HILOG_ERROR("DeletePreferences failed! account id (%{public}d)", accountId_);
    }

    pref_ = nullptr;
}
} // namespace Accessibility
} // namespace OHOS