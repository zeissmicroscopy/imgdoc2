#pragma once

#include <memory>
#include "IDbConnection.h"
#include "database_configuration.h"

class DbCreator
{
private:
    std::shared_ptr<IDbConnection> db_connection_;
public:
    DbCreator(std::shared_ptr<IDbConnection> dbConnection) :
        db_connection_(dbConnection)
    {}

    void CreateTables(const imgdoc2::ICreateOptions* create_options);

private:
    void Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options);

    std::string GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
    std::string GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration);
};
