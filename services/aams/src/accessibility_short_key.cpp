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

#include "accessibility_short_key.h"
#include "accessible_ability_manager_service.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace Accessibility {
namespace {
    constexpr int32_t SHORTCUT_TIMEOUT = 3000; // ms
} // namespace

AccessibilityShortKey::~AccessibilityShortKey()
{
    HILOG_INFO();

    if (subscribeId_ < 0) {
        return;
    }

    MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId_);
}

void AccessibilityShortKey::SubscribeShortKey(std::set<int32_t> preKeys, int32_t finalKey, int32_t holdTime)
{
    std::shared_ptr<MMI::KeyOption> keyOption = std::make_shared<MMI::KeyOption>();
    if (keyOption == nullptr) {
        HILOG_ERROR("Create keyOption failed.");
        return;
    }

    keyOption->SetPreKeys(preKeys);
    keyOption->SetFinalKey(finalKey);
    keyOption->SetFinalKeyDown(true);
    // MMI will get the real holdTime in SettingsData, so the input parameter may not take effect here.
    keyOption->SetFinalKeyDownDuration(holdTime);

    auto keyEventCallBack = std::bind(&AccessibilityShortKey::OnShortKey, this);
    int32_t subscribeId = MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption, keyEventCallBack);
    subscribeId_ = subscribeId;
    if (subscribeId < 0) {
        HILOG_ERROR("Subscribe key event failed, finalKey: %{public}d id: %{public}d", finalKey, subscribeId);
        return;
    }
}

void AccessibilityShortKey::Register()
{
    HILOG_INFO();

    std::set<int32_t> preDownKeysUp;
    preDownKeysUp.insert(MMI::KeyEvent::KEYCODE_VOLUME_UP);
    SubscribeShortKey(preDownKeysUp, MMI::KeyEvent::KEYCODE_VOLUME_DOWN, SHORTCUT_TIMEOUT);
}

void AccessibilityShortKey::OnShortKey()
{
    HILOG_INFO();

    Singleton<AccessibleAbilityManagerService>::GetInstance().OnShortKeyProcess();
}
} // namespace Accessibility
} // namespace OHOS
