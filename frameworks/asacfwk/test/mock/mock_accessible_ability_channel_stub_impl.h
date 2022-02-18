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

#ifndef MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_IMPL_H
#define MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_IMPL_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "accessible_ability_channel_stub.h"
#include "gesture_simulation.h"

namespace OHOS {
namespace Accessibility {

class AccessibleAbilityChannelStubImplMock : public AccessibleAbilityChannelStub {
public:
    AccessibleAbilityChannelStubImplMock()
    {}
    virtual ~AccessibleAbilityChannelStubImplMock()
    {}

    virtual bool SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
        const long elementId, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int mode)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock SearchElementInfoByAccessibilityId";
        return true;
    }

    virtual bool SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
        const std::string &text, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock SearchElementInfosByText";
        return true;
    }

    virtual bool FindFocusedElementInfo(const int accessibilityWindowId, const long elementId,
        const int focusType, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock FindFocusedElementInfo";
        return true;
    }

    virtual bool FocusMoveSearch(const int accessibilityWindowId, const long elementId,
        const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock FocusMoveSearch";
        return true;
    }

    virtual bool ExecuteAction(const int accessibilityWindowId, const long elementId, const int action,
        std::map<std::string, std::string> &actionArguments, const int requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock ExecuteAction";
        return true;
    }

    virtual std::vector<AccessibilityWindowInfo> GetWindows()
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock GetWindows";
        std::vector<AccessibilityWindowInfo> windowInfos {};
        AccessibilityWindowInfo window {};
        windowInfos.push_back(window);
        return windowInfos;
    }

    virtual bool ExecuteCommonAction(const int action)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock ExecuteCommonAction";
        return true;
    }

    virtual void SetOnKeyPressEventResult(const bool handled, const int sequence)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock SetOnKeyPressEventResult";
    }

    virtual float GetDisplayResizeScale(const int displayId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock GetDisplayResizeScale";
        return 0;
    }

    virtual float GetDisplayResizeCenterX(const int displayId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock GetDisplayResizeCenterX";
        return 0;
    }

    virtual float GetDisplayResizeCenterY(const int displayId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock GetDisplayResizeCenterY";
        return 0;
    }

    virtual Rect GetDisplayResizeRect(const int displayId)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock GetDisplayResizeRect";
        Rect rect {};
        rect.SetLeftTopScreenPostion(1, 1);
        return rect;
    }

    virtual bool ResetDisplayResize(const int displayId, const bool animate)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock ResetDisplayResize";
        return true;
    }

    virtual bool SetDisplayResizeScaleAndCenter(const int displayId, const float scale, const float centerX,
        const float centerY, const bool animate)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock SetDisplayResizeScaleAndCenter";
        return true;
    }

    virtual void SendSimulateGesture(const int requestId, const std::vector<GesturePathDefine> &gestureSteps)
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubImplMock SendSimulateGesture";
    }

};

} // namespace Accessibility
} // namespace OHOS

#endif // MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_IMPL_H