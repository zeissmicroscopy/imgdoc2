#include <exceptions.h>
#include <sqlite3.h>

using namespace std;
using namespace imgdoc2;

std::string imgdoc2::database_exception::GetSqliteErrorMessage() const
{
    if (this->GetIsSqliteErrorCodeValid())
    {
        return string(sqlite3_errstr(this->GetSqliteErrorCode()));
    }

    return string("No Sqlite-Errorcode available.");
}