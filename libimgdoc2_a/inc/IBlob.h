#pragma once

namespace imgdoc2
{
    class SQLITEIMGDOC_API IBlobOutput
    {
    public:
        virtual bool Reserve(size_t s) = 0;
        virtual bool GetPointer(size_t s, size_t offset, void*& ptr) = 0;

        virtual ~IBlob() = default;
    };
}