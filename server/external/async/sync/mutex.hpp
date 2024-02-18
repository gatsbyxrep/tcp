#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <atomic>
#include <coroutine>

#include "../../datastructures/mpmcqueue.hpp"
#include "../task.hpp"

namespace Async {
    class Mutex {
    public:
        Mutex();
        ~Mutex();
        void lock();
        void unlock();
    private:
        std::atomic_bool isAcquired;
        MPMCBlockingQueue<std::coroutine_handle<>> waitQueue;
    };
}

#endif // MUTEX_HPP
