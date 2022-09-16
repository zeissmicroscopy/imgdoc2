#include "../DbFactory.h"
#include "sqlite_DbConnection.h"

using namespace std;

/*static*/std::shared_ptr<IDbConnection> DbFactory::SqliteCreateNewDatabase(const char* filename)
{
    return SqliteDbConnection::SqliteCreateNewDatabase(filename);
}

/*static*/std::shared_ptr<IDbConnection> DbFactory::SqliteOpenExistingDatabase(const char* filename, bool readonly)
{
    return SqliteDbConnection::SqliteOpenExistingDatabase(filename, readonly);
}
