#pragma once

#include <string>

namespace imgdoc2
{
    class IOpenExistingOptions
    {
    public:
        virtual void SetFilename(const char* sz) = 0;

        virtual void SetOpenReadonly(bool read_only) = 0;

        virtual bool GetOpenReadonly() const = 0;

        virtual const std::string& GetFilename() const = 0;

        virtual ~IOpenExistingOptions() = default;
    };
}