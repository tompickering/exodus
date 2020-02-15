#ifndef GUARD_LOGGER_H
#define GUARD_LOGGER_H

#include <cstdarg>

namespace Log {

    enum Level {DEBUG, INFO, WARN, ERROR};

    class Logger {
        public:
            Logger();
            Logger(Log::Level);
            ~Logger();
            void log(Log::Level, const char* fnt, ...);
            void debug(const char* fmt, ...);
            void info(const char* fmt, ...);
            void warn(const char* fmt, ...);
            void error(const char* fmt, ...);
            void fatal(const char* fmt, ...);
            void set_level(Level);
        private:
            void vlog(Log::Level, const char*, va_list);
            Level level;
    };

}

#endif
