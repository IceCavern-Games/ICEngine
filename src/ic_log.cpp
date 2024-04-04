#include <ic_log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace IC
{
    std::shared_ptr<spdlog::logger> Log::s_AppLogger;
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: $v%$");

        s_CoreLogger = spdlog::stdout_color_mt("IC");
        s_CoreLogger->set_level(spdlog::level::trace);

        s_AppLogger = spdlog::stdout_color_mt("ICApp");
        s_AppLogger->set_level(spdlog::level::trace);
    }
}
