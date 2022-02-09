// /*
//  * Copyright (c) 2021 Huawei Device Co., Ltd.
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include "event_runner.h"

// #include <condition_variable>
// #include <mutex>
// #include <sstream>
// #include <thread>
// #include <unordered_map>
// #include <vector>

// #include <sys/prctl.h>

// #include "event_handler.h"
// #include "event_handler_utils.h"
// #include "event_inner_runner.h"
// #include "thread_local_data.h"
// #include "singleton.h"

// DEFINE_HILOG_LABEL("EventRunner");

// namespace OHOS {
// namespace AppExecFwk {
// namespace {
// // Invoke system call to set name of current thread.
// inline void SystemCallSetThreadName(const std::string &name)
// {
// }

// // Help to calculate hash code of object.
// template<typename T>
// inline size_t CalculateHashCode(const T &obj)
// {
//     return 0
// }

// // Thread collector is used to reclaim thread that needs to finish running.
// class ThreadCollector : public DelayedRefSingleton<ThreadCollector> {
//     DECLARE_DELAYED_REF_SINGLETON(ThreadCollector);

// public:
//     DISALLOW_COPY_AND_MOVE(ThreadCollector);

//     using ExitFunction = std::function<void()>;

//     void ReclaimCurrentThread()
//     {
//         // Get id of current thread.
//         auto threadId = std::this_thread::get_id();
//         HILOGD("Reclaim: Thread id: %{public}zu", CalculateHashCode(threadId));

//         {
//             // Add thread id to list and notify to reclaim.
//             std::lock_guard<std::mutex> lock(collectorLock_);
//             if (destroying_) {
//                 HILOGI("Reclaim: Thread collector is destroying");
//                 return;
//             }

//             reclaims_.emplace_back(threadId);
//             if (isWaiting_) {
//                 condition_.notify_one();
//             }
//         }

//         if (threadLock_.try_lock()) {
//             if ((!thread_) && (needCreateThread_)) {
//                 // Start daemon thread to collect finished threads, if not exist.
//                 thread_ = std::make_unique<std::thread>(&ThreadCollector::Run, this);
//             }
//             threadLock_.unlock();
//         }
//     }

//     bool Deposit(std::unique_ptr<std::thread> &thread, const ExitFunction &threadExit)
//     {
//         if ((!thread) || (!thread->joinable()) || (!threadExit)) {
//             auto threadState = thread ? (thread->joinable() ? "active" : "finished") : "null";
//             HILOGE("Deposit(%{public}s, %{public}s): Invalid parameter", threadState, threadExit ? "valid" : "null");
//             return false;
//         }

//         auto threadId = thread->get_id();
//         HILOGD("Deposit: New thread id: %{public}zu", CalculateHashCode(threadId));
//         // Save these information into map.
//         std::lock_guard<std::mutex> lock(collectorLock_);
//         if (destroying_) {
//             HILOGW("Deposit: Collector thread is destroying");
//             return false;
//         }
//         // Save thread object and its exit callback.
//         depositMap_.emplace(threadId,
//             ThreadExitInfo {
//                 .thread = std::move(thread),
//                 .threadExit = threadExit,
//             });
//         return true;
//     }

// private:
//     DEFINE_HILOG_LABEL("ThreadCollector");

//     struct ThreadExitInfo {
//         std::unique_ptr<std::thread> thread;
//         ExitFunction threadExit;
//     };

//     inline void ReclaimAll()
//     {
//         std::unique_lock<std::mutex> lock(collectorLock_);
//         // All thread deposited need to stop one by one.
//         while (!depositMap_.empty()) {
//             DoReclaimLocked(lock, depositMap_.begin());
//         }
//     }

//     void Stop()
//     {
//         {
//             // Stop the collect thread, while destruction of collector.
//             std::lock_guard<std::mutex> lock(collectorLock_);
//             destroying_ = true;
//             if (isWaiting_) {
//                 condition_.notify_all();
//             }
//         }

//         {
//             std::lock_guard<std::mutex> lock(threadLock_);
//             if ((thread_) && (thread_->joinable())) {
//                 // Wait utils collect thread finished, if exists.
//                 thread_->join();
//             }
//             needCreateThread_ = false;
//         }

//         ReclaimAll();
//     }

//     void DoReclaimLocked(std::unique_lock<std::mutex> &lock,
//         std::unordered_map<std::thread::id, ThreadExitInfo>::iterator it, bool needCallExit = true)
//     {
//         if (it == depositMap_.end()) {
//             return;
//         }

//         // Remove thread information from map.
//         auto threadId = it->first;
//         auto exitInfo = std::move(it->second);
//         (void)depositMap_.erase(it);

//         // Unlock, because stopping thread maybe spend lot of time, it should be out of the lock.
//         lock.unlock();

//         size_t hashThreadId = CalculateHashCode(threadId);
//         HILOGD("DoReclaim: Thread id: %{public}zu", hashThreadId);
//         if (needCallExit) {
//             // Call exit callback to stop loop in thread.
//             exitInfo.threadExit();
//         }
//         // Wait until thread finished.
//         exitInfo.thread->join();
//         HILOGD("DoReclaim: Done, thread id: %{public}zu", hashThreadId);

//         // Lock again.
//         lock.lock();
//     }

//     void Run()
//     {
//         HILOGD("Run: Collector thread is started");

//         std::unique_lock<std::mutex> lock(collectorLock_);
//         while (!destroying_) {
//             // Reclaim threads in list one by one.
//             while (!reclaims_.empty()) {
//                 auto threadId = reclaims_.back();
//                 reclaims_.pop_back();
//                 DoReclaimLocked(lock, depositMap_.find(threadId), false);
//             }

//             // Maybe stop running while doing reclaim, so check before waiting.
//             if (destroying_) {
//                 break;
//             }

//             isWaiting_ = true;
//             condition_.wait(lock);
//             isWaiting_ = false;
//         }

//         HILOGD("Run: Collector thread is stopped");
//     }

//     std::mutex collectorLock_;
//     std::condition_variable condition_;
//     bool isWaiting_ {false};
//     bool destroying_ {false};
//     std::vector<std::thread::id> reclaims_;
//     std::unordered_map<std::thread::id, ThreadExitInfo> depositMap_;

//     std::mutex threadLock_;
//     // Thread for collector
//     std::unique_ptr<std::thread> thread_;
//     bool needCreateThread_ {true};

//     // Avatar of thread collector, used to stop collector at the specified opportunity.
//     class Avatar {
//     public:
//         DISALLOW_COPY_AND_MOVE(Avatar);

//         Avatar() = default;
//         ~Avatar()
//         {
//             if (avatarEnabled_) {
//                 GetInstance().avatarDestructed_ = true;
//                 GetInstance().Stop();
//             }
//         }

//         inline void Disable() const
//         {
//             avatarEnabled_ = false;
//         }
//     };

//     // Mark whether avatar is destructed.
//     volatile bool avatarDestructed_ {false};
//     // Mark whether avatar is enabled.
//     static volatile bool avatarEnabled_;
//     static Avatar avatar_;
// };

// ThreadCollector::ThreadCollector() :
//     collectorLock_(), condition_(), reclaims_(), depositMap_(), threadLock_(), thread_(nullptr)
// {
//     // Thread collector is created, so enable avatar.
//     avatarEnabled_ = true;
// }

// ThreadCollector::~ThreadCollector()
// {
//     // If avatar is not destructed, stop collector by itself.
//     if (!avatarDestructed_) {
//         avatar_.Disable();
//         Stop();
//     }
// }

// class EventRunnerImpl final : public EventInnerRunner {
// public:
//     explicit EventRunnerImpl(const std::shared_ptr<EventRunner> &runner) : EventInnerRunner(runner)
//     {
//         queue_ = std::make_shared<EventQueue>();
//     }

//     ~EventRunnerImpl() final = default;
//     DISALLOW_COPY_AND_MOVE(EventRunnerImpl);

//     static void ThreadMain(const std::weak_ptr<EventRunnerImpl> &wp)
//     {
//     }

//     void Run() final
//     {

//     }

//     void Stop() final
//     {
//         queue_->Finish();
//     }

//     inline bool Attach(std::unique_ptr<std::thread> &thread)
//     {
//         return true;
//     }

//     inline void SetThreadName(const std::string &threadName)
//     {
//     }

// private:
//     DEFINE_HILOG_LABEL("EventRunnerImpl");
// };
// }  // unnamed namespace

// EventInnerRunner::EventInnerRunner(const std::shared_ptr<EventRunner> &runner) :
//     queue_(nullptr), owner_(runner), logger_(nullptr), threadName_(""), threadId_()
// {}

// std::shared_ptr<EventRunner> EventInnerRunner::GetCurrentEventRunner()
// {
//     return nullptr;
// }

// ThreadLocalData<std::weak_ptr<EventRunner>> EventInnerRunner::currentEventRunner;

// namespace {

// volatile bool ThreadCollector::avatarEnabled_ = false;

// /*
//  * All event runners are relied on 'currentEventRunner', so make sure destruction of 'currentEventRunner'
//  * should after all event runners finished. All event runners finished in destruction of 'ThreadCollector::Avatar',
//  * so instance of 'ThreadCollector::Avatar' MUST defined after 'currentEventRunner'.
//  */
// ThreadCollector::Avatar ThreadCollector::avatar_;

// }  // unnamed namespace

// std::shared_ptr<EventRunner> EventRunner::mainRunner_;

// std::shared_ptr<EventRunner> EventRunner::Create(bool inNewThread)
// {
//     if (inNewThread) {
//         return Create(std::string());
//     }

//     // Constructor of 'EventRunner' is private, could not use 'std::make_shared' to construct it.
//     std::shared_ptr<EventRunner> sp(new EventRunner(false));
//     auto innerRunner = std::make_shared<EventRunnerImpl>(sp);
//     sp->innerRunner_ = innerRunner;
//     sp->queue_ = innerRunner->GetEventQueue();

//     return sp;
// }

// std::shared_ptr<EventRunner> EventRunner::Create(const std::string &threadName)
// {
//     // Constructor of 'EventRunner' is private, could not use 'std::make_shared' to construct it.
//     std::shared_ptr<EventRunner> sp(new EventRunner(true));
//     auto innerRunner = std::make_shared<EventRunnerImpl>(sp);
//     sp->innerRunner_ = innerRunner;
//     sp->queue_ = innerRunner->GetEventQueue();

//     // Start new thread
//     innerRunner->SetThreadName(threadName);
//     auto thread =
//         std::make_unique<std::thread>(EventRunnerImpl::ThreadMain, std::weak_ptr<EventRunnerImpl>(innerRunner));
//     if (!innerRunner->Attach(thread)) {
//         HILOGW("Create: Failed to attach thread, maybe process is exiting");
//         innerRunner->Stop();
//         thread->join();
//     }

//     return sp;
// }

// std::shared_ptr<EventRunner> EventRunner::Current()
// {
//     auto runner = EventInnerRunner::GetCurrentEventRunner();
//     if (runner) {
//         return runner;
//     }
//     return nullptr;
// }

// EventRunner::EventRunner(bool deposit) : deposit_(deposit)
// {}

// EventRunner::~EventRunner()
// {
//     if (deposit_) {
//         innerRunner_->Stop();
//     }
// }

// ErrCode EventRunner::Run()
// {
//     if (deposit_) {
//         HILOGE("Run: Do not call, if event runner is deposited");
//         return EVENT_HANDLER_ERR_RUNNER_NO_PERMIT;
//     }

//     // Avoid more than one thread to start this runner.
//     if (running_.exchange(true)) {
//         HILOGW("Run: Already running");
//         return EVENT_HANDLER_ERR_RUNNER_ALREADY;
//     }

//     // Entry event loop.
//     innerRunner_->Run();

//     running_.store(false);
//     return ERR_OK;
// }

// ErrCode EventRunner::Stop()
// {
//     return ERR_OK;
// }

// void EventRunner::Dump(Dumper &dumper)
// {

// }

// void EventRunner::SetLogger(const std::shared_ptr<Logger> &logger)
// {
// }

// std::shared_ptr<EventQueue> EventRunner::GetCurrentEventQueue()
// {
// }

// uint64_t EventRunner::GetThreadId()
// {
//     return 0;
// }

// bool EventRunner::IsCurrentRunnerThread()
// {
//     return 0;
// }

// std::shared_ptr<EventRunner> EventRunner::GetMainEventRunner()
// {
//     return mainRunner_;
// }
// }  // namespace AppExecFwk
// }  // namespace OHOS
