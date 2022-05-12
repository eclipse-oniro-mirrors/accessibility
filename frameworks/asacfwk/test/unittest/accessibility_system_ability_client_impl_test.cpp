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

#include <gtest/gtest.h>
#include <memory>
#include "accessibility_system_ability_client_impl.h"
#include "accessible_ability_manager_service.h"
#include "mock_accessibility_element_operator.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

const static int32_t WINDOW_ID = 1;
const static int32_t ACCOUNT_ID = 1;
const static uint32_t ACCESSIBILITY_ABILITY_TYPES = 1;
const static int32_t COMPONENT_ID = 1;
const static uint32_t EVENT_TYPE = 1;
const static int32_t TYPE = 1;
const static uint32_t STATE_TYPE = 1;

namespace OHOS {
namespace Accessibility {
class AccessibilitySystemAbilityClientImplTest : public ::testing::Test {
public:
    AccessibilitySystemAbilityClientImplTest()
    {}
    ~AccessibilitySystemAbilityClientImplTest()
    {}
    std::shared_ptr<AccessibilitySystemAbilityClientImpl> impl_ = nullptr;

    static void SetUpTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStart();
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(ABILITY_MGR_SERVICE_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnAddSystemAbility(WINDOW_MANAGER_SERVICE_ID, "");
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest Start";
    }
    static void TearDownTestCase()
    {
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnStop();
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(ABILITY_MGR_SERVICE_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(COMMON_EVENT_SERVICE_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, "");
        Singleton<AccessibleAbilityManagerService>::GetInstance().OnRemoveSystemAbility(WINDOW_MANAGER_SERVICE_ID, "");
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest End";
    }
    void SetUp()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest SetUp()";
        impl_ = std::make_shared<AccessibilitySystemAbilityClientImpl>();
    };
    void TearDown()
    {
        GTEST_LOG_(INFO) << "AccessibilitySystemAbilityClientImplTest TearDown()";
        impl_ = nullptr;
    }
};

/**
 * @tc.number: RegisterElementOperator_001
 * @tc.name: RegisterElementOperator
 * @tc.desc: Test function RegisterElementOperator
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, RegisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityElementOperator> operator_ = std::make_shared<MockAccessibilityElementOperator>();
    EXPECT_EQ(0, impl_->RegisterElementOperator(WINDOW_ID, operator_, ACCOUNT_ID));
    impl_->DeregisterElementOperator(WINDOW_ID);
    GTEST_LOG_(INFO) << "RegisterElementOperator_001 end";
}

/**
 * @tc.number: DeregisterElementOperator_001
 * @tc.name: DeregisterElementOperator
 * @tc.desc: Test function DeregisterElementOperator
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, DeregisterElementOperator_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityElementOperator> operator_ = std::make_shared<MockAccessibilityElementOperator>();
    impl_->RegisterElementOperator(WINDOW_ID, operator_, ACCOUNT_ID);
    impl_->DeregisterElementOperator(WINDOW_ID);
    GTEST_LOG_(INFO) << "DeregisterElementOperator_001 end";
}

/**
 * @tc.number: GetAbilityList_001
 * @tc.name: GetAbilityList
 * @tc.desc: Test function GetAbilityList
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetAbilityList_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetAbilityList_001 start";
    AbilityStateType stateType = ABILITY_STATE_INVALID;
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }

    std::vector<AccessibilityAbilityInfo> infos {};
    infos = impl_->GetAbilityList(ACCESSIBILITY_ABILITY_TYPES, stateType);
    EXPECT_EQ(0, infos.size());
    GTEST_LOG_(INFO) << "GetAbilityList_001 end";
}

/**
 * @tc.number: IsAccessibilityCaptionEnabled_001
 * @tc.name: IsAccessibilityCaptionEnabled
 * @tc.desc: Test function IsAccessibilityCaptionEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsAccessibilityCaptionEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsAccessibilityCaptionEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_TRUE(impl_->IsAccessibilityCaptionEnabled());
    GTEST_LOG_(INFO) << "IsAccessibilityCaptionEnabled_001 end";
}

/**
 * @tc.number: IsEnabled_001
 * @tc.name: IsEnabled
 * @tc.desc: Test function IsEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_FALSE(impl_->IsEnabled());
    GTEST_LOG_(INFO) << "IsEnabled_001 end";
}

/**
 * @tc.number: IsTouchExplorationEnabled_001
 * @tc.name: IsTouchExplorationEnabled
 * @tc.desc: Test function IsTouchExplorationEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsTouchExplorationEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_FALSE(impl_->IsTouchExplorationEnabled());
    GTEST_LOG_(INFO) << "IsTouchExplorationEnabled_001 end";
}

/**
 * @tc.number: IsCaptionEnabled_001
 * @tc.name: IsCaptionEnabled
 * @tc.desc: Test function IsCaptionEnabled
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, IsCaptionEnabled_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "IsCaptionEnabled_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_FALSE(impl_->IsCaptionEnabled());
    GTEST_LOG_(INFO) << "IsCaptionEnabled_001 end";
}

/**
 * @tc.number: GetCaptionProperty_001
 * @tc.name: GetCaptionProperty
 * @tc.desc: Test function GetCaptionProperty
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCaptionProperty_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityConfig::CaptionProperty res;
    res = impl_->GetCaptionProperty();
    EXPECT_EQ(0xff000000, res.GetWindowColor());
    EXPECT_EQ(0xff000000, res.GetFontColor());
    EXPECT_EQ(0xff000000, res.GetBackgroundColor());
    GTEST_LOG_(INFO) << "GetCaptionProperty_001 end";
}

/**
 * @tc.number: SetCaptionProperty_001
 * @tc.name: SetCaptionProperty
 * @tc.desc: Test function SetCaptionProperty
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SetCaptionProperty_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityConfig::CaptionProperty caption;
    EXPECT_TRUE(impl_->SetCaptionProperty(caption));
    GTEST_LOG_(INFO) << "SetCaptionProperty_001 end";
}

/**
 * @tc.number: SendEvent_001
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SendEvent_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendEvent_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EventType eventType = TYPE_VIEW_LONG_CLICKED_EVENT;
    EXPECT_TRUE(impl_->SendEvent(eventType, COMPONENT_ID));
    GTEST_LOG_(INFO) << "SendEvent_001 end";
}

/**
 * @tc.number: SendEvent_002
 * @tc.name: SendEvent
 * @tc.desc: Test function SendEvent
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SendEvent_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SendEvent_002 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityEventInfo event {};
    EXPECT_FALSE(impl_->SendEvent(event));
    GTEST_LOG_(INFO) << "SendEvent_002 end";
}

/**
 * @tc.number: SubscribeStateObserver_001
 * @tc.name: SubscribeStateObserver
 * @tc.desc: Test function SubscribeStateObserver
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SubscribeStateObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SubscribeStateObserver_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityStateObserver> observer = nullptr;
    EXPECT_FALSE(impl_->SubscribeStateObserver(observer, EVENT_TYPE));
    impl_->UnsubscribeStateObserver(observer, EVENT_TYPE);
    GTEST_LOG_(INFO) << "SubscribeStateObserver_001 end";
}

/**
 * @tc.number: UnsubscribeStateObserver_001
 * @tc.name: UnsubscribeStateObserver
 * @tc.desc: Test function UnsubscribeStateObserver
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, UnsubscribeStateObserver_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UnsubscribeStateObserver_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityStateObserver> observer = nullptr;
    impl_->SubscribeStateObserver(observer, EVENT_TYPE);
    EXPECT_FALSE(impl_->UnsubscribeStateObserver(observer, EVENT_TYPE));
    GTEST_LOG_(INFO) << "UnsubscribeStateObserver_001 end";
}

/**
 * @tc.number: AddCaptionListener_001
 * @tc.name: AddCaptionListener
 * @tc.desc: Test function AddCaptionListener
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, AddCaptionListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AddCaptionListener_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityConfig::CaptionObserver> ob = nullptr;
    EXPECT_TRUE(impl_->AddCaptionListener(ob, TYPE));
    impl_->DeleteCaptionListener(ob, TYPE);
    GTEST_LOG_(INFO) << "AddCaptionListener_001 end";
}

/**
 * @tc.number: DeleteCaptionListener_001
 * @tc.name: DeleteCaptionListener
 * @tc.desc: Test DeleteCaptionListener
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, DeleteCaptionListener_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeleteCaptionListener_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    std::shared_ptr<AccessibilityConfig::CaptionObserver> ob = nullptr;
    impl_->AddCaptionListener(ob, TYPE);
    EXPECT_TRUE(impl_->DeleteCaptionListener(ob, TYPE));
    GTEST_LOG_(INFO) << "DeleteCaptionListener_001 end";
}

/**
 * @tc.number: GetCapabilitiesState_001
 * @tc.name: GetEnabledState
 * @tc.desc: Test function GetEnabledState
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetCapabilitiesState_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetCapabilitiesState_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_TRUE(impl_->GetEnabledState());
    EXPECT_TRUE(impl_->GetTouchGuideState());
    EXPECT_TRUE(impl_->GetGestureState());
    EXPECT_TRUE(impl_->GetKeyEventObserverState());
    GTEST_LOG_(INFO) << "GetCapabilitiesState_001 end";
}

/**
 * @tc.number: SetCaptionPropertyTojson_001
 * @tc.name: SetCaptionPropertyTojson
 * @tc.desc: Test function SetCaptionPropertyTojson
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SetCaptionPropertyTojson_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaptionPropertyTojson_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityConfig::CaptionProperty caption;
    caption.SetFontScale(101);
    EXPECT_TRUE(impl_->SetCaptionPropertyTojson(caption));
    GTEST_LOG_(INFO) << "SetCaptionPropertyTojson_001 end";
}

/**
 * @tc.number: SetCaption_001
 * @tc.name: SetCaptionStateTojson
 * @tc.desc: Test function SetCaptionStateTojson
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, SetCaption_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SetCaption_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    EXPECT_TRUE(impl_->SetCaptionStateTojson(true));
    GTEST_LOG_(INFO) << "SetCaption_001 end";
}

/**
 * @tc.number: ResetService_001
 * @tc.name: ResetService
 * @tc.desc: Test function ResetService
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, ResetService_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ResetService_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    wptr<IRemoteObject> remote = nullptr;
    impl_->ResetService(remote);
    GTEST_LOG_(INFO) << "ResetService_001 end";
}

/**
 * @tc.number: OnAccessibleAbilityManagerStateChanged_001
 * @tc.name: OnAccessibleAbilityManagerStateChanged
 * @tc.desc: Test function OnAccessibleAbilityManagerStateChanged
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, OnAccessibleAbilityManagerStateChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerStateChanged_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    impl_->OnAccessibleAbilityManagerStateChanged(STATE_TYPE);
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerStateChanged_001 end";
}

/**
 * @tc.number: OnAccessibleAbilityManagerCaptionPropertyChanged_001
 * @tc.name: OnAccessibleAbilityManagerCaptionPropertyChanged_001
 * @tc.desc: Test function OnAccessibleAbilityManagerCaptionPropertyChanged_001
 */
HWTEST_F(
    AccessibilitySystemAbilityClientImplTest, OnAccessibleAbilityManagerCaptionPropertyChanged_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerCaptionPropertyChanged_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    AccessibilityConfig::CaptionProperty caption;
    impl_->OnAccessibleAbilityManagerCaptionPropertyChanged(caption);
    GTEST_LOG_(INFO) << "OnAccessibleAbilityManagerCaptionPropertyChanged_001 end";
}

/**
 * @tc.number: GetInstalledAbilities_001
 * @tc.name: GetInstalledAbilities
 * @tc.desc: Test function GetInstalledAbilities
 */
HWTEST_F(AccessibilitySystemAbilityClientImplTest, GetInstalledAbilities_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "GetInstalledAbilities_001 start";
    if (!impl_) {
        GTEST_LOG_(INFO) << "Cann't get AccessibilitySystemAbilityClientImpl impl_";
        return;
    }
    impl_->GetInstalledAbilities();
    GTEST_LOG_(INFO) << "GetInstalledAbilities_001 end";
}
} // namespace Accessibility
} // namespace OHOS