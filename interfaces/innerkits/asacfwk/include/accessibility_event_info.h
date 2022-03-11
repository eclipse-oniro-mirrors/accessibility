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

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "accessibility_element_info.h"
#include "parcel.h"

namespace OHOS {
namespace Accessibility {
enum WindowUpdateType : int {
    WINDOW_UPDATE_INVALID = 0,
    WINDOW_UPDATE_ACCESSIBILITY_FOCUSED = 0x00000001,
    WINDOW_UPDATE_FOCUSED = 0x00000002,
    WINDOW_UPDATE_ACTIVE = 0x00000004,
    WINDOW_UPDATE_ADDED = 0x00000008,
    WINDOW_UPDATE_REMOVED = 0x00000010,
    WINDOW_UPDATE_BOUNDS = 0x00000020,
    WINDOW_UPDATE_TITLE = 0x00000040,
    WINDOW_UPDATE_LAYER = 0x00000080,
    WINDOW_UPDATE_PARENT = 0x00000100,
    WINDOW_UPDATE_CHILDREN = 0x00000200,
    WINDOW_UPDATE_PIP = 0x00000400,
};

enum WindowsContentChangeTypes : int {
    CONTENT_CHANGE_TYPE_INVALID = 0,
    CONTENT_CHANGE_TYPE_SUBTREE = 0x00000001,
    CONTENT_CHANGE_TYPE_TEXT = 0x00000002,
    CONTENT_CHANGE_TYPE_CONTENT_DESCRIPTION = 0x00000004,
};

enum EventType : uint32_t {
    TYPE_VIEW_INVALID = 0,
    TYPE_VIEW_CLICKED_EVENT = 0x00000001,
    TYPE_VIEW_LONG_CLICKED_EVENT = 0x00000002,
    TYPE_VIEW_SELECTED_EVENT = 0x00000004,
    TYPE_VIEW_FOCUSED_EVENT = 0x00000008,
    TYPE_VIEW_TEXT_UPDATE_EVENT = 0x00000010,
    TYPE_PAGE_STATE_UPDATE = 0x00000020,
    TYPE_NOTIFICATION_UPDATE_EVENT = 0x00000040,
    TYPE_VIEW_HOVER_ENTER_EVENT = 0x00000080,
    TYPE_VIEW_HOVER_EXIT_EVENT = 0x00000100,
    TYPE_TOUCH_GUIDE_GESTURE_BEGIN = 0x00000200,
    TYPE_TOUCH_GUIDE_GESTURE_END = 0x00000400,
    TYPE_PAGE_CONTENT_UPDATE = 0x00000800,
    TYPE_VIEW_SCROLLED_EVENT = 0x000001000,
    TYPE_VIEW_TEXT_SELECTION_UPDATE_EVENT = 0x000002000,
    TYPE_PUBLIC_NOTICE_EVENT = 0x00004000,
    TYPE_VIEW_ACCESSIBILITY_FOCUSED_EVENT = 0x00008000,
    TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED_EVENT = 0x00010000,
    TYPE_VIEW_TEXT_MOVE_UNIT_EVENT = 0x00020000,
    TYPE_TOUCH_GUIDE_BEGIN = 0x00040000,
    TYPE_TOUCH_GUIDE_END = 0x00080000,
    TYPE_TOUCH_BEGIN = 0x00100000,
    TYPE_TOUCH_END = 0x00200000,
    TYPE_WINDOW_UPDATE = 0x00400000,
    TYPE_INTERRUPT_EVENT = 0x00800000,
    TYPE_GESTURE_EVENT = 0x01000000,
    TYPE_MAX_NUM = 0x02000000,
    TYPES_ALL_MASK = 0xFFFFFFFF,
};

enum NotificationCategory : int {
    CATEGORY_INVALID = 0,
    CATEGORY_CALL = 0x00000001,
    CATEGORY_MSG = 0x00000002,
    CATEGORY_EMAIL = 0x00000004,
    CATEGORY_EVENT = 0x00000008,
    CATEGORY_PROMO = 0x00000010,
    CATEGORY_ALARM = 0x00000020,
    CATEGORY_PROGRESS = 0x00000040,
    CATEGORY_SOCIAL = 0x00000080,
    CATEGORY_ERR = 0x00000100,
    CATEGORY_TRANSPORT = 0x00000200,
    CATEGORY_SYS = 0x00000400,
    CATEGORY_SERVICE = 0x00000800,
    CATEGORY_OTHERS = 0x00001000,
};

// The types of swipe gestures which are performed on the touch screen.
enum GestureType : uint32_t {
    GESTURE_INVALID = 0,
    GESTURE_SWIPE_UP = 0x0001,
    GESTURE_SWIPE_DOWN = 0x0002,
    GESTURE_SWIPE_LEFT = 0x0003,
    GESTURE_SWIPE_RIGHT = 0x0004,
    GESTURE_SWIPE_LEFT_THEN_RIGHT = 0x0005,
    GESTURE_SWIPE_RIGHT_THEN_LEFT = 0x0006,
    GESTURE_SWIPE_UP_THEN_DOWN = 0x0007,
    GESTURE_SWIPE_DOWN_THEN_UP = 0x0008,
    GESTURE_SWIPE_LEFT_THEN_UP = 0x0009,
    GESTURE_SWIPE_LEFT_THEN_DOWN = 0x000A,
    GESTURE_SWIPE_RIGHT_THEN_UP = 0x000B,
    GESTURE_SWIPE_RIGHT_THEN_DOWN = 0x000C,
    GESTURE_SWIPE_UP_THEN_LEFT = 0x000D,
    GESTURE_SWIPE_UP_THEN_RIGHT = 0x000E,
    GESTURE_SWIPE_DOWN_THEN_LEFT = 0x000F,
    GESTURE_SWIPE_DOWN_THEN_RIGHT = 0x0010,

    GESTURE_TAP = 0x0011,
    GESTURE_DOUBLETAP = 0x0012,
    GESTURE_DOUBLETAP_HOLD = 0x0013,
    GESTURE_DRAG = 0x0014,
    GESTURE_SWIPE_SLOWLY = 0x0015,
    GESTURE_TRIPLETAP = 0x0016,
    GESTURE_SCALE = 0x0017,
    GESTURE_SCROLL = 0x0018,
    GESTURE_GESTURE_ALL = 0xFFFFFFFF
};

/*
* The class supply the content information about the event.
* The SetXxx() API is almost supplied to ACE to set the information.
* Parts of SetXxx() API is called by AAMS, such as: SetConnectId().
* The GetXxx() API is almost supplied to AA to get the information.
*/
class AccessibilityMemo : public Parcelable {
public:
    /**
     * @brief Construct
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityMemo() {}

    /**
     * @brief AAMS called to set the id of the IAccessibleAbilityChannel
     * @param channelId The id of the IAccessibleAbilityChannel
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetChannelId(const int channelId);

    /**
     * @brief Get the id of the IAccessibleAbilityChannel
     * @param -
     * @return The id of the IAccessibleAbilityChannel
     * @since 3
     * @sysCap Accessibility
     */
    int GetChannelId() const;

    /**
     * @brief Set the component of the event related.
     * @param componentId The id of component.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetSource(const int componentId);

    /**
     * @brief Get the node information related with the event
     * @param -
     * @return The node information related with the event
     * @since 3
     * @sysCap Accessibility
     */
    bool GetSource(AccessibilityElementInfo &elementInfo) const;

    /**
     * @brief Gets the view(component) ID associated with the accessibility event.
     * @param -
     * @return The view(component) ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    int GetViewId() const;

    /**
     * @brief Gets the accessibility ID of the component.
     * @param -
     * @return The accessibility ID of the component.
     * @since 3
     * @sysCap Accessibility
     */
    int GetAccessibilityId() const;

    /**
     * @brief Set the window ID associated with the accessibility event.
     * @param windowId The window ID associated with the accessibility event.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowId(const int windowId);

    /**
     * @brief Gets the window ID associated with the accessibility event.
     * @param -
     * @return The window ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */

    int GetWindowId() const;

    /**
     * @brief Gets the current index of listed items or text position associated with the current event.
     * @param -
     * @return The current index of listed items or text position associated with the current event.
     * @since 3
     * @sysCap Accessibility
     */
    int GetCurrentIndex() const;

    /**
     * @brief Sets the current index of listed items or text position associated with the current event.
     * @param index The current index of listed items or text position associated with the current event.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetCurrentIndex(const int index);

    /**
     * @brief Gets the start index of listed items on the screen.
     * @param -
     * @return The start index of listed items. It is unique value for the listed items.
     * @since 3
     * @sysCap Accessibility
     */
    int GetBeginIndex() const;

    /**
     * @brief Sets the start index of listed items on the screen.
     * @param index The start index of listed items. It is unique value for the listed items.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeginIndex(const int index);

    /**
     * @brief Gets the number items on the screen.
     * @param -
     * @return The number of items.
     * @since 3
     * @sysCap Accessibility
     */
    int GetItemCounts() const;

    /**
     * @brief Sets the number items on the screen.
     * @param ItemCounts The number of items.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetItemCounts(const int itemCounts);

    /**
     * @brief Gets the end index of listed items on the screen.
     * @param -
     * @return The end index of listed items on the screen.
     * @since 3
     * @sysCap Accessibility
     */
    int GetEndIndex() const;

    /**
     * @brief Sets the end index of listed items on the screen.
     * @param index The end index of listed items on the screen.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetEndIndex(const int index);

    /**
     * @brief Gets the class(component type) name.
     * @param -
     * @return The class(component type) name.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetComponentType() const;

    /**
     * @brief Sets the class(component type) name.
     * @param className The class(component type) name.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetComponentType(const std::string &className);

    /**
     * @brief Gets the before text changed of the component.
     * @param -
     * @return The before text changed of the component.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetBeforeText() const;

    /**
     * @brief Set the before text changed of the component.
     * @param beforeText The before text changed of the component.
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetBeforeText(const std::string &beforeText);

    /**
     * @brief Adds an text to the content list.
     * @param content Indicates the text to add.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void AddContent(const std::string &content);

    /**
     * @brief Gets the content list in the event.
     * @param -
     * @return Returns the content list in the event.
     * @since 3
     * @sysCap Accessibility
     */
    std::vector<std::string> GetContentList() const;

    /**
     * @brief Gets the last content of the component(The text of changed after).
     * @param -
     * @return The last content of the component(The text of changed after).
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetLatestContent() const;

    /**
     * @brief Sets the last content of the component.
     * @param content Indicates the last content to set for the component.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetLatestContent(const std::string &content);

    /**
     * @brief Gets the accessibility event description.
     * @param -
     * @return The accessibility event description
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetDescription() const;

    /**
     * @brief Sets the accessibility event description.
     * @param contentDescription The accessibility event description
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetDescription(const std::string &contentDescription);

    /*
     * @brief Used for IPC communication
     * @param parcel
     * @return rue: Read parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC communication
     * @param parcel
     * @return true: Write parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief
     * @param parcel Used for IPC communication
     * @return Read AccessibilityMemo from parcel data
     * @since 3
     * @sysCap Accessibility
     */
    static AccessibilityMemo *Unmarshalling(Parcel &parcel);

private:
    int channelId_ = 0;
    int componentId_ = 0;
    int windowId_ = 0;
    int currentIndex_ = 0;
    int beginIndex_ = 0;
    int endIndex_ = 0;
    std::vector<std::string> contents_ {};
    std::string componentType_ = "";
    std::string description_ = "";
    std::string beforeText_ = "";
    std::string latestConent_ = "";
    int elementId_ = 0;
    int itemCounts_ = 0;
};

/*
* The class define the event types and supply the api to
* get/set the property of event. and it triggerred by UI
* changed and sent to AA. According to the event property,
* AA can get event properties, such as: action triggerred
* the event and the source.
*/
class AccessibilityEventInfo : public AccessibilityMemo {
public:
    /**
     * @brief Construct
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo() {}

    /**
     * @brief Construct
     * @param eventType the type of event info
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo(EventType eventType);

    /**
     * @brief Construct
     * @param windowId The id of window
     * @param windowChangeTypes the window change type
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo(int windowId, WindowUpdateType windowChangeTypes);

    /**
     * @brief Gets the number of accessibility records.
     * @note It is same to getCount​()
     * @param -
     * @return The number of the records that describe the information of current event
     * @since 3
     * @sysCap Accessibility
     */
    int GetRecordCount() const;

    /**
     * @brief Set the number of accessibility records.
     * @note It is same to setCount​(int count)
     * @param recordCount The number of the records that describe the information of current event
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetRecordCount(const int recordCount);

    /**
     * @brief Adds an accessibility record to describe the information of the current event.
     * @note It is same to addRecord​(AccessibilityEventInfo eventInfo)
     * @param record The records that describe the information of current event
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void AddRecord(const AccessibilityEventInfo &record);

    /**
     * @brief Get the record by index.
     * @note Remained
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    AccessibilityEventInfo GetRecord(const int index);    // remained

    /**
     * @brief Gets the accessibility record list.
     * @param
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    std::vector<AccessibilityEventInfo> GetRecords();

    /**
     * @brief Gets the type of an accessibility event.
     * @note It is same to the getAccessibilityEventType​()
     * @param -
     * @return The type of an accessibility event.
     * @since 3
     * @sysCap Accessibility
     */
    EventType GetEventType() const;

    /**
     * @brief Get the window content changed types
     * @param -
     * @return The window content changed types. Refer to "WindowsContentChangeTypes"
     * @since 3
     * * @sysCap Accessibility
     */
    WindowsContentChangeTypes GetWindowContentChangeTypes() const;

    /**
     * @brief Set the window content changed types
     * @param changeTypes The window content changed types. Refer to "WindowsContentChangeTypes"
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowContentChangeTypes(const WindowsContentChangeTypes changeTypes);

    /**
     * @brief Get the window changed types
     * @param -
     * @return The window changed types.refer to WindowUpdateType
     * @since 3
     * @sysCap Accessibility
     */
    WindowUpdateType GetWindowChangeTypes() const;

    /**
     * @brief Set the window changed types
     * @param changeTypes The window changed types.refer to WindowUpdateType
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetWindowChangeTypes(const WindowUpdateType changeTypes);

    /**
     * @brief Sets the type of an accessibility event.
     * @note It is same to setAccessibilityEventType​(int accessibilityEventType)
     * @param eventType The type of an accessibility event. Refer to "EventType"
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetEventType(const EventType eventType);

    /**
     * @brief AA get the time of accessibility event is sent from ASAC.
     * @param -
     * @return The time of accessibility event is sent from ASAC
     * @since 3
     * @sysCap Accessibility
     */
    int64_t GetTimeStamp() const;

    /**
     * @brief ASAC set the time of accessibility is sent to AA
     * @param eventTime The time of accessibility event is sent from ASAC
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetTimeStamp(const int64_t eventTime);  // remained:for send event timestamp

    /**
     * @brief Gets the bundle name of the event source, that is, the bundle name of the target application.
     * @param -
     * @return Returns the bundle name of the event source.
     * @since 3
     * @sysCap Accessibility
     */
    std::string GetBundleName() const;

    /**
     * @brief Set the bundle name of the event source, that is, the bundle name of the target application.
     * @param bundleName The bundle name of the event source.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetBundleName(const std::string &bundleName);

    /**
     * @brief Set the move granularity of the text
     * @param granularity The move granularity of the text. Refer to "AccessibilityElementInfo.TextMoveUnit"
     * @return
     * @since 3
     * @sysCap Accessibility
     */
    void SetTextMovementStep(const TextMoveUnit granularity);

    /**
     * @brief Get the move granularity of the text
     * @param -
     * @return he move granularity of the text. Refer to "AccessibilityElementInfo.TextMoveUnit"
     * @since 3
     * @sysCap Accessibility
     */
    TextMoveUnit GetTextMovementStep() const;

    /**
     * @brief Sets the action that triggers the accessibility event.
     * @param action The operation of the event. Refer to "AccessibilityElementInfo.ActionType"
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetTriggerAction(const ActionType action);

    /**
     * @brief Gets the action that triggers the accessibility event.
     * @param -
     * @return The operation of the event. Refer to "AccessibilityElementInfo.ActionType"
     * @since 3
     * @sysCap Accessibility
     */
    ActionType GetTriggerAction() const;

    /**
     * @brief Set the information of accessibility event of [TYPE_NOTIFICATION_UPDATE_EVENT]
     * @param category Refer to [NotificationCategory], It maybe changed from APP
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetNotificationInfo(const NotificationCategory category);

    /**
     * @brief Get the information of accessibility event of [TYPE_NOTIFICATION_UPDATE_EVENT]
     * @param -
     * @return Refer to [NotificationCategory], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    NotificationCategory  GetNotificationInfo() const;

    /**
     * @brief Set the custemize gesture type of accessibility event of [TYPE_GESTURE_EVENT]
     * @param category Refer to [GestureType], It maybe changed from APP
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetGestureType(const GestureType gestureType);

    /**
     * @brief Get the custemize gesture type of accessibility event of [TYPE_GESTURE_EVENT]
     * @param -
     * @return Refer to [GestureType], It maybe changed from APP
     * @since 3
     * @sysCap Accessibility
     */
    GestureType GetGestureType() const;

    /**
     * @brief Used for IPC comminication
     * @param parcel
     * @return true: Read parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Used for IPC comminication
     * @param parcel
     * @return true: Write parcel data successfully; ohterwise is not.
     * @since 3
     * @sysCap Accessibility
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Used for IPC comminication
     * @param parcel
     * @return Read AccessibilityEventInfo from parcel data
     * @since 3
     * @sysCap Accessibility
     */
    static AccessibilityEventInfo *Unmarshalling(Parcel &parcel);

    /**
     * @brief Set the page ID associated with the accessibility event.
     * @param pageId The page ID associated with the accessibility event.
     * @return -
     * @since 3
     * @sysCap Accessibility
     */
    void SetPageId(const int pageId);

    /**
     * @brief Gets the page ID associated with the accessibility event.
     * @param -
     * @return The page ID associated with the accessibility event.
     * @since 3
     * @sysCap Accessibility
     */

    int GetPageId() const;

private:
    EventType eventType_ = TYPE_VIEW_INVALID;
    std::string bundleName_ = "";
    ActionType triggerAction_ = ACCESSIBILITY_ACTION_INVALID;
    int64_t timeStamp_ = 0;
    TextMoveUnit textMoveStep_ = STEP_CHARACTER;
    WindowsContentChangeTypes windowContentChangeTypes_ = CONTENT_CHANGE_TYPE_INVALID;
    WindowUpdateType windowChangeTypes_ = WINDOW_UPDATE_INVALID;
    GestureType gestureType_ = GESTURE_INVALID;
    int recordsCount_ = 0;
    std::vector<AccessibilityEventInfo> records_;
    NotificationCategory category_ = CATEGORY_INVALID;
    int pageId_ = 0;
};
} // namesapce Accessibility
} // namespace OHOS
#endif