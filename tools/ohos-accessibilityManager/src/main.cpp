/*
 * Copyright (C) 2022-2026 Huawei Device Co., Ltd.
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

// LCOV_EXCL_START
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include "accessibility_config.h"
#include "accessibility_system_ability_client.h"
#include "accessibility_def.h"

using namespace OHOS;
using namespace Accessibility;

constexpr int32_t ARGC_COUNT_TWO = 2;

void PrintHelp(const std::string& subcommand = "")
{
    if (subcommand.empty()) {
        std::cerr << "OHOS Accessibility Manager CLI Tool" << std::endl;
        std::cerr << "Usage: ohos-a11yManager <command> [options]" << std::endl;
        std::cerr << "       ohos-a11yManager <command> --help  Show help for a specific command" << std::endl;
        std::cerr << std::endl;
        std::cerr << "State Commands:" << std::endl;
        std::cerr << "  state-is-screen-reader-enabled      Check if screen reader is enabled" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Ability Commands:" << std::endl;
        std::cerr << "  ability-enable-screen-reader          Enable screen reader" << std::endl;
        std::cerr << "  ability-disable-screen-reader         Disable screen reader" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Magnification Commands:" << std::endl;
        std::cerr << "  magnification-set-state --state     Set screen magnification state" << std::endl;
        std::cerr << "  magnification-get-state             Get screen magnification state" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Shortkey Commands:" << std::endl;
        std::cerr << "  shortkey-set-state --state          Set shortkey state" << std::endl;
        std::cerr << "  shortkey-get-state                  Get shortkey state" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Contrast Commands:" << std::endl;
        std::cerr << "  high-contrast-set-state --state          Set high contrast text state" << std::endl;
        std::cerr << "  high-contrast-get-state                  Get high contrast text state" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Invert Commands:" << std::endl;
        std::cerr << "  invert-color-set-state --state            Set invert color state" << std::endl;
        std::cerr << "  invert-color-get-state                    Get invert color state" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Animation Commands:" << std::endl;
        std::cerr << "  animation-off-set-state --state         Set animation off state" << std::endl;
        std::cerr << "  animation-off-get-state                 Get animation off state" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Audio Commands:" << std::endl;
        std::cerr << "  audio-set-mono --state              Set audio mono state" << std::endl;
        std::cerr << "  audio-get-mono                      Get audio mono state" << std::endl;
        std::cerr << "  audio-set-balance --balance         Set audio balance" << std::endl;
        std::cerr << "  audio-get-balance                   Get audio balance" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Daltonization Commands:" << std::endl;
        std::cerr << "  daltonization-set-state --state     Set daltonization state" << std::endl;
        std::cerr << "  daltonization-get-state             Get daltonization state" << std::endl;
        std::cerr << "  daltonization-set-filter --type     Set daltonization filter type" << std::endl;
        std::cerr << "  daltonization-get-filter            Get daltonization filter type" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Click Commands:" << std::endl;
        std::cerr << "  click-set-response-time --time      Set click response time" << std::endl;
        std::cerr << "  click-get-response-time             Get click response time" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Ignore Repeat Click Commands:" << std::endl;
        std::cerr << "  ignore-repeat-click-set-state --state      Set ignore repeat click state" << std::endl;
        std::cerr << "  ignore-repeat-click-get-state              Get ignore repeat click state" << std::endl;
        std::cerr << "  ignore-repeat-click-set-time --interval    Set ignore repeat click interval" << std::endl;
        std::cerr << "  ignore-repeat-click-get-time               Get ignore repeat click time" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  --state     true/false" << std::endl;
        std::cerr << "  --time      Time value (0=short(default), 1=medium, 2=long)" << std::endl;
        std::cerr << "  --interval  Interval value (0=0.1s, 1=0.4s, 2=0.7s, 3=1.0s, 4=1.3s)" << std::endl;
        std::cerr << "  --balance   Audio balance value (float)" << std::endl;
        std::cerr << "  --type      Filter type (0=Normal, 1=Protanomaly, 2=Deuteranomaly, 3=Tritanomaly)" << std::endl;
        std::cerr << "  --help, -h  Show this help message" << std::endl;
    } else {
        std::cerr << "Command: " << subcommand << std::endl;
        std::cerr << "Usage: ohos-a11yManager " << subcommand << " [options]" << std::endl;
        std::cerr << std::endl;

        if (subcommand == "magnification-set-state" || subcommand == "shortkey-set-state" ||
            subcommand == "high-contrast-set-state" || subcommand == "invert-color-set-state" ||
            subcommand == "animation-off-set-state" || subcommand == "audio-set-mono" ||
            subcommand == "daltonization-set-state" || subcommand == "ignore-repeat-click-set-state") {
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --state     true/false   Enable or disable the feature" << std::endl;
        } else if (subcommand == "audio-set-balance") {
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --balance   float   Audio balance value" << std::endl;
        } else if (subcommand == "daltonization-set-filter") {
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --type      int   Filter type (0=Normal, 1=Protanomaly, 2=Deuteranomaly, 3=Tritanomaly)" << std::endl;
        } else if (subcommand == "click-set-response-time") {
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --time      int   Click response time (0=short(default), 1=medium, 2=long)" << std::endl;
        } else if (subcommand == "ignore-repeat-click-set-time") {
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --interval  int   Interval value (0=0.1s, 1=0.4s, 2=0.7s, 3=1.0s, 4=1.3s)" << std::endl;
        } else {
            std::cerr << "This command has no options." << std::endl;
        }
        std::cerr << std::endl;
        std::cerr << "Output:" << std::endl;
        std::cerr << "  JSON format with status and data fields" << std::endl;
    }
}

// Parsing parameters
std::string GetOption(const std::vector<std::string>& args, const std::string& option)
{
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == option && i + 1 < args.size()) {
            return args[i + 1];
        }
    }
    return "";
}

bool HasOption(const std::vector<std::string>& args, const std::string& option)
{
    for (const auto& arg : args) {
        if (arg == option) {
            return true;
        }
    }
    return false;
}

// JSON Output Helper Function
int OutputSuccess(const std::string& data)
{
    nlohmann::json root;
    root["type"] = "result";
    root["status"] = "success";

    if (nlohmann::json::accept(data)) {
        root["data"] = nlohmann::json::parse(data);
    } else {
        root["data"] = data;
    }

    std::cout << root.dump() << std::endl;
    return 0;
}

int OutputError(const std::string& code, const std::string& message, const std::string& suggestion = "")
{
    nlohmann::json root;
    root["type"] = "result";
    root["status"] = "failed";
    root["data"] = "";
    root["errCode"] = code;
    root["errMsg"] = message;
    root["suggestion"] = suggestion;

    std::cout << root.dump() << std::endl;
    return 1;
}

int CommandStateIsScreenReaderEnabled(const std::vector<std::string>& args)
{
    auto client = AccessibilitySystemAbilityClient::GetInstance();
    bool enabled = false;
    RetError ret = client->IsScreenReaderEnabled(enabled);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"enabled\":" + std::string(enabled ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to check screen reader state. Error: " << ret << std::endl;
    return OutputError("ERR_CHECK_SCREEN_READER", "Failed to check screen reader state", "Please check if accessibility service is running");
}

int CommandAbilityEnable(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string name = "com.ohos.screenreader/AccessibilityExtAbility";
    uint32_t capabilities = 31;

    RetError ret = config.EnableAbility(name, capabilities);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"name\":\"" + name + "\",\"capabilities\":" + std::to_string(capabilities) + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to enable screen reader. Error: " << ret << std::endl;
    return OutputError("ERR_ENABLE_SCREEN_READER", "Failed to enable screen reader", "Please check if you have sufficient permissions");
}

int CommandAbilityDisable(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string name = "com.ohos.screenreader/AccessibilityExtAbility";

    RetError ret = config.DisableAbility(name);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"name\":\"" + name + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to disable screen reader. Error: " << ret << std::endl;
    return OutputError("ERR_DISABLE_SCREEN_READER", "Failed to disable screen reader", "Please check if you have sufficient permissions");
}

int CommandMagnificationSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set magnification state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetScreenMagnificationState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set screen magnification state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_MAGNIFICATION", "Failed to set screen magnification state", "Please check if accessibility service is running");
}

int CommandMagnificationGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetScreenMagnificationState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get screen magnification state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_MAGNIFICATION", "Failed to get screen magnification state", "Please check if accessibility service is running");
}

int CommandShortkeySetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set shortkey state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetShortKeyState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set shortkey state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_SHORTKEY", "Failed to set shortkey state", "Please check if accessibility service is running");
}

int CommandShortkeyGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetShortKeyState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get shortkey state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_SHORTKEY", "Failed to get shortkey state", "Please check if accessibility service is running");
}

int CommandContrastSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set high contrast state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetHighContrastTextState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set high contrast state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_HIGH_CONTRAST", "Failed to set high contrast state", "Please check if accessibility service is running");
}

int CommandContrastGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetHighContrastTextState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get high contrast state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_HIGH_CONTRAST", "Failed to get high contrast state", "Please check if accessibility service is running");
}

int CommandInvertSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set invert color state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetInvertColorState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set invert color state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_INVERT_COLOR", "Failed to set invert color state", "Please check if accessibility service is running");
}

int CommandInvertGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetInvertColorState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get invert color state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_INVERT_COLOR", "Failed to get invert color state", "Please check if accessibility service is running");
}

int CommandAnimationSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set animation off state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetAnimationOffState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set animation off state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_ANIMATION_OFF", "Failed to set animation off state", "Please check if accessibility service is running");
}

int CommandAnimationGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetAnimationOffState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":\"" + std::string(state ? "true" : "false") + "\"}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get animation off state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_ANIMATION_OFF", "Failed to get animation off state", "Please check if accessibility service is running");
}

int CommandAudioSetMono(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set audio mono state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetAudioMonoState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set audio mono state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_AUDIO_MONO", "Failed to set audio mono state", "Please check if accessibility service is running");
}

int CommandAudioGetMono(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetAudioMonoState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get audio mono state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_AUDIO_MONO", "Failed to get audio mono state", "Please check if accessibility service is running");
}

int CommandAudioSetBalance(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string balanceStr = GetOption(args, "--balance");
    if (balanceStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set audio balance failed: Missing required parameter. Parameter: --balance", "Please provide balance value using --balance parameter");
    }

    float balance = std::stof(balanceStr);
    RetError ret = config.SetAudioBalance(balance);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"balance\":" + balanceStr + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set audio balance. Error: " << ret << std::endl;
    return OutputError("ERR_SET_AUDIO_BALANCE", "Failed to set audio balance", "Please check if the balance value is valid");
}

int CommandAudioGetBalance(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    float balance = 0.0f;
    RetError ret = config.GetAudioBalance(balance);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"balance\":" + std::to_string(balance) + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get audio balance. Error: " << ret << std::endl;
    return OutputError("ERR_GET_AUDIO_BALANCE", "Failed to get audio balance", "Please check if accessibility service is running");
}

int CommandDaltonizationSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set daltonization state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetDaltonizationState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set daltonization state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_DALTONIZATION", "Failed to set daltonization state", "Please check if accessibility service is running");
}

int CommandDaltonizationGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetDaltonizationState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get daltonization state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_DALTONIZATION", "Failed to get daltonization state", "Please check if accessibility service is running");
}

int CommandDaltonizationSetFilter(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string typeStr = GetOption(args, "--type");
    if (typeStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set daltonization filter failed: Missing required parameter. Parameter: --type", "Please provide filter type using --type parameter");
    }

    AccessibilityConfig::DALTONIZATION_TYPE type = static_cast<AccessibilityConfig::DALTONIZATION_TYPE>(std::stoi(typeStr));
    RetError ret = config.SetDaltonizationColorFilter(type);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"type\":" + typeStr + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set daltonization filter type. Error: " << ret << std::endl;
    return OutputError("ERR_SET_DALTONIZATION_FILTER", "Failed to set daltonization filter type", "Please check if the filter type is valid");
}

int CommandDaltonizationGetFilter(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    AccessibilityConfig::DALTONIZATION_TYPE type = AccessibilityConfig::DALTONIZATION_TYPE::Normal;
    RetError ret = config.GetDaltonizationColorFilter(type);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"type\":" + std::to_string(static_cast<int>(type)) + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get daltonization filter type. Error: " << ret << std::endl;
    return OutputError("ERR_GET_DALTONIZATION_FILTER", "Failed to get daltonization filter type", "Please check if accessibility service is running");
}

int CommandClickSetResponseTime(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string timeStr = GetOption(args, "--time");
    if (timeStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set click response time failed: Missing required parameter. Parameter: --time", "Please provide time value using --time parameter (0=short(default), 1=medium, 2=long)");
    }

    AccessibilityConfig::CLICK_RESPONSE_TIME time = static_cast<AccessibilityConfig::CLICK_RESPONSE_TIME>(std::stoi(timeStr));
    RetError ret = config.SetClickResponseTime(time);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"time\":" + timeStr + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set click response time. Error: " << ret << std::endl;
    return OutputError("ERR_SET_CLICK_RESPONSE", "Failed to set click response time", "Please check if the time value is valid (0=short(default), 1=medium, 2=long)");
}

int CommandClickGetResponseTime(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    AccessibilityConfig::CLICK_RESPONSE_TIME time = AccessibilityConfig::CLICK_RESPONSE_TIME::ResponseDelayShort;
    RetError ret = config.GetClickResponseTime(time);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"time\":" + std::to_string(static_cast<int>(time)) + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get click response time. Error: " << ret << std::endl;
    return OutputError("ERR_GET_CLICK_RESPONSE", "Failed to get click response time", "Please check if accessibility service is running");
}

int CommandRepeatClickSetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string stateStr = GetOption(args, "--state");
    if (stateStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set ignore repeat click state failed: Missing required parameter. Parameter: --state", "Please provide state value using --state parameter (true/false)");
    }

    bool state = (stateStr == "true");
    RetError ret = config.SetIgnoreRepeatClickState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set ignore repeat click state. Error: " << ret << std::endl;
    return OutputError("ERR_SET_REPEAT_CLICK", "Failed to set ignore repeat click state", "Please check if accessibility service is running");
}

int CommandRepeatClickGetState(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    bool state = false;
    RetError ret = config.GetIgnoreRepeatClickState(state);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"state\":" + std::string(state ? "true" : "false") + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get ignore repeat click state. Error: " << ret << std::endl;
    return OutputError("ERR_GET_REPEAT_CLICK", "Failed to get ignore repeat click state", "Please check if accessibility service is running");
}

int CommandRepeatClickSetTime(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    std::string intervalStr = GetOption(args, "--interval");
    if (intervalStr.empty()) {
        return OutputError("ERR_ARG_MISSING", "Set ignore repeat click time failed: Missing required parameter. Parameter: --interval", "Please provide interval value using --interval parameter (0=0.1s, 1=0.4s, 2=0.7s, 3=1.0s, 4=1.3s)");
    }

    AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME time = static_cast<AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME>(std::stoi(intervalStr));
    RetError ret = config.SetIgnoreRepeatClickTime(time);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"interval\":" + intervalStr + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to set ignore repeat click time. Error: " << ret << std::endl;
    return OutputError("ERR_SET_REPEAT_CLICK_TIME", "Failed to set ignore repeat click time", "Please check if the interval value is valid (0=0.1s, 1=0.4s, 2=0.7s, 3=1.0s, 4=1.3s)");
}

int CommandRepeatClickGetTime(const std::vector<std::string>& args)
{
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME time = AccessibilityConfig::IGNORE_REPEAT_CLICK_TIME::RepeatClickTimeoutShortest;
    RetError ret = config.GetIgnoreRepeatClickTime(time);
    if (ret == RetError::RET_OK) {
        std::string data = "{\"interval\":" + std::to_string(static_cast<int>(time)) + "}";
        return OutputSuccess(data);
    }
    std::cerr << "Failed to get ignore repeat click time. Error: " << ret << std::endl;
    return OutputError("ERR_GET_REPEAT_CLICK_TIME", "Failed to get ignore repeat click time", "Please check if accessibility service is running");
}

typedef int (*CommandHandler)(const std::vector<std::string>&);

std::unordered_map<std::string, CommandHandler> g_commands = {
    {"state-is-screen-reader-enabled", CommandStateIsScreenReaderEnabled},
    {"ability-enable-screen-reader", CommandAbilityEnable},
    {"ability-disable-screen-reader", CommandAbilityDisable},
    {"magnification-set-state", CommandMagnificationSetState},
    {"magnification-get-state", CommandMagnificationGetState},
    {"shortkey-set-state", CommandShortkeySetState},
    {"shortkey-get-state", CommandShortkeyGetState},
    {"high-contrast-set-state", CommandContrastSetState},
    {"high-contrast-get-state", CommandContrastGetState},
    {"invert-color-set-state", CommandInvertSetState},
    {"invert-color-get-state", CommandInvertGetState},
    {"animation-off-set-state", CommandAnimationSetState},
    {"animation-off-get-state", CommandAnimationGetState},
    {"audio-set-mono", CommandAudioSetMono},
    {"audio-get-mono", CommandAudioGetMono},
    {"audio-set-balance", CommandAudioSetBalance},
    {"audio-get-balance", CommandAudioGetBalance},
    {"daltonization-set-state", CommandDaltonizationSetState},
    {"daltonization-get-state", CommandDaltonizationGetState},
    {"daltonization-set-filter", CommandDaltonizationSetFilter},
    {"daltonization-get-filter", CommandDaltonizationGetFilter},
    {"click-set-response-time", CommandClickSetResponseTime},
    {"click-get-response-time", CommandClickGetResponseTime},
    {"ignore-repeat-click-set-state", CommandRepeatClickSetState},
    {"ignore-repeat-click-get-state", CommandRepeatClickGetState},
    {"ignore-repeat-click-set-time", CommandRepeatClickSetTime},
    {"ignore-repeat-click-get-time", CommandRepeatClickGetTime},
};

int main(int argc, char* argv[])
{
    if (argc < ARGC_COUNT_TWO) {
        PrintHelp();
        return 1;
    }

    std::vector<std::string> args;
    for (int i = ARGC_COUNT_TWO; i < argc; i++) {
        args.push_back(argv[i]);
    }

    std::string command = argv[1];

    if (command == "--help" || command == "-h") {
        PrintHelp();
        return 0;
    }

    // Check for subcommand help
    if (HasOption(args, "--help") || HasOption(args, "-h")) {
        PrintHelp(command);
        return 0;
    }

    auto it = g_commands.find(command);
    if (it == g_commands.end()) {
        return OutputError("ERR_UNKNOWN_COMMAND", "Unknown command: " + command, "Run 'ohos-a11yManager --help' for usage information");
    }

    return it->second(args);
}
// LCOV_EXCL_STOP