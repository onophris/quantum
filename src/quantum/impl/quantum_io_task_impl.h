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
//NOTE: DO NOT INCLUDE DIRECTLY

//##############################################################################################
//#################################### IMPLEMENTATIONS #########################################
//##############################################################################################

namespace Bloomberg {
namespace quantum {

template <class RET, class FUNC, class ... ARGS>
IoTask::IoTask(std::shared_ptr<Promise<RET>> promise,
               FUNC&& func,
               ARGS&&... args) :
    _func(Util::BindIoCaller(promise,
                             std::forward<FUNC>(func),
                             std::forward<ARGS>(args)...)),
    _terminated(ATOMIC_FLAG_INIT),
    _queueId((int)IQueue::QueueId::Any),
    _isHighPriority(false)
{
}

template <class RET, class FUNC, class ... ARGS>
IoTask::IoTask(std::shared_ptr<Promise<RET>> promise,
               int queueId,
               bool isHighPriority,
               FUNC&& func,
               ARGS&&... args) :
    _func(Util::BindIoCaller(promise,
                             std::forward<FUNC>(func),
                             std::forward<ARGS>(args)...)),
    _terminated(ATOMIC_FLAG_INIT),
    _queueId(queueId),
    _isHighPriority(isHighPriority)
{
}

inline
void IoTask::terminate()
{
    if (!_terminated.test_and_set())
    {
        //not used
    }
}

inline
int IoTask::run()
{
    return _func ? _func() : (int)ITask::RetCode::NotCallable;
}

inline
void IoTask::setQueueId(int queueId)
{
    _queueId = queueId;
}

inline
int IoTask::getQueueId()
{
    return _queueId;
}

inline
ITask::Type IoTask::getType() const
{
    return ITask::Type::IO;
}

inline
bool IoTask::isBlocked() const
{
    return false;
}

inline
bool IoTask::isHighPriority() const
{
    return _isHighPriority;
}

}}
