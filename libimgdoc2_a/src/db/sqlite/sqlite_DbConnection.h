#pragma once

#include <sqlite3.h>
#include "../IDbConnection.h"

class SqliteDbConnection : public IDbConnection
{
private:
    sqlite3* database_;
public:
    SqliteDbConnection(const char* dbFilename);

    virtual void Exec(const char* sql_statement) override;
    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) ;
    virtual std::shared_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) override;
    virtual bool StepStatement(IDbStatement* statement) override;

    virtual ~SqliteDbConnection() override;
};