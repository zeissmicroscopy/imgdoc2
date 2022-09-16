#pragma once

#include <memory>
#include "IDbConnection.h"

class DbFactory
{
public:
    static std::shared_ptr<IDbConnection> SqliteCreateNewDatabase(const char* filename);
    static std::shared_ptr<IDbConnection> SqliteOpenExistingDatabase(const char* filename, bool readonly);
};
