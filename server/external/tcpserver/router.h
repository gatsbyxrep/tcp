#ifndef ROUTER_H
#define ROUTER_H

#include "../async/async.h"

namespace TCP {
    class TCPServer;
    struct Stream {
        int socket;
        TCPServer* server;
        Async::Task<std::vector<char>> read(size_t size);
    };
    class Router {
    public:
        virtual ~Router() = default;
        virtual Async::Task<void> route(Stream stream) = 0;
    };
}

#endif // ROUTER_H
