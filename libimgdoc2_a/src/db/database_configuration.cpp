#include "database_configuration.h"

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

std::string DatabaseConfigurationCommon::GetTableNameForTilesDataOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesData);
}

std::string DatabaseConfigurationCommon::GetTableNameForTilesInfoOrThrow() const
{
    return this->GetTableNameOrThrow(TableTypeCommon::TilesInfo);
}

// ----------------------------------------------------------------------------

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
