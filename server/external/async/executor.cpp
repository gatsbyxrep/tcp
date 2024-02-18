#include "executor.hpp"
#include "task.hpp"

using namespace Async;

Executor::Executor(Config::Executor cfg)
    :   scheduler(std::make_shared<Scheduler>(cfg.scheduler)),
        cfg(cfg) {

}

void Executor::spawnTask(std::function<Task<void>()>&& awaitable) {
    auto workUnit = [awaitable = std::move(awaitable)]() mutable {
        auto task = spawnFinal(std::move(awaitable));
    };
    spawnFunc(workUnit);
}


void Executor::spawnFunc(std::function<void()>&& task) {
    scheduler->spawn(std::move(task));
}

void Executor::stop() {
    scheduler->stop();
}

void Executor::run(std::function<Task<void>()>&& handler) {
    auto task = spawnFinal(std::move(handler));
}

Executor::~Executor() {

}
