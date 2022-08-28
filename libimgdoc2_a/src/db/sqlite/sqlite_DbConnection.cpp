#include "sqlite_DbConnection.h"
#include "sqlite_DbStatement.h"
#include "custom_functions.h"
#include <exceptions.h>

using namespace std;
using namespace imgdoc2;

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

/*virtual*/void SqliteDbConnection::Execute(const char* sql_statement)
{
    int return_value = sqlite3_exec(this->database_, sql_statement, nullptr, nullptr, nullptr);
}

/*virtual*/void SqliteDbConnection::Execute(IDbStatement* statement)
{
    if (statement == nullptr)
    {
        throw invalid_argument("The argument 'statement' must not be null.");
    }

    auto sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement == nullptr)
    {
        throw imgdoc2_exception("Incorrect type encountered - object does not implement 'ISqliteDbStatement'-interface.");
    }

    const int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());

    // see https://www.sqlite.org/c3ref/step.html
    // Note that we intend that Execute-methods are used only for commands which do not return data,
    //  so this means that we do not expect 'SQLITE_ROW" here
    if (return_value != SQLITE_DONE)
    {
        throw database_exception("Error from 'sqlite3_step'", return_value);
    }
}

/*virtual*/std::int64_t SqliteDbConnection::ExecuteAndGetLastRowId(IDbStatement* statement)
{
    this->Execute(statement);

    // https://www.sqlite.org/c3ref/last_insert_rowid.html
    std::int64_t last_row_id = sqlite3_last_insert_rowid(this->database_);
    return last_row_id;
}

/*virtual*/std::shared_ptr<IDbStatement> SqliteDbConnection::PrepareStatement(const std::string& sql_statement)
{
    sqlite3_stmt* statement = nullptr;

    // https://www.sqlite.org/c3ref/prepare.html
    const int return_value = sqlite3_prepare_v2(
        this->database_,
        sql_statement.c_str(),
        -1,
        &statement,
        nullptr);
    if (return_value != SQLITE_OK || statement == nullptr)
    {
        throw database_exception("Error from 'sqlite3_prepare_v2'", return_value);
    }

    return make_shared<SqliteDbStatement>(statement);
}

/*virtual*/bool SqliteDbConnection::StepStatement(IDbStatement* statement)
{
    // try to cast "statement" to ISqliteStatement
    const auto sqlite_statement = dynamic_cast<ISqliteDbStatement*>(statement);
    if (sqlite_statement == nullptr)
    {
        throw runtime_error("incorrect type");
    }

    const int return_value = sqlite3_step(sqlite_statement->GetSqliteSqlStatement());

    // https://www.sqlite.org/c3ref/step.html
    if (return_value == SQLITE_ROW)
    {
        return true;
    }
    else if (return_value == SQLITE_DONE)
    {
        return false;
    }

    throw database_exception("Error from 'sqlite3_step'.", return_value);
}