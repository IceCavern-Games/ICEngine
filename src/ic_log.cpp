#include <ic_log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace IC
{
    std::shared_ptr<spdlog::logger> Log::_appLogger;
    std::shared_ptr<spdlog::logger> Log::_coreLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        _coreLogger = spdlog::stdout_color_mt("IC");
        _coreLogger->set_level(spdlog::level::trace);

        _appLogger = spdlog::stdout_color_mt("ICApp");
        _appLogger->set_level(spdlog::level::trace);
    }
}
