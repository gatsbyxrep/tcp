#include "logger.h"

#include <iomanip>
#include <ctime>
#include <iostream>

using namespace Logger;

InFileLogger::InFileLogger(std::filesystem::path path) {
    file = std::ofstream(path.string(), std::ios::out | std::ios::in | std::ios::app);
    if(!file.is_open()) {
        throw std::runtime_error("Cant open file for logging");
    }
}

InFileLogger::~InFileLogger() {
    file.close();
}

void InFileLogger::log(const std::string& str) {
    const size_t dateSymbols = 23; // Ex: 2018-09-19 13:50:04.000
    if(str.size() + dateSymbols >= 256) {
        throw std::runtime_error("Too long str to log");
    }
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    std::tm* currentTimeStruct = std::localtime(&currentTime_t);
    auto timeStr = std::put_time(currentTimeStruct, "%Y-%m-%d %H:%M:%S");
    std::lock_guard guard {mutex};
    file << '[' << timeStr << "] " << str << '\n';
    file.flush();
    std::cout << '[' << timeStr << "] " << str << '\n';
}
