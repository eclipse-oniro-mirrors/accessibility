/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef MOCK_ACCESSIBILITY_EVENT_INFO_H
#define MOCK_ACCESSIBILITY_EVENT_INFO_H

#include <vector>
#include "accessibility_def.h"
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"

namespace OHOS {
namespace Accessibility {
class MockAccessibilityEventInfo : public AccessibilityEventInfo {
public:
    MockAccessibilityEventInfo() {}

    MockAccessibilityEventInfo(EventType eventType);

    MockAccessibilityEventInfo(int32_t windowId, WindowUpdateType windowChangeTypes);

    /**
     * @brief Get the type of the accessibility event.
     * @return The type of the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    EventType GetEventType() const;

    /**
     * @brief Get the window content changed types
     * @return The window content changed types. Refer to "WindowsContentChangeTypes"
     * @since 3
     * * @sysCap Accessibility
     */
    WindowsContentChangeTypes GetWindowContentChangeTypes() const;

    /**
     * @brief Set the window content changed types
     * @param changeTypes The window content changed types. Refer to "WindowsContentChangeTypes"
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowContentChangeTypes(const WindowsContentChangeTypes changeTypes);

    /**
     * @brief Get the window changed types
     * @return The window changed types.refer to WindowUpdateType
     * @since 3
     * @sysCap Accessibility
     */
    WindowUpdateType GetWindowChangeTypes() const;

    /**
     * @brief Set the window changed types
     * @param changeTypes The window changed types.refer to WindowUpdateType
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowChangeTypes(const WindowUpdateType changeTypes);

    /**
     * @brief Sets the type of an accessibility event.
     * @note It is same to setAccessibilityEventType​(int32_t accessibilityEventType)
     * @param eventType The type of an accessibility event. Refer to "EventType"
     * @since 3
     * @sysCap Accessibility
     */
    void SetEventType(const EventType eventType);

    /**
     * @brief AA get the time of accessibility event is sent from ASAC.
     * @return The time of accessibility event is sent from ASAC
     * @since 3
     * @sysCap Accessibility
     */
    int64_t GetTimeStamp() const;

    /**
     * @brief ASAC set the time of accessibility is sent to AA
     * @param timeStamp The time of accessibility event is sent from ASAC
     * @since 3
     * @sysCap Accessibility
     */
    void SetTimeStamp(int64_t timeStamp);  // Remained:for send event timestamp

    /**
     * @brief Gets the bundle name of the event source, that is, the bundle name of the target application.
     * @return Returns the bundle name of the event source.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetBundleName() const;

    /**
     * @brief Set the bundle name of the event source, that is, the bundle name of the target application.
     * @param bundleName The bundle name of the event source.
     * @since 3
     * @sysCap Accessibility
     */
    void SetBundleName(const std::string &bundleName);

    /**
     * @brief Get the notification content of the event source.
     * @return Return the notification content of the event source.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetNotificationContent() const;

    /**
     * @brief Set the notification content of the event source.
     * @param notificationContent The notification content of the event source.
     * @since 3
     * @sysCap Accessibility
     */
    void SetNotificationContent(const std::string &notificationContent);

    /**
     * @brief Set the move granularity of the text
     * @param granularity The move granularity of the text. Refer to "AccessibilityElementInfo.TextMoveUnit"
     * @since 3
     * @sysCap Accessibility
     */
    void SetTextMovementStep(const TextMoveUnit granularity);

    /**
     * @brief Get the move granularity of the text
     * @return The move granularity of the text. Refer to "AccessibilityElementInfo.TextMoveUnit"
     * @since 3
     * @sysCap Accessibility
     */
    TextMoveUnit GetTextMovementStep() const;

    /**
     * @brief Sets the action that triggers the accessibility event.
     * @param action The operation of the event. Refer to "AccessibilityElementInfo.ActionType"
     * @since 3
     * @sysCap Accessibility
     */
    void SetTriggerAction(const ActionType action);

    /**
     * @brief Gets the action that triggers the accessibility event.
     * @return The operation of the event. Refer to "AccessibilityElementInfo.ActionType"
     * @since 3
     * @sysCap Accessibility
     */
    ActionType GetTriggerAction() const;

    /**
     * @brief Set the information of accessibility event of [TYPE_NOTIFICATION_UPDATE_EVENT]
     * @param category Refer to [NotificationCategory], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    void SetNotificationInfo(const NotificationCategory category);

    /**
     * @brief Get the information of accessibility event of [TYPE_NOTIFICATION_UPDATE_EVENT]
     * @return Refer to [NotificationCategory], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    NotificationCategory GetNotificationInfo() const;

    /**
     * @brief Set the customize gesture type of accessibility event of [TYPE_GESTURE_EVENT]
     * @param category Refer to [GestureType], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    void SetGestureType(const GestureType gestureType);

    /**
     * @brief Get the customize gesture type of accessibility event of [TYPE_GESTURE_EVENT]
     * @return Refer to [GestureType], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    GestureType GetGestureType() const;

    /**
     * @brief Set the page ID associated with the accessibility event.
     * @param pageId The page ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    void SetPageId(const int32_t pageId);

    /**
     * @brief Gets the page ID associated with the accessibility event.
     * @return The page ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetPageId() const;

    /**
     * @brief Set the element info associated with the accessibility event.
     * @param elementInfo The element info associated with the accessibility event.
     * @since 12
     * @sysCap Accessibility
     */
    void SetElementInfo(const AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get the element info pointer associated with the accessibility event.
     * @return The element info associated with the accessibility event.
     * @since 12
     * @sysCap Accessibility
     */
    const AccessibilityElementInfo &GetElementInfo() const;

    /**
     * @brief Get the text to announce.
     * @return Return the text to announce.
     * @since 12
     * @sysCap Accessibility
     */
    const std::string &GetTextAnnouncedForAccessibility() const;

    /**
     * @brief Set the text to announce of the event.
     * @param textAnnouncedForAccessibility The text to announce.
     * @since 12
     * @sysCap Accessibility
     */
    void SetTextAnnouncedForAccessibility(const std::string &textAnnouncedForAccessibility);

    /**
     * @brief Get the inspectorKey.
     * @return Return the inspectorKey.
     * @since 12
     * @sysCap Accessibility
     */
    const std::string &GetInspectorKey() const;

    /**
     * @brief Set the inspectorKey of the event.
     * @param inspectorKey The inspectorKey.
     * @since 12
     * @sysCap Accessibility
     */
    void SetInspectorKey(const std::string &inspectorKey);

    /**
     * @brief Set the request focus elementId.
     * @param requestFocusElementId The elementId to focus.
     * @since 12
     * @sysCap Accessibility
     */
    void SetRequestFocusElementId(const int32_t requestFocusElementId);
protected:
    EventType eventType_ = TYPE_VIEW_INVALID;
    std::string bundleName_ = "";
    ActionType triggerAction_ = ACCESSIBILITY_ACTION_INVALID;
    int64_t timeStamp_ = 0;
    TextMoveUnit textMoveStep_ = STEP_CHARACTER;
    WindowsContentChangeTypes windowContentChangeTypes_ = CONTENT_CHANGE_TYPE_INVALID;
    WindowUpdateType windowChangeTypes_ = WINDOW_UPDATE_INVALID;
    GestureType gestureType_ = GESTURE_INVALID;
    NotificationCategory category_ = CATEGORY_INVALID;
    int32_t pageId_ = 0;
    std::string notificationContent_ = "";
    AccessibilityElementInfo elementInfo_ {};
    std::string textAnnouncedForAccessibility_ = "";
    std::string inspectorKey_ = "";
    int32_t requestFocusElementId_ = -1;
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EVENT_INFO_H
