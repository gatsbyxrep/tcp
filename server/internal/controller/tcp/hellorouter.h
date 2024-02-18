#ifndef HELLOROUTER_H
#define HELLOROUTER_H

#include "../../../external/tcpserver/router.h"
#include "../../usecases/hellocodogramrecievedusecase.h"

namespace Controller {
    class HelloRouter : public TCP::Router {
    public:
        HelloRouter(Usecases::HelloCodogramRecievedUsecaseInterface& helloUsecase);
        Async::Task<void> route(TCP::Stream s);
    private:
        Usecases::HelloCodogramRecievedUsecaseInterface& helloUsecase;
    };
}

#endif // HELLOROUTER_H
