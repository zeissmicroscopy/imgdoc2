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
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDatabaseTransaction() = default;
        IDatabaseTransaction(const IDatabaseTransaction&) = delete;             // copy constructor
        IDatabaseTransaction& operator=(const IDatabaseTransaction&) = delete;  // copy assignment
        IDatabaseTransaction(IDatabaseTransaction&&) = delete;                  // move constructor
        IDatabaseTransaction& operator=(IDatabaseTransaction&&) = delete;       // move assignment
    };
}