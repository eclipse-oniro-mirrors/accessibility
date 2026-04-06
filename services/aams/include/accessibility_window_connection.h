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

#ifndef ACCESSIBILITY_WINDOW_CONNECTION_H
#define ACCESSIBILITY_WINDOW_CONNECTION_H

#include "iaccessibility_element_operator.h"
#include "hilog_wrapper.h"
#include "ffrt.h"
#include "safe_map.h"

#include <unordered_set>

namespace OHOS {
namespace Accessibility {
class AccessibilityWindowConnection : public RefBase {
public:
    AccessibilityWindowConnection(const int32_t windowId, const int32_t accountId);
    AccessibilityWindowConnection(const int32_t windowId, const int32_t treeId,
        const sptr<IAccessibilityElementOperator> &elementOperator, const int32_t accountId);
    ~AccessibilityWindowConnection();

    sptr<IAccessibilityElementOperator> GetProxy(uint64_t displayId);
    inline sptr<IAccessibilityElementOperator> GetRawProxy(uint64_t displayId)
    {
        std::lock_guard<ffrt::mutex> lock(proxyMutex_);
        return proxyMap_[displayId].first;
    }
    inline int GetCardProxySize()
    {
        return cardProxy_.Size();
    }

    inline void SetAncoFlag(bool flag)
    {
        isAnco_ = flag;
    }

    inline bool IsAnco()
    {
        return isAnco_;
    }

    inline void SetUseBrokerFlag(bool flag)
    {
        isUseBrokerProxy_ = flag;
    }
 
    inline bool GetUseBrokerFlag()
    {
        return isUseBrokerProxy_;
    }

    void SetProxy(uint64_t displayId, sptr<IAccessibilityElementOperator> proxy);
    
    void SetBrokerProxy(sptr<IAccessibilityElementOperator> proxy);
 
    void ResetProxy();
 
    void ResetBrokerProxy();

    sptr<IAccessibilityElementOperator> GetCardProxy(const int32_t treeId);

    RetError SetCardProxy(const int32_t treeId, sptr<IAccessibilityElementOperator> operation);

    uint32_t GetTokenIdMap(const int32_t treeId);

    RetError SetTokenIdMap(const int32_t treeId, const uint32_t tokenId);

    bool CheckScbTokenIdMap(uint32_t tokenId);

    void GetAllTreeId(std::vector<int32_t> &treeIds);

    RetError GetRootParentId(int32_t treeId, int64_t &elementId);

    RetError SetRootParentId(const int32_t treeId, const int64_t elementId);

    void EraseProxy(const int32_t treeId);

    void AddDeathRecipient(sptr<IAccessibilityElementOperator> elementOperator, bool isBroker, uint64_t displayId);
 
    void AddTreeDeathRecipient(int32_t windowId, int32_t accountId, int32_t treeId, uint64_t displayId);
 
    void RemoveTreeDeathRecipient(int32_t treeId);

    void ClearFocus();
 
    void OutsideTouch();

private:
    class InteractionOperationDeathRecipient final : public IRemoteObject::DeathRecipient {
    public:
        InteractionOperationDeathRecipient(int32_t windowId, int32_t accountId, uint64_t displayId)
            : windowId_(windowId), accountId_(accountId), displayId_(displayId){};
        InteractionOperationDeathRecipient(int32_t windowId, int32_t treeId, int32_t accountId, uint64_t displayId)
            : windowId_(windowId), treeId_(treeId), accountId_(accountId), displayId_(displayId){};
        ~InteractionOperationDeathRecipient() final = default;
        DISALLOW_COPY_AND_MOVE(InteractionOperationDeathRecipient);
 
        void OnRemoteDied(const wptr<IRemoteObject> &remote) final;
        int32_t windowId_ = INVALID_WINDOW_ID;
        int32_t treeId_ = INVALID_TREE_ID;
        int32_t accountId_ = 0;
        uint64_t displayId_ = 0;
    };

private:
    int32_t windowId_;
    int32_t accountId_;
    int32_t treeId_ = 0;
    SafeMap<int32_t, sptr<IAccessibilityElementOperator>> cardProxy_;
    sptr<IAccessibilityElementOperator> brokerProxy_;
    bool isUseBrokerProxy_ = false;
    SafeMap<int32_t, uint32_t> tokenIdMap_;
    SafeMap<int32_t, int64_t> treeIdParentId_;
    bool isAnco_ = false;
    ffrt::mutex proxyMutex_;

    SafeMap<uint64_t, sptr<IRemoteObject::DeathRecipient>> proxyDeathRecipientMap_;
    sptr<IRemoteObject::DeathRecipient> brokerProxyDeathRecipient_;
    SafeMap<int32_t, sptr<IRemoteObject::DeathRecipient>> childTreeProxyDeathRecipient_;
    ffrt::mutex proxyMapMutex_;
    std::unordered_map<uint64_t, std::pair<sptr<IAccessibilityElementOperator>, sptr<IRemoteObject::DeathRecipient>>>
        proxyMap_;
    std::unordered_set<uint32_t> scbTokenMap_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_WINDOW_CONNECTION_H