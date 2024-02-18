#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <string>

#include "../../internal/entities/hellocodogram.h"
#include "../../config/config.h"

namespace API {
    class ServerAPI {
    public:
        ServerAPI(Config::Server cfg);
        ~ServerAPI();
        void hello(const Entities::HelloCodogram& hello);
    private:
        int clientSocket;
        Config::Server cfg;

        void setupConnection();
    };
}

#endif // SERVERAPI_H
