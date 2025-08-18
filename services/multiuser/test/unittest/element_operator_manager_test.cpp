/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "accessibility_element_info.h"
#include "accessibility_window_connection.h"
#include "iaccessibility_element_operator.h"
#include "iaccessibility_element_operator_callback.h"
#define private public
#define protected public
#include "element_operator_manager.h"
#undef private
#undef protected
#include "mock_element_operator_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Accessibility {
namespace {
constexpr int32_t TEST_WINDOW_ID = 1;
constexpr int32_t TEST_TREE_ID = 2;
constexpr int32_t TEST_ACCOUNT_ID = 100;
constexpr int64_t TEST_ELEMENT_ID = 1000;
constexpr int32_t TEST_TOKEN_ID = 12345;
constexpr int32_t TEST_REQUEST_ID = 999;
}

class ElementOperatorManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        manager_ = std::make_unique<ElementOperatorManager>();
        mockOperator_ = new MockElementOperatorManagerElementOperator();
        mockConnection_ = CreateMockConnection(TEST_WINDOW_ID, TEST_ACCOUNT_ID);
    }

    void TearDown() override
    {
        manager_.reset();
        mockConnection_ = nullptr;
        mockOperator_ = nullptr;
    }

protected:
    std::unique_ptr<ElementOperatorManager> manager_;
    sptr<AccessibilityWindowConnection> mockConnection_;
    sptr<MockElementOperatorManagerElementOperator> mockOperator_;

    sptr<AccessibilityWindowConnection> CreateMockConnection(int32_t windowId, int32_t accountId)
    {
        auto mockOp = new MockElementOperatorManagerElementOperator();
        return new MockElementOperatorManagerWindowConnection(windowId, mockOp, accountId);
    }
};

/**
 * @tc.number: ElementOperatorManager_AddAccessibilityWindowConnection_001
 * @tc.name: AddAccessibilityWindowConnection
 * @tc.desc: Test adding accessibility window connection successfully.
 */
HWTEST_F(ElementOperatorManagerTest, AddAccessibilityWindowConnection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddAccessibilityWindowConnection_001 start";
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    auto connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(connection, mockConnection_);
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddAccessibilityWindowConnection_001 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveAccessibilityWindowConnection_001
 * @tc.name: RemoveAccessibilityWindowConnection
 * @tc.desc: Test removing accessibility window connection successfully.
 */
HWTEST_F(ElementOperatorManagerTest, RemoveAccessibilityWindowConnection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveAccessibilityWindowConnection_001 start";
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), mockConnection_);
    manager_->RemoveAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), nullptr);
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveAccessibilityWindowConnection_001 end";
}

/**
 * @tc.number: ElementOperatorManager_GetAccessibilityWindowConnection_001
 * @tc.name: GetAccessibilityWindowConnection
 * @tc.desc: Test getting accessibility window connection successfully.
 */
HWTEST_F(ElementOperatorManagerTest, GetAccessibilityWindowConnection_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetAccessibilityWindowConnection_001 start";
    auto connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(connection, nullptr);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(connection, mockConnection_);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetAccessibilityWindowConnection_001 end";
}

/**
 * @tc.number: ElementOperatorManager_GetRootParentId_001
 * @tc.name: GetRootParentId
 * @tc.desc: Test getting root parent ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, GetRootParentId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetRootParentId_001 start";
    int64_t parentId = 0;
    ErrCode result = manager_->GetRootParentId(TEST_WINDOW_ID, TEST_TREE_ID, parentId);
    EXPECT_EQ(result, RET_ERR_NO_CONNECTION);

    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    mockConnection_->SetRootParentId(TEST_TREE_ID, 1000);
    result = manager_->GetRootParentId(TEST_WINDOW_ID, TEST_TREE_ID, parentId);
    EXPECT_EQ(result, RET_OK);
    EXPECT_EQ(parentId, 1000);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetRootParentId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_ClearFocus_001
 * @tc.name: ClearFocus
 * @tc.desc: Test clearing focus successfully.
 */
HWTEST_F(ElementOperatorManagerTest, ClearFocus_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearFocus_001 start";
    RetError result = manager_->ClearFocus(TEST_WINDOW_ID);
    EXPECT_EQ(result, RET_ERR_NO_CONNECTION);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    result = manager_->ClearFocus(TEST_WINDOW_ID);
    EXPECT_EQ(result, RET_OK);
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearFocus_001 end";
}

/**
 * @tc.number: ElementOperatorManager_ClearFocus_002
 * @tc.name: ClearFocus
 * @tc.desc: Test clearing focus with no element operator.
 */
HWTEST_F(ElementOperatorManagerTest, ClearFocus_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearFocus_002 start";
    sptr<AccessibilityWindowConnection> nullConnection =
        new MockElementOperatorManagerWindowConnection(TEST_WINDOW_ID, nullptr, TEST_ACCOUNT_ID);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, nullConnection);
    RetError result = manager_->ClearFocus(TEST_WINDOW_ID);
    EXPECT_EQ(result, RET_ERR_NO_CONNECTION);
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearFocus_002 end";
}

/**
 * @tc.number: ElementOperatorManager_OutsideTouch_001
 * @tc.name: OutsideTouch
 * @tc.desc: Test outside touch functionality with valid connection.
 */
HWTEST_F(ElementOperatorManagerTest, OutsideTouch_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_001 start";
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    auto connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_NE(connection, nullptr);
    EXPECT_EQ(connection, mockConnection_);
    EXPECT_NO_THROW(manager_->OutsideTouch(TEST_WINDOW_ID));
    connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_NE(connection, nullptr);
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_001 end";
}

/**
 * @tc.number: ElementOperatorManager_OutsideTouch_002
 * @tc.name: OutsideTouch
 * @tc.desc: Test outside touch with no connection.
 */
HWTEST_F(ElementOperatorManagerTest, OutsideTouch_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_002 start";
    auto connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(connection, nullptr);
    EXPECT_NO_THROW(manager_->OutsideTouch(TEST_WINDOW_ID));
    connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(connection, nullptr);
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_002 end";
}

/**
 * @tc.number: ElementOperatorManager_OutsideTouch_003
 * @tc.name: OutsideTouch
 * @tc.desc: Test outside touch with null proxy.
 */
HWTEST_F(ElementOperatorManagerTest, OutsideTouch_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_003 start";
    sptr<AccessibilityWindowConnection> nullProxyConnection =
        new MockElementOperatorManagerWindowConnection(TEST_WINDOW_ID, nullptr, TEST_ACCOUNT_ID);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, nullProxyConnection);
    auto connection = manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_NE(connection, nullptr);
    EXPECT_NO_THROW(manager_->OutsideTouch(TEST_WINDOW_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_OutsideTouch_003 end";
}

/**
 * @tc.number: ElementOperatorManager_GetAsacConnections_001
 * @tc.name: GetAsacConnections
 * @tc.desc: Test getting all accessibility connections.
 */
HWTEST_F(ElementOperatorManagerTest, GetAsacConnections_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetAsacConnections_001 start";
    const auto& connections = manager_->GetAsacConnections();
    EXPECT_EQ(connections.size(), 0);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    const auto& updatedConnections = manager_->GetAsacConnections();
    EXPECT_EQ(updatedConnections.size(), 1);
    EXPECT_EQ(updatedConnections.at(TEST_WINDOW_ID), mockConnection_);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetAsacConnections_001 end";
}

/**
 * @tc.number: ElementOperatorManager_ClearAllConnections_001
 * @tc.name: ClearAllConnections
 * @tc.desc: Test clearing all connections successfully.
 */
HWTEST_F(ElementOperatorManagerTest, ClearAllConnections_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearAllConnections_001 start";
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID + 1, mockConnection_);
    EXPECT_EQ(manager_->GetAsacConnections().size(), 2);
    manager_->ClearAllConnections();
    EXPECT_EQ(manager_->GetAsacConnections().size(), 0);
    GTEST_LOG_(INFO) << "ElementOperatorManager_ClearAllConnections_001 end";
}

/**
 * @tc.number: ElementOperatorManager_GenerateRequestId_001
 * @tc.name: GenerateRequestId
 * @tc.desc: Test generating request ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, GenerateRequestId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GenerateRequestId_001 start";
    int32_t requestId1 = manager_->GenerateRequestId();
    int32_t requestId2 = manager_->GenerateRequestId();
    EXPECT_NE(requestId1, requestId2);
    EXPECT_GT(requestId1, 0);
    EXPECT_GT(requestId2, 0);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GenerateRequestId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_GetTreeIdBySplitElementId_001
 * @tc.name: GetTreeIdBySplitElementId
 * @tc.desc: Test getting tree ID by split element ID.
 */
HWTEST_F(ElementOperatorManagerTest, GetTreeIdBySplitElementId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetTreeIdBySplitElementId_001 start";
    int64_t elementId = (static_cast<int64_t>(TEST_TREE_ID) << 40) | TEST_ELEMENT_ID;
    int32_t treeId = ElementOperatorManager::GetTreeIdBySplitElementId(elementId);
    EXPECT_EQ(treeId, TEST_TREE_ID);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetTreeIdBySplitElementId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_DeleteConnectionAndDeathRecipient_001
 * @tc.name: DeleteConnectionAndDeathRecipient
 * @tc.desc: Test deleting connection and death recipient successfully.
 */
HWTEST_F(ElementOperatorManagerTest, DeleteConnectionAndDeathRecipient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_DeleteConnectionAndDeathRecipient_001 start";
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), mockConnection_);
    manager_->DeleteConnectionAndDeathRecipient(TEST_WINDOW_ID, mockConnection_);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), nullptr);
    GTEST_LOG_(INFO) << "ElementOperatorManager_DeleteConnectionAndDeathRecipient_001 end";
}

/**
 * @tc.number: ElementOperatorManager_VerifyingToKenId_001
 * @tc.name: VerifyingToKenId
 * @tc.desc: Test verifying token ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, VerifyingToKenId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_VerifyingToKenId_001 start";
    RetError result = manager_->VerifyingToKenId(TEST_TOKEN_ID, TEST_WINDOW_ID, TEST_ELEMENT_ID);
    EXPECT_NE(result, RET_OK);
    GTEST_LOG_(INFO) << "ElementOperatorManager_VerifyingToKenId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_GetParentElementRecursively_001
 * @tc.name: GetParentElementRecursively
 * @tc.desc: Test getting parent element recursively.
 */
HWTEST_F(ElementOperatorManagerTest, GetParentElementRecursively_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetParentElementRecursively_001 start";
    std::vector<AccessibilityElementInfo> infos;
    bool result = manager_->GetParentElementRecursively(TEST_WINDOW_ID, TEST_ELEMENT_ID, infos);
    EXPECT_FALSE(result);
    GTEST_LOG_(INFO) << "ElementOperatorManager_GetParentElementRecursively_001 end";
}

/**
 * @tc.number: ElementOperatorManager_SetElementOperatorDeathRecipient_001
 * @tc.name: SetElementOperatorDeathRecipient
 * @tc.desc: Test setting element operator death recipient successfully.
 */
HWTEST_F(ElementOperatorManagerTest, SetElementOperatorDeathRecipient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_SetElementOperatorDeathRecipient_001 start";
    sptr<IRemoteObject> remoteObject = mockOperator_->AsObject();
    EXPECT_NO_THROW(manager_->SetElementOperatorDeathRecipient(TEST_WINDOW_ID, remoteObject, TEST_ACCOUNT_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_SetElementOperatorDeathRecipient_001 end";
}

/**
 * @tc.number: ElementOperatorManager_SetElementOperatorDeathRecipient_002
 * @tc.name: SetElementOperatorDeathRecipient
 * @tc.desc: Test setting element operator death recipient with tree ID.
 */
HWTEST_F(ElementOperatorManagerTest, SetElementOperatorDeathRecipient_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_SetElementOperatorDeathRecipient_002 start";
    sptr<IRemoteObject> remoteObject = mockOperator_->AsObject();
    EXPECT_NO_THROW(manager_->SetElementOperatorDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID,
        remoteObject, TEST_ACCOUNT_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_SetElementOperatorDeathRecipient_002 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveElementOperatorDeathRecipient_001
 * @tc.name: RemoveElementOperatorDeathRecipient
 * @tc.desc: Test removing element operator death recipient successfully.
 */
HWTEST_F(ElementOperatorManagerTest, RemoveElementOperatorDeathRecipient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveElementOperatorDeathRecipient_001 start";
    sptr<IRemoteObject> remoteObject = mockOperator_->AsObject();
    EXPECT_NO_THROW(manager_->SetElementOperatorDeathRecipient(TEST_WINDOW_ID, remoteObject, TEST_ACCOUNT_ID));
    EXPECT_NO_THROW(manager_->RemoveElementOperatorDeathRecipient(TEST_WINDOW_ID, remoteObject));
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveElementOperatorDeathRecipient_001 end";
}

/**
 * @tc.number: ElementOperatorManager_AddDeathRecipient_001
 * @tc.name: AddDeathRecipient
 * @tc.desc: Test adding death recipient successfully.
 */
HWTEST_F(ElementOperatorManagerTest, AddDeathRecipient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddDeathRecipient_001 start";
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new ElementOperatorManager::ElementOperatorDeathRecipient(TEST_WINDOW_ID, TEST_ACCOUNT_ID);
    EXPECT_NO_THROW(manager_->AddDeathRecipient(TEST_WINDOW_ID, deathRecipient));
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddDeathRecipient_001 end";
}

/**
 * @tc.number: ElementOperatorManager_AddDeathRecipient_002
 * @tc.name: AddDeathRecipient
 * @tc.desc: Test adding death recipient with tree ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, AddDeathRecipient_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddDeathRecipient_002 start";
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new ElementOperatorManager::ElementOperatorDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID, TEST_ACCOUNT_ID);
    EXPECT_NO_THROW(manager_->AddDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID, deathRecipient));
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddDeathRecipient_002 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveDeathRecipient_001
 * @tc.name: RemoveDeathRecipient
 * @tc.desc: Test removing death recipient successfully.
 */
HWTEST_F(ElementOperatorManagerTest, RemoveDeathRecipient_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveDeathRecipient_001 start";
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new ElementOperatorManager::ElementOperatorDeathRecipient(TEST_WINDOW_ID, TEST_ACCOUNT_ID);
    EXPECT_NO_THROW(manager_->AddDeathRecipient(TEST_WINDOW_ID, deathRecipient));
    EXPECT_NO_THROW(manager_->RemoveDeathRecipient(TEST_WINDOW_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveDeathRecipient_001 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveDeathRecipient_002
 * @tc.name: RemoveDeathRecipient
 * @tc.desc: Test removing death recipient with tree ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, RemoveDeathRecipient_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveDeathRecipient_002 start";
    sptr<IRemoteObject::DeathRecipient> deathRecipient =
        new ElementOperatorManager::ElementOperatorDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID, TEST_ACCOUNT_ID);
    EXPECT_NO_THROW(manager_->AddDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID, deathRecipient));
    EXPECT_NO_THROW(manager_->RemoveDeathRecipient(TEST_WINDOW_ID, TEST_TREE_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveDeathRecipient_002 end";
}

/**
 * @tc.number: ElementOperatorManager_AddRequestId_001
 * @tc.name: AddRequestId
 * @tc.desc: Test adding request ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, AddRequestId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddRequestId_001 start";
    sptr<IAccessibilityElementOperatorCallback> callback = nullptr;
    EXPECT_NO_THROW(manager_->AddRequestId(TEST_WINDOW_ID, TEST_TREE_ID, TEST_REQUEST_ID, callback));
    GTEST_LOG_(INFO) << "ElementOperatorManager_AddRequestId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveRequestId_001
 * @tc.name: RemoveRequestId
 * @tc.desc: Test removing request ID successfully.
 */
HWTEST_F(ElementOperatorManagerTest, RemoveRequestId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveRequestId_001 start";
    sptr<IAccessibilityElementOperatorCallback> callback = nullptr;
    manager_->AddRequestId(TEST_WINDOW_ID, TEST_TREE_ID, TEST_REQUEST_ID, callback);
    ErrCode result = manager_->RemoveRequestId(TEST_REQUEST_ID);
    EXPECT_EQ(result, ERR_OK);
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveRequestId_001 end";
}

/**
 * @tc.number: ElementOperatorManager_RemoveRequestId_002
 * @tc.name: RemoveRequestId
 * @tc.desc: Test removing non-existent request ID
 */
HWTEST_F(ElementOperatorManagerTest, RemoveRequestId_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveRequestId_002 start";
    ErrCode result = manager_->RemoveRequestId(TEST_REQUEST_ID);
    EXPECT_EQ(result, ERR_OK);
    GTEST_LOG_(INFO) << "ElementOperatorManager_RemoveRequestId_002 end";
}

/**
 * @tc.number: ElementOperatorManager_StopCallbackWait_001
 * @tc.name: StopCallbackWait
 * @tc.desc: Test stopping callback wait by window ID.
 */
HWTEST_F(ElementOperatorManagerTest, StopCallbackWait_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_StopCallbackWait_001 start";
    EXPECT_NO_THROW(manager_->StopCallbackWait(TEST_WINDOW_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_StopCallbackWait_001 end";
}

/**
 * @tc.number: ElementOperatorManager_StopCallbackWait_002
 * @tc.name: StopCallbackWait
 * @tc.desc: Test stopping callback wait by window ID and tree ID.
 */
HWTEST_F(ElementOperatorManagerTest, StopCallbackWait_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_StopCallbackWait_002 start";
    EXPECT_NO_THROW(manager_->StopCallbackWait(TEST_WINDOW_ID, TEST_TREE_ID));
    GTEST_LOG_(INFO) << "ElementOperatorManager_StopCallbackWait_002 end";
}

/**
 * @tc.number: ElementOperatorManager_MultipleConnections_001
 * @tc.name: MultipleConnections
 * @tc.desc: Test managing multiple connections.
 */
HWTEST_F(ElementOperatorManagerTest, MultipleConnections_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_MultipleConnections_001 start";
    sptr<AccessibilityWindowConnection> mockConnection2 = CreateMockConnection(TEST_WINDOW_ID + 1, TEST_ACCOUNT_ID);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID, mockConnection_);
    manager_->AddAccessibilityWindowConnection(TEST_WINDOW_ID + 1, mockConnection2);

    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), mockConnection_);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID + 1), mockConnection2);
    EXPECT_EQ(manager_->GetAsacConnections().size(), 2);

    manager_->RemoveAccessibilityWindowConnection(TEST_WINDOW_ID);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID), nullptr);
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(TEST_WINDOW_ID + 1), mockConnection2);
    EXPECT_EQ(manager_->GetAsacConnections().size(), 1);
    GTEST_LOG_(INFO) << "ElementOperatorManager_MultipleConnections_001 end";
}

/**
 * @tc.number: ElementOperatorManager_EdgeCases_001
 * @tc.name: EdgeCases
 * @tc.desc: Test edge cases and error conditions.
 */
HWTEST_F(ElementOperatorManagerTest, EdgeCases_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ElementOperatorManager_EdgeCases_001 start";
    EXPECT_EQ(manager_->GetAccessibilityWindowConnection(-1), nullptr);
    int64_t parentId = 0;
    EXPECT_EQ(manager_->GetRootParentId(-1, TEST_TREE_ID, parentId), RET_ERR_NO_CONNECTION);
    EXPECT_EQ(manager_->ClearFocus(-1), RET_ERR_NO_CONNECTION);

    manager_->RemoveAccessibilityWindowConnection(-1);
    int32_t treeId = ElementOperatorManager::GetTreeIdBySplitElementId(0);
    EXPECT_EQ(treeId, 0);
    GTEST_LOG_(INFO) << "ElementOperatorManager_EdgeCases_001 end";
}

} // namespace Accessibility
} // namespace OHOS
