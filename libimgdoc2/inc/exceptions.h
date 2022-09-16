#pragma once

#include <stdexcept>
#include <string>

namespace imgdoc2
{
    /// Base class for all imgdoc2-specific exceptions.
    class imgdoc2_exception : public std::runtime_error
    {
    public:
        /// Constructor.
        /// \param szErrMsg Message describing the error.
        explicit imgdoc2_exception(const char* szErrMsg)
            : std::runtime_error(szErrMsg)
        {}
    };

    class database_exception : public std::runtime_error
    {
    private:
        int sqlite_errorcode_;
        bool sqlite_errorcode_valid_;
    public:
        /// Constructor.
        /// \param szErrMsg Message describing the error.
        explicit database_exception(const char* szErrMsg)  
            : std::runtime_error(szErrMsg), sqlite_errorcode_(-1), sqlite_errorcode_valid_(false)
        {}

        /// Constructor.
       /// \param szErrMsg Message describing the error.
        explicit database_exception(const char* szErrMsg, int sqlite_error_code)
            : std::runtime_error(szErrMsg), sqlite_errorcode_(sqlite_error_code), sqlite_errorcode_valid_(true)
        {}

        bool GetIsSqliteErrorCodeValid() const { return this->sqlite_errorcode_valid_; }

        int GetSqliteErrorCode() const { return this->sqlite_errorcode_; }

        std::string GetSqliteErrorMessage() const;
    };

    class invalid_operation_exception : public std::runtime_error
    {
        public:
        /// Constructor.
        /// \param szErrMsg Message describing the error.
        explicit invalid_operation_exception(const char* szErrMsg)
            : std::runtime_error(szErrMsg)
        {}
    };

    /// Exception for signalling logical errors during database-discovery.
    class discovery_exception : public std::runtime_error
    {
    public:
        /// Constructor.
        /// \param szErrMsg Message describing the error.
        explicit discovery_exception(const char* szErrMsg)
            : std::runtime_error(szErrMsg)
        {}
    };
}
