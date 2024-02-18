cmake_minimum_required(VERSION 3.5)

project(TCPClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TCPClient main.cpp
    external/serverapi/serverapi.h external/serverapi/serverapi.cpp
    config/config.h config/config.cpp
    internal/entities/hellocodogram.h internal/entities/hellocodogram.cpp
    internal/usecases/sendhellousecase.h internal/usecases/sendhellousecase.cpp)

include(GNUInstallDirs)
install(TARGETS TCPClient
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
target_compile_options(TCPClient PRIVATE -Wall -Wextra -Wpedantic -Werror)
