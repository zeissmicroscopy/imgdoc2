#pragma once

#include <sqlite3.h>
#include <IEnvironment.h>
#include "../IDbConnection.h"

/// Implementation of the IDbConnection-interface specific to SQLite.
class SqliteDbConnection : public IDbConnection
{
private:
    std::shared_ptr<imgdoc2::IHostingEnvironment> environment_;
    sqlite3* database_;
    int transaction_count_;
public:
    SqliteDbConnection(sqlite3* database, std::shared_ptr<imgdoc2::IHostingEnvironment> environment = nullptr);
    SqliteDbConnection() = delete;

    static std::shared_ptr<IDbConnection> SqliteCreateNewDatabase(const char* filename, std::shared_ptr<imgdoc2::IHostingEnvironment> environment);
    static std::shared_ptr<IDbConnection> SqliteOpenExistingDatabase(const char* filename, bool readonly, std::shared_ptr<imgdoc2::IHostingEnvironment> environment);

    virtual void Execute(const char* sql_statement) override;
    virtual void Execute(IDbStatement* statement) override;
    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) override;
    virtual std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) override;
    virtual bool StepStatement(IDbStatement* statement) override;

    virtual void BeginTransaction() override;
    virtual void EndTransaction(bool commit) override;
    virtual bool IsTransactionPending() const override;

    virtual std::vector<IDbConnection::ColumnInfo> GetTableInfo(const char* table_name) override;
    virtual std::vector<IDbConnection::IndexInfo> GetIndicesOfTable(const char* table_name) override;

    [[nodiscard]] virtual const std::shared_ptr<imgdoc2::IHostingEnvironment>& GetHostingEnvironment() const override;

    virtual ~SqliteDbConnection() override;
};