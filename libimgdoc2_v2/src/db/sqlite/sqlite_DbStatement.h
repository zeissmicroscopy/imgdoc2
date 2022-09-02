#pragma once

#include <sqlite3.h>
#include "../IDbConnection.h"
#include "ISqlite_DbStatement.h"

class SqliteDbStatement : public IDbStatement, public ISqliteDbStatement
{
private:
    sqlite3_stmt* sql_statement_;
public:
    SqliteDbStatement(sqlite3_stmt* sql_statement);
    virtual ~SqliteDbStatement() override;

    virtual void Reset() override;
    virtual void BindNull(int index) override;
    virtual void BindInt32(int index, std::int32_t value) override;
    virtual void BindInt64(int index, std::int64_t value) override;
    virtual void BindDouble(int index, double value) override;
    virtual void BindBlob_Static(int index, const void* data, size_t size) override;

    virtual sqlite3_stmt* GetSqliteSqlStatement() override;

    virtual int GetResultInt32(int column) override;
    virtual std::int64_t GetResultInt64(int column) override;
    virtual double GetResultDouble(int column) override;
    virtual std::string GetResultString(int column) override;
};