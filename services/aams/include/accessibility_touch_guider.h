/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed On an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ACCESSIBILITY_TOUCH_GUIDER_H
#define ACCESSIBILITY_TOUCH_GUIDER_H

#include <string>
#include "accessibility_element_info.h"
#include "accessibility_event_transmission.h"
#include "accessibility_gesture_recognizer.h"

namespace OHOS {
namespace Accessibility {
class TouchGuider;
class AccessibleAbilityManagerService;
class AccessibilityGestureRecognizer;

#define MAX_POINTER_COUNT  32
#define EXIT_GESTURE_REC_TIMEOUT 2000
#define MAX_DRAG_GESTURE_COSINE 0.525321989
#define POINTER_COUNT_1 1
#define POINTER_COUNT_2 2
#define MINI_POINTER_DISTANCE_DIP 200
#define DIVIDE_2(num) ((num) / 2)
#define INDEX_0 0
#define INDEX_1 1
#define INIT_POINT_ID -1
#define INIT_MMIPOINT 0

/**
 * @brief touch Guider state define
 */
enum class TouchGuideState : int {
    TOUCH_GUIDING,
    DRAGGING,
    TRANSMITING,
    GESTURE_RECOGNIZING
};

/**
 * @brief Click location define
 */
enum ClickLocation : int {
    CLICK_NONE,
    CLICK_ACCESSIBILITY_FOCUS,
    CLICK_LAST_TOUCH_GUIDE
};

/**
 * @brief struct to record injected pointers.
 */
struct InjectedEventRecorder {
    int downPointers; 
    int downPointerNum;
    long lastDownTime;
    std::shared_ptr<TouchEvent> lastHoverEvent;
};

/**
 * @brief struct to record received pointers.
 */
struct ReceivedEventRecorder {
    float pointerDownX[MAX_POINTER_COUNT];
    float pointerDownY[MAX_POINTER_COUNT];
    std::shared_ptr<TouchEvent> lastEvent;
};

class TGEventHandler : public AppExecFwk::EventHandler {
public:
    TGEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, TouchGuider &tgServer);
    virtual ~TGEventHandler() = default;
    /**
     * @brief Process the event of install system bundles.
     * @param event Indicates the event to be processed.
     * @return
     */
    virtual void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    /**
     * @brief Send HoverEnter and HoverMove to Multimodal.
     * @param 
     * @return
     */
    void HoverEnterAndMoveRunner();

    /**
     * @brief Send HoverExit to Multimodal.
     * @param
     * @return
     */
    void HoverExitRunner();
    TouchGuider &tgServer_;
};

class TouchGuider : public EventTransmission {
public:
    /**
     * @brief A constructor used to create a touchGuide instance.
     * @param
     * @return
     */    
    TouchGuider();

    /**
     * @brief A destructor used to delete the touchGuide instance.
     * @param
     * @return
     */
    ~TouchGuider() {}

    /**
     * @brief TouchGuide start up.
     * @param
     * @return
     */
    void StartUp();

    /**
     * @brief Handle touch events from previous event stream node.
     *
     * @param event  the touch event to be handled.
     * @return
     */
    void OnTouchEvent(TouchEvent &event) override;

    /**
     * @brief Handle accessibility events from previous event stream node.
     *
     * @param event  the accessibility event  to be handled.
     * @return
     */
    void OnAccessibilityEvent(AccessibilityEventInfo &event) override;
   
    /**
     * @brief Clear event state from specific input source.
     * @param inputSource  the input source.
     * @return
     */
    void ClearEvents(uint32_t inputSource) override;
   
    /**
     * @brief Destroy event state.
     * @param
     * @return
     */
    void DestroyEvents() override;

    /**
     * @brief Send event to multimodal input.
     * @param event the event prepared to send to Multimodal
     * @param action the action of the event
     * @return
     */
    void SendEventToMultimodal(TouchEvent &event, int action);
    
    /**
     * @brief Send accessibility event to specific AccessiblityAbility.
     * @param eventType the type of the event
     * @return
     */
    void SendAccessibilityEventToAA(EventType eventType);

    /**
     * @brief Get hover enter and move event.
     * @param
     * @return Returns motionEvent_ vector.
     */
    std::vector<TouchEvent> getHoverEnterAndMoveEvent();

    /**
     * @brief Clear hover enter and move event.
     * @param 
     * @return
     */
    void ClearHoverEnterAndMoveEvent();

    /**
     * @brief Get last received event.
     * @param 
     * @return Returns last event ptr.
     */
    std::shared_ptr<TouchEvent> getLastReceivedEvent();

    static constexpr uint32_t EXIT_GESTURE_REC_MSG = 0;
    static constexpr uint32_t SEND_HOVER_ENTER_MOVE_MSG = 1;
    static constexpr uint32_t SEND_HOVER_EXIT_MSG = 2;
    static constexpr uint32_t SEND_TOUCH_INTERACTION_END_MSG = 3;
    static constexpr uint32_t SEND_TOUCH_GUIDE_END_MSG = 4;

private:
    class TouchGuideListener : public AccessibilityGestureRecognizeListener{
    public:
        /**
         * @brief A constructor used to create a TouchGuideListener instance.
         * @param
         * @return
         */ 
        explicit TouchGuideListener(TouchGuider &server) : server_(server) {};

        /**
         * @brief Prepare to send the event corresponding to the long press to the Multimodal.
         * @param event the touch event from Multimodal
         * @return
         */ 
        void OnDoubleTapLongPress(TouchEvent &event) override;

        /**
         * @brief Prepare to send the event corresponding to the single tap to the Multimodal.
         * @param event the touch event from Multimodal
         * @return
         */ 
        bool OnDoubleTap(TouchEvent &event) override;

        /**
         * @brief Send GESTURE_BEGIN to AccessiblityAbility.
         * @param 
         * @return
         */ 
        bool OnStarted() override;

        /**
         * @brief Send GESTURE_END and TOUCH_END to AccessiblityAbility.
         * @param gestureId the id of gesture
         * @return
         */ 
        bool OnCompleted(int gestureId) override;

        /**
         * @brief The gesture has been cancelled.
         * @param event the touch event from Multimodal
         * @return
         */ 
        bool OnCancelled(TouchEvent &event) override;
    private:

        /**
         * @brief Get the click position.
         * @param outPoint the click point
         * @return the click location
         */ 
        int GetClickPosition(MmiPoint &outPoint);

        /**
         * @brief Send the single tap events to the Multimodal.
         * @param event the event prepared to send to Multimodal
         * @param point the click point
         * @return whether the message is successfully sent.
         */ 
        bool TransformToSingleTap(TouchEvent &event, MmiPoint &point);
        TouchGuider &server_;
    };
    /**
     * @brief Determine whether to clear the touchguide.
     * @param 
     * @return
     */ 
    void Clear();

    /**
     * @brief clear the touchguide.
     * @param event the last event from Multimodal
     * @return
     */ 
    void Clear(TouchEvent &event);

    /**
     * @brief Handle touch events on touchExploring state.
     * @param event the touch event from Multimodal
     * @return
     */
    void HandleTouchGuidingState(TouchEvent &event);

    /**
     * @brief Handle touch events on dragging state.
     * @param event the touch event from Multimodal
     * @return
     */
    void HandleDraggingState(TouchEvent &event);

    /**
     * @brief Handle touch events on transmiting state.
     * @param event the touch event from Multimodal
     * @return
     */
    void HandleTransmitingState(TouchEvent &event);    

    /**
     * @brief Determine whether it is a drag gesture.
     * @param event the touch event from Multimodal
     * @return whether the dragGesture is accepted.
     */
    bool IsDragGestureAccept(TouchEvent &event);

    /**
     * @brief Get Angle Cos value.
     * @param offsetX the X value
     * @param offsetY the Y value
     * @param isGetX whether is the Angle corresponding to the X axis
     * @return Angle Cos value.
     */
    float GetAngleCos(float offsetX, float offsetY, bool isGetX);

    /**
     * @brief Get the info of injected event.
     * @param event the event prepared to send to Multimodal
     * @return 
     */
    void RecordInjectedEvent(TouchEvent &event);

    /**
     * @brief Get the info of Received event.
     * @param event event the touch event from Multimodal
     * @return 
     */
    void RecordReceivedEvent(TouchEvent &event);

    /**
     * @brief Clear received recorder info.
     * @param 
     * @return 
     */
    void ClearReceivedEventRecorder();

    /**
     * @brief Clear Injected recorder info.
     * @param 
     * @return 
     */
    void ClearInjectedEventRecorder();

    /**
     * @brief Send exit event to multimodal.
     * @param 
     * @return 
     */
    void SendExitEvents();

    /**
     * @brief Send all down events to multimodal.
     * @param event the event prepared to send to Multimodal
     * @return 
     */
    void SendAllDownEvents(TouchEvent &event);

    /**
     * @brief Send all up events to multimodal.
     * @param event the event prepared to send to Multimodal
     * @return
     */
    void SendUpForAllInjectedEvent(TouchEvent &event);

    /**
     * @brief Send exit message.
     * @param 
     * @return
     */
    void PostGestureRecognizeExit();

    /**
     * @brief Send enter and move message.
     * @param event event the touch event from Multimodal
     * @return
     */
    void PostHoverEnterAndMove(TouchEvent &event);

    /**
     * @brief Send exit message.
     * @param 
     * @return
     */
    void PostHoverExit();

    /**
     * @brief Send accessibility event message.
     * @param innerEventID the id of inner event
     * @return
     */
    void PostAccessibilityEvent(uint32_t innerEventID);

    /**
     * @brief Cancel message.
     * @param innerEventID the id of inner event
     * @return
     */
    void CancelPostEvent(uint32_t innerEventID);

    /**
     * @brief Cancel message if it has been sent.
     * @param innerEventID the id of inner event
     * @return
     */
    void CancelPostEventIfNeed(uint32_t innerEventID);

    /**
     * @brief Check whether it has been sending.
     * @param innerEventID the id of inner event
     * @return
     */
    bool HasEventPending(uint32_t innerEventID);

    /**
     * @brief Force send and remove event.
     * @param innerEventID the id of inner event
     * @param event event the touch event from Multimodal
     * @return
     */
    void ForceSendAndRemoveEvent(uint32_t innerEventID, TouchEvent &event);

    /**
     * @brief Handle down events on touchExploring state.
     * @param event event the touch event from Multimodal
     * @return
     */    
    void HandleTouchGuidingStateInnerDown(TouchEvent &event);

    /**
     * @brief Handle move events on touchExploring state.
     * @param event event the touch event from Multimodal
     * @return
     */  
    void HandleTouchGuidingStateInnerMove(TouchEvent &event);

    /**
     * @brief Handle move events on dragging state.
     * @param event event the touch event from Multimodal
     * @return
     */ 
    void HandleDraggingStateInnerMove(TouchEvent &event);

    int currentState_ = -1;
    int longPressPointId_ = INIT_POINT_ID;
    float longPressOffsetX_ = INIT_MMIPOINT;
    float longPressOffsetY_ = INIT_MMIPOINT;
    bool isTouchGuiding_ = false;
    ReceivedEventRecorder receivedRecorder_ = {};
    InjectedEventRecorder injectedRecorder_ = {};
    std::vector<TouchEvent> motionEvent_{};
    AccessibilityGestureRecognizer gestureRecognizer_;
    std::unique_ptr<TouchGuideListener> touchGuideListener_ = nullptr;
    std::shared_ptr<AccessibleAbilityManagerService> pAams_ = nullptr;
    std::shared_ptr<TGEventHandler> handler_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
};
}  // namespace Accessibility
}  // namespace OHOS
#endif  // ACCESSIBILITY_TOUCH_GUIDER_H