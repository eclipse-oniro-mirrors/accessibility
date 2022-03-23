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

#ifndef MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H
#define MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H

#include <vector>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "accessible_ability_channel_stub.h"

namespace OHOS {
namespace Accessibility {
class AccessibleAbilityChannelStubMock : public IRemoteStub<IAccessibleAbilityChannel> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAccessibleAbilityChannelMock");

    AccessibleAbilityChannelStubMock() {}
    virtual ~AccessibleAbilityChannelStubMock() {}

    virtual bool SearchElementInfoByAccessibilityId(const int accessibilityWindowId,
        const long elementId, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback,
        const int mode)
    {
        (void)accessibilityWindowId;
        (void)elementId;
        (void)requestId;
        (void)callback;
        (void)mode;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock SearchElementInfoByAccessibilityId";
        return true;
    }

    virtual bool SearchElementInfosByText(const int accessibilityWindowId, const long elementId,
        const std::string &text, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        (void)accessibilityWindowId;
        (void)elementId;
        (void)text;
        (void)requestId;
        (void)callback;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock SearchElementInfosByText";
        return true;
    }

    virtual bool FindFocusedElementInfo(const int accessibilityWindowId, const long elementId,
        const int focusType, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        (void)accessibilityWindowId;
        (void)elementId;
        (void)focusType;
        (void)requestId;
        (void)callback;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock FindFocusedElementInfo";
        return true;
    }

    virtual bool FocusMoveSearch(const int accessibilityWindowId, const long elementId,
        const int direction, const int requestId, const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        (void)accessibilityWindowId;
        (void)elementId;
        (void)direction;
        (void)requestId;
        (void)callback;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock FocusMoveSearch";
        return true;
    }

    virtual bool ExecuteAction(const int accessibilityWindowId, const long elementId, const int action,
        std::map<std::string, std::string> &actionArguments, const int requestId,
        const sptr<IAccessibilityElementOperatorCallback> &callback)
    {
        (void)accessibilityWindowId;
        (void)elementId;
        (void)action;
        (void)actionArguments;
        (void)requestId;
        (void)callback;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock ExecuteAction";
        return true;
    }

    virtual std::vector<AccessibilityWindowInfo> GetWindows()
    {
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock GetWindows";
        std::vector<AccessibilityWindowInfo> windows {};
        return windows;
    }

    virtual bool ExecuteCommonAction(const int action)
    {
        (void)action;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock ExecuteCommonAction";
        return true;
    }

    virtual void SetOnKeyPressEventResult(const bool handled, const int sequence)
    {
        (void)handled;
        (void)sequence;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock SetOnKeyPressEventResult";
    }

    virtual float GetDisplayResizeScale(const int displayId)
    {
        (void)displayId;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock GetDisplayResizeScale";
        return 0;
    }

    virtual float GetDisplayResizeCenterX(const int displayId)
    {
        (void)displayId;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock GetDisplayResizeCenterX";
        return 0;
    }

    virtual float GetDisplayResizeCenterY(const int displayId)
    {
        (void)displayId;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock GetDisplayResizeCenterY";
        return 0;
    }

    virtual Rect GetDisplayResizeRect(const int displayId)
    {
        (void)displayId;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock GetDisplayResizeRect";
        Rect rect {};
        return rect;
    }

    virtual bool ResetDisplayResize(const int displayId, const bool animate)
    {
        (void)displayId;
        (void)animate;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock ResetDisplayResize";
        return true;
    }

    virtual bool SetDisplayResizeScaleAndCenter(const int displayId, const float scale, const float centerX,
        const float centerY, const bool animate)
    {
        (void)displayId;
        (void)scale;
        (void)centerX;
        (void)centerY;
        (void)animate;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock SetDisplayResizeScaleAndCenter";
        return true;
    }

    virtual void SendSimulateGesture(const int requestId, const std::vector<GesturePathDefine> &gestureSteps)
    {
        (void)requestId;
        (void)gestureSteps;
        GTEST_LOG_(INFO) << "MOCK AccessibleAbilityChannelStubMock SendSimulateGesture";
    }
};
} // namespace Accessibility
} // namespace OHOS
#endif // MOCK_ACCESSIBLE_ABILITY_CHANNEL_STUB_H