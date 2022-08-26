#include "sqlite_DbConnection.h"
#include "sqlite_DbStatement.h"
#include "custom_functions.h"

using namespace std;

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

/*virtual*/std::int64_t SqliteDbConnection::ExecuteAndGetLastRowId(IDbStatement* statement)
{
    auto sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement == nullptr)
    {
        throw runtime_error("incorrect type");
    }

    // https://www.sqlite.org/c3ref/last_insert_rowid.html
    int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());

    std::int64_t last_row_id = sqlite3_last_insert_rowid(this->database_);
    return last_row_id;
}

/*virtual*/std::shared_ptr<IDbStatement> SqliteDbConnection::PrepareStatement(const std::string& sql_statement)
{
    sqlite3_stmt* statement;
    int return_value = sqlite3_prepare_v2(
        this->database_,
        sql_statement.c_str(),
        -1,
        &statement,
        nullptr);
    // TODO: error handling

    return make_shared<SqliteDbStatement>(statement);
}

/*virtual*/bool SqliteDbConnection::StepStatement(IDbStatement* statement)
{
    // try to cast "statement" to ISqliteStatement
    auto sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement==nullptr)
    {
        throw runtime_error("incorrect type");
    }

    int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());

    // https://www.sqlite.org/c3ref/step.html
    if (return_value == SQLITE_ROW)
    {
        return true;
    }
    else if (return_value == SQLITE_DONE)
    {
        return false;
    }

    throw runtime_error("");
}