#include "sqlite_DbStatement.h"

using namespace std;

SqliteDbStatement::SqliteDbStatement(sqlite3_stmt* sql_statement)  :
    sql_statement_(sql_statement)
{
}

/*virtual*/SqliteDbStatement::~SqliteDbStatement()
{
    // TODO: check error (-> https://www.sqlite.org/c3ref/finalize.html)
    sqlite3_finalize(this->sql_statement_);
}

/*virtual*/void SqliteDbStatement::Reset()
{
}

/*virtual*/void SqliteDbStatement::BindNull(int index)
{
    // TODO: error handling (https://www.sqlite.org/c3ref/bind_blob.html)
    int return_code = sqlite3_bind_null(this->sql_statement_, index);
}

/*virtual*/void SqliteDbStatement::BindInt32(int index, std::int32_t value)
{
    // TODO: error handling (https://www.sqlite.org/c3ref/bind_blob.html)
    int return_code = sqlite3_bind_int(this->sql_statement_, index, value);
}

/*virtual*/void SqliteDbStatement::BindInt64(int index, std::int64_t value)
{
    // TODO: error handling (https://www.sqlite.org/c3ref/bind_blob.html)
    int return_code = sqlite3_bind_int64(this->sql_statement_, index, value);
}

/*virtual*/void SqliteDbStatement::BindDouble(int index, double value)
{
    int return_code = sqlite3_bind_double(this->sql_statement_, index, value);
}

/*virtual*/void SqliteDbStatement::BindBlob_Static(int index, const void* data, size_t size)
{
    int return_code = sqlite3_bind_blob64(this->sql_statement_, index, data, size, nullptr);
}

/*virtual*/sqlite3_stmt* SqliteDbStatement::GetSqliteSqlStatement() 
{
    return this->sql_statement_;
}

/*virtual*/int SqliteDbStatement::GetResultInt32(int column)
{
    int value = sqlite3_column_int(this->sql_statement_, column);
    return value;
}

/*virtual*/std::int64_t SqliteDbStatement::GetResultInt64(int column)
{
    int64_t value = sqlite3_column_int64(this->sql_statement_, column);
    return value;
}

/*virtual*/double SqliteDbStatement::GetResultDouble(int column)
{
    double value = sqlite3_column_double(this->sql_statement_, column);
    return value;
}