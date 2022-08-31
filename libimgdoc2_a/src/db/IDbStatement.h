#pragma once

#include <cstdint>

class IDbStatement
{
public:
    virtual void Reset() = 0;

    virtual void BindNull(int index) = 0;
    virtual void BindInt32(int index, std::int32_t value) = 0;
    virtual void BindInt64(int index, std::int64_t value) = 0;
    virtual void BindDouble(int index, double value) = 0;

    /// Bind a "static" BLOB. Static means that the pointer MUST remain valid valid until either the prepared statement is 
    /// finalized or the same SQL parameter is bound to something else. 
    ///
    /// \param  index   Index of the parameter to bind.
    /// \param  data    The pointer to the data.
    /// \param  size    The size of the data (in bytes).
    virtual void BindBlob_Static(int index, const void* data, size_t size)=0;

    virtual int GetResultInt32(int column) = 0;
    virtual std::int64_t GetResultInt64(int column) = 0;
    virtual double GetResultDouble(int column) = 0;

    virtual ~IDbStatement() = default;

public:
    // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
    IDbStatement() = default;
    IDbStatement(const IDbStatement&) = delete;             // copy constructor
    IDbStatement& operator=(const IDbStatement&) = delete;  // copy assignment
    IDbStatement(IDbStatement&&) = delete;                  // move constructor
    IDbStatement& operator=(IDbStatement&&) = delete;       // move assignment
};