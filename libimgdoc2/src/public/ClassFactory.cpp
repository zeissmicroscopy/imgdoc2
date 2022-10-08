#include "ClassFactory.h"
#include "../doc/document.h"
#include "../src/db/DbFactory.h"
#include "../src/db/database_creator.h"
#include "../src/db/database_discovery.h"

using namespace std;
using namespace imgdoc2;

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::CreateNew(imgdoc2::ICreateOptions* create_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(create_options->GetFilename().c_str(), environment);

    // check pre-conditions
    // TODO(JBL)

    // tweak settings

    DbCreator db_creator(db_connection);
    const auto configuration = db_creator.CreateTables(create_options);

    const auto database_configuration_2d = std::dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    // TODO(JBL): 3D version should follow here

    return {};
}

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(open_existing_options->GetFilename().c_str(), environment);

    DbDiscovery database_discovery{ db_connection };
    database_discovery.DoDiscovery();

    const auto database_configuration_2d = std::dynamic_pointer_cast<DatabaseConfiguration2D>(database_discovery.GetDatabaseConfiguration());
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    // TODO(JBL): 3d version should follow here

    return {};
}