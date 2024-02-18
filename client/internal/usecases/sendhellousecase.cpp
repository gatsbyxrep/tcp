#include "sendhellousecase.h"

#include <iostream>
#include <thread>
#include <chrono>

using namespace Usecases;

SendHelloUsecase::SendHelloUsecase(Config::App cfg, API::ServerAPI& api)
    : isRunning(false),
       cfg(cfg),
       api(api) {}

void SendHelloUsecase::startHello() {
    isRunning = true;
    Entities::HelloCodogram hello = {
        .name = cfg.name
    };
    while(isRunning) {
        api.hello(hello);
        std::cout << "Sent hello from " << hello.name << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.sayTimeout));
    }
}

void SendHelloUsecase::stop() {
    isRunning = false;
}
