#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    struct TCP {
        std::string port;
    };

    struct Scheduler {
        size_t workerThreadsCount;
    };

    struct Executor {
        Scheduler scheduler;
    };

    struct Config {
        TCP tcp;
        Executor exe;
    };
}

#endif // CONFIG_H
