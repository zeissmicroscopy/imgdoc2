#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../libimgdoc2_v2/src/db/DbFactory.h"
#include "../libimgdoc2_v2/src/db/database_creator.h"
#include "../libimgdoc2_v2/src/db/database_discovery.h"

using namespace imgdoc2;
using namespace std;
using namespace testing;

TEST(DbDiscoveryTest, CreateAndDiscover2D)
{
    // Note: the syntax below allows to have multiple connections to the "in-memory-data database",
//        cf. https://sqlite.org/inmemorydb.html
    auto dbConnection = DbFactory::SqliteCreateNewDatabase("file:memdb1?mode=memory&cache=shared");
    shared_ptr< DatabaseConfiguration2D> database_configuration_from_creation;

    {
        DbCreator db_creator(dbConnection);
        auto create_options = ClassFactory::CreateCreateOptionsUp();
        create_options->AddDimension('a');
        create_options->AddDimension('b');
        create_options->AddDimension('c');
        create_options->AddIndexForDimension('a');
        create_options->AddIndexForDimension('c');
        auto configuration = db_creator.CreateTables(create_options.get());
        database_configuration_from_creation = dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    }

    shared_ptr< DatabaseConfiguration2D> database_configuration_from_discovery;
    {
        DbDiscovery db_discovery(dbConnection);
        db_discovery.DoDiscovery();
        database_configuration_from_discovery = dynamic_pointer_cast<DatabaseConfiguration2D>(db_discovery.GetDatabaseConfiguration());
    }

    dbConnection.reset();

    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesInfoOrThrow() == database_configuration_from_discovery->GetTableNameForTilesInfoOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTableNameForTilesDataOrThrow() == database_configuration_from_discovery->GetTableNameForTilesDataOrThrow());
    EXPECT_TRUE(database_configuration_from_creation->GetTileDimensions().size() == database_configuration_from_discovery->GetTileDimensions().size());
    EXPECT_THAT(database_configuration_from_creation->GetTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetTileDimensions()));
    EXPECT_THAT(database_configuration_from_creation->GetIndexedTileDimensions(), UnorderedElementsAreArray(database_configuration_from_discovery->GetIndexedTileDimensions()));
}