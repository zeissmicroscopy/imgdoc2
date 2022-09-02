#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include "IDbStatement.h"

class IDbConnection
{
public:
    struct ColumnInfo
    {
        std::string column_name;
        std::string column_type;
    };
public:
    virtual void Execute(const char* sql_statement) = 0;
    virtual void Execute(IDbStatement* statement) = 0;
    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) = 0;

    virtual std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) = 0;

    virtual bool StepStatement(IDbStatement* statement) = 0;

    virtual void BeginTransaction() = 0;
    virtual void EndTransaction(bool commit) = 0;
    virtual bool IsTransactionPending() const = 0;

    virtual std::vector<ColumnInfo> GetTableInfo(const char* table_name) = 0;

    virtual ~IDbConnection() = default;

public:
    // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
    IDbConnection() = default;
    IDbConnection(const IDbConnection&) = delete;             // copy constructor
    IDbConnection& operator=(const IDbConnection&) = delete;  // copy assignment
    IDbConnection(IDbConnection&&) = delete;                  // move constructor
    IDbConnection& operator=(IDbConnection&&) = delete;       // move assignment

public:
    void Execute(const std::string& sql_statement)
    {
        this->Execute(sql_statement.c_str());
    }
};
