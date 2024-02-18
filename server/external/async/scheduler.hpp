#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <csignal>
#include <functional>
#include <thread>
#include <vector>

#include "../../config/config.h"

#include "../datastructures/mpmcqueue.hpp"

namespace Async {
    class Scheduler {
    public:
        Scheduler(Config::Scheduler cfg);
        ~Scheduler();
        void stop();
        void spawn(std::function<void()>&& routine);
    private:
        void run() noexcept;
    private:
        std::sig_atomic_t isRunning;
        std::vector<std::thread> threadPool;
        MPMCBlockingQueue<std::function<void()>> routines;
        [[maybe_unused]] Config::Scheduler cfg;
    };
}

#endif // SCHEDULER_HPP
