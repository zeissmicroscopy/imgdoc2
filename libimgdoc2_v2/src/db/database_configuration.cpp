#include "database_configuration.h"
#include "database_constants.h"
#include <exception>

using namespace std;

void DatabaseConfigurationCommon::SetTableName(TableTypeCommon tableType, const char* name)
{
    if (name != nullptr)
    {
        this->map_tabletype_to_tablename_[tableType] = name;
    }
    else
    {
        this->map_tabletype_to_tablename_.erase(tableType);
    }
}

/*virtual*/bool DatabaseConfigurationCommon::TryGetTableName(TableTypeCommon tt, std::string* name) const
{
    const auto it = this->map_tabletype_to_tablename_.find(tt);
    if (it == this->map_tabletype_to_tablename_.cend())
    {
        return false;
    }

    if (name != nullptr)
    {
        *name = it->second;
    }

    return true;
}

std::string DatabaseConfigurationCommon::GetTableNameOrThrow(TableTypeCommon tt) const
{
    std::string s;
    if (!this->TryGetTableName(tt, &s))
    {
        throw std::runtime_error("table-name not present");
    }

    return s;
}

bool DatabaseConfigurationCommon::TryGetColumnNameOfGeneralInfoTable(int columnIdentifier, std::string* column_name) const
{
    const char* requested_column_name = nullptr;
    switch (columnIdentifier)
    {
    case kGeneralInfoTable_Column_Key:
        requested_column_name = "Key";
        break;
    case kGeneralInfoTable_Column_ValueString:
        requested_column_name = "ValueString";
        break;
    }

    if (requested_column_name == nullptr)
    {
        return false;
    }

    if (column_name != nullptr)
    {
        *column_name = requested_column_name;
    }

    return true;
}

void DatabaseConfigurationCommon::SetColumnNameForBlobTable(int column_identifier, const char* column_name)
{
    SetColumnName(this->map_blobtable_columnids_to_columnname_, column_identifier, column_name);
}

bool DatabaseConfigurationCommon::TryGetColumnNameOfBlobTable(int column_identifier, std::string* column_name) const
{
    return GetColumnName(this->map_blobtable_columnids_to_columnname_, column_identifier, column_name);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesDataOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesData);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesInfoOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesInfo);
}

std::string DatabaseConfigurationCommon::GetTableNameForGeneralTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::GeneralInfo);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesSpatialIndexTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesSpatialIndex);
}

std::string DatabaseConfigurationCommon::GetTableNameForBlobTableOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::Blobs);
}

std::string DatabaseConfigurationCommon::GetColumnNameOfGeneralInfoTableOrThrow(int columnIdentifier) const
{
    string s;
    if (!this->TryGetColumnNameOfGeneralInfoTable(columnIdentifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

bool DatabaseConfigurationCommon::GetIsUsingSpatialIndex() const
{
    const auto it = this->map_tabletype_to_tablename_.find(TableTypeCommon::TilesSpatialIndex);
    return it != this->map_tabletype_to_tablename_.cend();
}

bool DatabaseConfigurationCommon::GetHasBlobsTable() const
{
    const auto it = this->map_tabletype_to_tablename_.find(TableTypeCommon::Blobs);
    return it != this->map_tabletype_to_tablename_.cend();
}

bool DatabaseConfigurationCommon::IsDimensionIndexed(imgdoc2::Dimension dimension) const
{
    return this->indexed_dimensions_.find(dimension) != this->indexed_dimensions_.cend();
}

std::string DatabaseConfigurationCommon::GetColumnNameOfBlobTableOrThrow(int column_identifier) const
{
    std::string s;
    if (!this->TryGetColumnNameOfBlobTable(column_identifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

/*static*/void DatabaseConfigurationCommon::SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name)
{
    if (column_name != nullptr)
    {
        map[columnIdentifier] = column_name;
    }
    else
    {
        map.erase(columnIdentifier);
    }
}

/*static*/bool DatabaseConfigurationCommon::GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name)
{
    const auto it = map.find(columnIdentifier);
    if (it == map.cend())
    {
        return false;
    }

    if (column_name != nullptr)
    {
        *column_name = it->second;
    }

    return true;
}

// ----------------------------------------------------------------------------

const std::string& DatabaseConfiguration2D::GetDocTypeConstant() const
{
    static string kTiles2DDocType = "Tiles2D";
    return kTiles2DDocType;
}

void DatabaseConfiguration2D::SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesinfotable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration2D::SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilesdatatable_columnids_to_columnname_, columnIdentifier, column_name);
}

void DatabaseConfiguration2D::SetColumnNameForTilesSpatialIndexTable(int columnIdentifier, const char* column_name)
{
    SetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}

bool DatabaseConfiguration2D::TryGetColumnNameOfTilesSpatialIndexTable(int columnIdentifier, std::string* column_name) const
{
    return GetColumnName(this->map_tilespatialindextable_columnids_to_columnname_, columnIdentifier, column_name);
}


std::string DatabaseConfiguration2D::GetColumnNameOfTilesDataTableOrThrow(int columnIdentifier) const
{
    std::string s;
    if (!this->TryGetColumnNameOfTilesDataTable(columnIdentifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

std::string DatabaseConfiguration2D::GetColumnNameOfTilesInfoTableOrThrow(int columnIdentifier) const
{
    std::string s;
    if (!this->TryGetColumnNameOfTilesInfoTable(columnIdentifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

std::string DatabaseConfiguration2D::GetColumnNameOfTilesSpatialIndexTableOrThrow(int columnIdentifier) const
{
    std::string s;
    if (!this->TryGetColumnNameOfTilesSpatialIndexTable(columnIdentifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

void DatabaseConfiguration2D::SetDefaultColumnNamesForTilesInfoTable()
{
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk, DbConstants::kTilesInfoTable_Column_Pk_DefaultName/*"Pk"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX, DbConstants::kTilesInfoTable_Column_TileX_DefaultName/*"TileX"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY, DbConstants::kTilesInfoTable_Column_TileY_DefaultName/*"TileY"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW, DbConstants::kTilesInfoTable_Column_TileW_DefaultName /*"TileW"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH, DbConstants::kTilesInfoTable_Column_TileH_DefaultName/*"TileH"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel, DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName/*"PyramidLevel"*/);
   this->SetColumnNameForTilesInfoTable(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId, DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName/*"TileDataId"*/);
}

void DatabaseConfiguration2D::SetDefaultColumnNamesForTilesDataTable()
{
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_Pk, DbConstants::kTilesDataTable_Column_Pk_DefaultName/*"Pk"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth, DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName/*"PixelWidth"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight, DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName/*"PixelHeight"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType, DbConstants::kTilesDataTable_Column_PixelType_DefaultName/*"PixelType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType, DbConstants::kTilesDataTable_Column_TileDataType_DefaultName/*"TileDataType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType, DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName/*"BinDataStorageType"*/);
    this->SetColumnNameForTilesDataTable(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId, DbConstants::kTilesDataTable_Column_BinDataId_DefaultName/*"BinDataId"*/);
}