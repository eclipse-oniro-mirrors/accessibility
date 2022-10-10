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

#ifndef ACCESSIBILITY_EVENT_INFO_H
#define ACCESSIBILITY_EVENT_INFO_H

#include <vector>
#include "accessibility_def.h"

namespace OHOS {
namespace Accessibility {
/*
* The class supply the content information about the event.
* The SetXxx() API is almost supplied to ACE to set the information.
* Parts of SetXxx() API is called by AAMS, such as: SetConnectId().
* The GetXxx() API is almost supplied to AA to get the information.
*/
class AccessibilityMemo {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityMemo() {}

    /**
     * @brief Set the component of the event related.
     * @param componentId The id of component.
     * @since 3
     * @sysCap Accessibility
     */
    void SetSource(const int32_t componentId);

    /**
     * @brief Gets the view(component) ID associated with the accessibility event.
     * @return The view(component) ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetViewId() const;

    /**
     * @brief Gets the accessibility ID of the component.
     * @return The accessibility ID of the component.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetAccessibilityId() const;

    /**
     * @brief Set the window ID associated with the accessibility event.
     * @param windowId The window ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int32_t windowId);

    /**
     * @brief Gets the window ID associated with the accessibility event.
     * @return The window ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */

    int32_t GetWindowId() const;

    /**
     * @brief Gets the current index of listed items or text position associated with the current event.
     * @return The current index of listed items or text position associated with the current event.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetCurrentIndex() const;

    /**
     * @brief Sets the current index of listed items or text position associated with the current event.
     * @param index The current index of listed items or text position associated with the current event.
     * @since 3
     * @sysCap Accessibility
     */
    void SetCurrentIndex(const int32_t index);

    /**
     * @brief Gets the start index of listed items on the screen.
     * @return The start index of listed items. It is unique value for the listed items.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetBeginIndex() const;

    /**
     * @brief Sets the start index of listed items on the screen.
     * @param index The start index of listed items. It is unique value for the listed items.
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeginIndex(const int32_t index);

    /**
     * @brief Gets the number items on the screen.
     * @return The number of items.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetItemCounts() const;

    /**
     * @brief Sets the number items on the screen.
     * @param ItemCounts The number of items.
     * @since 3
     * @sysCap Accessibility
     */
    void SetItemCounts(const int32_t itemCounts);

    /**
     * @brief Gets the end index of listed items on the screen.
     * @return The end index of listed items on the screen.
     * @since 3
     * @sysCap Accessibility
     */
    int32_t GetEndIndex() const;

    /**
     * @brief Sets the end index of listed items on the screen.
     * @param index The end index of listed items on the screen.
     * @since 3
     * @sysCap Accessibility
     */
    void SetEndIndex(const int32_t index);

    /**
     * @brief Gets the class(component type) name.
     * @return The class(component type) name.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetComponentType() const;

    /**
     * @brief Sets the class(component type) name.
     * @param className The class(component type) name.
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentType(const std::string &className);

    /**
     * @brief Gets the before text changed of the component.
     * @return The before text changed of the component.
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetBeforeText() const;

    /**
     * @brief Set the before text changed of the component.
     * @param beforeText The before text changed of the component.
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeforeText(const std::string &beforeText);

    /**
     * @brief Adds an text to the content list.
     * @param content Indicates the text to add.
     * @since 3
     * @sysCap Accessibility
     */
    void AddContent(const std::string &content);

    /**
     * @brief Get the content list in the event.
     * @return The content list in the event.
     * @since 3
     * @sysCap Accessibility
     */
    const std::vector<std::string> &GetContentList() const;

    /**
     * @brief Gets the last content of the component(The text of changed after).
     * @return The last content of the component(The text of changed after).
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetLatestContent() const;

    /**
     * @brief Sets the last content of the component.
     * @param content The last content of component.
     * @since 3
     * @sysCap Accessibility
     */
    void SetLatestContent(const std::string &content);

    /**
     * @brief Gets the accessibility event description.
     * @return The accessibility event description
     * @since 3
     * @sysCap Accessibility
     */
    const std::string &GetDescription() const;

    /**
     * @brief Sets the accessibility event description.
     * @param contentDescription The accessibility event description
     * @since 3
     * @sysCap Accessibility
     */
    void SetDescription(const std::string &contentDescription);

protected:
    int32_t componentId_ = 0;
    int32_t windowId_ = 0;
    int32_t currentIndex_ = 0;
    int32_t beginIndex_ = 0;
    int32_t endIndex_ = 0;
    std::vector<std::string> contents_ {};
    std::string componentType_ = "";
    std::string description_ = "";
    std::string beforeText_ = "";
    std::string latestContent_ = "";
    int32_t elementId_ = 0;
    int32_t itemCounts_ = 0;
};

/*
* The class define the event types and supply the api to
* get/set the property of event. and it triggered by UI
* changed and sent to AA. According to the event property,
* AA can get event properties, such as: action triggered
* the event and the source.
*/
class AccessibilityEventInfo : public AccessibilityMemo {
public:
    /**
     * @brief Construct
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo() {}

    /**
     * @brief Construct
     * @param eventType the type of event info
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo(EventType eventType);

    /**
     * @brief Construct
     * @param windowId The id of window
     * @param windowChangeTypes the window change type
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo(int32_t windowId, WindowUpdateType windowChangeTypes);

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
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EVENT_INFO_H
