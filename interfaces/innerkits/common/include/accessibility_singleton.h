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

#ifndef ACCESSIBILITY_SINGLETON_H
#define ACCESSIBILITY_SINGLETON_H

namespace OHOS {
namespace Accessibility {
#define ACCESSIBILITY_DECLARE_SINGLETON(MyClass)    \
public:                                             \
    static MyClass& GetInstance();                  \
private:                                            \
    MyClass& operator=(const MyClass&) = delete;    \
    MyClass(const MyClass&) = delete;               \
    MyClass();                                      \
    ~MyClass();                                     \
    static MyClass instance_;

#define ACCESSIBILITY_DEFINE_SINGLETON(MyClass)     \
MyClass MyClass::instance_;                         \
MyClass& MyClass::GetInstance()                     \
{                                                   \
    return instance_;                               \
}
} // namespace Accessibility
} // namespace OHOS
#endif // ACCESSIBILITY_SINGLETON_H