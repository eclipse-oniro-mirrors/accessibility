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

#ifndef HILOG_WRAPPER_H
#define HILOG_WRAPPER_H

#define CONFIG_HILOG
#ifdef CONFIG_HILOG
#include "hilog/log.h"

#ifdef HILOG_INFO
#undef HILOG_INFO
#endif

#ifdef HILOG_DEBUG
#undef HILOG_DEBUG
#endif

#ifdef HILOG_FATAL
#undef HILOG_FATAL
#endif

#ifdef HILOG_ERROR
#undef HILOG_ERROR
#endif

#ifdef HILOG_WARN
#undef HILOG_WARN
#endif

#ifndef AAMS_LOG_DOMAIN
#define AAMS_LOG_DOMAIN 0xD001D04
#endif

#ifndef AAMS_LOG_TAG
#define AAMS_LOG_TAG "AccessibleAbilityManagerService"
#endif

#ifdef LOG_LABEL
#undef LOG_LABEL
#endif

#ifndef RELEASE_VERSION
#define HILOG_FATAL(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_FATAL, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_ERROR(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_ERROR, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_WARN(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_WARN, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_INFO(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_INFO, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_DEBUG(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_DEBUG, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[%{public}s(%{public}s:%{public}d)]" fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#else // RELEASE_VERSION
#define HILOG_FATAL(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_FATAL, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[(%{public}s:%{public}d)]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_ERROR(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_ERROR, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[(%{public}s:%{public}d)]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_WARN(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_WARN, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[(%{public}s:%{public}d)]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_INFO(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_INFO, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[(%{public}s:%{public}d)]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define HILOG_DEBUG(fmt, ...)            \
    ((void)HILOG_IMPL(LOG_CORE, LOG_DEBUG, AAMS_LOG_DOMAIN, AAMS_LOG_TAG, \
    "[(%{public}s:%{public}d)]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#endif // RELEASE_VERSION
#else

#define HILOG_FATAL(...)
#define HILOG_ERROR(...)
#define HILOG_WARN(...)
#define HILOG_INFO(...)
#define HILOG_DEBUG(...)
#endif  // CONFIG_HILOG

#endif  // HILOG_WRAPPER_H