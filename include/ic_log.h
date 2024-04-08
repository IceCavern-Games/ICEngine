#pragma once

#include <memory>

#include <spdlog/spdlog.h>

namespace IC
{
    class Log
    {
    private:
        static std::shared_ptr<spdlog::logger> s_AppLogger;
        static std::shared_ptr<spdlog::logger> s_CoreLogger;

    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> &GetAppLogger() { return s_AppLogger; }
        inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
    };
}

// App Log Macros
#define IC_APP_TRACE(...) ::IC::Log::GetAppLogger()->trace(__VA_ARGS__)
#define IC_APP_INFO(...) ::IC::Log::GetAppLogger()->info(__VA_ARGS__)
#define IC_APP_WARN(...) ::IC::Log::GetAppLogger()->warn(__VA_ARGS__)
#define IC_APP_ERROR(...) ::IC::Log::GetAppLogger()->error(__VA_ARGS__)
#define IC_APP_FATAL(...) ::IC::Log::GetAppLogger()->fatal(__VA_ARGS__)

// Core Log Macros
#define IC_CORE_TRACE(...) ::IC::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define IC_CORE_INFO(...) ::IC::Log::GetCoreLogger()->info(__VA_ARGS__)
#define IC_CORE_WARN(...) ::IC::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define IC_CORE_ERROR(...) ::IC::Log::GetCoreLogger()->error(__VA_ARGS__)
#define IC_CORE_FATAL(...) ::IC::Log::GetCoreLogger()->fatal(__VA_ARGS__)
