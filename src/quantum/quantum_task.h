/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef QUANTUM_TASK_H
#define QUANTUM_TASK_H

#include <iostream>
#include <memory>
#include <list>
#include <utility>
#include <quantum/interface/quantum_iterminate.h>
#include <quantum/interface/quantum_icontext.h>
#include <quantum/interface/quantum_iqueue.h>
#include <quantum/interface/quantum_itask_continuation.h>
#include <quantum/interface/quantum_itask_accessor.h>
#include <quantum/quantum_traits.h>
#include <quantum/quantum_util.h>

namespace Bloomberg {
namespace quantum {

//==============================================================================================
//                                 class Task
//==============================================================================================
/// @class Task.
/// @brief Runnable object representing a coroutine.
/// @note For internal use only.
class Task : public ITaskContinuation,
             public std::enable_shared_from_this<Task>
{
public:
    using ptr = std::shared_ptr<Task>;
    using wptr = std::weak_ptr<Task>;
    
    template <class RET, class FUNC, class ... ARGS>
    Task(std::shared_ptr<Context<RET>> ctx,
         ITask::Type type,
         FUNC&& func,
         ARGS&&... args);
    
    template <class RET, class FUNC, class ... ARGS>
    Task(std::shared_ptr<Context<RET>> ctx,
         int queueId,
         bool isHighPriority,
         ITask::Type type,
         FUNC&& func,
         ARGS&&... args);
    
    Task(const Task& task) = delete;
    Task(Task&& task) = default;
    Task& operator=(const Task& task) = delete;
    Task& operator=(Task&& task) = default;
    
    ~Task();
    
    //ITerminate
    void terminate() final;
    
    //ITask
    int run() final;
    void setQueueId(int queueId) final;
    int getQueueId() final;
    Type getType() const final;
    bool isBlocked() const final;
    bool isHighPriority() const final;
    
    //ITaskContinuation
    ITaskContinuation::ptr getNextTask() final;
    void setNextTask(ITaskContinuation::ptr nextTask) final;
    ITaskContinuation::ptr getPrevTask() final;
    void setPrevTask(ITaskContinuation::ptr prevTask) final;
    ITaskContinuation::ptr getFirstTask() final;
    
    //Returns a final or error handler task in the chain and in the process frees all
    //the subsequent continuation tasks
    ITaskContinuation::ptr getErrorHandlderOrFinalTask() final;
    
private:
    ITaskAccessor::ptr          _ctx; //holds execution context
    Traits::context_t           _coro; //the current runnable coroutine
    int                         _queueId;
    bool                        _isHighPriority;
    int                         _rc; //return from the co-routine
    ITaskContinuation::ptr      _next; //Task scheduled to run after current completes.
    ITaskContinuation::wptr     _prev; //Previous task in the chain
    ITask::Type                 _type;
    std::atomic_flag            _terminated;
};

}}

#include <quantum/impl/quantum_task_impl.h>

#endif //QUANTUM_TASK_H