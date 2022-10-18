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

#include <mutex>
#include "mock_input_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace MMI {
static int mockKeyCode = -1;
static std::vector<int32_t> mockTouchActions;
static std::function<void(std::shared_ptr<MMI::KeyEvent>)> mockKeyEventCallback = nullptr;
static std::shared_ptr<MMI::IInputEventConsumer> mockInputEventConsumer = nullptr;
static std::mutex g_mtx;

int MockInputManager::GetKeyCode()
{
    return mockKeyCode;
}

void MockInputManager::ClearTouchActions()
{
    std::lock_guard<std::mutex> lock(g_mtx);
    mockTouchActions.clear();
}

std::vector<int32_t> MockInputManager::GetTouchActions()
{
    std::lock_guard<std::mutex> lock(g_mtx);
    return mockTouchActions;
}

int32_t MockInputManager::GetTouchActionOfTargetIndex(int32_t index)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    int32_t size = static_cast<int32_t>(mockTouchActions.size());
    if (size > index) {
        return mockTouchActions[index];
    }
    return -1;
}

void MockInputManager::ClearInputEventConsumer()
{
    HILOG_DEBUG();
    mockInputEventConsumer = nullptr;
}

std::shared_ptr<IInputEventConsumer> MockInputManager::GetInputEventConsumer()
{
    HILOG_DEBUG();
    return mockInputEventConsumer;
}

InputManager *InputManager::instance_ = new(std::nothrow) InputManager();
InputManager *InputManager::GetInstance()
{
    HILOG_DEBUG();
    return instance_;
}

void InputManager::MoveMouse(int32_t offsetX, int32_t offsetY)
{
    HILOG_DEBUG();
    (void)offsetX;
    (void)offsetY;
}

void InputManager::SimulateInputEvent(std::shared_ptr<KeyEvent> keyEvent)
{
    HILOG_DEBUG();
    mockKeyCode = keyEvent->GetKeyCode();
}

void InputManager::SimulateInputEvent(std::shared_ptr<PointerEvent> pointerEvent)
{
    HILOG_DEBUG();
    std::lock_guard<std::mutex> lock(g_mtx);
    int32_t touchAction = pointerEvent->GetPointerAction();
    mockTouchActions.push_back(touchAction);
}

int32_t InputManager::AddInterceptor(std::shared_ptr<IInputEventConsumer> interceptorId)
{
    HILOG_DEBUG();
    mockInputEventConsumer = interceptorId;
    return 0;
}

int32_t InputManager::AddInterceptor(std::function<void(std::shared_ptr<KeyEvent>)> interceptor)
{
    HILOG_DEBUG();
    mockKeyEventCallback = interceptor;
    return 0;
}

std::function<void(std::shared_ptr<KeyEvent>)> MockInputManager::GetKeyEventInterceptor()
{
    HILOG_DEBUG();
    return mockKeyEventCallback;
}

void InputManager::RemoveInterceptor(int32_t interceptorId)
{
    HILOG_DEBUG();
}
} // namespace MMI
} // namespace OHOS