#pragma once

#include <vector>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>
#include <imgdoc2.h>

class DatabaseConfigurationCommon
{
public:
    enum class TableTypeCommon
    {
        GeneralInfo,
        TilesData,
        TilesInfo,
        TilesSpatialIndex
    };

    /*static constexpr int kGeneralTableItems_Version = 1;
    static constexpr int kGeneralTableItems_TilesDataTable = 2;
    static constexpr int kGeneralTableItems_TilesInfoTable = 3;
    static constexpr int kGeneralTableItems_DocType = 4;*/
    enum class GeneralTableItems
    {
        kVersion,
        kTilesDataTable,
        kTilesInfoTable,
        kDocType
    };

    static constexpr int kGeneralInfoTable_Column_Key = 1;
    static constexpr int kGeneralInfoTable_Column_ValueString = 2;
private:
    std::unordered_set<imgdoc2::Dimension> dimensions_;
    std::map<TableTypeCommon, std::string> map_tabletype_to_tablename_;
    std::string dimension_column_prefix_;
public:
    template<typename ForwardIterator>
    void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->dimensions_.clear();
        for (auto it = begin; it != end; ++it)
        {
            this->dimensions_.insert(*it);
        }
    }

    const std::unordered_set<imgdoc2::Dimension>& GetTileDimensions() const { return this->dimensions_; }

    void SetDimensionColumnPrefix(const char* prefix) { this->dimension_column_prefix_ = prefix; };
    const std::string& GetDimensionsColumnPrefix() const { return this->dimension_column_prefix_; }

    void SetTableName(TableTypeCommon tableType, const char* name);

    virtual bool TryGetTableName(TableTypeCommon tt, std::string* name) const;

    std::string GetGeneralTableItem(GeneralTableItems item) const;

    bool TryGetColumnNameOfGeneralInfoTable(int columnIdentifier, std::string* column_name) const;

    virtual ~DatabaseConfigurationCommon() = default;
public:
    std::string GetTableNameOrThrow(TableTypeCommon tt) const;
    std::string GetTableNameForTilesDataOrThrow() const;
    std::string GetTableNameForTilesInfoOrThrow() const;
    std::string GetTableNameForGeneralTableOrThrow() const;
    std::string GetColumnNameOfGeneralInfoTableOrThrow(int columnIdentifier) const;
    std::string GetTableNameForTilesSpatialIndexTableOrThrow() const;

    bool GetUsingSpatialIndex() const;
};


/// This class is intended to capture the "state of the database configuration" for 2D-documents.
class DatabaseConfiguration2D : public DatabaseConfigurationCommon
{
private:
    std::map<int, std::string> map_tilesinfotable_columnids_to_columnname_;
    std::map<int, std::string> map_tilesdatatable_columnids_to_columnname_;
    std::map<int, std::string> map_tilespatialindextable_columnids_to_columnname_;
public:
    static constexpr int kTilesInfoTable_Column_Pk = 1;
    static constexpr int kTilesInfoTable_Column_TileX = 2;
    static constexpr int kTilesInfoTable_Column_TileY = 3;
    static constexpr int kTilesInfoTable_Column_TileW = 4;
    static constexpr int kTilesInfoTable_Column_TileH = 5;
    static constexpr int kTilesInfoTable_Column_PyramidLevel = 6;
    static constexpr int kTilesInfoTable_Column_TileDataId = 7;

    static constexpr int kTilesDataTable_Column_Pk = 1;
    static constexpr int kTilesDataTable_Column_PixelWidth = 2;
    static constexpr int kTilesDataTable_Column_PixelHeight = 3;
    static constexpr int kTilesDataTable_Column_PixelType = 4;
    static constexpr int kTilesDataTable_Column_TileDataType = 5;
    static constexpr int kTilesDataTable_Column_BinDataStorageType = 6;
    //static constexpr int kTilesDataTable_Column_DataBinHdrBlob = 6;

    static constexpr int kTilesSpatialIndexTable_Column_Pk = 1;
    static constexpr int kTilesSpatialIndexTable_Column_MinX = 2;
    static constexpr int kTilesSpatialIndexTable_Column_MaxX = 3;
    static constexpr int kTilesSpatialIndexTable_Column_MinY = 4;
    static constexpr int kTilesSpatialIndexTable_Column_MaxY = 5;

    /// Gets document type constant - the string for the row "DocType" in the general-table which identifies
    /// the data as "tiles-2D".
    ///
    /// \returns    The document type constant.
    const std::string& GetDocTypeConstant() const;

    void SetColumnNameForTilesInfoTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesInfoTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForTilesDataTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesDataTable(int columnIdentifier, std::string* column_name) const;

    void SetColumnNameForTilesSpatialIndexTable(int columnIdentifier, const char* column_name);
    bool TryGetColumnNameOfTilesSpatialIndexTable(int columnIdentifier, std::string* column_name) const;

public:
    std::string GetColumnNameOfTilesInfoTableOrThrow(int columnIdentifier) const;
    std::string GetColumnNameOfTilesDataTableOrThrow(int columnIdentifier) const;
    std::string GetColumnNameOfTilesSpatialIndexTableOrThrow(int columnIdentifier) const;
private:
    static void SetColumnName(std::map<int, std::string>& map, int columnIdentifier, const char* column_name);
    static bool GetColumnName(const std::map<int, std::string>& map, int columnIdentifier, std::string* column_name);
};
