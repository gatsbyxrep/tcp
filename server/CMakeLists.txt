cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(server main.cpp
    config/config.h config/config.cpp
    external/tcpserver/tcpserver.h external/tcpserver/tcpserver.cpp
    external/async/scheduler.hpp external/async/scheduler.cpp
    external/datastructures/mpmcqueue.hpp
    external/async/executor.hpp external/async/executor.cpp
    external/async/task.hpp external/async/task.cpp
    external/async/promise.hpp
    external/async/future.hpp
    external/async/async.h
    external/async/sync/mutex.hpp external/async/sync/mutex.cpp
    external/tcpserver/router.h external/tcpserver/router.cpp
    internal/controller/tcp/hellorouter.h internal/controller/tcp/hellorouter.cpp
    internal/entities/hellocodogram.h internal/entities/hellocodogram.cpp
    internal/usecases/hellocodogramrecievedusecase.h internal/usecases/hellocodogramrecievedusecase.cpp
    external/logger/logger.h external/logger/logger.cpp
)

include(GNUInstallDirs)
install(TARGETS server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic -Werror)
