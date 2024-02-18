#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <filesystem>
#include <fstream>
#include <mutex>

namespace Logger {
    class Logger {
    public:
        virtual ~Logger() = default;
        virtual void log(const std::string& str) = 0; // String view?
    };
    class InFileLogger : public Logger {
    public:
        InFileLogger(std::filesystem::path path);
        ~InFileLogger();
        void log(const std::string& str);
    private:
        std::ofstream file; // Guarded by mutex;
        std::mutex mutex;
    };


}

#endif // LOGGER_H
