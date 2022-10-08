#pragma once

namespace imgdoc2
{
    constexpr int LogLevel_ERROR = 1;
    constexpr int LogLevel_WARN = 2;
    constexpr int LogLevel_INFO = 3;
    constexpr int LogLevel_TRACE = 4;
    constexpr int LogLevel_DEBUG = 15;

    class IHostingEnvironment
    {
    public:
        virtual void Log(int level, const char* message) = 0;
        virtual bool IsLogLevelActive(int level) = 0;
        virtual void ReportFatalErrorAndExit(const char* message) = 0;

        virtual ~IHostingEnvironment() = default;
    };
}
