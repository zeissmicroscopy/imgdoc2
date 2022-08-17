#pragma once

#include <vector>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <string>
#include <imgdoc2.h>

class DatabaseConfigurationCommon
{
public:
    enum class TableTypeCommon
    {
        TilesData,
        TilesInfo,
        TilesSpatialIndex
    };
private:
    std::unordered_set<imgdoc2::Dimension> dimensions_;
    std::map<TableTypeCommon, std::string> map_tabletype_to_tablename_;
public:
    template<typename ForwardIterator>
    void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->dimensions_.clear();
        std::copy(begin, end, std::back_inserter(this->dimensions_));
    }

    void SetTableName(TableTypeCommon tableType, const char* name);

    virtual bool TryGetTableName(TableTypeCommon tt, std::string* name) const;
};


/// This class is intended to capture the "state of the database configuration" for 2D-documents.
class DatabaseConfiguration2D : DatabaseConfigurationCommon
{
private:
    std::map<int, std::string> map_tilesinfotable_columnids_to_columnname_;
    std::map<int, std::string> map_tilesdatatable_columnids_to_columnname_;
public:
    const int kTilesInfoTable_Column_Pk = 1;
    const int kTilesInfoTable_Column_TileX = 2;
    const int kTilesInfoTable_Column_TileY = 3;
    const int kTilesInfoTable_Column_TileW = 4;
    const int kTilesInfoTable_Column_TileH = 5;
    const int kTilesInfoTable_Column_PyramidLevel = 6;
    const int kTilesInfoTable_Column_TileDataId = 7;

    const int kTilesDataTable_Column_Pk = 1;
    const int kTilesDataTable_Column_PixelWidth = 2;
    const int kTilesDataTable_Column_PixelHeight = 3;
    const int kTilesDataTable_Column_PixelType = 4;
    const int kTilesDataTable_Column_DataBinHdrBlob = 5;

    void SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name);
    bool GetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name);
    bool GetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const;

private:
    static void SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name);
    static bool GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name);
};
