#pragma once

namespace imgdoc2
{
    class IBlobOutput
    {
    public:
        virtual bool Reserve(size_t s) = 0;
        virtual bool SetData(size_t offset, size_t size, const void* data)=0;
        //virtual bool GetPointer(size_t s, size_t offset, void*& ptr) = 0;

        virtual ~IBlobOutput() = default;
    };
}