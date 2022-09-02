#pragma once

#include <sqlite3.h>
#include "../IDbConnection.h"

class SqliteDbConnection : public IDbConnection
{
private:
    sqlite3* database_;
    int transaction_count_;
public:
    SqliteDbConnection(sqlite3* database);
    SqliteDbConnection() = delete;

    static std::shared_ptr<IDbConnection> SqliteCreateNewDatabase(const char* filename);
    static std::shared_ptr<IDbConnection> SqliteOpenExistingDatabase(const char* filename, bool readonly);

    virtual void Execute(const char* sql_statement) override;
    virtual void Execute(IDbStatement* statement) override;
    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) override;
    virtual std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) override;
    virtual bool StepStatement(IDbStatement* statement) override;

    virtual void BeginTransaction() override;
    virtual void EndTransaction(bool commit) override;
    virtual bool IsTransactionPending() const override;

    virtual std::vector<IDbConnection::ColumnInfo> GetTableInfo(const char* table_name) override;

    virtual ~SqliteDbConnection() override;
};