#pragma once

namespace imgdoc2
{
    class IDataObjBase
    {
    public:
        virtual void GetData(const void** p, size_t* s) const = 0;
        //virtual void GetHeader(const void** p, size_t* s) const = 0;

        virtual ~IDataObjBase() = default;
    };
}
