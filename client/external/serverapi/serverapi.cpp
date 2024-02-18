#include "serverapi.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace API;

ServerAPI::ServerAPI(Config::Server cfg)
    : cfg(cfg) {
    setupConnection();
}

ServerAPI::ServerAPI::~ServerAPI() {
    close(clientSocket);
}

void ServerAPI::setupConnection() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        throw std::runtime_error("Error creating socket");
        return;
    }
    sockaddr_in serverAddr;
    std::cout << "Connecting to server " << cfg.ip << ':' << cfg.port << '\n';
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::atoi(cfg.port.c_str()));
    serverAddr.sin_addr.s_addr = inet_addr(cfg.ip.c_str());
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cout << "Errno: " << errno << '\n';
        throw std::runtime_error("Error connecting to server");
        return;
    }
}

void ServerAPI::hello(const Entities::HelloCodogram& hello) {
    std::vector<char> data;
    data.resize(hello.name.size() + 2);
    data[0] = 1;
    data[1] = hello.name.size();
    std::strcpy(data.data() + 2, hello.name.c_str());
    ::send(clientSocket, data.data(), data.size(), 0);
}


