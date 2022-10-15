/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MOCK_INPUT_MANAGER_H
#define MOCK_INPUT_MANAGER_H

#include "input_manager.h"

namespace OHOS {
namespace MMI {
class MockInputManager {
public:
    static int GetKeyCode();
    static void ClearTouchActions();
    static std::vector<int32_t> GetTouchActions();
    static int32_t GetTouchActionOfTargetIndex(int32_t index);
    static void ClearInputEventConsumer();
    static std::shared_ptr<IInputEventConsumer> GetInputEventConsumer();
    static std::function<void(std::shared_ptr<KeyEvent>)> GetKeyEventInterceptor();
};
} // namespace MMI
} // namespace OHOS
#endif // MOCK_INPUT_MANAGER_H