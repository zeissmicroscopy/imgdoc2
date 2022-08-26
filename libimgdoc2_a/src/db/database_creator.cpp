#include <sstream>
#include <imgdoc2.h>
#include "database_creator.h"
#include "database_configuration.h"
#include "DbFactory.h"
#include "../doc/document.h"

using namespace std;

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::CreateNew(imgdoc2::ICreateOptions* create_options)
{
    auto db_connection = DbFactory::SqliteCreateNewDatabase(create_options->GetFilename().c_str());

    // check pre-conditions

    // tweak settings

    DbCreator db_creator(db_connection);
    const auto configuration = db_creator.CreateTables(create_options);

    const auto database_configuration_2d = std::dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    // TODO: 3d version should follow here
}

std::shared_ptr< DatabaseConfigurationCommon> DbCreator::CreateTables(const imgdoc2::ICreateOptions* create_options)
{
    // construct the "database configuration" based on the the create_options
    auto database_configuration = make_shared<DatabaseConfiguration2D>();
    this->Initialize2dConfigurationFromCreateOptions(database_configuration.get(), create_options);

    // TODO: make those operations a transaction
    auto sql_statement = this->GenerateSqlStatementForCreatingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Exec(sql_statement);

    sql_statement = this->GenerateSqlStatementForFillingGeneralTable_Sqlite(database_configuration.get());
    this->db_connection_->Exec(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesDataTable_Sqlite(database_configuration.get());
    this->db_connection_->Exec(sql_statement);

    sql_statement = this->GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(database_configuration.get());
    this->db_connection_->Exec(sql_statement);

    if (create_options->GetUseSpatialIndex())
    {
        sql_statement = this->GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(database_configuration.get());
        this->db_connection_->Exec(sql_statement);
    }

    return database_configuration;
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesDataTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << database_configuration->GetTableNameForTilesDataOrThrow() << "]("
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] INTEGER PRIMARY KEY,"
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "] INTEGER(4),"
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "] INTEGER(4),"
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "] INTEGER(1),"
        "[" << database_configuration->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "] INTEGER(1)";

    ss << ");";

    return ss.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingTilesInfoTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto ss = stringstream();
    // Notes:
    // * "INTEGER PRIMARY KEY" makes the column-name an alias for the RowId-column
    ss << "CREATE TABLE[" << database_configuration->GetTableNameForTilesInfoOrThrow() << "]("
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "] INTEGER PRIMARY KEY,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "] DOUBLE NOT NULL,"
        "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "] INTEGER(1) NOT NULL,";

    //const auto tile_dimension = database_configuration->GetTileDimensions();
    //for (auto d : tile_dimension)
    //{
    //    ss << "[" << database_configuration->GetDimensionsColumnPrefix() << d << "] INTEGER(4) NOT NULL,";
    //}

    ss << "[" << database_configuration->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] INTEGER(8) NOT NULL";

    for (const auto dim : database_configuration->GetTileDimensions())
    {
        string colName = database_configuration->GetDimensionsColumnPrefix() + dim;
        ss << ", [" << colName << "] INTEGER(4) NOT NULL";
    }

    ss << ");";
    return ss.str();
}

std::string DbCreator::GenerateSqlStatementForCreatingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << database_configuration->GetTableNameForGeneralTableOrThrow() << "](" <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "] TEXT(40) UNIQUE," <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "] TEXT);";

    return ss.str();
}

std::string DbCreator::GenerateSqlStatementForFillingGeneralTable_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto ss = stringstream();
    ss << "INSERT INTO [" << database_configuration->GetTableNameForGeneralTableOrThrow() << "]" <<
        "([" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_Key) << "], " <<
        "[" << database_configuration->GetColumnNameOfGeneralInfoTableOrThrow(DatabaseConfigurationCommon::kGeneralInfoTable_Column_ValueString) << "])" <<
        " VALUES('" << database_configuration->GetGeneralTableItem(DatabaseConfigurationCommon::GeneralTableItems::kVersion) << "','" << "0.0.1-alpha" << "')," <<
        "('" << database_configuration->GetGeneralTableItem(DatabaseConfigurationCommon::GeneralTableItems::kTilesDataTable) << "','" << database_configuration->GetTableNameForTilesDataOrThrow() << "')," <<
        "('" << database_configuration->GetGeneralTableItem(DatabaseConfigurationCommon::GeneralTableItems::kTilesInfoTable) << "','" << database_configuration->GetTableNameForTilesInfoOrThrow() << "')," <<
        "('" << database_configuration->GetGeneralTableItem(DatabaseConfigurationCommon::GeneralTableItems::kDocType) << "','" << database_configuration->GetDocTypeConstant() << "');";

    return ss.str();
}

void DbCreator::Initialize2dConfigurationFromCreateOptions(DatabaseConfiguration2D* database_configuration, const imgdoc2::ICreateOptions* create_options)
{
    database_configuration->SetDimensionColumnPrefix("Dim_");
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::GeneralInfo, "GENERAL");
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesData, "TILESDATA");
    database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesInfo, "TILESINFO");

    database_configuration->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_Pk, "Pk");
    database_configuration->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth, "PixelWidth");
    database_configuration->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight, "PixelHeight");
    database_configuration->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType, "PixelType");
    database_configuration->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType, "TileDataType");
    database_configuration->SetTileDimensions(create_options->GetDimensions().cbegin(), create_options->GetDimensions().cend());

    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk, "Pk");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX, "TileX");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY, "TileY");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW, "TileW");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH, "TileH");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel, "PyramidLevel");
    database_configuration->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId, "TileDataId");

    if (create_options->GetUseSpatialIndex())
    {
        database_configuration->SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesSpatialIndex, "TILESSPATIALINDEX");
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk, "id");
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Minx, "minX");
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX, "maxX");
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY, "minY");
        database_configuration->SetColumnNameForTilesSpatialIndexTable(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY, "maxY");
    }
}

std::string DbCreator::GenerateSqlStatementForCreatingSpatialTilesIndex_Sqlite(const DatabaseConfiguration2D* database_configuration)
{
    auto ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << database_configuration->GetTableNameForTilesSpatialIndexTableOrThrow() << " USING rtree(" <<
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << "," <<         // Integer primary key
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Minx) << "," <<       // Minimum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << "," <<       // Maximum X coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "," <<       // Minimum Y coordinate"
        database_configuration->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ");";        // Maximum Y coordinate"
    return ss.str();
}