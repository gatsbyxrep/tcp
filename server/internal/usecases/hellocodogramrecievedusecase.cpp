#include "hellocodogramrecievedusecase.h"

#include "../entities/hellocodogram.h"

using namespace Usecases;

HelloCodogramRecievedUsecase::HelloCodogramRecievedUsecase(Logger::Logger& logger)
    : logger(logger) {}

void HelloCodogramRecievedUsecase::hello(Entities::HelloCodogram hello) {
    logger.log(hello.name);
}
