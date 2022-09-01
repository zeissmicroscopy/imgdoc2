#pragma once

#include <sqlite3.h>
#include "../IDbConnection.h"

class SqliteDbConnection : public IDbConnection
{
private:
    sqlite3* database_;
    int transaction_count_;
public:
    SqliteDbConnection() = delete;
    SqliteDbConnection(const char* dbFilename);

    virtual void Execute(const char* sql_statement) override;
    virtual void Execute(IDbStatement* statement) override;
    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) ;
    virtual std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) override;
    virtual bool StepStatement(IDbStatement* statement) override;

    virtual void BeginTransaction() override;
    virtual void EndTransaction(bool commit) override;
    virtual bool IsTransactionPending() const override;

    virtual ~SqliteDbConnection() override;
};