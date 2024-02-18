#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <memory>

#include "../../config/config.h"

#include "common.hpp"
#include "scheduler.hpp"
#include "task.hpp"

namespace Async {
    class Executor {
    public:
        Executor(Config::Executor cfg);
        ~Executor();
        template <typename Callable>
        void scheduleSuspended(Callable&& callable) {
            scheduler->spawn(std::move(callable));
        }
        void spawnFunc(std::function<void()>&& task);
        void spawnTask(std::function<Task<void>()>&& awaitable);
        void stop();
        void run(std::function<Task<void>()>&& handler);
    private:
        std::shared_ptr<Scheduler> scheduler;
        [[maybe_unused]] Config::Executor cfg;
    };
}

#endif // EXECUTOR_HPP
