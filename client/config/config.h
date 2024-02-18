#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config {
    struct Server {
        std::string ip;
        std::string port;
    };
    struct App {
        std::string name;
        unsigned int sayTimeout;
    };

    struct Config {
        Server server;
        App app;
    };
}

#endif // CONFIG_H
