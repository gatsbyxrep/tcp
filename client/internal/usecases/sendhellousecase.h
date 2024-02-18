#ifndef SENDHELLOUSECASE_H
#define SENDHELLOUSECASE_H

#include <csignal>

#include "../../config/config.h"
#include "../../external/serverapi/serverapi.h"

namespace Usecases {
    class SendHelloUsecaseInterface {
    public:
        virtual ~SendHelloUsecaseInterface() = default;
        virtual void startHello() = 0;
        virtual void stop() = 0;
    };

    class SendHelloUsecase : public SendHelloUsecaseInterface {
    public:
        SendHelloUsecase(Config::App cfg, API::ServerAPI& api);
        void stop() override;
        void startHello() override;
    private:
        std::sig_atomic_t isRunning;
        Config::App cfg;
        API::ServerAPI& api;
    };
}

#endif // SENDHELLOUSECASE_H
