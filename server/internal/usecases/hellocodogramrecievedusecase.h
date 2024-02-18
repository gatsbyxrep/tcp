#ifndef HELLOCODOGRAMRECIEVEDUSECASE_H
#define HELLOCODOGRAMRECIEVEDUSECASE_H

#include "../entities/hellocodogram.h"
#include "../../external/logger/logger.h"

namespace Usecases {
    class HelloCodogramRecievedUsecaseInterface {
    public:
        virtual ~HelloCodogramRecievedUsecaseInterface() = default;
        virtual void hello(Entities::HelloCodogram hello) = 0;
    };

    class HelloCodogramRecievedUsecase : public HelloCodogramRecievedUsecaseInterface {
    public:
        HelloCodogramRecievedUsecase(Logger::Logger& logger);
        void hello(Entities::HelloCodogram hello);
    private:
        Logger::Logger& logger;
    };
}

#endif // HELLOCODOGRAMRECIEVEDUSECASE_H
