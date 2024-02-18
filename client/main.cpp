#include <iostream>
#include <string>

#include "external/serverapi/serverapi.h"
#include "internal/usecases/sendhellousecase.h"

Usecases::SendHelloUsecase* helloUc;

void signalsHandler(int) {
    helloUc->stop();
}

int main(int argc, char** args) {
    signal(SIGINT, &signalsHandler);
    std::string name = "Name";
    std::string port = "1234";
    std::string timeout = "1000";
    if(argc == 4) {
        name = std::string{args[1]};
        port = std::string{args[2]};
        timeout = std::string{args[3]};
    } else {
        std::cout << "Invalid args count (want 3), default settings will be used\n";
    }
    Config::Config cfg {
        .server = {
            .ip = "127.0.0.1",
            .port = port
        },
        .app = {
            .name = name,
            .sayTimeout = static_cast<unsigned int>(std::atoi(timeout.c_str()))
        }
    };
    API::ServerAPI server{cfg.server};
    Usecases::SendHelloUsecase helloSender {cfg.app, server};
    helloUc = &helloSender;
    helloSender.startHello();
    return 0;
}
