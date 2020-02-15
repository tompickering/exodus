#include "logger.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

namespace Log {

    Logger::Logger() {
        level = Level::INFO;
    }

    Logger::Logger(Level lev) {
        level = lev;
    }

    Logger::~Logger() {
    }

    void Logger::vlog(Level lev, const char* fmt, va_list args) {
        vprintf(fmt, args);
        printf("\n");
    }

    void Logger::log(Level lev, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vlog(lev, fmt, args);
        va_end(args);
    }

    void Logger::debug(const char* fmt, ...) {
        if (level > DEBUG)
            return;
        va_list args;
        va_start(args, fmt);
        vlog(Level::DEBUG, fmt, args);
        va_end(args);
    }

    void Logger::info(const char* fmt, ...) {
        if (level > INFO)
            return;
        va_list args;
        va_start(args, fmt);
        vlog(Level::INFO, fmt, args);
        va_end(args);
    }

    void Logger::warn(const char* fmt, ...) {
        if (level > WARN)
            return;
        va_list args;
        va_start(args, fmt);
        vlog(Level::WARN, fmt, args);
        va_end(args);
    }

    void Logger::error(const char* fmt, ...) {
        if (level > ERROR)
            return;
        va_list args;
        va_start(args, fmt);
        vlog(Level::ERROR, fmt, args);
        va_end(args);
    }

    void Logger::fatal(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vlog(Level::ERROR, fmt, args);
        va_end(args);
        exit(1);
    }

    void Logger::set_level(Level new_level) {
        level = new_level;
    }

}
