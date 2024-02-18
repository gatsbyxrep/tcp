#include <iostream>

#include "external/tcpserver/tcpserver.h"
#include "external/logger/logger.h"
#include "internal/usecases/hellocodogramrecievedusecase.h"
#include "internal/controller/tcp/hellorouter.h"

TCP::TCPServer* tcpServer;

void signalsHandler(int) {
    std::cout << "Signal interrupt\n";
    tcpServer->stop();
}

int main(int argc, char** args) {
    signal(SIGINT, &signalsHandler);
    std::string port = "1234";
    if(argc == 2) {
        port = std::string{args[1]};
    } else {
        std::cout << "Invalid args count (want 1), default settings will be used\n";
    }
    Config::Config cfg = {
        .tcp = {
            .port = port
        },
        .exe = {
            .scheduler = {
                .workerThreadsCount = 4
            }
        }
    };
    Logger::InFileLogger log {"log.txt"};
    Usecases::HelloCodogramRecievedUsecase helloUsecase {log};
    Controller::HelloRouter router {helloUsecase};
    TCP::TCPServer server {cfg, router, log};
    tcpServer = &server;
    // TODO: Gracefull shutdown
    return 0;
}
