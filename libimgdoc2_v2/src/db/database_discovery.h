#pragma once

#include <memory>
#include "IDbConnection.h"
#include "database_configuration.h"

class DbDiscovery
{
private:
    std::shared_ptr<IDbConnection> db_connection_;

    std::shared_ptr<DatabaseConfigurationCommon> configuration_;
private:
    struct GeneralDataDiscoveryResult
    {
        imgdoc2::DocumentType document_type;
        std::string tilesdatatable_name;
        std::string tileinfotable_name;
    };

public:
    DbDiscovery(std::shared_ptr<IDbConnection> dbConnection) :
        db_connection_(std::move(dbConnection))
    {}

    void DoDiscovery();

    std::shared_ptr<DatabaseConfigurationCommon> GetDatabaseConfiguration();

private:
    GeneralDataDiscoveryResult DiscoverGeneralTable();
};
