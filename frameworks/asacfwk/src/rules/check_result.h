/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef FRAMEWORKS_ASACFWK_SRC_RULES_CHECK_RESULT_H
#define FRAMEWORKS_ASACFWK_SRC_RULES_CHECK_RESULT_H

namespace OHOS::Accessibility {

enum class HandleResult : int32_t {
    TO_NEXT,
    HANDLED,
};

struct CheckResult {
    bool isReadable;
    HandleResult handleResult;
};
} // namespace OHOS::Accessibility
#endif // FRAMEWORKS_ASACFWK_SRC_RULES_CHECK_RESULT_H