/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef ACCESSIBILITY_UT_HELPER_H
#define ACCESSIBILITY_UT_HELPER_H

#include <mutex>
#include "accessibility_event_info.h"
#include "hilog/log.h"
#include "iremote_object.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityAbilityHelper {
public:
    static AccessibilityAbilityHelper& GetInstance()
    {
        static AccessibilityAbilityHelper helper;
        return helper;
    }

    std::vector<int32_t> GetTouchEventActionVector()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return touchAction_;
    }

    int32_t GetTouchEventActionOfTargetIndex(int32_t index)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        int32_t size = static_cast<int32_t>(touchAction_.size());
        if (size > index) {
            return touchAction_[index];
        }
        return -1;
    }

    void ClearTouchEventActionVector()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        touchAction_.clear();
    }

    void SetTouchEventActionVectors(int32_t touchAction)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        touchAction_.push_back(touchAction);
    }

    int32_t GetKeyCode() const
    {
        return keyCode_;
    }

    void ClearKeyCode()
    {
        keyCode_ = 0;
    }

    void SetKeyCode(int32_t keyCode)
    {
        keyCode_ = keyCode;
    }

    bool GetDestroyState()
    {
        return isDestroyEvents_;
    }

    void SetDestroyState()
    {
        isDestroyEvents_ = true;
    }

    int GetTestElementId()
    {
        return testElementId_;
    }

    void SetTestElementId(int ElementId)
    {
        testElementId_ = ElementId;
    }

    int GetTestChannelElementId()
    {
        return testChannelElementId_;
    }

    void SetTestChannelElementId(int ElementId)
    {
        testChannelElementId_ = ElementId;
    }

    int GetTestChannelRequestId()
    {
        return testChannelrequestId_;
    }

    void SetTestChannelRequestId(int ElementId)
    {
        testChannelrequestId_ = ElementId;
    }

    int GetTestChannelMode()
    {
        return testChannelmode_;
    }

    void SetTestChannelMode(int ElementId)
    {
        testChannelmode_ = ElementId;
    }

    int GetTestClearFocus()
    {
        return testClearFocus_;
    }

    void SetTestClearFocus(int ElementId)
    {
        testClearFocus_ = ElementId;
    }

    int GetTestOutsideTouch()
    {
        return testOutsideTouch_;
    }

    void SetTestOutsideTouch(int ElementId)
    {
        testOutsideTouch_ = ElementId;
    }

    int GetGestureId()
    {
        return uTgestureId_;
    }

    void SetGestureId(int gestureId)
    {
        uTgestureId_ = gestureId;
    }

    std::vector<EventType> GetEventTypeVector()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return eventType_;
    }

    EventType GetEventTypeOfTargetIndex(int32_t index)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        int32_t size = static_cast<int32_t>(eventType_.size());
        if (size > index) {
            return eventType_[index];
        }
        return TYPE_VIEW_INVALID;
    }

    void ClearEventTypeActionVector()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        eventType_.clear();
    }

    void SetEventTypeVector(EventType eventType)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        eventType_.push_back(eventType);
    }

    int GetTestChannelId()
    {
        return testChannelId_;
    }

    void SetTestChannelId(int ChannalId)
    {
        testChannelId_ = ChannalId;
    }

    int GetTestEventType()
    {
        return testEventType_;
    }

    void SetTestEventType(int EventType)
    {
        testEventType_ = EventType;
    }

    int GetTestGesture()
    {
        return testGesture_;
    }

    void SetTestGesture(int Gesture)
    {
        testGesture_ = Gesture;
    }

    int GetTestKeyPressEvent()
    {
        return testKeyPressEvent_;
    }

    void SetTestKeyPressEvent(int KeyPressEvent)
    {
        testKeyPressEvent_ = KeyPressEvent;
    }

    int GetTestDisplayId()
    {
        return testDisplayId_;
    }

    void SetTestDisplayId(int DisplayId)
    {
        testDisplayId_ = DisplayId;
    }

    int GetTestStateType()
    {
        return testStateType_;
    }

    void SetTestStateType(int StateType)
    {
        testStateType_ = StateType;
    }

    bool GetTestKeyEvent()
    {
        return testKeyEvent_;
    }

    void SetTestKeyEvent(bool KeyEvent)
    {
        testKeyEvent_ = true;
    }

    int GetSendEventTimes()
    {
        return sendEventTimes_;
    }

    void AddSendEventTimes()
    {
        sendEventTimes_++;
    }

    void ClearSendEventTimes()
    {
        sendEventTimes_ = 0;
    }

    uint32_t GetTestSequence() const
    {
        return testSequence_;
    }

    void SetTestSequence(uint32_t sequenceNum)
    {
        testSequence_ = sequenceNum;
    }

    void SetShortKeyTargetAbilityState(bool state)
    {
        enableShortKeyTargetAbility_ = state;
    }

    bool GetShortKeyTargetAbilityState() const
    {
        return enableShortKeyTargetAbility_;
    }

    void SetZoomState(bool state)
    {
        zoomState_ = state;
    }

    bool GetZoomState() const
    {
        return zoomState_;
    }

    void SetScrollState(bool state)
    {
        scrollState_ = state;
    }

    bool GetScrollState() const
    {
        return scrollState_;
    }
    
    void AddUserId(int32_t id)
    {
        userIds_.emplace_back(id);
    }

    void RemoveUserId(int32_t id)
    {
        for (auto iter = userIds_.begin(); iter != userIds_.end();) {
            if (*iter == id) {
                userIds_.erase(iter);
            } else {
                iter++;
            }
        }
    }

    void GetUserIds(std::vector<int32_t> &userIds) const
    {
        for (auto& id : userIds_) {
            userIds.emplace_back(id);
        }
    }

    void AddPackage(const std::string &name)
    {
        packages_.emplace_back(name);
    }

    void RemovePackage(const std::string &name)
    {
        for (auto iter = packages_.begin(); iter != packages_.end();) {
            if (*iter == name) {
                packages_.erase(iter);
            } else {
                iter++;
            }
        }
    }

    void GetPackages(std::vector<std::string> &packages) const
    {
        for (auto& name : packages_) {
            packages.emplace_back(name);
        }
    }

    void SetCurrentUserId(int32_t id)
    {
        currentUserId_ = id;
    }

    int32_t GetCurrentUserId() const
    {
        return currentUserId_;
    }

    void ChangePackage(bool isChanged)
    {
        isPackageChanged_ = isChanged;
    }

    bool GetChangePackageFlag() const
    {
        return isPackageChanged_;
    }

    void SetNeedAccountDataNullFlag(bool flag)
    {
        needAccountDataNull_ = flag;
    }

    bool GetNeedAccountDataNullFlag() const
    {
        return needAccountDataNull_;
    }

    void SetEventWindowId(int32_t windowId)
    {
        eventWindowId_ = windowId;
    }

    int32_t GetEventWindowId() const
    {
        return eventWindowId_;
    }

    void SetEventWindowChangeType(WindowUpdateType type)
    {
        eventWindowChangeType_ = type;
    }

    WindowUpdateType GetEventWindowChangeType() const
    {
        return eventWindowChangeType_;
    }

    void SetRealId(int32_t realId)
    {
        realId_ = realId;
    }

    int32_t GetRealId() const
    {
        return realId_;
    }

    void SetExecuteAction(int32_t action)
    {
        executeAction_ = action;
    }

    int32_t GetExecuteAction() const
    {
        return executeAction_;
    }
public:
    static const int32_t accountId_ = 100;

private:
    std::vector<int32_t> touchAction_;
    bool isDestroyEvents_ = false;
    int testElementId_ = -1;
    int testClearFocus_ = -1;
    int testOutsideTouch_ = -1;
    int testChannelElementId_ = -1;
    int testChannelrequestId_ = -1;
    int testChannelmode_ = -1;
    int uTgestureId_;
    std::vector<EventType> eventType_;
    int testChannelId_ = -1;
    int testEventType_ = -1;
    int testGesture_ = -1;
    int testKeyPressEvent_ = -1;
    int testDisplayId_ = -1;
    int testStateType_ = -1;
    bool testKeyEvent_ = false;
    std::atomic<int> sendEventTimes_ = 0;
    uint32_t testSequence_ = 0;
    int32_t keyCode_ = 0;
    bool enableShortKeyTargetAbility_ = false;
    bool zoomState_ = false;
    bool scrollState_ = false;
    std::mutex mtx_;
    std::vector<int32_t> userIds_;
    std::vector<std::string> packages_;
    int32_t currentUserId_ = -1;
    bool isPackageChanged_ = false;
    bool needAccountDataNull_ = false;
    int32_t eventWindowId_ = -1;
    WindowUpdateType eventWindowChangeType_ = WINDOW_UPDATE_INVALID;
    int32_t realId_ = 0;
    int32_t executeAction_ = ACCESSIBILITY_ACTION_INVALID;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_UT_HELPER_H