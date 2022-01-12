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
 
#include "multimoduleDummy.h"
#include "touch_event.h"

namespace OHOS {
namespace Accessibility {

int MTtouchAction = -1;
std::vector<int> mtTouchAction;

void MultimodalInputFilter::InjectEventToMultimodal(MultimodalEvent &event) {
    TouchEvent* touchAction = (TouchEvent*)&event;
    MTtouchAction = touchAction->GetAction();
    mtTouchAction.push_back(MTtouchAction);
    // delete touchAction;
}

void MultimodalInputManager::RegisterMultimodalInputFilter(MultimodalInputFilter &inputFilter) {
}

void MultimodalInputManager::DeregisterMultimodalInputFilter() {
}
}
}