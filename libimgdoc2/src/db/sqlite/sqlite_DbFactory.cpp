#include <IEnvironment.h>
#include "../DbFactory.h"
#include "sqlite_DbConnection.h"

using namespace std;

/*static*/std::shared_ptr<IDbConnection> DbFactory::SqliteCreateNewDatabase(const char* filename, std::shared_ptr<imgdoc2::IHostingEnvironment> environment)
{
    return SqliteDbConnection::SqliteCreateNewDatabase(filename, environment);
}

/*static*/std::shared_ptr<IDbConnection> DbFactory::SqliteOpenExistingDatabase(const char* filename, bool readonly, std::shared_ptr<imgdoc2::IHostingEnvironment> environment)
{
    return SqliteDbConnection::SqliteOpenExistingDatabase(filename, readonly, environment);
}
