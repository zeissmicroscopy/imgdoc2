#pragma once

#include <sqlite3.h>
#include "../IDbConnection.h"

class SqliteDbConnection : public IDbConnection
{
private:
    sqlite3* database_;
public:
    SqliteDbConnection(const char* dbFilename);

    virtual void Exec(const char* sql_statement);
    virtual ~SqliteDbConnection();
};