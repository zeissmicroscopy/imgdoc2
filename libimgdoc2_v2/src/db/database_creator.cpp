#include <sstream>
#include <gsl/gsl>
#include <imgdoc2.h>
#include "database_creator.h"
#include "database_configuration.h"
#include "database_constants.h"
#include "DbFactory.h"
#include "database_discovery.h"
#include "../doc/document.h"
#include "utilities.h"

using namespace std;

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::CreateNew(imgdoc2::ICreateOptions* create_options)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(create_options->GetFilename().c_str());

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

std::shared_ptr< DatabaseConfigurationCommon> DbCreator::CreateTables(const imgdoc2::ICreateOptions* create_options)
{
    // construct the "database configuration" based on the the create_options
    auto database_configuration = make_shared<DatabaseConfiguration2D>();
    this->Initialize2dConfigurationFromCreateOptions(database_configuration.get(), create_options);

    // TODO(JBL): make those operations a transaction
    auto sql_statement = this->GenerateSqlStatementForCreatingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForFillingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesDataTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(database_configuration.get());
    this->db_connection_->Execute(sql_statement);

    if (create_options->GetUseSpatialIndex())
    {
        sql_statement = this->GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);
        
        // and, add its name to the "General" table
        this->SetGeneralTableInfoForSpatialIndex(database_configuration.get());
    }

    if (create_options->GetCreateBlobTable())
    {
        sql_statement = GenerateSqlStatementForCreatingBlobTable_Sqlite(database_configuration.get());
        this->db_connection_->Execute(sql_statement);
        this->SetBlobTableNameInGeneralTable(database_configuration.get());
    }

    return database_configuration;
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto string_stream = stringstream();
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesDataOrThrow() << "]("
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] INTEGER PRIMARY KEY," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "] INTEGER(4) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "] INTEGER(1) NOT NULL," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType) << "] INTEGER(1)," <<
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "] INTEGER(8)";
    string_stream << ");";

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto string_stream = stringstream();
    // Notes:
    // * "INTEGER PRIMARY KEY" makes the column-name an alias for the RowId-column
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "]("
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "] INTEGER PRIMARY KEY,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "] INTEGER(1) NOT NULL,";

    string_stream << "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] INTEGER(8) NOT NULL";

    for (const auto dim : database_configuration->GetTileDimensions())
    {
        string colName = database_configuration->GetDimensionsColumnPrefix() + dim;
        string_stream << ", [" << colName << "] INTEGER(4) NOT NULL";
    }

    string_stream << ");";

    // create the indices for the "dimension tables"
    for (const auto dim : database_configuration->GetIndexedTileDimensions())
    {
        string_stream << "CREATE INDEX [" << database_configuration->GetIndexForDimensionColumnPrefix() << dim << "] ON "
            << "["<<database_configuration->GetTableNameForTilesInfoOrThrow()<<"] "
            << "( [" << database_configuration->GetDimensionsColumnPrefix() << dim << "]);";
    }

    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto string_stream = stringstream();
    string_stream << "CREATE TABLE[" << database_configuration->GetTableNameForGeneralTableOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "] TEXT(40) UNIQUE," <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "] TEXT);";

    return string_stream.str();
}

void DbCreator::SetGeneralTableInfoForSpatialIndex(const DatabaseConfiguration2D* database_configuration)
{
    // insert an item into the "General"-table where we notify about the name of the "tiles-spatial-table"
    Utilities::WriteStringIntoPropertyBag(
        this->db_connection_.get(),
        database_configuration->GetTableNameForGeneralTableOrThrow(),
        database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key),
        database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString),
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kSpatialIndexTable),
        database_configuration->GetTableNameForTilesSpatialIndexTableOrThrow());
}

std::string DbCreator::GenerateSqlStatementForFillingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto string_stream = stringstream();
    string_stream << "INSERT INTO [" << database_configuration->GetTableNameForGeneralTableOrThrow() << "]" <<
        "([" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "], " <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "])" <<
        " VALUES('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kVersion) << "','" << "0.0.1-alpha" << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesDataTable) << "','" << database_configuration->GetTableNameForTilesDataOrThrow() << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesInfoTable) << "','" << database_configuration->GetTableNameForTilesInfoOrThrow() << "')," <<
        "('" << DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kDocType) << "','" << database_configuration->GetDocTypeConstant() << "');";

    return string_stream.str();
}

void DbCreator::Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options)
{
    database_configuration->SetDimensionColumnPrefix(DbConstants::kDimensionColumnPrefix_Default/*"Dim_"*/);
    database_configuration->SetIndexForDimensionColumnPrefix(DbConstants::kIndexForDimensionColumnPrefix_Default/*"IndexForDim_"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::GeneralInfo, DbConstants::kGeneralTable_Name/*"GENERAL"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesData, DbConstants::kTilesDataTable_DefaultName/*"TILESDATA"*/);
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesInfo, DbConstants::kTilesInfoTable_DefaultName/*"TILESINFO"*/);

    database_configuration->SetDefaultColumnNamesForTilesDataTable();

    database_configuration->SetDefaultColumnNamesForTilesInfoTable();
    database_configuration->SetTileDimensions(create_options->GetDimensions().cbegin(), create_options->GetDimensions().cend());
    database_configuration->SetIndexedTileDimensions(create_options->GetIndexedDimensions().cbegin(), create_options->GetIndexedDimensions().cend());

    if (create_options->GetUseSpatialIndex())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesSpatialIndex, DbConstants::kTilesSpatialIndexTable_DefaultName/*"TILESSPATIALINDEX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk, DbConstants::kSqliteSpatialIndexTable_Column_Pk_DefaultName/*"id"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX, DbConstants::kSqliteSpatialIndexTable_Column_minX_DefaultName /*"minX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX, DbConstants::kSqliteSpatialIndexTable_Column_maxX_DefaultName /*"maxX"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY, DbConstants::kSqliteSpatialIndexTable_Column_minY_DefaultName /*"minY"*/);
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY, DbConstants::kSqliteSpatialIndexTable_Column_maxY_DefaultName /*"maxY"*/);
    }

    if (create_options->GetCreateBlobTable())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::Blobs, "BLOBS");
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Pk, "Pk");
        database_configuration->SetColumnNameForBlobTable(DatabaseConfiguration2D::kBlobTable_Column_Data, "Data");
    }
}

std::string DbCreator::GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetIsUsingSpatialIndex() == true);

    auto string_stream = ostringstream();
    string_stream << "CREATE VIRTUAL TABLE " << database_configuration->GetTableNameForTilesSpatialIndexTableOrThrow() << " USING rtree(" <<
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << "," <<         // Integer primary key
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "," <<       // Minimum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << "," <<       // Maximum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "," <<       // Minimum Y coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ");";        // Maximum Y coordinate"
    return string_stream.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingBlobTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    Expects(database_configuration != nullptr && database_configuration->GetHasBlobsTable() == true);

    auto string_stream = ostringstream();
    string_stream << "CREATE TABLE [" << database_configuration->GetTableNameForBlobTableOrThrow() << "] (" <<
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "] INTEGER PRIMARY KEY,"
        "[" << database_configuration->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] BLOB );";

    return string_stream.str();
}

void DbCreator::SetBlobTableNameInGeneralTable(const DatabaseConfiguration2D* database_configuration)
{
    auto string_stream = stringstream();
    string_stream << "INSERT INTO [" << database_configuration->GetTableNameForGeneralTableOrThrow() << "]" <<
        "([" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "], " <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "])" <<
        " VALUES('" << "BlobTable" << "','" << database_configuration->GetTableNameForBlobTableOrThrow() << "');";
    this->db_connection_->Execute(string_stream.str());
}

// ----------------------------------------------------------------------------

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(open_existing_options->GetFilename().c_str());

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
