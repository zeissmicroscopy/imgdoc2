#include "../DbFactory.h"
#include "sqlite_DbConnection.h"

using namespace std;

/*static*/std::shared_ptr<IDbConnection> DbFactory::SqliteCreateNewDatabase(const char* filename)
{
    return make_shared<SqliteDbConnection>(filename);
}