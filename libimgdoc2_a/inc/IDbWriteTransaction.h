#pragma once

namespace imgdoc2
{
    class IDbWriteTransaction
    {
    public:
        virtual void BeginTransaction() = 0;
        virtual void CommitTransaction() = 0;
        virtual void RollbackTransaction() = 0;
        virtual ~IDbWriteTransaction() = default;
    };

}