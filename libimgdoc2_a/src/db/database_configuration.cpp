#include "database_configuration.h"
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

std::string DatabaseConfigurationCommon::GetColumnNameOfGeneralInfoTableOrThrow(int columnIdentifier) const
{
    string s;
    if (!this->TryGetColumnNameOfGeneralInfoTable(columnIdentifier, &s))
    {
        throw std::runtime_error("column-name not present");
    }

    return s;
}

std::string DatabaseConfigurationCommon::GetGeneralTableItem(GeneralTableItems item) const
{
    switch (item)
    {
    case GeneralTableItems::kVersion:
        return "Version";
    case GeneralTableItems::kTilesDataTable:
        return "TilesDataTable";
    case GeneralTableItems::kTilesInfoTable:
        return "TilesInfoTable";
    case GeneralTableItems::kDocType:
        return "DocType";
    }

    throw invalid_argument("invalid argument for 'item' specified.");
}

bool DatabaseConfigurationCommon::GetUsingSpatialIndex() const
{
    const auto it = this->map_tabletype_to_tablename_.find(TableTypeCommon::TilesSpatialIndex);
    return it != this->map_tabletype_to_tablename_.cend();
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

/*static*/void DatabaseConfiguration2D::SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name)
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

/*static*/bool DatabaseConfiguration2D::GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name)
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

