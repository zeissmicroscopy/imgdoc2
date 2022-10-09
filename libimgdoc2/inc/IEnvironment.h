#pragma once

namespace imgdoc2
{
    constexpr int LogLevel_ERROR = 1;
    constexpr int LogLevel_WARN = 2;
    constexpr int LogLevel_INFO = 3;
    constexpr int LogLevel_TRACE = 4;
    constexpr int LogLevel_DEBUG = 15;

    /// In this class functionality "to be provided by a hosting environment" ist gathered.
    /// The prime example is "debug logging", which is a cross cutting concern and it is allowed
    /// to route debug print to the hosting environment.
    class IHostingEnvironment
    {
    public:

        /// This method is used for debug output. It may be called concurrently.
        ///
        /// \param  level   The log level (c.f. constants "LogLeve..." for possible values).
        /// \param  message The message text (in UTF8-encoding).
        virtual void Log(int level, const char* message) = 0;

        virtual bool IsLogLevelActive(int level) = 0;
        virtual void ReportFatalErrorAndExit(const char* message) = 0;

        virtual ~IHostingEnvironment() = default;
    public:
        // no copy and no move
        IHostingEnvironment() = default;
        IHostingEnvironment(const IHostingEnvironment&) = delete;             // copy constructor
        IHostingEnvironment& operator=(const IHostingEnvironment&) = delete;  // copy assignment
        IHostingEnvironment(IHostingEnvironment&&) = delete;                  // move constructor
        IHostingEnvironment& operator=(IHostingEnvironment&&) = delete;       // move assignment
    };
}
