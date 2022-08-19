#include "sqlite_DbConnection.h"
#include "custom_functions.h"

SqliteDbConnection::SqliteDbConnection(const char* dbFilename)
{
    sqlite3* database;
    int returnValue = sqlite3_open_v2(
        dbFilename,
        &database,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        nullptr);

    // TODO - error handling
    //  if (returnValue...)

    SqliteCustomFunctions::SetupCustomQueries(database);

    this->database_ = database;
}

/*virtual*/SqliteDbConnection::~SqliteDbConnection()
{
    // Note: calling "sqlite3_close_v2" with nullptr is harmless
    sqlite3_close_v2(this->database_);
}

/*virtual*/void SqliteDbConnection::Exec(const char* sql_statement)
{
    int return_value = sqlite3_exec(this->database_, sql_statement, nullptr, nullptr, nullptr);
}