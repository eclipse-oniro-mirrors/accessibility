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

#include "input_manager.h"
#include "pointer_event.h"
#include <vector>

namespace OHOS {
std::vector<int32_t> g_mtTouchAction;
std::function<void(std::shared_ptr<MMI::PointerEvent>)> g_pointerCallback = nullptr;
namespace MMI {
int MTtouchAction = -1;

InputManager *InputManager::mInstance_ = nullptr;

InputManager *InputManager::GetInstance()
{
    if (mInstance_ == nullptr) {
        mInstance_ = new InputManager();
    }
    return mInstance_;
}

void InputManager::SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{

}

void InputManager::SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    MTtouchAction = pointerEvent->GetPointerAction();
    g_mtTouchAction.push_back(MTtouchAction);
}

int32_t InputManager::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptorId)
{
    return 0;
}

void InputManager::RemoveInterceptor(int32_t interceptorId)
{

}

void InputManager::UpdateDisplayInfo(const std::vector<PhysicalDisplayInfo> &physicalDisplays,
    const std::vector<LogicalDisplayInfo> &logicalDisplays)
{

}

void InputManager::SetWindowInputEventConsumer(std::shared_ptr<OHOS::MMI::IInputEventConsumer> inputEventConsumer)
{

}

int32_t InputManager::SubscribeKeyEvent(std::shared_ptr<KeyOption> keyOption,
    std::function<void(std::shared_ptr<KeyEvent>)> callback)
{
    return 0;
}

void InputManager::UnsubscribeKeyEvent(int32_t subscriberId)
{

}

void InputManager::RemoveMonitor(int32_t monitorId)
{

}

void InputManager::MarkConsumed(int32_t monitorId, int32_t eventId)
{

}

int32_t InputManager::AddInterceptor(int32_t sourceType, std::function<void(std::shared_ptr<PointerEvent>)> interceptor)
{
    g_pointerCallback = interceptor;
    return 0;
}

int32_t InputManager::AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor)
{
    return 0;
}
}
}