#include "tcpserver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include "router.h"

using namespace TCP;

SocketReady::SocketReady(std::shared_ptr<TCPServer> server)
    : server(server) {}

ReadyRead::ReadyRead(int socketFd, std::shared_ptr<TCPServer> server)
    : SocketReady(server) {
    info.socketFd = socketFd;
    info.event = EPOLLIN;
}

void ReadyRead::schedule(std::coroutine_handle<> handle) const {
    auto f = [handle]() mutable {
        handle.resume();
    };
    server->schedule(info, f);
}

TCPServer::TCPServer(Config::Config cfg, Router& router, Logger::Logger& logger)
    : isRunning(false),
    socketFd(-1),
    epollFd(-1),
    router(router),
    logger(logger),
    exe(std::make_shared<Async::Executor>(cfg.exe)),
    cfg(cfg) {
    socketFd = createSocketFd();
    setToNonBlocking(socketFd);
    epollFd = epoll_create1(0);

    thread = std::thread(&TCPServer::run, this);
}

TCPServer::~TCPServer() {
    if(thread.joinable()) {
        thread.join();
    }
    close(socketFd);
    close(epollFd);
    logger.log("TCPServer::~TCPServer");
}

void TCPServer::stop() {
    isRunning = false;
    exe->stop();
}

void TCPServer::run() {
    auto f = [&]() -> Async::Task<void> {
        isRunning = true;
        logger.log("Starting server on port: " + cfg.tcp.port);
        addEventToEpoll(socketFd, EPOLLIN);
        std::vector<epoll_event> events;
        const size_t descriptors = 1024; // TODO: take from config
        events.resize(descriptors);
        int clientSocket = -1;
        while(isRunning) {
            int readyCount = epoll_wait(epollFd, events.data(), descriptors, -1);
            if(readyCount < 0) {
                throw std::runtime_error("epoll_wait error");
            }
            for(size_t i = 0; i < static_cast<size_t>(readyCount); ++i) {
                if(events[i].data.fd == socketFd) {
                    clientSocket = co_await accept();
                    setToNonBlocking(clientSocket);
                    addEventToEpoll(clientSocket, EPOLLIN);
                } else if(events[i].events & EPOLLIN) {
                    epollIn(events[i].data.fd);
                }
                // TODO: EPOLLOUT
            }
        }
        co_return;
    };
    exe->run(std::move(f));
}

int TCPServer::createSocketFd() {
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd == -1) {
        throw std::runtime_error("Cant create a socket");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(std::atoi(cfg.tcp.port.c_str()));
    if(::bind(socketFd, (sockaddr*)(&server), sizeof(server)) < 0) {
        throw std::runtime_error("Cant bind");
    }
    if(::listen(socketFd, SOMAXCONN) == -1) {
        throw std::runtime_error("Listen error");
    }
    return socketFd;
}

void TCPServer::setToNonBlocking(int socketFd) {
    int flags = fcntl(socketFd, F_GETFL, 0);
    int res = fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
    if(res < 0) {
        throw std::runtime_error("Cant setup socket in non-blocking mode");
    }
}

void TCPServer::addEventToEpoll(int socketFd, uint32_t events) {
    epoll_event event;
    event.events = events;
    event.data.fd = socketFd;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, socketFd, &event);
}

void TCPServer::epollIn(int socket) {
    std::lock_guard guard{suspendedMutex};
    SuspendedInfo sInfo {
        socket,
        EPOLLIN
    };
    if(socketsHandlers.find(sInfo) != socketsHandlers.end()) {
        std::function<void()> f = socketsHandlers[sInfo];
        exe->spawnFunc(std::move(f));
    } else {
        auto f = [socket = socket, tcp = this]() -> Async::Task<void> {
            Stream s {
                .socket = socket,
                .server = tcp
            };
            co_await tcp->router.route(s);
            co_return;
        };
        exe->spawnTask(std::move(f));
    }
}

Async::Task<int> TCPServer::accept() {
    int clientSocket = -1;
    while(clientSocket < 0) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        clientSocket = ::accept(socketFd, (sockaddr*)(&clientAddr), &clientAddrLen);
        if(clientSocket < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                co_await ReadyRead{clientSocket, std::shared_ptr<TCP::TCPServer>(this)};
            }
        }
    }
    setToNonBlocking(clientSocket);
    addEventToEpoll(clientSocket, EPOLLIN | EPOLLET);
    co_return clientSocket;
}

Async::Task<std::vector<char>> TCP::Stream::read(size_t size) {
    int bytesToRead = size;
    std::vector<char> result;
    result.resize(size);
    char* writeOffset = result.data();
    while(bytesToRead > 0) {
        const int part = ::read(socket, writeOffset, bytesToRead);
        if(part < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                co_await ReadyRead{socket, std::shared_ptr<TCP::TCPServer>(server)};
                continue;
            }
        }
        bytesToRead = bytesToRead - part;
        writeOffset = writeOffset + part;
    }
    co_return result;
}


