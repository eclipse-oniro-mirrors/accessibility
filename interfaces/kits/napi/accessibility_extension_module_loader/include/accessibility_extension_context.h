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

#ifndef ACCESSIBILITY_EXTENSION_CONTEXT_H
#define ACCESSIBILITY_EXTENSION_CONTEXT_H

#include <map>
#include <memory>
#include "accessibility_element_info.h"
#include "accessibility_event_info.h"
#include "accessibility_gesture_inject_path.h"
#include "accessibility_gesture_result_listener.h"
#include "accessibility_window_info.h"
#include "extension_context.h"

namespace OHOS {
namespace Accessibility {
class AccessibilityExtensionContext : public AbilityRuntime::ExtensionContext {
public:
    AccessibilityExtensionContext() = default;
    virtual ~AccessibilityExtensionContext() = default;

    /**
     * @brief Obtains elementInfo of focus.
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetFocus(const int32_t focusType, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Obtains elementInfo of focus.
     * @param sourceInfo
     * @param focusType The type of focus. It contains FOCUS_TYPE_INPUT and FOCUS_TYPE_ACCESSIBILITY.
     * @param elementInfo The accessibilityElementInfo of focus.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetFocusByElementInfo(const AccessibilityElementInfo &sourceInfo, const int32_t focusType,
        AccessibilityElementInfo &elementInfo);

    /**
     * @brief Sends simulate gestures to the screen.
     * @param sequence The sequence of gesture.
     * @param gesturePath The gesture which need to send.
     * @param listener The listener of the gesture.
     * @return Return true if the gesture sends successfully, else return false.
     */
    bool InjectGesture(const uint32_t sequence, const std::shared_ptr<AccessibilityGestureInjectPath> &gesturePath,
        const std::shared_ptr<AccessibilityGestureResultListener> &listener);

    /**
     * @brief Sends simulate gestures to the screen.
     * @param sequence The sequence of gesture.
     * @param gesturePaths The gesture which need to send.
     * @param listener The listener of the gesture.
     * @return Return true if the gesture sends successfully, else return false.
     */
    bool InjectGesture(const uint32_t sequence,
        const std::vector<std::shared_ptr<AccessibilityGestureInjectPath>> &gesturePaths,
        const std::shared_ptr<AccessibilityGestureResultListener> &listener);

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetRoot(AccessibilityElementInfo &elementInfo);

    /**
     * @brief Obtains elementInfo of the accessible root node.
     * @param windowInfo The source window info to get root.
     * @param elementInfo The elementInfo of the accessible root node.
     * @return Return true if obtains elementInfo successfully, else return false.
     */
    bool GetRootByWindow(const AccessibilityWindowInfo &windowInfo,
        AccessibilityElementInfo &elementInfo);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param windows The information of windows.
     * @return Return true if obtains windowInfo successfully, else return false.
     */
    bool GetWindows(std::vector<AccessibilityWindowInfo> &windows);

    /**
     * @brief Obtains the list of interactive windows on the device, in the layers they are visible to users.
     * @param displayId the id of display
     * @param windows The information of windows.
     * @return Return true if obtains windowInfo successfully, else return false.
     */
    bool GetWindows(const uint64_t displayId, std::vector<AccessibilityWindowInfo> &windows);

    /**
     * @brief Executes a specified action.
     * @param action The action of OHOS::Accessibility::GlobalAction.
     * @return Return true if executes action successfully, else return false.
     */
    bool ExecuteCommonAction(const GlobalAction action);

    /**
     * @brief Gets the next focused node in the specified direction of the currently focused node.
     * @param elementInfo
     * @param direction Indicates the direction to obtain the next focused node. Refer to FocusMoveDirection
     * @param nextElementInfo The info of next element.
     * @return Return true if gets next elementInfo successfully, else return false.
     */
    bool GetNext(const AccessibilityElementInfo &elementInfo, const FocusMoveDirection direction,
        AccessibilityElementInfo &nextElementInfo);

    /**
     * @brief Obtains information about the accessibility child window at a specified index.
     * @param index
     * @param parent
     * @param child
     * @return Return true if gets child windowInfo successfully, else return false.
     */
    bool GetChildWindowInfo(const int32_t index, const AccessibilityWindowInfo &parent,
        AccessibilityWindowInfo &child);

    /**
     * @brief Get the child node information by childId
     * @param index
     * @param parent
     * @param child The element info of child
     * @return Return true if gets child elementInfo successfully, else return false.
     */
    bool GetChildElementInfo(const int32_t index, const AccessibilityElementInfo &parent,
        AccessibilityElementInfo &child);

    /**
     * @brief Searches for node information based on the specified content.
     * @param elementInfo
     * @param text specified content
     * @param elementInfos
     * @return Return true if gets elementInfos successfully, else return false.
     */
    bool GetByContent(const AccessibilityElementInfo &elementInfo, const std::string &text,
        std::vector<AccessibilityElementInfo> &elementInfos);

    /**
     * @brief Obtains an anchor accessibility node that anchors this window to another window.
     * @param windowInfo
     * @param elementInfo
     * @return Return true if gets elementInfos successfully, else return false.
     */
    bool GetAnchor(const AccessibilityWindowInfo &windowInfo, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get the node information related with the event
     * @param eventInfo
     * @param elementInfo
     * @return Return true if gets elementInfos successfully, else return false.
     */
    bool GetSource(const AccessibilityEventInfo &eventInfo, AccessibilityElementInfo &elementInfo);

    /**
     * @brief Get the parent window
     * @param child
     * @param parent
     * @return Return true if gets info successfully, else return false.
     */
    bool GetParentWindowInfo(const AccessibilityWindowInfo &child, AccessibilityWindowInfo &parent);

    /**
     * @brief Get Parent node information
     * @param child
     * @param parent
     * @return Return true if gets info successfully, else return false.
     */
    bool GetParentElementInfo(const AccessibilityElementInfo &child, AccessibilityElementInfo &parent);

    /**
     * @brief Get the node information labeled component
     * @param elementInfo
     * @param labeledElementInfo
     * @return Return true if gets infos successfully, else return false.
     */
    bool GetLabeled(const AccessibilityElementInfo &elementInfo, AccessibilityElementInfo &labeledElementInfo);

    /**
     * @brief Executes a specified action.
     * @param elementInfo
     * @param action: the action type
     * @param actionArguments: The parameter for action type. such as:
     *      action: ACCESSIBILITY_ACTION_NEXT_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_HTML_ITEM,
     *                  actionArguments(ACTION_ARGU_HTML_ELEMENT,HtmlItemType)
     *      action: ACCESSIBILITY_ACTION_NEXT_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_PREVIOUS_TEXT,
     *                  actionArguments(ACTION_ARGU_MOVE_UNIT,MOVE_UNIT_XXX)
     *      action: ACCESSIBILITY_ACTION_SET_SELECTION,
     *                  actionArguments({ACTION_ARGU_SELECT_TEXT_START,"1"(start location)},
     *                                  {ACTION_ARGU_SELECT_TEXT_END,"10"(end location)})
     *      action: ACCESSIBILITY_ACTION_SET_TEXT,
     *                  actionArguments(ACTION_ARGU_SET_TEXT,"the text of setted")
     * @return true Perform action succeed, otherwise is not.
     */
    bool ExecuteAction(const AccessibilityElementInfo &elementInfo, const ActionType action,
        const std::map<std::string, std::string> &actionArguments);

    /**
     * @brief Set event types to filter.
     * @param filter The event types which you want.
     * @return Return true if sets event types successfully, else return false.
     */
    bool SetEventTypeFilter(const uint32_t filter);

    /**
     * @brief Set target bundle names.
     * @param targetBundleNames The target bundle name
     * @return Return true if sets target bundle names successfully, else return false.
     */
    bool SetTargetBundleName(const std::vector<std::string> &targetBundleNames);
};
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_EXTENSION_CONTEXT_H