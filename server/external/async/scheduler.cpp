#include "scheduler.hpp"

#include <iostream>

using namespace Async;

Scheduler::Scheduler(Config::Scheduler cfg)
    :   isRunning(true),
        threadPool(cfg.workerThreadsCount),
        cfg(cfg) {
    for (size_t i = 0; i != cfg.workerThreadsCount; i++) {
        threadPool[i] = std::thread(&Scheduler::run, this);
    }
}

void Scheduler::spawn(std::function<void()>&& routine) {
    routines.push(std::move(routine));
}

Scheduler::~Scheduler() {
    for (auto& worker : threadPool) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void Scheduler::stop() {
    isRunning = false;
}

void Scheduler::run() noexcept {
    while (isRunning) {
        try {
            auto routine = routines.pop();
            if(routine.has_value()) {
                routine.value()();
            }
        } catch (const std::exception& e) {

        }
    }
}
