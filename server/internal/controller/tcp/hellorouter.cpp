#include "hellorouter.h"

#include <iostream>

using namespace Controller;

HelloRouter::HelloRouter(Usecases::HelloCodogramRecievedUsecaseInterface& helloUsecase)
    : helloUsecase(helloUsecase) {
}

Async::Task<void> HelloRouter::route(TCP::Stream s) {
    auto data = co_await s.read(2);
    switch(static_cast<int>(data[0])) {
        case 1: {
            auto name = co_await s.read((static_cast<int>(data[1])));
            Entities::HelloCodogram hello = {
                .id = 1,
                .name = std::string{name.data()}
            };
            helloUsecase.hello(hello);
            break;
        }
    }
    co_return;
}
