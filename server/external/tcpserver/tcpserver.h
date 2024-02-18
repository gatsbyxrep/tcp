#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <csignal>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <functional>

#include "../logger/logger.h"

#include "router.h"
#include "../async/async.h"

namespace TCP {
    struct SuspendedInfo {
        int socketFd;
        int event;
        friend bool operator ==(const TCP::SuspendedInfo& l, const TCP::SuspendedInfo& r) {
            return l.socketFd == r.socketFd;
        }
    };
}



namespace std {
    template <>
    struct std::hash<TCP::SuspendedInfo> {
        std::size_t operator()(const TCP::SuspendedInfo& k) const {
            static auto hasher = std::hash<int>();
            return hasher(k.socketFd);
        }
    };
}

namespace TCP {
    class TCPServer {
    public:
        TCPServer(Config::Config cfg, Router& router, Logger::Logger& logger);
        ~TCPServer();
        void stop();
        void run();
        template <typename Callable>
        void schedule(SuspendedInfo suspendedInfo, Callable&& callable) {
            std::lock_guard lock{suspendedMutex};
            socketsHandlers.try_emplace(suspendedInfo, std::forward<Callable>(callable));
        }
    private:
        std::sig_atomic_t isRunning;
        int socketFd;
        int epollFd;
        sockaddr_in server;
        std::thread thread;
        Router& router;
        Logger::Logger& logger;
        std::shared_ptr<Async::Executor> exe;
        std::unordered_map<SuspendedInfo, std::function<void()>> socketsHandlers;
        std::mutex suspendedMutex;
        Config::Config cfg;

        int createSocketFd();
        void setToNonBlocking(int socketFd);
        void addEventToEpoll(int socketFd, uint32_t events);
        void epollIn(int socket);
        Async::Task<int> accept();
    };

    struct SocketReady : public Async::Schedulable {
        SocketReady(std::shared_ptr<TCPServer> server);
        SuspendedInfo info;
        std::shared_ptr<TCPServer> server;
    };

    struct ReadyRead : public SocketReady {
        ReadyRead(int socketFd, std::shared_ptr<TCPServer> server);
        void schedule(std::coroutine_handle<> handle) const override;
    };
}

#endif // TCPSERVER_H
