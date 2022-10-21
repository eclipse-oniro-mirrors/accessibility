/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_MT_HELPER_H
#define ACCESSIBILITY_MT_HELPER_H

#include <vector>
#include "accessibility_def.h"
#include "hilog/log.h"
#include "i_accessible_ability_channel.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityHelper {
public:
    static AccessibilityHelper& GetInstance()
    {
        static AccessibilityHelper helper;
        return helper;
    }
    int32_t GetTestWindowId() const
    {
        return testWindowId_;
    }
    void SetTestWindowId(int32_t windowId)
    {
        testWindowId_ = windowId;
    }
    std::vector<EventType>& GetEventType()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return mTeventType_;
    }
    EventType GetEventTypeOfTargetIndex(int32_t index)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        int32_t size = static_cast<int32_t>(mTeventType_.size());
        if (size > index) {
            return mTeventType_[index];
        }
        return TYPE_VIEW_INVALID;
    }
    void PushEventType(EventType eventType)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        mTeventType_.push_back(eventType);
    }
    int32_t GetGestureId() const
    {
        return mTgestureId_;
    }
    void SetGestureId(int32_t gestureId)
    {
        mTgestureId_ = gestureId;
    }
    OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel>& GetTestStub()
    {
        return testStub_;
    }
    void SetTestStub(const OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel>& stub)
    {
        testStub_ = stub;
    }
    uint32_t GetTestStateType() const
    {
        return testStateType_;
    }
    void SetTestStateType(uint32_t stateType)
    {
        testStateType_ = stateType;
    }
    int32_t GetTestEventType() const
    {
        return testEventType_;
    }
    void SetTestEventType(int32_t testEventType)
    {
        testEventType_ = testEventType;
    }
    int32_t GetTestWindowChangeTypes() const
    {
        return testWindowChangeTypes_;
    }
    void SetTestWindowChangeTypes(int32_t testWindowChangeTypes)
    {
        testWindowChangeTypes_ = testWindowChangeTypes;
    }
    int32_t GetTestChannalId() const
    {
        return testChannalId_;
    }
    void SetTestChannalId(int32_t testChannalId)
    {
        testChannalId_ = testChannalId;
    }
    int32_t GetTestKeyPressEvent() const
    {
        return testKeyPressEvent_;
    }
    void SetTestKeyPressEvent(int32_t testKeyPressEvent)
    {
        testKeyPressEvent_ = testKeyPressEvent;
    }
    int32_t GetTestDisplayId() const
    {
        return testDisplayId_;
    }
    void SetTestDisplayId(int32_t testDisplayId)
    {
        testDisplayId_ = testDisplayId;
    }
    int32_t GetTestGesture() const
    {
        return testGesture_;
    }
    void SetTestGesture(int32_t testGesture)
    {
        testGesture_ = testGesture;
    }
    int32_t GetTestGestureSimulateResult() const
    {
        return testGestureSimulateResult_;
    }
    void SetTestGestureSimulateResult(int32_t testGestureSimulateResult)
    {
        testGestureSimulateResult_ = testGestureSimulateResult;
    }
public:
    static const int32_t accountId_ = 100;

private:
    OHOS::sptr<OHOS::Accessibility::IAccessibleAbilityChannel> testStub_ = nullptr;
    std::vector<EventType> mTeventType_ = {};
    int32_t mTgestureId_ = 0;
    uint32_t testStateType_ = 0;
    int32_t testEventType_ = 0;
    int32_t testWindowChangeTypes_ = 0;
    int32_t testWindowId_ = 0;
    int32_t testChannalId_ = -1;
    int32_t testGesture_ = -1;
    int32_t testKeyPressEvent_ = -1;
    int32_t testDisplayId_ = -1;
    int32_t testGestureSimulateResult_ = -1;
    std::mutex mtx_;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_MT_HELPER_H