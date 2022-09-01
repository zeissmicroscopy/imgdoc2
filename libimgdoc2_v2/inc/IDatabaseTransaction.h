#pragma once

namespace imgdoc2
{
    class IDatabaseTransaction
    {
    public:
        virtual void BeginTransaction() = 0;
        virtual void CommitTransaction() = 0;
        virtual void RollbackTransaction() = 0;
        virtual ~IDatabaseTransaction() = default;
    };
}